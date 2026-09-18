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

#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPEncoding.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>
#include <telink_aliro/telink_aliro.h>

using namespace chip;
using namespace chip::app::Clusters::DoorLock;

AliroDelegate AliroDelegate::sInstance;

#if defined(CONFIG_ALIRO_CSA_TEST_CREDENTIALS)
namespace {

// Public demo credentials used by the CSA Aliro Test Tool example project.
// These values are deliberately fixed and are not production credentials.
constexpr uint8_t kCsaReaderSigningKey[] = {
    0x8a, 0xef, 0xdf, 0xf8, 0xd5, 0xb4, 0x7a, 0xa9, 0xa3, 0xed, 0xba, 0xc7, 0xa3, 0x45, 0xed, 0x22,
    0x21, 0x02, 0x15, 0x12, 0xfd, 0x55, 0xab, 0xde, 0x3b, 0x8e, 0xe0, 0xf2, 0x08, 0x95, 0x26, 0x93,
};

constexpr uint8_t kCsaReaderVerificationKey[] = {
    0x04, 0x39, 0x28, 0xf3, 0x22, 0x01, 0x9d, 0x47, 0x57, 0x89, 0x3b, 0xde, 0x6a, 0x0f, 0xe5, 0xe1, 0x3e,
    0x3e, 0x53, 0x7b, 0x9c, 0xa0, 0xf5, 0x49, 0xc0, 0xbd, 0x2f, 0x40, 0xf7, 0x90, 0x60, 0x25, 0x2a, 0x0a,
    0x4f, 0x29, 0x11, 0x92, 0x15, 0x7a, 0x95, 0xcb, 0x6e, 0xb2, 0x02, 0x75, 0x94, 0x28, 0xc0, 0x0c, 0xd8,
    0x34, 0x99, 0x8c, 0x5d, 0x0e, 0xab, 0x19, 0x2e, 0xe8, 0x87, 0x3c, 0x5d, 0x34, 0xee,
};

constexpr uint8_t kCsaReaderGroupIdentifier[] = {
    0x00, 0x11, 0x33, 0x44, 0x66, 0x77, 0x99, 0xaa, 0x00, 0x11, 0x33, 0x44, 0x66, 0x77, 0x99, 0xaa,
};

constexpr uint8_t kCsaReaderGroupSubIdentifier[] = {
    0x11, 0x33, 0x44, 0x66, 0x77, 0x99, 0xaa, 0x00, 0x11, 0x33, 0x44, 0x66, 0x77, 0x99, 0xaa, 0x00,
};

constexpr uint8_t kCsaEndpointVerificationKey[] = {
    0x04, 0x74, 0x2d, 0xf7, 0x36, 0xd0, 0xfc, 0x9b, 0xe9, 0x78, 0xc4, 0x5b, 0x00, 0xe8, 0xfd, 0xf7, 0xce,
    0xa6, 0x84, 0xea, 0x10, 0x5a, 0xe5, 0x74, 0xc1, 0x50, 0x5a, 0x2c, 0x24, 0xab, 0x61, 0x98, 0xe3, 0x12,
    0x5b, 0x7f, 0x1b, 0x7e, 0x1d, 0x13, 0x4c, 0x55, 0xec, 0xe6, 0x96, 0x81, 0xba, 0x8e, 0xcc, 0x18, 0xa3,
    0x83, 0x6d, 0xc5, 0x19, 0x9c, 0x75, 0x9f, 0x31, 0xe8, 0xcc, 0xf1, 0x7e, 0x3e, 0xfa,
};

static_assert(sizeof(kCsaReaderSigningKey) == 32);
static_assert(sizeof(kCsaReaderVerificationKey) == kAliroReaderVerificationKeySize);
static_assert(sizeof(kCsaReaderGroupIdentifier) == kAliroReaderGroupIdentifierSize);
static_assert(sizeof(kCsaReaderGroupSubIdentifier) == kAliroReaderGroupSubIdentifierSize);

} // namespace
#endif

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
    ChipLogProgress(Zcl, "[Aliro] Read BLEUWBProtocolVersion (index=%u)", static_cast<unsigned>(index));

    (void) index;
    (void) protocolVersion;
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
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

