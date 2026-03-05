/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_soc_perf_client_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkSocPerfClientAdapterWrapper::ArkSocPerfClientAdapterWrapper(ArkWebRefPtr<ArkSocPerfClientAdapter> ref) : ctocpp_(ref)
{}

void ArkSocPerfClientAdapterWrapper::ApplySocPerfConfigById(int32_t id)
{
    if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
        content::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
            base::BindOnce(&ArkSocPerfClientAdapterWrapper::ApplySocPerfConfigById,
                base::Unretained(this), id));
        return;
    }
    ctocpp_->ApplySocPerfConfigById(id);
}

void ArkSocPerfClientAdapterWrapper::ApplySocPerfConfigByIdEx(int32_t id, bool onOffTag)
{
    if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
        content::GetUIThreadTaskRunner()->PostTask(FROM_HERE,
            base::BindOnce(&ArkSocPerfClientAdapterWrapper::ApplySocPerfConfigByIdEx,
                base::Unretained(this), id, onOffTag));
        return;
    }
    ctocpp_->ApplySocPerfConfigByIdEx(id, onOffTag);
}

} // namespace OHOS::ArkWeb
