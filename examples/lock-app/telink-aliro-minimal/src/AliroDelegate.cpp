/*
 *
 *    Copyright (c) 2026 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "AliroDelegate.h"
#include "AliroStorage.h"
#include "LockManager.h"

#include <cstdio>
#include <cstring>
#include <errno.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPEncoding.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>
#include <telink_aliro/telink_aliro.h>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

AliroDelegate AliroDelegate::sInstance;

namespace {

void CredentialStorageKey(char * key, size_t size, CredentialTypeEnum type, uint16_t index)
{
    char prefix = 'i';
    if (type == CredentialTypeEnum::kAliroEvictableEndpointKey)
    {
        prefix = 'e';
    }
    else if (type == CredentialTypeEnum::kAliroNonEvictableEndpointKey)
    {
        prefix = 'n';
    }

    snprintf(key, size, "aliro/%c/%u", prefix, static_cast<unsigned>(index));
}

} // namespace

// ---------------------------------------------------------------------------
// DoorLock::Delegate - Aliro provisioning attributes
// ---------------------------------------------------------------------------

CHIP_ERROR AliroDelegate::GetAliroReaderVerificationKey(MutableByteSpan & verificationKey)
{
    ChipLogProgress(Zcl, "[Aliro] Read ReaderVerificationKey (configured=%u)", mAliroStateInitialized);

    if (!mAliroStateInitialized)
    {
        verificationKey.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    return CopySpanToMutableSpan(ByteSpan(mAliroReaderVerificationKey), verificationKey);
}

CHIP_ERROR AliroDelegate::GetAliroReaderGroupIdentifier(MutableByteSpan & groupIdentifier)
{
    ChipLogProgress(Zcl, "[Aliro] Read ReaderGroupIdentifier (configured=%u)", mAliroStateInitialized);

    if (!mAliroStateInitialized)
    {
        groupIdentifier.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    return CopySpanToMutableSpan(ByteSpan(mAliroReaderGroupIdentifier), groupIdentifier);
}

CHIP_ERROR AliroDelegate::GetAliroReaderGroupSubIdentifier(MutableByteSpan & groupSubIdentifier)
{
    ChipLogProgress(Zcl, "[Aliro] Read ReaderGroupSubIdentifier (configured=%u)", mAliroStateInitialized);

    if (!mAliroStateInitialized)
    {
        groupSubIdentifier.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    return CopySpanToMutableSpan(ByteSpan(mAliroReaderGroupSubIdentifier), groupSubIdentifier);
}

CHIP_ERROR AliroDelegate::CopyProtocolVersionIntoSpan(uint16_t protocolVersionValue, MutableByteSpan & protocolVersion)
{
    static_assert(sizeof(protocolVersionValue) == kAliroProtocolVersionSize);

    if (protocolVersion.size() < kAliroProtocolVersionSize)
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    // Per Aliro spec, protocol version encoding is big-endian.
    Encoding::BigEndian::Put16(protocolVersion.data(), protocolVersionValue);
    protocolVersion.reduce_size(kAliroProtocolVersionSize);
    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDelegate::GetAliroExpeditedTransactionSupportedProtocolVersionAtIndex(size_t index,
                                                                                      MutableByteSpan & protocolVersion)
{
    ChipLogProgress(Zcl, "[Aliro] Read ExpeditedProtocolVersion (index=%u)", static_cast<unsigned>(index));

    // Only claim support for the one known protocol version for now: 0x0100.
    constexpr uint16_t knownProtocolVersion = 0x0100;

    if (index > 0)
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    return CopyProtocolVersionIntoSpan(knownProtocolVersion, protocolVersion);
}

CHIP_ERROR AliroDelegate::GetAliroGroupResolvingKey(MutableByteSpan & groupResolvingKey)
{
    ChipLogProgress(Zcl, "[Aliro] Read GroupResolvingKey (configured=%u)", mAliroStateInitialized);

    if (!mAliroStateInitialized || !mAliroHasGroupResolvingKey)
    {
        groupResolvingKey.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    return CopySpanToMutableSpan(ByteSpan(mAliroGroupResolvingKey), groupResolvingKey);
}

CHIP_ERROR AliroDelegate::GetAliroSupportedBLEUWBProtocolVersionAtIndex(size_t index, MutableByteSpan & protocolVersion)
{
    // Only claim support for the one known protocol version for now: 0x0100.
    constexpr uint16_t knownProtocolVersion = 0x0100;

    if (index > 0)
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    return CopyProtocolVersionIntoSpan(knownProtocolVersion, protocolVersion);
}

uint8_t AliroDelegate::GetAliroBLEAdvertisingVersion()
{
    ChipLogProgress(Zcl, "[Aliro] Read BLEAdvertisingVersion");

    // For now the only defined value of the BLE advertising version for Aliro is 0.
    return 0;
}

uint16_t AliroDelegate::GetNumberOfAliroCredentialIssuerKeysSupported()
{
    ChipLogProgress(Zcl, "[Aliro] Read NumberOfCredentialIssuerKeysSupported");
    return APP_MAX_ALIRO_ISSUER_KEYS;
}

uint16_t AliroDelegate::GetNumberOfAliroEndpointKeysSupported()
{
    ChipLogProgress(Zcl, "[Aliro] Read NumberOfEndpointKeysSupported");
    return APP_MAX_ALIRO_ENDPOINT_KEYS;
}

CHIP_ERROR AliroDelegate::RestoreAliroState()
{
    VerifyOrReturnError(!mAliroStorageRestored, CHIP_ERROR_INCORRECT_STATE);

    AliroStorage::ReaderConfig config = {};
    CHIP_ERROR err = AliroStorage::Load(AliroStorage::kReaderConfigKey, &config, sizeof(config));
    const bool hasReaderConfig = (err == CHIP_NO_ERROR);

    if (err != CHIP_NO_ERROR && err != CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND)
    {
        ChipLogError(Zcl, "Failed to restore Aliro reader configuration: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    if (hasReaderConfig)
    {
        VerifyOrReturnError(config.version == AliroStorage::kRecordVersion && config.hasGroupResolvingKey <= 1,
        CHIP_ERROR_INVALID_ARGUMENT);

        int ret;
#if CONFIG_ALIRO_TRANSPORT_BLE
        ret = telink_aliro_ble_set_group_resolving_key(config.hasGroupResolvingKey ? config.groupResolvingKey : nullptr,
            config.hasGroupResolvingKey ? sizeof(config.groupResolvingKey) : 0);

        VerifyOrReturnError(ret == 0, CHIP_ERROR_INTERNAL,
            ChipLogError(Zcl, "Failed to restore Aliro BLE resolving key: %d", ret));
#endif

        ret = telink_aliro_set_reader_config(config.signingKey, sizeof(config.signingKey),
            config.verificationKey, sizeof(config.verificationKey),
            config.groupIdentifier, sizeof(config.groupIdentifier),
            config.groupSubIdentifier, sizeof(config.groupSubIdentifier));

        VerifyOrReturnError(ret == 0, CHIP_ERROR_INTERNAL,
            ChipLogError(Zcl, "Failed to restore Aliro reader configuration: %d", ret));
    }

    const CredentialTypeEnum credentialTypes[] = {
        CredentialTypeEnum::kAliroCredentialIssuerKey,
        CredentialTypeEnum::kAliroEvictableEndpointKey,
        CredentialTypeEnum::kAliroNonEvictableEndpointKey,
    };

    for (CredentialTypeEnum type : credentialTypes)
    {
        for (uint16_t index = 1; index <= SlotCountForType(type); ++index)
        {
            char key[24];
            CredentialStorageKey(key, sizeof(key), type, index);

            AliroStorage::Credential record = {};
            err = AliroStorage::Load(key, &record, sizeof(record));
            if (err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND)
            {
                continue;
            }
            ReturnErrorOnFailure(err);

            VerifyOrReturnError(record.version == AliroStorage::kRecordVersion && record.index == index &&
                record.type == to_underlying(type) && record.status != to_underlying(DlCredentialStatus::kAvailable) &&
                record.dataSize <= kAliroCredentialMaxSize, CHIP_ERROR_INVALID_ARGUMENT);

            CredentialSlot * slot = FindSlot(index, type);
            if (slot == nullptr && IsEndpointCredentialType(type))
            {
                slot = FindAvailableEndpointSlot();
            }
            VerifyOrReturnError(slot != nullptr, CHIP_ERROR_NO_MEMORY);

            memset(slot->data, 0, sizeof(slot->data));
            memcpy(slot->data, record.data, record.dataSize);
            slot->dataSize       = record.dataSize;
            slot->status         = static_cast<DlCredentialStatus>(record.status);
            slot->type           = type;
            slot->index          = index;
            slot->createdBy      = static_cast<FabricIndex>(record.createdBy);
            slot->lastModifiedBy = static_cast<FabricIndex>(record.lastModifiedBy);
        }
    }

    if (hasReaderConfig)
    {
        memcpy(mAliroReaderVerificationKey, config.verificationKey, sizeof(mAliroReaderVerificationKey));
        memcpy(mAliroReaderGroupIdentifier, config.groupIdentifier, sizeof(mAliroReaderGroupIdentifier));
        memcpy(mAliroReaderGroupSubIdentifier, config.groupSubIdentifier, sizeof(mAliroReaderGroupSubIdentifier));
        memcpy(mAliroGroupResolvingKey, config.groupResolvingKey, sizeof(mAliroGroupResolvingKey));
        mAliroHasGroupResolvingKey = config.hasGroupResolvingKey != 0;
        mAliroStateInitialized = true;
    }

    mAliroStorageRestored = true;
    return CHIP_NO_ERROR;
}

#if 1 //REMOVE
void LogAliroKey(const char * name, const uint8_t * data, size_t size)
{
    constexpr size_t kMaxKeySize = 65;
    constexpr size_t kBytesPerLine = 32;
    constexpr char kHexDigits[] = "0123456789ABCDEF";

    if (data == nullptr || size > kMaxKeySize)
    {
        ChipLogError(Zcl, "[Aliro][DEBUG] Invalid %s length: %u", name, static_cast<unsigned>(size));
        return;
    }

    ChipLogProgress(Zcl, "[Aliro][DEBUG] %s (%u bytes):", name, static_cast<unsigned>(size));
    for (size_t offset = 0; offset < size; offset += kBytesPerLine)
    {
        char hex[2 * kBytesPerLine + 1];
        const size_t count = size - offset < kBytesPerLine ? size - offset : kBytesPerLine;
        for (size_t i = 0; i < count; ++i)
        {
            const uint8_t byte = data[offset + i];
            hex[2 * i]         = kHexDigits[byte >> 4];
            hex[2 * i + 1]     = kHexDigits[byte & 0x0F];
        }
        hex[2 * count] = '\0';
        ChipLogProgress(Zcl, "[Aliro][DEBUG]   offset=%u: %s", static_cast<unsigned>(offset), hex);
        memset(hex, 0, sizeof(hex));
    }
}
#endif

CHIP_ERROR AliroDelegate::StartAliroTransports()
{

#if 1 //REMOVE
    ChipLogProgress(Zcl, "xxxxx [Aliro][DEBUG] Current RAM state: restored=%u, reader=%u, NFC=%u, BLE=%u",
                    static_cast<unsigned>(mAliroStorageRestored), static_cast<unsigned>(mAliroStateInitialized),
                    static_cast<unsigned>(mAliroNfcStarted), static_cast<unsigned>(mAliroBleStarted));


    ChipLogProgress(Zcl, "xxxxx mAliroStorageRestored=%u, mAliroStateInitialized=%u, HasAliroAuthorizedEndpoint=%u",
        mAliroStorageRestored, mAliroStateInitialized, LockMgr().HasAliroAuthorizedEndpoint());

    if (mAliroStateInitialized)
    {
        // The signing key is held by the Aliro library, not cached in this delegate.
        ChipLogProgress(Zcl, "[Aliro][DEBUG] Reader Signing Key: not cached in AliroDelegate RAM");
        LogAliroKey("Reader Verification Key", mAliroReaderVerificationKey, sizeof(mAliroReaderVerificationKey));
        LogAliroKey("Reader Group Identifier", mAliroReaderGroupIdentifier, sizeof(mAliroReaderGroupIdentifier));
        LogAliroKey("Reader Group Sub-Identifier", mAliroReaderGroupSubIdentifier,
                    sizeof(mAliroReaderGroupSubIdentifier));

        if (mAliroHasGroupResolvingKey)
        {
            LogAliroKey("Group Resolving Key (SECRET)", mAliroGroupResolvingKey, sizeof(mAliroGroupResolvingKey));
        }
        else
        {
            ChipLogProgress(Zcl, "[Aliro][DEBUG] Group Resolving Key: not configured");
        }
    }
    else
    {
        ChipLogProgress(Zcl, "[Aliro][DEBUG] Reader configuration: not configured");
    }

    unsigned issuerKeyCount = 0;
    for (const auto & slot : mIssuerKeys)
    {
        if (slot.status == DlCredentialStatus::kAvailable)
        {
            continue;
        }

        ++issuerKeyCount;
        ChipLogProgress(Zcl, "[Aliro][DEBUG] Issuer key: index=%u, status=%u, createdBy=%u, lastModifiedBy=%u",
                        static_cast<unsigned>(slot.index), static_cast<unsigned>(to_underlying(slot.status)),
                        static_cast<unsigned>(slot.createdBy), static_cast<unsigned>(slot.lastModifiedBy));
        LogAliroKey("Credential Issuer Key", slot.data, slot.dataSize);
    }

    unsigned endpointKeyCount = 0;
    for (const auto & slot : mEndpointKeys)
    {
        if (slot.status == DlCredentialStatus::kAvailable)
        {
            continue;
        }

        ++endpointKeyCount;
        const char * type = slot.type == CredentialTypeEnum::kAliroEvictableEndpointKey ? "Evictable" : "Non-evictable";
        ChipLogProgress(Zcl, "[Aliro][DEBUG] %s endpoint key: index=%u, status=%u, createdBy=%u, lastModifiedBy=%u",
                        type, static_cast<unsigned>(slot.index), static_cast<unsigned>(to_underlying(slot.status)),
                        static_cast<unsigned>(slot.createdBy), static_cast<unsigned>(slot.lastModifiedBy));
        LogAliroKey("Endpoint Public Key", slot.data, slot.dataSize);
    }

    ChipLogProgress(Zcl, "[Aliro][DEBUG] Installed keys in RAM: issuers=%u, endpoints=%u", issuerKeyCount, endpointKeyCount);
#endif



    if (!mAliroStorageRestored || !mAliroStateInitialized || !LockMgr().HasAliroAuthorizedEndpoint())
    {
        return CHIP_NO_ERROR;
    }

#if CONFIG_ALIRO_TRANSPORT_NFC || CONFIG_ALIRO_TRANSPORT_BLE
    int err;
#endif

#if CONFIG_ALIRO_TRANSPORT_NFC
    if (!mAliroNfcStarted)
    {
        err = telink_aliro_nfc_start();
        if (err != 0 && err != -EALREADY)
        {
            ChipLogError(Zcl, "Failed to start Aliro NFC: %d", err);
            return CHIP_ERROR_INTERNAL;
        }
        mAliroNfcStarted = true;
    }
#endif

#if CONFIG_ALIRO_TRANSPORT_BLE
    if (!mAliroBleStarted)
    {
        err = telink_aliro_ble_start();
        if (err != 0)
        {
            ChipLogError(Zcl, "Failed to start Aliro BLE: %d", err);
            return CHIP_ERROR_INTERNAL;
        }
        mAliroBleStarted = true;
    }
#endif

    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDelegate::SetAliroReaderConfig(const ByteSpan & signingKey, const ByteSpan & verificationKey,
                                               const ByteSpan & groupIdentifier, const Optional<ByteSpan> & groupResolvingKey)
{
    VerifyOrReturnError(signingKey.size() == AliroStorage::kSigningKeySize, CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(verificationKey.size() == sizeof(mAliroReaderVerificationKey), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(groupIdentifier.size() == sizeof(mAliroReaderGroupIdentifier), CHIP_ERROR_INVALID_ARGUMENT);

    if (groupResolvingKey.HasValue())
    {
        VerifyOrReturnError(groupResolvingKey.Value().size() == sizeof(mAliroGroupResolvingKey), CHIP_ERROR_INVALID_ARGUMENT);
    }

    static_assert(sizeof(AliroStorage::ReaderConfig{}.verificationKey) == sizeof(mAliroReaderVerificationKey));
    static_assert(sizeof(AliroStorage::ReaderConfig{}.groupIdentifier) == sizeof(mAliroReaderGroupIdentifier));
    static_assert(sizeof(AliroStorage::ReaderConfig{}.groupSubIdentifier) == sizeof(mAliroReaderGroupSubIdentifier));
    static_assert(sizeof(AliroStorage::ReaderConfig{}.groupResolvingKey) == sizeof(mAliroGroupResolvingKey));
    static_assert(sizeof(AliroStorage::Credential{}.data) == kAliroCredentialMaxSize);

    if (mAliroStateInitialized)
    {
        AliroStorage::ReaderConfig current = {};
        ReturnErrorOnFailure(AliroStorage::Load(AliroStorage::kReaderConfigKey, &current, sizeof(current)));
        VerifyOrReturnError(current.version == AliroStorage::kRecordVersion, CHIP_ERROR_INVALID_ARGUMENT);

        const bool unchanged = memcmp(current.signingKey, signingKey.data(), signingKey.size()) == 0 &&
            memcmp(current.verificationKey, verificationKey.data(), verificationKey.size()) == 0 &&
            memcmp(current.groupIdentifier, groupIdentifier.data(), groupIdentifier.size()) == 0 &&
            current.hasGroupResolvingKey == (groupResolvingKey.HasValue() ? 1 : 0) &&
            (!groupResolvingKey.HasValue() ||
             memcmp(current.groupResolvingKey, groupResolvingKey.Value().data(), groupResolvingKey.Value().size()) == 0);

        // Reconfiguration of an active reader requires a preceding ClearAliroReaderConfig
        VerifyOrReturnError(unchanged, CHIP_ERROR_INCORRECT_STATE);
        ChipLogError(Zcl, "xxxxx StartAliroTransports 1");
        CHIP_ERROR startErr = StartAliroTransports();
        if (startErr != CHIP_NO_ERROR)
        {
            ChipLogError(Zcl, "Aliro transport start failed: %" CHIP_ERROR_FORMAT, startErr.Format());
        }
        return CHIP_NO_ERROR;
    }

    AliroStorage::ReaderConfig config = {};
    config.version = AliroStorage::kRecordVersion;
    memcpy(config.signingKey, signingKey.data(), signingKey.size());
    memcpy(config.verificationKey, verificationKey.data(), verificationKey.size());
    memcpy(config.groupIdentifier, groupIdentifier.data(), groupIdentifier.size());
    if (groupResolvingKey.HasValue())
    {
        config.hasGroupResolvingKey = 1;
        memcpy(config.groupResolvingKey, groupResolvingKey.Value().data(), groupResolvingKey.Value().size());
    }

    ReturnErrorOnFailure(Crypto::DRBG_get_bytes(config.groupSubIdentifier, sizeof(config.groupSubIdentifier)));

    int err;
#if CONFIG_ALIRO_TRANSPORT_BLE
    err = telink_aliro_ble_set_group_resolving_key(config.hasGroupResolvingKey ? config.groupResolvingKey : nullptr,
        config.hasGroupResolvingKey ? sizeof(config.groupResolvingKey) : 0);

    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL, ChipLogError(Zcl, "Unable to apply Aliro BLE group resolving key: %d", err));
#endif

    err = telink_aliro_set_reader_config(config.signingKey, sizeof(config.signingKey),
        config.verificationKey, sizeof(config.verificationKey),
        config.groupIdentifier, sizeof(config.groupIdentifier),
        config.groupSubIdentifier, sizeof(config.groupSubIdentifier));
    if (err != 0)
    {
#if CONFIG_ALIRO_TRANSPORT_BLE
        telink_aliro_ble_set_group_resolving_key(nullptr, 0);
#endif
        ChipLogError(Zcl, "Unable to apply Aliro reader configuration: %d", err);
        return CHIP_ERROR_INTERNAL;
    }

    CHIP_ERROR storageErr = AliroStorage::Save(AliroStorage::kReaderConfigKey, &config, sizeof(config));
    if (storageErr != CHIP_NO_ERROR)
    {
        telink_aliro_clear_reader_config();
#if CONFIG_ALIRO_TRANSPORT_BLE
        telink_aliro_ble_set_group_resolving_key(nullptr, 0);
#endif
        return storageErr;
    }

    memcpy(mAliroReaderVerificationKey, config.verificationKey, sizeof(mAliroReaderVerificationKey));
    memcpy(mAliroReaderGroupIdentifier, config.groupIdentifier, sizeof(mAliroReaderGroupIdentifier));
    memcpy(mAliroReaderGroupSubIdentifier, config.groupSubIdentifier, sizeof(mAliroReaderGroupSubIdentifier));
    memcpy(mAliroGroupResolvingKey, config.groupResolvingKey, sizeof(mAliroGroupResolvingKey));
    mAliroHasGroupResolvingKey = (config.hasGroupResolvingKey != 0);
    mAliroStateInitialized = true;
    ChipLogError(Zcl, "xxxxx StartAliroTransports 2");
    CHIP_ERROR startErr = StartAliroTransports();
    if (startErr != CHIP_NO_ERROR)
    {
        ChipLogError(Zcl, "Aliro reader configured, transport start failed: %" CHIP_ERROR_FORMAT, startErr.Format());
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR AliroDelegate::ClearAliroReaderConfig()
{
#if CONFIG_ALIRO_TRANSPORT_BLE
    if (mAliroBleStarted)
    {
        int err = telink_aliro_ble_stop();
        VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
            ChipLogError(Zcl, "Unable to stop Aliro BLE advertising: %d", err));
        mAliroBleStarted = false;
    }
#endif

    int err = telink_aliro_clear_reader_config();
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
        ChipLogError(Zcl, "Unable to clear Aliro reader configuration: %d", err));

#if CONFIG_ALIRO_TRANSPORT_BLE
    err = telink_aliro_ble_set_group_resolving_key(nullptr, 0);
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
        ChipLogError(Zcl, "Unable to clear Aliro BLE group resolving key: %d", err));
#endif

    ReturnErrorOnFailure(AliroStorage::Remove(AliroStorage::kReaderConfigKey));

    memset(mAliroReaderVerificationKey, 0, sizeof(mAliroReaderVerificationKey));
    memset(mAliroReaderGroupIdentifier, 0, sizeof(mAliroReaderGroupIdentifier));
    memset(mAliroReaderGroupSubIdentifier, 0, sizeof(mAliroReaderGroupSubIdentifier));
    memset(mAliroGroupResolvingKey, 0, sizeof(mAliroGroupResolvingKey));
    mAliroStateInitialized = false;
    mAliroHasGroupResolvingKey = false;
    mAliroNfcStarted = false;
    return CHIP_NO_ERROR;
}

// ---------------------------------------------------------------------------
// Aliro credential storage
// ---------------------------------------------------------------------------

/* static */ bool AliroDelegate::IsAliroCredentialType(CredentialTypeEnum type)
{
    switch (type)
    {
    case CredentialTypeEnum::kAliroCredentialIssuerKey:
    case CredentialTypeEnum::kAliroEvictableEndpointKey:
    case CredentialTypeEnum::kAliroNonEvictableEndpointKey:
        return true;
    default:
        return false;
    }
}

