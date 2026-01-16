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
#include <array>
#include <iostream>
#include <mutex>
#include <string>

#include "ani_user_agent_brand_version.h"
#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_controller.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
std::recursive_mutex g_brandVersionLock;
bool g_brandVersionMode = true;
const char* ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME = "@ohos.web.webview.webview.UserAgentBrandVersion";
} // namespace

ani_object CreateUserAgentBrandVersionObject(ani_env* env, std::shared_ptr<NWebUserAgentBrandVersionImpl> version)
{
    if (!version) {
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(g_brandVersionLock);
    g_brandVersionMode = false;
    ani_object obj = {};
    if (!AniParseUtils::CreateObjectVoid(env, ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, obj)) {
        WVLOG_E("CreateObjectVoid failed");
        g_brandVersionMode = true;
        return nullptr;
    }
    NWebUserAgentBrandVersionImpl* brandVersion = new (std::nothrow)
        NWebUserAgentBrandVersionImpl(version->GetBrand(), version->GetMajorVersion(), version->GetFullVersion());
    if (!brandVersion) {
        WVLOG_E("new NWebUserAgentBrandVersionImpl failed");
        g_brandVersionMode = true;
        return nullptr;
    }
    if (!AniParseUtils::Wrap(
        env, obj, ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, reinterpret_cast<ani_long>(brandVersion))) {
        WVLOG_E("NWebUserAgentBrandVersionImpl wrap failed");
        delete brandVersion;
        brandVersion = nullptr;
        g_brandVersionMode = true;
        return nullptr;
    }
    g_brandVersionMode = true;
    return obj;
}

static void SetBrand(ani_env* env, ani_object object, ani_string brandObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, brandObj, value)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "brand", "string"));
        return;
    }
    if (value == "") {
        WVLOG_E("brand is null");
        return;
    }
    WVLOG_D("SetBrand %{public}s.", value.c_str());
    userAgentBrandVersion->SetBrand(value);
}

static void SetMajorVersion(ani_env* env, ani_object object, ani_string majorVersionObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, majorVersionObj, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "majorVersion", "string"));
        return;
    }
    if (value == "") {
        WVLOG_E("majorVersion is null");
        return;
    }
    WVLOG_D("SetMajorVersion %{public}s.", value.c_str());
    userAgentBrandVersion->SetMajorVersion(value);
}

static void SetFullVersion(ani_env* env, ani_object object, ani_string fullVersionObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, fullVersionObj, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "fullVersion", "string"));
        return;
    }
    if (value == "") {
        WVLOG_E("fullVersion is null");
        return;
    }
    WVLOG_D("SetFullVersion %{public}s.", value.c_str());
    userAgentBrandVersion->SetFullVersion(value);
}

static ani_string GetBrand(ani_env* env, ani_object object)
{
    ani_string brand = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return nullptr;
    }

    std::string result = userAgentBrandVersion->GetBrand();
    env->String_NewUTF8(result.c_str(), result.size(), &brand);

    return brand;
}

static ani_string GetMajorVersion(ani_env* env, ani_object object)
{
    ani_string majorVersion = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return nullptr;
    }

    std::string result = userAgentBrandVersion->GetMajorVersion();
    env->String_NewUTF8(result.c_str(), result.size(), &majorVersion);

    return majorVersion;
}

static ani_string GetFullVersion(ani_env* env, ani_object object)
{
    ani_string fullVersion = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* userAgentBrandVersion = reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, object));
    if (!userAgentBrandVersion) {
        WVLOG_E("userAgentBrandVersion is null");
        return nullptr;
    }

    std::string result = userAgentBrandVersion->GetFullVersion();
    env->String_NewUTF8(result.c_str(), result.size(), &fullVersion);

    return fullVersion;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_D("UserAgentBrandVersion native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(g_brandVersionLock);
        if (g_brandVersionMode) {
            WVLOG_D("UserAgentBrandVersion mode true");
            NWebUserAgentBrandVersionImpl* version = new (std::nothrow) NWebUserAgentBrandVersionImpl();
            if (version == nullptr) {
                WVLOG_E("new UserAgentBrandVersion failed");
                return;
            }
            if (!AniParseUtils::Wrap(
                env, object, ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, reinterpret_cast<ani_long>(version))) {
                WVLOG_E("UserAgentBrandVersion wrap failed");
                delete version;
                version = nullptr;
            }
        }
    }
}

ani_status StsUserAgentBrandVersionInit(ani_env* env)
{
    WVLOG_D("StsUserAgentBrandVersionInit");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class versionCls = nullptr;
    ani_status status = env->FindClass(ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, &versionCls);
    if (status != ANI_OK || !versionCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "setBrand", nullptr, reinterpret_cast<void*>(SetBrand) },
        ani_native_function { "setMajorVersion", nullptr, reinterpret_cast<void*>(SetMajorVersion) },
        ani_native_function { "setFullVersion", nullptr, reinterpret_cast<void*>(SetFullVersion) },
        ani_native_function { "getBrand", nullptr, reinterpret_cast<void*>(GetBrand) },
        ani_native_function { "getMajorVersion", nullptr, reinterpret_cast<void*>(GetMajorVersion) },
        ani_native_function { "getFullVersion", nullptr, reinterpret_cast<void*>(GetFullVersion) },
    };

    status = env->Class_BindNativeMethods(versionCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS