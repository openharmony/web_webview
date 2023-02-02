/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_web_async_controller.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_store_web_archive_callback.h"
#include <memory>

#undef LOG_TAG
#define LOG_TAG "NapiWebAsyncController"

namespace {
// Max length of string.
constexpr size_t MAX_WEB_STRING_LENGTH = 40960;
} // namespace

namespace OHOS {
namespace NWeb {
napi_value NapiWebAsyncController::Init(napi_env env, napi_value exports)
{
    const std::string WEB_ASYNC_CLIENT_CLASS_NAME = "WebAsyncController";
    napi_property_descriptor properties[] = {
        {"storeWebArchive", nullptr, JS_StoreWebArchive, nullptr, nullptr, nullptr,
         napi_default, nullptr},
    };
    napi_value constructor = nullptr;
    napi_define_class(env, WEB_ASYNC_CLIENT_CLASS_NAME.c_str(), WEB_ASYNC_CLIENT_CLASS_NAME.length(),
        JS_NapiWebAsyncController, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "define js class WebAsyncController failed");
    napi_status status = napi_set_named_property(env, exports, "WebAsyncController", constructor);
    NAPI_ASSERT(env, status == napi_ok, "set property WebAsyncController failed");
    return exports;
}

napi_value NapiWebAsyncController::StoreWebArchiveInternal(napi_env env, napi_callback_info cbinfo,
    const std::string &baseName, bool autoName)
{
    napi_value thisVar = nullptr;
    size_t argc = 1;
    size_t argcPromise = 2;
    size_t argcCallback = 3;
    napi_value argv[3] = {0};

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);

    // Get the native object of NapiWebAsyncController.
    NapiWebAsyncController *webAsyncController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webAsyncController);

    if (!webAsyncController) {
        return result;
    }

    // If there are three parameters, it is considered to be called by callback,
    // if it is one, it is considered to be called by returning a promise.
    if (argc == argcCallback) {
        napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], 1, &jsCallback);

        if (jsCallback) {
            webAsyncController->StoreWebArchiveCallback(baseName, autoName, env, std::move(jsCallback));
        }
        return result;
    } else if (argc == argcPromise) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            webAsyncController->StoreWebArchivePromise(baseName, autoName, env, deferred);
        }
        return promise;
    }

    return result;
}

napi_value NapiWebAsyncController::JS_StoreWebArchive(napi_env env, napi_callback_info cbinfo)
{
    napi_value thisVar = nullptr;

    size_t argc = 1;
    size_t argcPromise = 2;
    size_t argcCallback = 3;
    napi_value argv[3] = {0};

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc == argcPromise || argc == argcCallback, "requires 2 or 3 parameter");

    // Check first param is string.
    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for parameter 1");

    // Get the first param as string.
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufferSize);
    NAPI_ASSERT_BASE(env, bufferSize > 0, "buffer len = 0", result);

    bufferSize = std::min(bufferSize, MAX_WEB_STRING_LENGTH);
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv[0], stringValue, bufferSize + 1, &jsStringLength);
    NAPI_ASSERT_BASE(env, jsStringLength == bufferSize, "string length wrong", result);
    std::string baseName(stringValue);

    // Check second param is boolean.
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    napi_typeof(env, argv[1], &valueType);
    NAPI_ASSERT(env, valueType == napi_boolean, "type mismatch for parameter 2");

    // Get the second param as bool.
    bool autoName = false;
    NAPI_CALL(env, napi_get_value_bool(env, argv[1], &autoName));

    // Check third param is a function.
    if (argc == argcCallback) {
        napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        NAPI_ASSERT(env, valueType == napi_function, "type mismatch for parameter 3");
    }

    return StoreWebArchiveInternal(env, cbinfo, baseName, autoName);
}

napi_value NapiWebAsyncController::JS_NapiWebAsyncController(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));

    NAPI_ASSERT(env, valueType == napi_object, "Wrong type of arguments. Expects an object as first argument.");

    napi_value obj = argv[0];
    napi_value getNWebIdFunc;

    napi_status status = napi_get_named_property(env, obj, "getWebId", &getNWebIdFunc);
    if (status != napi_status::napi_ok) {
        return nullptr;
    }

    napi_value result;
    status = napi_call_function(env, obj, getNWebIdFunc, 0, nullptr, &result);
    if (status != napi_status::napi_ok) {
        return nullptr;
    }

    int32_t nwebId = -1;
    napi_get_value_int32(env, result, &nwebId);

    NapiWebAsyncController *webAsyncController = new (std::nothrow) NapiWebAsyncController(env, thisVar, nwebId);
    if (webAsyncController == nullptr) {
        HILOG_ERROR(LOG_APP, "new webAsyncController failed");
        return nullptr;
    }

    napi_wrap(
        env, thisVar, webAsyncController,
        [](napi_env env, void *data, void *hint) {
            NapiWebAsyncController *webAsyncController = static_cast<NapiWebAsyncController *>(data);
            delete webAsyncController;
        },
        nullptr, nullptr);

    return thisVar;
}

NapiWebAsyncController::NapiWebAsyncController(napi_env env, napi_value thisVar, int32_t nwebId) : nwebId_(nwebId) {}

void NapiWebAsyncController::StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env,
    napi_ref jsCallback)
{
    std::weak_ptr<NWeb> nweb_weak = OHOS::NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
    auto nweb = nweb_weak.lock();
    if (!nweb) {
        HILOG_ERROR(LOG_APP, "not found a valid nweb");
        napi_value callback = nullptr;
        napi_value jsResult = nullptr;
        napi_value callbackResult = nullptr;

        napi_get_reference_value(env, jsCallback, &callback);
        napi_get_null(env, &jsResult);
        napi_call_function(env, nullptr, callback, 1, &jsResult, &callbackResult);
        napi_delete_reference(env, jsCallback);
        return;
    }

    if (jsCallback == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, jCallback = std::move(jsCallback)](std::string result) {
        if (!env) {
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        napi_value callback = nullptr;
        napi_get_reference_value(env, jCallback, &callback);

        napi_value jsResult = nullptr;
        napi_value callbackResult = nullptr;
        if (result.empty()) {
            napi_get_null(env, &jsResult);
        } else {
            napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &jsResult);
        }
        napi_call_function(env, nullptr, callback, 1, &jsResult, &callbackResult);

        napi_delete_reference(env, jCallback);
        napi_close_handle_scope(env, scope);
    });
    nweb->StoreWebArchive(baseName, autoName, callbackImpl);

    return;
}

void NapiWebAsyncController::StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env,
    napi_deferred deferred)
{
    std::weak_ptr<NWeb> nweb_weak = OHOS::NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
    auto nweb = nweb_weak.lock();
    if (!nweb) {
        HILOG_ERROR(LOG_APP, "not found a valid nweb");
        napi_value jsResult = nullptr;
        napi_get_null(env, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
        return;
    }

    if (deferred == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, deferred](std::string result) {
        if (!env) {
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        napi_value jsResult = nullptr;
        if (!result.empty()) {
            napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &jsResult);
            napi_resolve_deferred(env, deferred, jsResult);
        } else {
            napi_get_null(env, &jsResult);
            napi_reject_deferred(env, deferred, jsResult);
        }
        napi_close_handle_scope(env, scope);
    });
    nweb->StoreWebArchive(baseName, autoName, callbackImpl);
    return;
}
} // namespace NWeb
} // namespace OHOS