/* static */ bool AliroDelegate::IsEndpointCredentialType(CredentialTypeEnum type)
{
    return type == CredentialTypeEnum::kAliroEvictableEndpointKey ||
        type == CredentialTypeEnum::kAliroNonEvictableEndpointKey;
}

AliroDelegate::CredentialSlot * AliroDelegate::FindSlot(uint16_t index, CredentialTypeEnum type)
{
    if (type == CredentialTypeEnum::kAliroCredentialIssuerKey)
    {
        return index > 0 && index <= APP_MAX_ALIRO_ISSUER_KEYS ? &mIssuerKeys[index - 1] : nullptr;
    }

    VerifyOrReturnValue(IsEndpointCredentialType(type), nullptr);
    for (auto & slot : mEndpointKeys)
    {
        if (slot.status != DlCredentialStatus::kAvailable && slot.type == type && slot.index == index)
        {
            return &slot;
        }
    }

    return nullptr;
}

const AliroDelegate::CredentialSlot * AliroDelegate::FindSlot(uint16_t index, CredentialTypeEnum type) const
{
    return const_cast<AliroDelegate *>(this)->FindSlot(index, type);
}

AliroDelegate::CredentialSlot * AliroDelegate::FindAvailableEndpointSlot()
{
    for (auto & slot : mEndpointKeys)
    {
        if (slot.status == DlCredentialStatus::kAvailable)
        {
            return &slot;
        }
    }

    return nullptr;
}

