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
#include <app/server/Server.h>
#include <errno.h>
#include <zephyr/sys/atomic.h>

#if CONFIG_ALIRO_TRANSPORT_BLE
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
constexpr atomic_val_t kNoPendingAliroAction = -1;
atomic_t sPendingAliroState                 = kNoPendingAliroAction;

void ApplyAliroLockState(intptr_t argument)
{
    const auto state = static_cast<telink_aliro_lock_state>(argument);
    LockManager::Action_t action;

    switch (state)
    {
    case TELINK_ALIRO_LOCK_STATE_SECURED:
        action = LockManager::LOCK_ACTION;
        break;
    case TELINK_ALIRO_LOCK_STATE_UNSECURED:
        action = LockManager::UNLOCK_ACTION;
        break;
    default:
        LOG_ERR("Invalid scheduled Aliro lock state: %u", static_cast<unsigned>(state));
        atomic_set(&sPendingAliroState, kNoPendingAliroAction);
        return;
    }

    LOG_INF("Applying authenticated Aliro RKE action on Matter thread: %s",
            state == TELINK_ALIRO_LOCK_STATE_SECURED ? "SECURED" : "UNSECURED");

    if (!LockMgr().LockAction(AppEvent::kEventType_DeviceAction, action, LockManager::OperationSource::kAliro,
                              kExampleEndpointId))
    {
        LOG_ERR("Aliro lock action failed");
    }

    atomic_set(&sPendingAliroState, kNoPendingAliroAction);
}

#if CONFIG_ALIRO_TRANSPORT_BLE

uint8_t sAliroServiceData[26];
constexpr uint8_t kAliroAdvertisingFlags[] = { BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR };

#if CONFIG_BT_EXT_ADV

struct bt_le_ext_adv * sAliroExtAdvertising;

#else

const bt_data kAliroAdvertisingData[] = {
    BT_DATA(BT_DATA_FLAGS, kAliroAdvertisingFlags, sizeof(kAliroAdvertisingFlags)),
    BT_DATA(BT_DATA_SVC_DATA16, sAliroServiceData, sizeof(sAliroServiceData)),
};

