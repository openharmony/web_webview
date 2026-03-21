/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "webview_message_ffi.h"

#include "webview_controller_impl.h"
#include "webview_function.h"
#include "webview_utils.h"
#include "webview_javascript_execute_callback.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "webview_log.h"
#include "parameters.h"
#include "pixel_map.h"
#include "cj_lambda.h"
#include "webview_value.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {

extern "C" {
    // WebMessagePort
    void FfiOHOSWebMessagePortPostMessageEvent(int64_t msgPortId, char* stringValue, int32_t *errCode)
    {
        WEBVIEWLOGD("message port post message");
        auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        auto romMsg = std::make_shared<OHOS::NWeb::WebViewValue>(NWebRomValue::Type::NONE);
        std::string message(stringValue);
        webMsg->SetType(NWebValue::Type::STRING);
        webMsg->SetString(message);
        romMsg->SetType(NWebRomValue::Type::STRING);
        romMsg->SetString(message);
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            *errCode = NWebError::CAN_NOT_POST_MESSAGE;
            return;
        }
        *errCode = msgPort->PostPortMessage(webMsg, romMsg);
        return;
    }

    void FfiOHOSWebMessagePortPostMessageEventArr(int64_t msgPortId, CArrUI8 arrBuf, int32_t *errCode)
    {
        WEBVIEWLOGD("message port post message");
        auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        auto romMsg = std::make_shared<OHOS::NWeb::WebViewValue>(NWebRomValue::Type::NONE);
        std::vector<uint8_t> vecData(arrBuf.head, arrBuf.head + arrBuf.size);
        webMsg->SetType(NWebValue::Type::BINARY);
        webMsg->SetBinary(vecData);
        romMsg->SetType(NWebRomValue::Type::BINARY);
        romMsg->SetBinary(vecData);
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            *errCode = NWebError::CAN_NOT_POST_MESSAGE;
            return;
        }
        *errCode = msgPort->PostPortMessage(webMsg, romMsg);
        return;
    }

    void FfiOHOSWebMessagePortPostMessageEventExt(int64_t msgPortId, int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("message PostMessageEventExt start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessagePortPostMessageEventExt error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            *errCode = NWebError::CAN_NOT_POST_MESSAGE;
            return;
        }
        if (!msgPort->IsExtentionType()) {
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        *errCode = msgPort->PostPortMessage(webMessageExt->GetData(), webMessageExt->GetValue());
        return;
    }

    bool FfiOHOSWebMessagePortIsExtentionType(int64_t msgPortId)
    {
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            return false;
        }
        return msgPort->IsExtentionType();
    }

    void FfiOHOSWebMessagePortOnMessageEvent(int64_t msgPortId, void (*callback)(RetWebMessage), int32_t *errCode)
    {
        WEBVIEWLOGD("message port set OnMessageEvent callback");
        std::function<void(RetWebMessage)> onMsgEventFunc = CJLambda::Create(callback);
        auto callbackImpl = std::make_shared<NWebMessageCallbackImpl>(onMsgEventFunc);
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            *errCode = NWebError::CAN_NOT_REGISTER_MESSAGE_EVENT;
            return;
        }
        *errCode = msgPort->SetPortMessageCallback(callbackImpl);
        return;
    }

    void FfiOHOSWebMessagePortOnMessageEventExt(int64_t msgPortId, void (*callback)(int64_t), int32_t *errCode)
    {
        WEBVIEWLOGD("message port set OnMessageEventExt callback");
        std::function<void(int64_t)> onMsgEventFunc = CJLambda::Create(callback);
        auto callbackImpl = std::make_shared<NWebWebMessageExtCallbackImpl>(onMsgEventFunc);
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("post message failed, ffi unwrap msg port failed");
            *errCode = NWebError::CAN_NOT_REGISTER_MESSAGE_EVENT;
            return;
        }
        *errCode = msgPort->SetPortMessageCallback(callbackImpl);
        return;
    }

    void FfiOHOSWebMessagePortClose(int64_t msgPortId, int32_t *errCode)
    {
        WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(msgPortId);
        if (msgPort == nullptr) {
            WEBVIEWLOGE("close message failed, ffi unwrap msg port failed");
            return;
        }
        *errCode = msgPort->ClosePort();
        return;
    }
    // WebMessageExt
    int64_t FfiOHOSWebMessageExtImplConstructor()
    {
        auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        auto romMsg = std::make_shared<OHOS::NWeb::WebViewValue>(NWebRomValue::Type::NONE);
        WebMessageExtImpl* nativeWebMessageExtImpl = FFIData::Create<WebMessageExtImpl>(webMsg, romMsg);
        if (nativeWebMessageExtImpl == nullptr) {
            WEBVIEWLOGE("new webMessageExt failed");
            return -1;
        }
        return nativeWebMessageExtImpl->GetID();
    }

    int32_t FfiOHOSWebMessageExtImplGetType(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetType::GetType start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetType::GetType error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return -1;
        }
        int32_t type = webMessageExt->GetType();
        *errCode = NWebError::NO_ERROR;
        return type;
    }

    char* FfiOHOSWebMessageExtImplGetString(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetString::GetString start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetString::GetString error");
            *errCode = NWebError::INIT_ERROR;
            return nullptr;
        }

        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::STRING)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return nullptr;
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetString::GetString error");
            *errCode = NWebError::INIT_ERROR;
            return nullptr;
        }
        std::string msgStr = data->GetString();
        *errCode = NWebError::NO_ERROR;
        return MallocCString(msgStr);
    }

    RetNumber FfiOHOSWebMessageExtImplGetNumber(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetNumber::GetNumber start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        RetNumber ret = { .numberInt = 0, .numberDouble = 0.0 };
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetNumber::GetNumber error");
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }

        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::NUMBER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return ret;
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetNumber::GetNumber error");
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        if (data->GetType() == NWebValue::Type::INTEGER) {
            ret.numberInt = data->GetInt64();
        } else {
            ret.numberDouble = data->GetDouble();
        }
        return ret;
    }

    bool FfiOHOSWebMessageExtImplGetBoolean(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetBoolean::GetBoolean start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetBoolean::GetBoolean error");
            *errCode = NWebError::INIT_ERROR;
            return false;
        }

        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return false;
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetBoolean::GetBoolean error");
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        *errCode = NWebError::NO_ERROR;
        return data->GetBoolean();
    }

    CArrUI8 FfiOHOSWebMessageExtImplGetArrayBuffer(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetArrayBuffer::GetArrayBuffer start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetArrayBuffer::GetArrayBuffer error");
            *errCode = NWebError::INIT_ERROR;
            return CArrUI8{nullptr, 0};
        }

        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return CArrUI8{nullptr, 0};
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetArrayBuffer::GetArrayBuffer error");
            *errCode = NWebError::INIT_ERROR;
            return CArrUI8{nullptr, 0};
        }
        std::vector<uint8_t> msgArr = data->GetBinary();
        uint8_t* result = VectorToCArrUI8(msgArr);
        if (result == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetArrayBuffer malloc failed");
            *errCode = NWebError::NEW_OOM;
            return CArrUI8{nullptr, 0};
        }
        *errCode = NWebError::NO_ERROR;
        return CArrUI8{result, msgArr.size()};
    }

    CArrValue ConvertToCArr(std::shared_ptr<NWebMessage> data, int32_t *errCode)
    {
        CArrValue ret = { .strHead = nullptr, .intHead = nullptr, .doubleHead = nullptr,
            .boolHead = nullptr, .size = 0 };
        if (data->GetType() == NWebValue::Type::STRINGARRAY) {
            std::vector<std::string> values = data->GetStringArray();
            ret.size = static_cast<int64_t>(values.size());
            ret.strHead = OHOS::Webview::VectorToCArrString(values);
        } else if (data->GetType() == NWebValue::Type::BOOLEANARRAY) {
            std::vector<bool> values = data->GetBooleanArray();
            auto arr = static_cast<bool*>(malloc(sizeof(bool) * values.size()));
            if (!arr) {
                *errCode = NWebError::NEW_OOM;
                return ret;
            }
            for (uint32_t i = 0; i < values.size(); i++) {
                arr[i] = values[i];
            }
            ret.boolHead = arr;
            ret.size = static_cast<int64_t>(values.size());
        } else if (data->GetType() == NWebValue::Type::DOUBLEARRAY) {
            std::vector<double> values = data->GetDoubleArray();
            auto arr = static_cast<double*>(malloc(sizeof(double) * values.size()));
            if (!arr) {
                *errCode = NWebError::NEW_OOM;
                return ret;
            }
            for (uint32_t i = 0; i < values.size(); i++) {
                arr[i] = values[i];
            }
            ret.doubleHead = arr;
            ret.size = static_cast<int64_t>(values.size());
        } else {
            std::vector<int64_t> values = data->GetInt64Array();
            auto arr = static_cast<int64_t*>(malloc(sizeof(int64_t) * values.size()));
            if (!arr) {
                *errCode = NWebError::NEW_OOM;
                return ret;
            }
            for (uint32_t i = 0; i < values.size(); i++) {
                arr[i] = values[i];
            }
            ret.intHead = arr;
            ret.size = static_cast<int64_t>(values.size());
        }
        return ret;
    }

    CArrValue FfiOHOSWebMessageExtImplGetArray(int64_t msgExtId, int32_t *errCode)
    {
        CArrValue ret = { .strHead = nullptr, .intHead = nullptr, .doubleHead = nullptr,
            .boolHead = nullptr, .size = 0 };
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return ret;
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        return ConvertToCArr(data, errCode);
    }

    CError FfiOHOSWebMessageExtImplGetError(int64_t msgExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplGetError::GetError start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        auto err = CError{.errorName = nullptr, .errorMsg = nullptr};
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetError::GetError error");
            *errCode = NWebError::INIT_ERROR;
            return err;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ERROR)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return err;
        }
        auto data = webMessageExt->GetData();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetError::GetError error");
            *errCode = NWebError::INIT_ERROR;
            return err;
        }
        *errCode = NWebError::NO_ERROR;
        std::string errorName = data->GetErrName();
        std::string errorMsg = data->GetErrName() + ": " + data->GetErrMsg();
        err.errorName = MallocCString(errorName);
        err.errorMsg = MallocCString(errorMsg);
        return err;
    }

    void FfiOHOSWebMessageExtImplSetType(int64_t msgExtId, int32_t type, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplSetType::SetType");
        if (type <= static_cast<int>(WebMessageType::NOTSUPPORT) || type > static_cast<int>(WebMessageType::ERROR)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetArrayBuffer::GetArrayBuffer error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        webMessageExt->SetType(type);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetString(int64_t msgExtId, char* message, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSWebMessageExtImplSetString::SetString start");
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetString::SetString error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::STRING)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::string value = std::string(message);
        webMessageExt->SetString(value);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetNumber(int64_t msgExtId, double value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetNumber::SetNumber error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::NUMBER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        webMessageExt->SetNumber(value);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetBoolean(int64_t msgExtId, bool value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetBoolean::SetBoolean error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        webMessageExt->SetBoolean(value);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetArrayBuffer(int64_t msgExtId, CArrUI8 value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetArrayBuffer::SetArrayBuffer error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        uint8_t *arrBuf = value.head;
        size_t byteLength = static_cast<size_t>(value.size);
        std::vector<uint8_t> vecData(arrBuf, arrBuf + byteLength);
        webMessageExt->SetArrayBuffer(vecData);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetArrayString(int64_t msgExtId, CArrString value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetArrayString error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::vector<std::string> outValue;
        for (int64_t i = 0; i < value.size; i++) {
            outValue.push_back(std::string(value.head[i]));
        }
        webMessageExt->SetStringArray(outValue);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetArrayInt(int64_t msgExtId, CArrI64 value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetArrayInt error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::vector<int64_t> outValue;
        for (int64_t i = 0; i < value.size; i++) {
            outValue.push_back(value.head[i]);
        }
        webMessageExt->SetInt64Array(outValue);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetArrayDouble(int64_t msgExtId,
        OHOS::Webview::CArrDouble value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetArrayDouble error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::vector<double> outValue;
        for (int64_t i = 0; i < value.size; i++) {
            outValue.push_back(value.head[i]);
        }
        webMessageExt->SetDoubleArray(outValue);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetArrayBoolean(int64_t msgExtId,
        OHOS::Webview::CArrBool value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplSetArrayBoolean error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::vector<bool> outValue;
        for (int64_t i = 0; i < value.size; i++) {
            outValue.push_back(value.head[i]);
        }
        webMessageExt->SetBooleanArray(outValue);
        *errCode = NWebError::NO_ERROR;
        return;
    }

    void FfiOHOSWebMessageExtImplSetError(int64_t msgExtId, OHOS::Webview::CError value, int32_t *errCode)
    {
        WebMessageExtImpl* webMessageExt = FFIData::GetData<WebMessageExtImpl>(msgExtId);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebMessageExtImplGetError::GetError error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return;
        }
        if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ERROR)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return;
        }
        std::string nameVal = std::string(value.errorName);
        std::string msgVal = std::string(value.errorMsg);
        *errCode = NWebError::NO_ERROR;
        webMessageExt->SetError(nameVal, msgVal);
        return;
    }

    // WebJsMessageExtImpl
    int32_t FfiOHOSJsMessageExtImplGetType(int64_t jsExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSJsMessageExtImplGetType::GetType start");
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        if (webJsMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetType::GetType error");
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return -1;
        }
        int32_t type = webJsMessageExt->GetType();
        *errCode = NWebError::NO_ERROR;
        return type;
    }

    char* FfiOHOSJsMessageExtImplGetString(int64_t jsExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSJsMessageExtImplGetString::GetString start");
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        if (webJsMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetString::GetString error");
            *errCode = NWebError::INIT_ERROR;
            return nullptr;
        }

        if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::STRING)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return nullptr;
        }
        auto data = webJsMessageExt->GetJsMsgResult();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetString::GetString error");
            *errCode = NWebError::INIT_ERROR;
            return nullptr;
        }
        std::string msgStr = data->GetString();
        *errCode = NWebError::NO_ERROR;
        return MallocCString(msgStr);
    }

    RetNumber FfiOHOSJsMessageExtImplGetNumber(int64_t jsExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSJsMessageExtImplGetNumber::GetNumber start");
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        RetNumber ret = { .numberInt = 0, .numberDouble = 0.0 };
        if (webJsMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetNumber::GetNumber error");
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }

        if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::NUMBER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return ret;
        }
        auto data = webJsMessageExt->GetJsMsgResult();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetNumber::GetNumber error");
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        if (data->GetType() == NWebValue::Type::INTEGER) {
            ret.numberInt = data->GetInt64();
        } else {
            ret.numberDouble = data->GetDouble();
        }
        return ret;
    }

    bool FfiOHOSJsMessageExtImplGetBoolean(int64_t jsExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSJsMessageExtImplGetBoolean::GetBoolean start");
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        if (webJsMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetBoolean::GetBoolean error");
            *errCode = NWebError::INIT_ERROR;
            return false;
        }

        if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::BOOLEAN)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return false;
        }
        auto data = webJsMessageExt->GetJsMsgResult();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetBoolean::GetBoolean error");
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        *errCode = NWebError::NO_ERROR;
        return data->GetBoolean();
    }

    CArrUI8 FfiOHOSJsMessageExtImplGetArrayBuffer(int64_t jsExtId, int32_t *errCode)
    {
        WEBVIEWLOGD("FfiOHOSJsMessageExtImplGetArrayBuffer::GetArrayBuffer start");
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        if (webJsMessageExt == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetArrayBuffer::GetArrayBuffer error");
            *errCode = NWebError::INIT_ERROR;
            return CArrUI8{nullptr, 0};
        }

        if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAYBUFFER)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return CArrUI8{nullptr, 0};
        }
        auto data = webJsMessageExt->GetJsMsgResult();
        if (data == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetArrayBuffer::GetArrayBuffer error");
            *errCode = NWebError::INIT_ERROR;
            return CArrUI8{nullptr, 0};
        }
        std::vector<uint8_t> msgArr = data->GetBinary();
        uint8_t* result = VectorToCArrUI8(msgArr);
        if (result == nullptr) {
            WEBVIEWLOGE("FfiOHOSJsMessageExtImplGetArrayBuffer malloc failed");
            *errCode = NWebError::NEW_OOM;
            return CArrUI8{nullptr, 0};
        }
        *errCode = NWebError::NO_ERROR;
        return CArrUI8{result, msgArr.size()};
    }

    CArrValue FfiOHOSJsMessageExtImplGetArray(int64_t jsExtId, int32_t *errCode)
    {
        CArrValue ret = { .strHead = nullptr, .intHead = nullptr, .doubleHead = nullptr,
            .boolHead = nullptr, .size = 0 };
        WebJsMessageExtImpl* webJsMessageExt = FFIData::GetData<WebJsMessageExtImpl>(jsExtId);
        if (webJsMessageExt == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAY)) {
            *errCode = NWebError::TYPE_NOT_MATCH_WITCH_VALUE;
            return ret;
        }
        auto data = webJsMessageExt->GetJsMsgResult();
        if (data == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        return ConvertToCArr(data, errCode);
    }
}
}
}