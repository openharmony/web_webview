/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "verifypackageinstall_fuzzer.h"

#include <securec.h>
#define private public
#define protected public
#include "app_fwk_update_service.h"
#undef protected
#undef private
#include <fuzzer/FuzzedDataProvider.h>

#include "ipc_skeleton.h"
#include "system_ability_ondemand_reason.h"
namespace OHOS {
const int FOUNDATION_UID = 5523;
int g_callingUid = 0;
constexpr uint8_t MAX_STRING_LENGTH = 64;
constexpr int MAX_NUMBER = 1000;
int IPCSkeleton::GetCallingUid()
{
    return g_callingUid;
}

bool VerifyPackageInstallFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    int32_t saId = 123;
    bool runOnCreate = false;
    std::shared_ptr<NWeb::AppFwkUpdateService> appFwkUpdateService =
        std::make_shared<NWeb::AppFwkUpdateService>(saId, runOnCreate);
    if (appFwkUpdateService == nullptr) {
        return true;
    }
    const std::string bundleName((const char*)data, size);
    const std::string hapPath((const char*)data, size);
    int32_t isSuccess = 0;
    g_callingUid = FOUNDATION_UID + 1;
    appFwkUpdateService->VerifyPackageInstall(bundleName, hapPath, isSuccess);
    appFwkUpdateService->NotifyFWKAfterBmsStart();
    appFwkUpdateService->NotifyArkWebInstallSuccess();
    appFwkUpdateService->OnStop();
    appFwkUpdateService->PostDelayUnloadTask();

    FuzzedDataProvider dataProvider(data, size);
    int systemAbilityId = dataProvider.ConsumeIntegralInRange<int>(0, MAX_NUMBER);
    std::string deviceId = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    appFwkUpdateService->OnAddSystemAbility(systemAbilityId, deviceId);
    appFwkUpdateService->OnRemoveSystemAbility(systemAbilityId, deviceId);
    appFwkUpdateService->SubscribePackageChangedEvent();
    std::string bundleNames = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string hapPaths = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    appFwkUpdateService->OnPackageChangedEvent(bundleNames, hapPaths);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::VerifyPackageInstallFuzzTest(data, size);
    return 0;
}