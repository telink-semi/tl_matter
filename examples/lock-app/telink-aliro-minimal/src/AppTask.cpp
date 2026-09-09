/*
 *
 *    Copyright (c) 2023-2024 Project CHIP Authors
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

#include "AppTask.h"
#include "AliroDelegate.h"
#include "ButtonManager.h"
#include "LEDManager.h"
#include <LockManager.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/data-model/Nullable.h>
#include <errno.h>
#include <zephyr/sys/atomic.h>
#if CONFIG_ALIRO_TRANSPORT_BLE && !CONFIG_BT_EXT_ADV
#include <platform/Zephyr/BLEAdvertisingArbiter.h>
#include <system/SystemError.h>
#endif

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace ::chip::app::Clusters::DoorLock;
using namespace chip;
using namespace chip::app;
using namespace ::chip::DeviceLayer;
using namespace ::chip::DeviceLayer::Internal;

AppTask AppTask::sAppTask;

namespace {
// Publish BUSY before queuing an Aliro action so the reader cannot acknowledge
// the previous final state while the application thread has not run yet.
constexpr atomic_val_t kNoPendingAliroAction = -1;
atomic_t sPendingAliroState                 = kNoPendingAliroAction;

#if CONFIG_ALIRO_TRANSPORT_BLE && !CONFIG_BT_EXT_ADV
uint8_t sAliroServiceData[26];
constexpr uint8_t kAliroAdvertisingFlags[] = { BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };
const bt_data kAliroAdvertisingData[] = {
    BT_DATA(BT_DATA_FLAGS, kAliroAdvertisingFlags, sizeof(kAliroAdvertisingFlags)),
    BT_DATA(BT_DATA_SVC_DATA16, sAliroServiceData, sizeof(sAliroServiceData)),
};
BLEAdvertisingArbiter::Request sAliroAdvertisingRequest = {};

// The SDK invokes these callbacks from Set/ClearAliroReaderConfig, on the
// Matter thread. The arbiter owns restarts and gives commissioning priority.
int StartAliroAdvertising(uint8_t identity, const uint8_t * serviceData, size_t size)
{
    if (serviceData == nullptr || size != sizeof(sAliroServiceData))
    {
        return -EINVAL;
    }
    memcpy(sAliroServiceData, serviceData, size);
    sAliroAdvertisingRequest.priority        = UINT8_MAX;
    sAliroAdvertisingRequest.options         = BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY;
    sAliroAdvertisingRequest.minInterval     = BT_GAP_ADV_FAST_INT_MIN_2;
    sAliroAdvertisingRequest.maxInterval     = BT_GAP_ADV_FAST_INT_MAX_2;
    sAliroAdvertisingRequest.advertisingData = Span<const bt_data>(kAliroAdvertisingData);
    sAliroAdvertisingRequest.identity        = identity;
    sAliroAdvertisingRequest.useIdentity     = true;
    sAliroAdvertisingRequest.onStarted      = [](int result) {
        if (result == 0)
        {
            LOG_INF("Aliro legacy BLE advertising started");
        }
        else if (result != -ENOMEM)
        {
            LOG_ERR("Aliro legacy BLE advertising failed: %d", result);
        }
    };
    CHIP_ERROR err = BLEAdvertisingArbiter::InsertRequest(sAliroAdvertisingRequest);
    // The arbiter retains the request and retries after the commissioning
    // connection is released, even when the single connection slot is busy.
    if (err == CHIP_NO_ERROR || err == System::MapErrorZephyr(-ENOMEM))
    {
        return 0;
    }
    BLEAdvertisingArbiter::CancelRequest(sAliroAdvertisingRequest);
    return -EIO;
}

int StopAliroAdvertising()
{
    BLEAdvertisingArbiter::CancelRequest(sAliroAdvertisingRequest);
    return 0;
}
#endif
} // namespace

CHIP_ERROR AppTask::Init(void)
{
    SetExampleButtonCallbacks(LockActionEventHandler);
    ReturnErrorOnFailure(InitCommonParts());

    LedManager::getInstance().setLed(LedManager::EAppLed_App0, LockMgr().IsLocked());

    chip::app::DataModel::Nullable<chip::app::Clusters::DoorLock::DlLockState> state;
    chip::EndpointId endpointId{ kExampleEndpointId };
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    chip::app::Clusters::DoorLock::Attributes::LockState::Get(endpointId, state);

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    CHIP_ERROR err = LockMgr().Init(state, LockStateChanged);

    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("LockMgr().Init() failed");
        return err;
    }

    // Register a Door Lock delegate to handle Aliro provisioning attributes/commands.
    ReturnErrorOnFailure(DoorLockServer::Instance().SetDelegate(kExampleEndpointId, &AliroDelegate::GetInstance()));

    telink_aliro_callbacks aliroCallbacks = {};
    aliroCallbacks.get_lock_state             = GetAliroLockState;
    aliroCallbacks.request_lock_state         = RequestAliroLockState;
    aliroCallbacks.authorize_endpoint         = AuthorizeAliroEndpoint;

    if (telink_aliro_init(&aliroCallbacks) != 0)
    {
        LOG_ERR("Aliro reader initialization failed");
        return CHIP_ERROR_INTERNAL;
    }

#if CONFIG_ALIRO_TRANSPORT_NFC
    if (telink_aliro_nfc_start() != 0)
    {
        LOG_ERR("Aliro NFC initialization failed");
        return CHIP_ERROR_INTERNAL;
    }
#endif

#if CONFIG_ALIRO_TRANSPORT_BLE
#if !CONFIG_BT_EXT_ADV
    const telink_aliro_ble_advertising_callbacks advertisingCallbacks = { StartAliroAdvertising, StopAliroAdvertising };
    if (telink_aliro_ble_set_advertising_callbacks(&advertisingCallbacks) != 0)
    {
        return CHIP_ERROR_INTERNAL;
    }
#endif
    if (telink_aliro_ble_init() != 0)
    {
        LOG_ERR("Aliro BLE initialization failed");
        return CHIP_ERROR_INTERNAL;
    }
#endif

    return CHIP_NO_ERROR;
}

int AppTask::GetAliroLockState(enum telink_aliro_lock_state * state, void * context)
{
    (void) context;

    if (state == nullptr)
    {
        return -EINVAL;
    }

    const atomic_val_t pending = atomic_get(&sPendingAliroState);
    if (pending != kNoPendingAliroAction)
    {
        *state = static_cast<telink_aliro_lock_state>(pending);
        return 0;
    }

    switch (LockMgr().getLockState())
    {
    case LockManager::kState_LockCompleted:
        *state = TELINK_ALIRO_LOCK_STATE_SECURED;
        break;
    case LockManager::kState_UnlockCompleted:
        *state = TELINK_ALIRO_LOCK_STATE_UNSECURED;
        break;
    case LockManager::kState_LockInitiated:
        *state = TELINK_ALIRO_LOCK_STATE_BUSY_SECURED;
        break;
    case LockManager::kState_UnlockInitiated:
        *state = TELINK_ALIRO_LOCK_STATE_BUSY_UNSECURED;
        break;
    default:
        *state = TELINK_ALIRO_LOCK_STATE_JAMMED;
        break;
    }

    return 0;
}

int AppTask::RequestAliroLockState(enum telink_aliro_lock_state state, void * context)
{
    (void) context;

    if (state != TELINK_ALIRO_LOCK_STATE_SECURED && state != TELINK_ALIRO_LOCK_STATE_UNSECURED)
    {
        return -ENOTSUP;
    }

    const atomic_val_t busy = state == TELINK_ALIRO_LOCK_STATE_SECURED ? TELINK_ALIRO_LOCK_STATE_BUSY_SECURED
                                                                    : TELINK_ALIRO_LOCK_STATE_BUSY_UNSECURED;
    if (!atomic_cas(&sPendingAliroState, kNoPendingAliroAction, busy))
    {
        return -EBUSY;
    }

    AppEvent event           = {};
    event.Type               = AppEvent::kEventType_DeviceAction;
    event.DeviceEvent.Action = static_cast<uint8_t>(state);
    event.Handler            = AliroLockActionEventHandler;
    if (!GetAppTask().PostEvent(&event))
    {
        atomic_set(&sPendingAliroState, kNoPendingAliroAction);
        return -ENOBUFS;
    }
    return 0;
}

int AppTask::AuthorizeAliroEndpoint(const uint8_t * publicKey, size_t publicKeySize, void * context)
{
    (void) context;

    if (publicKey == nullptr)
    {
        return -EINVAL;
    }

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    const bool authorized = LockMgr().ValidateAliroEndpointKey(chip::ByteSpan(publicKey, publicKeySize));
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    ChipLogProgress(Zcl, "[Aliro] Authenticated endpoint %s", authorized ? "accepted" : "rejected");
    return authorized ? 0 : -EACCES;
}

void AppTask::AliroLockActionEventHandler(AppEvent * event)
{
    if (event == nullptr)
    {
        return;
    }

    LockManager::Action_t action;
    switch (event->DeviceEvent.Action)
    {
    case TELINK_ALIRO_LOCK_STATE_SECURED:
        action = LockManager::LOCK_ACTION;
        break;
    case TELINK_ALIRO_LOCK_STATE_UNSECURED:
        action = LockManager::UNLOCK_ACTION;
        break;
    default:
        return;
    }

    if (!LockMgr().LockAction(AppEvent::kEventType_DeviceAction, action, LockManager::OperationSource::kAliro, kExampleEndpointId))
    {
        LOG_ERR("Aliro lock action failed");
    }
    // LockManager now exposes the initiated/completed state, or the existing
    // state if it rejected the request. Release the temporary queue state.
    atomic_set(&sPendingAliroState, kNoPendingAliroAction);
}

/* This is a button handler only */
void AppTask::LockActionEventHandler(AppEvent * aEvent)
{
    switch (LockMgr().getLockState())
    {
    case LockManager::kState_NotFulyLocked:
    case LockManager::kState_LockCompleted:
        LockMgr().LockAction(AppEvent::kEventType_DeviceAction, LockManager::UNLOCK_ACTION, LockManager::OperationSource::kButton,
                             kExampleEndpointId);
        break;
    case LockManager::kState_UnlockCompleted:
        LockMgr().LockAction(AppEvent::kEventType_DeviceAction, LockManager::LOCK_ACTION, LockManager::OperationSource::kButton,
                             kExampleEndpointId);
        break;
    default:
        LOG_INF("Lock is in intermediate state, ignoring button");
        break;
    }
}