size_t AliroDelegate::SlotCountForType(CredentialTypeEnum type) const
{
    return type == CredentialTypeEnum::kAliroCredentialIssuerKey ? APP_MAX_ALIRO_ISSUER_KEYS
        : (type == CredentialTypeEnum::kAliroEvictableEndpointKey || type == CredentialTypeEnum::kAliroNonEvictableEndpointKey)
        ? APP_MAX_ALIRO_ENDPOINT_KEYS
        : 0;
}

bool AliroDelegate::GetCredential(uint16_t index, CredentialTypeEnum type, EmberAfPluginDoorLockCredentialInfo & out)
{
    VerifyOrReturnValue(index > 0 && index <= SlotCountForType(type), false);

    const CredentialSlot * slot = FindSlot(index, type);

    out.status             = slot != nullptr ? slot->status : DlCredentialStatus::kAvailable;
    out.credentialType     = type;
    out.createdBy          = slot != nullptr ? slot->createdBy : 0;
    out.lastModifiedBy     = slot != nullptr ? slot->lastModifiedBy : 0;
    out.creationSource     = DlAssetSource::kMatterIM;
    out.modificationSource = DlAssetSource::kMatterIM;
    out.credentialData     = slot != nullptr ? chip::ByteSpan{ slot->data, slot->dataSize } : chip::ByteSpan{};

    ChipLogProgress(Zcl, "AliroDelegate::GetCredential [type=%u,index=%u,status=%d]", to_underlying(type), index,
        static_cast<int>(out.status));

    return true;
}