CHIP_ERROR AliroDelegate::SetAliroReaderConfig(const ByteSpan & signingKey, const ByteSpan & verificationKey,
                                               const ByteSpan & groupIdentifier, const Optional<ByteSpan> & groupResolvingKey)
{
    uint8_t groupSubIdentifier[sizeof(mAliroReaderGroupSubIdentifier)];
    ByteSpan effectiveSigningKey      = signingKey;
    ByteSpan effectiveVerificationKey = verificationKey;
    ByteSpan effectiveGroupIdentifier = groupIdentifier;
    Optional<ByteSpan> effectiveGroupResolvingKey = groupResolvingKey;

    VerifyOrReturnError(verificationKey.size() == sizeof(mAliroReaderVerificationKey), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(groupIdentifier.size() == sizeof(mAliroReaderGroupIdentifier), CHIP_ERROR_INVALID_ARGUMENT);

    if (groupResolvingKey.HasValue())
    {
        VerifyOrReturnError(groupResolvingKey.Value().size() == sizeof(mAliroGroupResolvingKey), CHIP_ERROR_INVALID_ARGUMENT);
    }

#if defined(CONFIG_ALIRO_CSA_TEST_CREDENTIALS)
    effectiveSigningKey      = ByteSpan(kCsaReaderSigningKey);
    effectiveVerificationKey = ByteSpan(kCsaReaderVerificationKey);
    effectiveGroupIdentifier = ByteSpan(kCsaReaderGroupIdentifier);
    effectiveGroupResolvingKey.ClearValue();
    memcpy(groupSubIdentifier, kCsaReaderGroupSubIdentifier, sizeof(groupSubIdentifier));
    ChipLogError(Zcl, "CSA TEST CREDENTIALS active: using fixed public demo credentials");
#else
    ReturnErrorOnFailure(Crypto::DRBG_get_bytes(groupSubIdentifier, sizeof(groupSubIdentifier)));
#endif

    int err = telink_aliro_set_reader_config(effectiveSigningKey.data(), effectiveSigningKey.size(),
                                             effectiveVerificationKey.data(), effectiveVerificationKey.size(),
                                             effectiveGroupIdentifier.data(), effectiveGroupIdentifier.size(), groupSubIdentifier,
                                             sizeof(groupSubIdentifier));
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
                        ChipLogError(Zcl, "Unable to apply Aliro reader configuration: %d", err));

#if CONFIG_ALIRO_TRANSPORT_BLE
    err = telink_aliro_ble_set_group_resolving_key(
        effectiveGroupResolvingKey.HasValue() ? effectiveGroupResolvingKey.Value().data() : nullptr,
        effectiveGroupResolvingKey.HasValue() ? effectiveGroupResolvingKey.Value().size() : 0);
    if (err != 0)
    {
        ChipLogError(Zcl, "Unable to apply Aliro BLE group resolving key: %d", err);
        (void) telink_aliro_clear_reader_config();
        return CHIP_ERROR_INTERNAL;
    }

    err = telink_aliro_ble_start();
    if (err != 0)
    {
        ChipLogError(Zcl, "Unable to start Aliro BLE advertising: %d", err);
        (void) telink_aliro_ble_set_group_resolving_key(nullptr, 0);
        (void) telink_aliro_clear_reader_config();
        return CHIP_ERROR_INTERNAL;
    }
#endif

    memcpy(mAliroReaderVerificationKey, effectiveVerificationKey.data(), sizeof(mAliroReaderVerificationKey));
    memcpy(mAliroReaderGroupIdentifier, effectiveGroupIdentifier.data(), sizeof(mAliroReaderGroupIdentifier));
    memcpy(mAliroReaderGroupSubIdentifier, groupSubIdentifier, sizeof(mAliroReaderGroupSubIdentifier));

    mAliroHasGroupResolvingKey = effectiveGroupResolvingKey.HasValue();
    if (mAliroHasGroupResolvingKey)
    {
        memcpy(mAliroGroupResolvingKey, effectiveGroupResolvingKey.Value().data(), sizeof(mAliroGroupResolvingKey));
    }
    else
    {
        memset(mAliroGroupResolvingKey, 0, sizeof(mAliroGroupResolvingKey));
    }

    mAliroStateInitialized = true;
    return CHIP_NO_ERROR;
}

#if defined(CONFIG_ALIRO_CSA_TEST_CREDENTIALS)
CHIP_ERROR AliroDelegate::InitializeCsaTestCredentials()
{
    return SetAliroReaderConfig(ByteSpan(kCsaReaderSigningKey), ByteSpan(kCsaReaderVerificationKey),
                                ByteSpan(kCsaReaderGroupIdentifier), NullOptional);
}

bool AliroDelegate::IsCsaTestEndpointKey(const ByteSpan & key) const
{
    return key.size() == sizeof(kCsaEndpointVerificationKey) &&
        memcmp(key.data(), kCsaEndpointVerificationKey, sizeof(kCsaEndpointVerificationKey)) == 0;
}
#endif

CHIP_ERROR AliroDelegate::ClearAliroReaderConfig()
{
    int err;

#if CONFIG_ALIRO_TRANSPORT_BLE
    err = telink_aliro_ble_stop();
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
                        ChipLogError(Zcl, "Unable to stop Aliro BLE advertising: %d", err));
#endif

    err = telink_aliro_clear_reader_config();
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
                        ChipLogError(Zcl, "Unable to clear Aliro reader configuration: %d", err));

#if CONFIG_ALIRO_TRANSPORT_BLE
    err = telink_aliro_ble_set_group_resolving_key(nullptr, 0);
    VerifyOrReturnError(err == 0, CHIP_ERROR_INTERNAL,
                        ChipLogError(Zcl, "Unable to clear Aliro BLE group resolving key: %d", err));
#endif

    memset(mAliroReaderVerificationKey, 0, sizeof(mAliroReaderVerificationKey));
    memset(mAliroReaderGroupIdentifier, 0, sizeof(mAliroReaderGroupIdentifier));
    memset(mAliroReaderGroupSubIdentifier, 0, sizeof(mAliroReaderGroupSubIdentifier));
    memset(mAliroGroupResolvingKey, 0, sizeof(mAliroGroupResolvingKey));
    mAliroStateInitialized = false;
    mAliroHasGroupResolvingKey = false;
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

    if (status == DlCredentialStatus::kAvailable)
    {
        if (slot != nullptr)
        {
            memset(slot->data, 0, sizeof(slot->data));
            *slot = CredentialSlot{};
        }
        return true;
    }

    VerifyOrReturnValue(data.size() <= kAliroCredentialMaxSize, false);

    if (slot == nullptr && IsEndpointCredentialType(type))
    {
        slot = FindAvailableEndpointSlot();
    }
    VerifyOrReturnValue(slot != nullptr, false);

    memset(slot->data, 0, sizeof(slot->data));
    memcpy(slot->data, data.data(), data.size());
    slot->dataSize       = data.size();
    slot->status         = status;
    slot->type           = type;
    slot->index          = index;
    slot->createdBy      = creator;
    slot->lastModifiedBy = modifier;

    ChipLogProgress(Zcl, "AliroDelegate::SetCredential [type=%u,index=%u,dataSize=%u]", to_underlying(type), index,
                    static_cast<unsigned int>(data.size()));

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
