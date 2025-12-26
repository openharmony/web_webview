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
#include "ani_webview_blankless_callback.h"

#include "ani_business_error.h"
#include "ani_class_name.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS::NWeb {
using namespace NWebError;
namespace {
constexpr int INTEGER_ONE = 1;
}

AniWebviewBlanklessCallback::AniWebviewBlanklessCallback(ani_env* env, ani_ref callback)
{
    WVLOG_D("AniWebviewBlanklessCallback::AniWebviewBlanklessCallback");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
    if (callback) {
        env->GlobalReference_Create(callback, &callbackRef_);
    }
}

AniWebviewBlanklessCallback::~AniWebviewBlanklessCallback()
{
    WVLOG_D("AniWebviewBlanklessCallback::~AniWebviewBlanklessCallback");
    ani_env* env = GetEnv();
    if (env && callbackRef_) {
        env->GlobalReference_Delete(callbackRef_);
    } else {
        WVLOG_E("global ref not delete");
    }
}

void AniWebviewBlanklessCallback::OnReceiveValue(const std::string& key, int32_t state,
    int64_t timestamp, const std::string& reason)
{
    if (!vm_ || !callbackRef_) {
        return;
    }

    ani_env* env = GetEnv();

    ani_object blanklessLoadingInfo  = {};
    if (!AniParseUtils::CreateObjectVoid(env, ANI_CLASS_BLANKLESS_LOADING_INFO_INNER, blanklessLoadingInfo)) {
        WVLOG_E("CreateBlanklessLoadingInfo FAILED");
        return;
    }
    ani_string aniKey;
    if (env->String_NewUTF8(key.c_str(), key.size(), &aniKey) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(blanklessLoadingInfo, "key", aniKey);
    }
    env->Object_SetPropertyByName_Long(blanklessLoadingInfo, "timestamp", static_cast<ani_long>(timestamp));
    ani_enum_item aniState;
    if (AniParseUtils::GetEnumItemByIndex(env, ANI_ENUM_BLANKLESS_LOADING_STATE, std::abs(state), aniState)) {
        env->Object_SetPropertyByName_Ref(blanklessLoadingInfo, "state", aniState);
    }
    
    ani_string aniReason;
    if (env->String_NewUTF8(reason.c_str(), reason.size(), &aniReason) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(blanklessLoadingInfo, "reason", aniReason);
    }

    ani_fn_object callbackFn = static_cast<ani_fn_object>(callbackRef_);
    ani_ref fnResult {};
    ani_ref argv[INTEGER_ONE] = { static_cast<ani_ref>(blanklessLoadingInfo) };
    if (env->FunctionalObject_Call(callbackFn, ani_size(INTEGER_ONE), argv, &fnResult) != ANI_OK) {
        WVLOG_E("AniWebviewBlanklessCallback::OnReceiveValue executes callback failed");
    }
}

} // namespace OHOS::NWeb
