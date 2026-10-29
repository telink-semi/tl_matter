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

#pragma once

#include <lib/core/CHIPError.h>
#include <platform/KeyValueStoreManager.h>
#include <stddef.h>
#include <stdint.h>

namespace AliroStorage {

constexpr uint32_t kRecordVersion = 1;
constexpr size_t kSigningKeySize = 32;
constexpr char kReaderConfigKey[] = "aliro/reader";

struct ReaderConfig
{
    uint32_t version;
    uint8_t signingKey[kSigningKeySize];
    uint8_t verificationKey[65];
    uint8_t groupIdentifier[16];
    uint8_t groupSubIdentifier[16];
    uint8_t groupResolvingKey[16];
    uint8_t hasGroupResolvingKey;
};

struct Credential
{
    uint32_t version;
    uint16_t index;
    uint16_t dataSize;
    uint8_t type;
    uint8_t status;
    uint8_t createdBy;
    uint8_t lastModifiedBy;
    uint8_t data[65];
};

inline CHIP_ERROR Load(const char * key, void * data, size_t size)
{
    size_t outSize = 0;
    CHIP_ERROR err = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Get(key, data, size, &outSize);

    if (err == CHIP_NO_ERROR && outSize != size)
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    return err;
}

inline CHIP_ERROR Save(const char * key, const void * data, size_t size)
{
    return chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(key, data, size);
}

inline CHIP_ERROR Remove(const char * key)
{
    CHIP_ERROR err = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Delete(key);

    return err == CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND ? CHIP_NO_ERROR : err;
}

} // namespace AliroStorage