bool AliroDelegate::SetCredential(uint16_t index, chip::FabricIndex creator, chip::FabricIndex modifier, DlCredentialStatus status,
                                  CredentialTypeEnum type, const chip::ByteSpan & data)
{
    VerifyOrReturnValue(index > 0 && index <= SlotCountForType(type), false);
    CredentialSlot * slot = FindSlot(index, type);

    char key[24];
    CredentialStorageKey(key, sizeof(key), type, index);

    if (status == DlCredentialStatus::kAvailable)
    {
        CHIP_ERROR err = AliroStorage::Remove(key);
        VerifyOrReturnValue(err == CHIP_NO_ERROR, false,
            ChipLogError(Zcl, "Failed to delete Aliro credential: %" CHIP_ERROR_FORMAT, err.Format()));

        if (slot != nullptr)
        {
            memset(slot->data, 0, sizeof(slot->data));
            *slot = CredentialSlot{};
        }
        return true;
    }

    VerifyOrReturnValue(data.size() <= kAliroCredentialMaxSize, false);
    static_assert(sizeof(AliroStorage::Credential{}.data) == kAliroCredentialMaxSize);

    if (slot == nullptr && IsEndpointCredentialType(type))
    {
        slot = FindAvailableEndpointSlot();
    }
    VerifyOrReturnValue(slot != nullptr, false);

    AliroStorage::Credential record = {};
    record.version                  = AliroStorage::kRecordVersion;
    record.index                    = index;
    record.dataSize                 = static_cast<uint16_t>(data.size());
    record.type                     = to_underlying(type);
    record.status                   = to_underlying(status);
    record.createdBy                = creator;
    record.lastModifiedBy           = modifier;
    memcpy(record.data, data.data(), data.size());

    CHIP_ERROR err = AliroStorage::Save(key, &record, sizeof(record));
    VerifyOrReturnValue(err == CHIP_NO_ERROR, false,
        ChipLogError(Zcl, "Failed to save Aliro credential: %" CHIP_ERROR_FORMAT, err.Format()));

    memset(slot->data, 0, sizeof(slot->data));
    memcpy(slot->data, record.data, record.dataSize);
    slot->dataSize       = record.dataSize;
    slot->status         = status;
    slot->type           = type;
    slot->index          = index;
    slot->createdBy      = creator;
    slot->lastModifiedBy = modifier;

    ChipLogProgress(Zcl, "AliroDelegate::SetCredential [type=%u,index=%u,dataSize=%u]", to_underlying(type), index,
        static_cast<unsigned int>(data.size()));
    ChipLogError(Zcl, "xxxxx StartAliroTransports 3");
    CHIP_ERROR startErr = StartAliroTransports();
    if (startErr != CHIP_NO_ERROR)
    {
        ChipLogError(Zcl, "Aliro credential saved, transport start failed: %" CHIP_ERROR_FORMAT, startErr.Format());
    }

    return true;
}

bool AliroDelegate::FindEndpointKey(const chip::ByteSpan & key, CredentialTypeEnum & type, uint16_t & index) const
{
    VerifyOrReturnValue(key.size() == kAliroCredentialMaxSize, false);

    for (const auto & slot : mEndpointKeys)
    {
        if (slot.status != DlCredentialStatus::kAvailable && slot.dataSize == key.size() &&
            memcmp(slot.data, key.data(), key.size()) == 0)
        {
            type  = slot.type;
            index = slot.index;
            return true;
        }
    }

    return false;
}

bool AliroDelegate::HasEndpointKey(CredentialTypeEnum type, uint16_t index) const
{
    if (!IsEndpointCredentialType(type))
    {
        return false;
    }

    const CredentialSlot * slot = FindSlot(index, type);
    return slot != nullptr && slot->status != DlCredentialStatus::kAvailable && slot->dataSize == kAliroCredentialMaxSize;
}
