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
#include "ani_web_storage.h"

#include <array>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <securec.h>
#include <regex>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_helper.h"
#include "nweb_web_storage.h"
#include "application_context.h"
#include "ohos_resource_adapter_impl.h"
#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_createpdf_execute_callback.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"
#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"
#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
static const char* WEB_STORAGE_CLASS_NAME = "L@ohos/web/webview/webview/WebStorage;";
static const char* ANI_CLASS_WEB_STORAGE_ORIGIN_INNER = "L@ohos/web/webview/webview/WebStorageOriginInner;";
}

void DeleteAllData(ani_env *env, ani_object object, ani_object incognitoObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> webStorage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!webStorage) {
        WVLOG_E("webStorage is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if(env->Reference_IsUndefined(incognitoObj, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        webStorage->DeleteAllData(false);
    } else {
        ani_boolean incognito = ANI_FALSE;
        if (env->Object_CallMethodByName_Boolean(incognitoObj, "booleanValue", nullptr, &incognito) != ANI_OK) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
            return;
        }
        webStorage->DeleteAllData(static_cast<bool>(incognito));
    }
}

void DeleteOrigin(ani_env *env, ani_object object, ani_string origin)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if(env->Reference_IsUndefined(origin, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("origin is undefined");
        return;
    }
    std::string originStr;
    if (!AniParseUtils::ParseString(env, origin, originStr)) {
        WVLOG_E("parse origin failed");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> webStorage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!webStorage) {
        WVLOG_E("webStorage is nullptr");
        return;
    }
    webStorage->DeleteOrigin(originStr);
}

static bool SetStringToObj(ani_env *env, ani_object obj, const char *propertyName, const std::string &propertyValue) {
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (obj == nullptr) {
        WVLOG_E("obj is nullptr");
        return false;
    }
    ani_string aniStr = nullptr;
    if (env->String_NewUTF8(propertyValue.c_str(), propertyValue.size(), &aniStr) != ANI_OK) {
        WVLOG_E("create aniStr failed");
        return false;
    }
    if (env->Object_SetPropertyByName_Ref(obj, propertyName, aniStr) != ANI_OK) {
        WVLOG_E("set property failed");
        return false;
    }
    return true;
}

static bool SetLongToObj(ani_env *env, ani_object obj, const char *propertyName, long propertyValue) {
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (obj == nullptr) {
        WVLOG_E("obj is nullptr");
        return false;
    }

    if (env->Object_SetPropertyByName_Double(obj, propertyName, propertyValue) != ANI_OK) {
        WVLOG_E("set property failed");
        return false;
    }

    return true;
}

static ani_object CreateWebStorageOriginObj(ani_env *env, std::shared_ptr<NWebWebStorageOrigin> origin)
{
    ani_object obj = {};
    bool ret = AniParseUtils::CreateObjectVoid(env, ANI_CLASS_WEB_STORAGE_ORIGIN_INNER, obj);
    if (!ret) {
        WVLOG_E("CreateObjectVoid failed");
        return nullptr;
    }
    if (!SetStringToObj(env, obj, "origin", origin->GetOrigin()) || !SetLongToObj(env, obj, "quota", origin->GetQuota())
        || !SetLongToObj(env, obj, "usage", origin->GetUsage())) {
        WVLOG_E("set NWebWebStorageOrigin failed");
        return nullptr;
    }
    return obj;
}

static ani_object GetOriginsSync(ani_env *env, ani_object obj)
{
    ani_object arrayObj = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_class arrayCls = nullptr;
    if (env->FindClass("Lescompat/Array;", &arrayCls) != ANI_OK) {
        WVLOG_E("find class escompat/Array; failed");
        return nullptr;
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        WVLOG_E("find class escompat/Array; failed");
        return nullptr;
    }

    std::shared_ptr<OHOS::NWeb::NWebWebStorage> webStorage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!webStorage) {
        WVLOG_E("webStorage is nullptr");
        return nullptr;
    }
    std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins = webStorage->GetOrigins();
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, origins.size()) != ANI_OK) {
        WVLOG_E("Object_New Array failed");
        return nullptr;
    }
    ani_size index = 0;
    for (auto origin : origins) {
        ani_object obj = CreateWebStorageOriginObj(env, origin);
        if (obj == nullptr) {
            WVLOG_E("obj is nullptr");
            break;
        }
        if (env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", index, obj) != ANI_OK) {
            WVLOG_E("Object_CallMethodByName_Void failed");
            break;
        }
        index++;
    }
    return arrayObj;
}

ani_double GetOriginQuotaSync(ani_env *env, ani_object obj, ani_string origin)
{
    ani_double quota = -1.0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return quota;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if(env->Reference_IsUndefined(origin, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("origin is undefined");
        return quota;
    }
    std::string originStr;
    if (!AniParseUtils::ParseString(env, origin, originStr)) {
        WVLOG_E("parse origin failed");
        return quota;
    }
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> webStorage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!webStorage) {
        WVLOG_E("webStorage is nullptr");
        return quota;
    }
    long quotaValue = webStorage->GetOriginQuota(originStr);
    return static_cast<ani_double>(quotaValue);
}

ani_double GetOriginUsageSync(ani_env *env, ani_object obj, ani_string origin)
{
    ani_double usage = -1.0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return usage;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if(env->Reference_IsUndefined(origin, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("origin is undefined");
        return usage;
    }
    std::string originStr;
    if (!AniParseUtils::ParseString(env, origin, originStr)) {
        WVLOG_E("parse origin failed");
        return usage;
    }
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> webStorage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!webStorage) {
        WVLOG_E("webStorage is nullptr");
        return usage;
    }
    long usageValue = webStorage->GetOriginUsage(originStr);
    return static_cast<ani_double>(usageValue);
}

ani_status StsWebStorageInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webStorageCls = nullptr;
    ani_status status = env->FindClass(WEB_STORAGE_CLASS_NAME, &webStorageCls);
    if (status != ANI_OK || !webStorageCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_STORAGE_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array storageMethods = {
        ani_native_function { "deleteAllData", nullptr, reinterpret_cast<void *>(DeleteAllData) },
        ani_native_function { "deleteOrigin", nullptr, reinterpret_cast<void *>(DeleteOrigin) },
        ani_native_function { "getOriginsSync", nullptr, reinterpret_cast<void *>(GetOriginsSync) },
        ani_native_function { "getOriginQuotaSync", nullptr, reinterpret_cast<void *>(GetOriginQuotaSync) },
        ani_native_function { "getOriginUsageSync", nullptr, reinterpret_cast<void *>(GetOriginUsageSync) },
    };

    status = env->Class_BindNativeMethods(webStorageCls, storageMethods.data(), storageMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