const bt_data kAliroScanResponseData[] = {
    BT_DATA(BT_DATA_UUID16_SOME, sAliroServiceData, 2),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static_assert(4 + 2 + sizeof(CONFIG_BT_DEVICE_NAME) - 1 <= 31, "Aliro scan response exceeds the 31-byte limit");

BLEAdvertisingArbiter::Request sAliroAdvertisingRequest = {};

#endif

int StartAliroAdvertising(uint8_t identity, const uint8_t * serviceData, size_t size)
{
    if (serviceData == nullptr || size != sizeof(sAliroServiceData))
    {
        return -EINVAL;
    }

    memcpy(sAliroServiceData, serviceData, size);

#if CONFIG_BT_EXT_ADV

    const bt_data advertisingData[] = {
        BT_DATA(BT_DATA_FLAGS, kAliroAdvertisingFlags, sizeof(kAliroAdvertisingFlags)),
        BT_DATA(BT_DATA_SVC_DATA16, sAliroServiceData, sizeof(sAliroServiceData)),
        BT_DATA(BT_DATA_UUID16_SOME, sAliroServiceData, 2),
        BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    };

    bt_le_adv_param advertisingParameters = {};

    advertisingParameters.id           = identity;
    advertisingParameters.sid          = 0;
    advertisingParameters.options      = BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY | BT_LE_ADV_OPT_EXT_ADV;
    advertisingParameters.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    advertisingParameters.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;

    int err = bt_le_ext_adv_create(&advertisingParameters, nullptr, &sAliroExtAdvertising);
    if (err != 0)
    {
        return err;
    }

    err = bt_le_ext_adv_set_data(sAliroExtAdvertising, advertisingData, ARRAY_SIZE(advertisingData), nullptr, 0);
    if (err != 0)
    {
        (void) bt_le_ext_adv_delete(sAliroExtAdvertising);
        sAliroExtAdvertising = nullptr;
        return err;
    }

    bt_le_ext_adv_start_param startParameters = {};

    err = bt_le_ext_adv_start(sAliroExtAdvertising, &startParameters);
    if (err != 0)
    {
        (void) bt_le_ext_adv_delete(sAliroExtAdvertising);
        sAliroExtAdvertising = nullptr;
        return err;
    }

    LOG_INF("Aliro BLE advertising started");

    return 0;

#else

    sAliroAdvertisingRequest.priority         = UINT8_MAX;
    sAliroAdvertisingRequest.options          = BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY;
    sAliroAdvertisingRequest.minInterval      = BT_GAP_ADV_FAST_INT_MIN_2;
    sAliroAdvertisingRequest.maxInterval      = BT_GAP_ADV_FAST_INT_MAX_2;
    sAliroAdvertisingRequest.advertisingData  = Span<const bt_data>(kAliroAdvertisingData);
    sAliroAdvertisingRequest.scanResponseData = Span<const bt_data>(kAliroScanResponseData);
    sAliroAdvertisingRequest.identity         = identity;
    sAliroAdvertisingRequest.useIdentity      = true;
    sAliroAdvertisingRequest.onStarted        = [](int result) {
        if (result == 0)
        {
            LOG_INF("Aliro BLE advertising started");
        }
        else if (result != -ENOMEM)
        {
            LOG_ERR("Aliro BLE advertising request failed: %d", result);
        }
    };

    CHIP_ERROR err = BLEAdvertisingArbiter::InsertRequest(sAliroAdvertisingRequest);

    if (err == CHIP_NO_ERROR || err == System::MapErrorZephyr(-ENOMEM))
    {
        return 0;
    }

    BLEAdvertisingArbiter::CancelRequest(sAliroAdvertisingRequest);

    return -EIO;

#endif
}

int StopAliroAdvertising()
{
#if CONFIG_BT_EXT_ADV

    if (sAliroExtAdvertising == nullptr)
    {
        return 0;
    }

    int err = bt_le_ext_adv_stop(sAliroExtAdvertising);
    if (err != 0)
    {
        return err;
    }

    err = bt_le_ext_adv_delete(sAliroExtAdvertising);
    if (err != 0)
    {
        return err;
    }

    sAliroExtAdvertising = nullptr;

    return 0;

#else
    BLEAdvertisingArbiter::CancelRequest(sAliroAdvertisingRequest);

    return 0;
#endif
}

#endif /* CONFIG_ALIRO_TRANSPORT_BLE */

CHIP_ERROR StartAliro()
{
#if CONFIG_ALIRO_TRANSPORT_NFC
    int err = telink_aliro_nfc_start();
    if (err != 0 && err != -EALREADY)
    {
        LOG_ERR("Aliro NFC start failed: %d", err);
        return CHIP_ERROR_INTERNAL;
    }
#endif

#if CONFIG_ALIRO_TRANSPORT_BLE

#if defined(CONFIG_ALIRO_CSA_TEST_CREDENTIALS)
    ReturnErrorOnFailure(AliroDelegate::GetInstance().InitializeCsaTestCredentials());
#endif

    if (telink_aliro_ble_start() != 0)
    {
        LOG_ERR("Aliro BLE start failed");
        return CHIP_ERROR_INTERNAL;
    }
#endif

    return CHIP_NO_ERROR;
}

void MatterDeviceEventHandler(const ChipDeviceEvent * event, intptr_t arg)
{
    (void) arg;

    if (event->Type != DeviceEventType::kCommissioningComplete)
    {
        return;
    }

    CHIP_ERROR err = StartAliro();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Failed to start Aliro: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

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

#if CONFIG_ALIRO_TRANSPORT_BLE
    const telink_aliro_ble_advertising_callbacks advertisingCallbacks = { StartAliroAdvertising, StopAliroAdvertising };

    if (telink_aliro_ble_init(&advertisingCallbacks) != 0)
    {
        return CHIP_ERROR_INTERNAL;
    }
#endif

    ReturnErrorOnFailure(PlatformMgr().AddEventHandler(MatterDeviceEventHandler, 0));

    if (Server::GetInstance().GetFabricTable().FabricCount())
    {
        /* Device is already commissioned, starting Aliro */
        ReturnErrorOnFailure(StartAliro());
    }

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

    const CHIP_ERROR err = chip::DeviceLayer::PlatformMgr().ScheduleWork(ApplyAliroLockState, static_cast<intptr_t>(state));
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Failed to schedule Aliro lock action: %" CHIP_ERROR_FORMAT, err.Format());
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

#if defined(CONFIG_ALIRO_CSA_TEST_CREDENTIALS)
    if (AliroDelegate::GetInstance().IsCsaTestEndpointKey(chip::ByteSpan(publicKey, publicKeySize)))
    {
        LOG_WRN("CSA test endpoint accepted by test-credential authorization bypass");
        return 0;
    }
#endif

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

    ApplyAliroLockState(event->DeviceEvent.Action);
}

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