void AppTask::LockStateChanged(LockManager::State_t state)
{
    switch (state)
    {
    case LockManager::State_t::kState_LockInitiated:
        LOG_INF("Callback: Lock action initiated");
        LedManager::getInstance().setLed(LedManager::EAppLed_App0, 50, 50);
        break;
    case LockManager::State_t::kState_LockCompleted:
        LOG_INF("Callback: Lock action completed");
        LedManager::getInstance().setLed(LedManager::EAppLed_App0, true);
        break;
    case LockManager::State_t::kState_UnlockInitiated:
        LOG_INF("Callback: Unlock action initiated");
        LedManager::getInstance().setLed(LedManager::EAppLed_App0, 50, 50);
        break;
    case LockManager::State_t::kState_UnlockCompleted:
        LOG_INF("Callback: Unlock action completed");
        LedManager::getInstance().setLed(LedManager::EAppLed_App0, false);
        break;
    case LockManager::State_t::kState_NotFulyLocked:
        LOG_INF("Callback: Lock not fully locked. Unexpected state");
        LedManager::getInstance().setLed(LedManager::EAppLed_App0, 10, 90);
        break;
    }
}

void AppTask::LinkButtons(ButtonManager & buttonManager)
{
    buttonManager.addCallback(FactoryResetButtonEventHandler, 0, true);
    buttonManager.addCallback(ExampleActionButtonEventHandler, 1, true);
}

void AppTask::LinkLeds(LedManager & ledManager)
{
#if CONFIG_CHIP_ENABLE_APPLICATION_STATUS_LED
    ledManager.linkLed(LedManager::EAppLed_Status, 0);
    ledManager.linkLed(LedManager::EAppLed_App0, 1);
#else
    ledManager.linkLed(LedManager::EAppLed_App0, 0);
#endif // CONFIG_CHIP_ENABLE_APPLICATION_STATUS_LED
}
