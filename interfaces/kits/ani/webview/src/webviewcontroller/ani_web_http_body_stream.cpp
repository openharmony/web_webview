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

#include <array>
#include <iostream>

#include "web_scheme_handler_request.h"
#include "webview_controller.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "arkweb_scheme_handler.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_HTTP_BODY_STREAM_CLASS_NAME = "L@ohos/web/webview/webview/WebHttpBodyStream;";
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_I("WebHttpBodyStream native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}
static ani_boolean JsIsChunked(ani_env* env, ani_object object)
{
    WVLOG_D("WebHttpBodyStream JsIsChunked.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is null");
        return ANI_FALSE;
    }
    return stream->IsChunked() ? ANI_TRUE : ANI_FALSE;
}

static ani_boolean JsIsInMemory(ani_env* env, ani_object object)
{
    WVLOG_D("WebHttpBodyStream JsIsInMemory.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is null");
        return ANI_FALSE;
    }
    return stream->IsInMemory() ? ANI_TRUE : ANI_FALSE;
}

static ani_boolean JsIsEof(ani_env* env, ani_object object)
{
    WVLOG_D("WebHttpBodyStream JsIsEof.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is null");
        return ANI_FALSE;
    }
    return stream->IsEof() ? ANI_TRUE : ANI_FALSE;
}

static void JSInitialize(ani_env* env, ani_object object)
{
    WVLOG_E("WebHttpBodyStream JSInitialize.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* request = reinterpret_cast<WebSchemeHandlerRequest *>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("WebHttpBodyStream JSInitialize.222");
        return ;
    }

    ArkWeb_HttpBodyStream* arkWebPostStream = request->GetHttpBodyStream();
    if (!arkWebPostStream) {
        WVLOG_E("getHttpBodyStream: arkWebPostStream is nullptr");
        return ;
    }
    WebHttpBodyStream* stream = new (std::nothrow) WebHttpBodyStream(env, arkWebPostStream);
    if (!stream) {
        WVLOG_E("stream is nullptr");
        return;
    }
    WVLOG_E("stream creat success");
    ani_resolver deferred {};
    ani_object promise {};
    ani_status status = env->Promise_New(&deferred, &promise);
    if (status != ANI_OK) {
        WVLOG_E("promise_new failed");
        return;
    }
    if (promise && deferred) {
        WVLOG_E("WebHttpBodyStream JSInitialize.httpBodyStream->Init");
        stream->Init(nullptr, std::move(deferred));
    }
}

static ani_object JsRead(ani_env* env, ani_object object, ani_double size)
{
    WVLOG_D("WebHttpBodyStream JsRead.");
    ani_object arrayBufferObj = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is nullptr");
        return nullptr;
    }
    std::shared_ptr<OHOS::NWeb::WebHttpBodyStream> httpBodyStream;
    if (stream != nullptr) {
        httpBodyStream = std::shared_ptr<OHOS::NWeb::WebHttpBodyStream>(stream, [](OHOS::NWeb::WebHttpBodyStream*) {});
    }
    if (httpBodyStream == nullptr) {
        WVLOG_E("httpBodyStream is nullptr");
        return nullptr;
    }
    ani_resolver deferred {};
    ani_object promise {};
    ani_status status = env->Promise_New(&deferred, &promise);
    if (status != ANI_OK) {
        WVLOG_E("promise_new failed");
        return nullptr;
    }
    if (promise && deferred) {
        httpBodyStream->Read(static_cast<int>(size), nullptr, std::move(deferred));
    }
    return arrayBufferObj;
}

static ani_double GetPosition(ani_env* env, ani_object object)
{
    WVLOG_D("WebHttpBodyStream GetSize.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return -1;
    }

    ani_double position;

    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is nullptr");
        return -1;
    }
    std::shared_ptr<OHOS::NWeb::WebHttpBodyStream> httpBodyStream;
    if (stream != nullptr) {
        httpBodyStream = std::shared_ptr<OHOS::NWeb::WebHttpBodyStream>(stream, [](OHOS::NWeb::WebHttpBodyStream*) {});
    }
    if (httpBodyStream == nullptr) {
        WVLOG_E("httpBodyStream is nullptr");
        return -1;
    }
    position = static_cast<ani_double>(httpBodyStream->GetSize());
    return position;
}

static ani_double GetSize(ani_env* env, ani_object object)
{
    WVLOG_D("WebHttpBodyStream GetSize.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return -1;
    }

    ani_double size;

    auto* stream = reinterpret_cast<WebHttpBodyStream*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is nullptr");
        return -1;
    }
    std::shared_ptr<OHOS::NWeb::WebHttpBodyStream> httpBodyStream;
    if (stream != nullptr) {
        httpBodyStream = std::shared_ptr<OHOS::NWeb::WebHttpBodyStream>(stream, [](OHOS::NWeb::WebHttpBodyStream*) {});
    }
    if (httpBodyStream == nullptr) {
        WVLOG_E("httpBodyStream is nullptr");
        return -1;
    }
    size = static_cast<ani_double>(httpBodyStream->GetSize());
    return size;
}

ani_status StsWebHttpBodyStreamInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class WebHttpBodyStreamCls = nullptr;
    ani_status status = env->FindClass(WEB_HTTP_BODY_STREAM_CLASS_NAME, &WebHttpBodyStreamCls);
    if (status != ANI_OK || !WebHttpBodyStreamCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_HTTP_BODY_STREAM_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "isChunked", nullptr, reinterpret_cast<void*>(JsIsChunked) },
        ani_native_function { "isInMemory", nullptr, reinterpret_cast<void*>(JsIsInMemory) },
        ani_native_function { "isEof", nullptr, reinterpret_cast<void*>(JsIsEof) },
        ani_native_function { "initializeSync", nullptr, reinterpret_cast<void*>(JSInitialize) },
        ani_native_function { "readSync", nullptr, reinterpret_cast<void*>(JsRead) },
        ani_native_function { "getPosition", nullptr, reinterpret_cast<void*>(GetPosition) },
        ani_native_function { "getSize", nullptr, reinterpret_cast<void*>(GetSize) },
    };

    status = env->Class_BindNativeMethods(WebHttpBodyStreamCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS