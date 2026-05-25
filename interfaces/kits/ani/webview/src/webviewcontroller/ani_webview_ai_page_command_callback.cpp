/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ani_webview_ai_page_command_callback.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
namespace {
const std::string TASK_ID = "ai_page_command";
}

AniWebviewAIPageCommandCallback::AniWebviewAIPageCommandCallback(ani_env* env, ani_resolver resolver)
    : resolver_(resolver)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
}

void AniWebviewAIPageCommandCallback::OnReceiveValue(const std::string& result)
{
    auto sharedThis = shared_from_this();
    if (!mainHandler_) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }

    auto task = [sharedThis, result]() { sharedThis->ResolvePromise(result); };
    if (!mainHandler_->PostTask(std::move(task), TASK_ID, 0, AppExecFwk::EventQueue::Priority::HIGH, {})) {
        WVLOG_E("PostTask failed");
    }
}

void AniWebviewAIPageCommandCallback::ResolvePromise(const std::string& result)
{
    if (vm_ == nullptr || resolver_ == nullptr) {
        WVLOG_E("vm or resolver is nullptr");
        return;
    }

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK || env == nullptr) {
        WVLOG_E("GetEnv failed");
        return;
    }

    ani_string stringValue = nullptr;
    if (env->String_NewUTF8(result.c_str(), result.size(), &stringValue) != ANI_OK) {
        WVLOG_E("String_NewUTF8 failed");
        return;
    }
    ani_ref value = static_cast<ani_ref>(stringValue);

    if (env->PromiseResolver_Resolve(resolver_, value) != ANI_OK) {
        WVLOG_E("PromiseResolver_Resolve failed");
    }
}

} // namespace OHOS::NWeb
