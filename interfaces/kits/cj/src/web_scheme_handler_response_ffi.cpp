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

#include "web_scheme_handler_response_ffi.h"

#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "web_scheme_handler_response_impl.h"
#include "webview_controller_impl.h"
#include "webview_log.h"
#include "webview_utils.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
extern "C" {
// WebSchemeHandlerResponse
int64_t FfiWebSchemeHandlerResponseConstructor()
{
    auto nativeWebSchemeHandlerResponse = FFIData::Create<WebSchemeHandlerResponseImpl>();
    if (nativeWebSchemeHandlerResponse == nullptr) {
        WEBVIEWLOGE("new Web Scheme Handler Response failed");
        return -1;
    }
    return nativeWebSchemeHandlerResponse->GetID();
}

RetDataCString FfiWebSchemeHandlerResponseGetUrl(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return ret;
    }
    char* url = nativeWebSchemeHandlerResponse->GetUrl();
    ret.code = NWebError::NO_ERROR;
    ret.data = url ? MallocCString(url) : nullptr;
    OH_ArkWeb_ReleaseString(url);
    return ret;
}

RetDataCString FfiWebSchemeHandlerResponseGetStatusText(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return ret;
    }
    char* statusText = nativeWebSchemeHandlerResponse->GetStatusText();
    ret.code = NWebError::NO_ERROR;
    ret.data = statusText ? MallocCString(statusText) : nullptr;
    OH_ArkWeb_ReleaseString(statusText);
    return ret;
}

RetDataCString FfiWebSchemeHandlerResponseGetMimeType(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return ret;
    }
    char* mimeType = nativeWebSchemeHandlerResponse->GetMimeType();
    ret.code = NWebError::NO_ERROR;
    ret.data = mimeType ? MallocCString(mimeType) : nullptr;
    OH_ArkWeb_ReleaseString(mimeType);
    return ret;
}

RetDataCString FfiWebSchemeHandlerResponseGetEncoding(int64_t id)
{
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return ret;
    }
    char* encoding = nativeWebSchemeHandlerResponse->GetEncoding();
    ret.code = NWebError::NO_ERROR;
    ret.data = encoding ? MallocCString(encoding) : nullptr;
    OH_ArkWeb_ReleaseString(encoding);
    return ret;
}

RetDataCString FfiWebSchemeHandlerResponseGetHeaderByName(int64_t id, const char* name)
{
    std::string cname = name;
    RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return ret;
    }
    char* header = nativeWebSchemeHandlerResponse->GetHeaderByName(cname.c_str());
    ret.code = NWebError::NO_ERROR;
    ret.data = header ? MallocCString(header) : nullptr;
    OH_ArkWeb_ReleaseString(header);
    return ret;
}

int32_t FfiWebSchemeHandlerResponseGetNetErrorCode(int64_t id, int32_t* errCode)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    int32_t errorCode = nativeWebSchemeHandlerResponse->GetErrorCode();
    *errCode = NWebError::NO_ERROR;
    return errorCode;
}

int32_t FfiWebSchemeHandlerResponseGetStatus(int64_t id, int32_t* errCode)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        *errCode = NWebError::INIT_ERROR;
        return -1;
    }
    int32_t status = nativeWebSchemeHandlerResponse->GetStatus();
    *errCode = NWebError::NO_ERROR;
    return status;
}

int32_t FfiWebSchemeHandlerResponseSetStatus(int64_t id, int32_t status)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    if (!status) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetStatus(status);
}

int32_t FfiWebSchemeHandlerResponseSetStatusText(int64_t id, const char* statusText)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    std::string cstatusText = statusText;
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    if (cstatusText.length() == 0) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetStatusText(cstatusText.c_str());
}

int32_t FfiWebSchemeHandlerResponseSetUrl(int64_t id, const char* url)
{
    std::string curl = url;
    if (curl.length() == 0) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetUrl(curl.c_str());
}

int32_t FfiWebSchemeHandlerResponseSetMimeType(int64_t id, const char* mimeType)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    std::string cmimeType = mimeType;
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    if (cmimeType.length() == 0) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetMimeType(cmimeType.c_str());
}

int32_t FfiWebSchemeHandlerResponseSetEncoding(int64_t id, const char* encoding)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    std::string cencoding = encoding;
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    if (cencoding.length() == 0) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetEncoding(cencoding.c_str());
}

int32_t FfiWebSchemeHandlerResponseSetHeaderByName(int64_t id, const char* name, const char* value, bool overwrite)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    std::string cname = name;
    std::string cvalue = value;
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    if (cname.length() == 0 || cvalue.length() == 0) {
        return NWebError::PARAM_CHECK_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetHeaderByName(cname.c_str(), cvalue.c_str(), overwrite);
}

int32_t FfiWebSchemeHandlerResponseSetErrorCode(int64_t id, int32_t code)
{
    auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(id);
    if (nativeWebSchemeHandlerResponse == nullptr) {
        return NWebError::INIT_ERROR;
    }
    return nativeWebSchemeHandlerResponse->SetErrorCode(code);
}
}
} // namespace Webview
} // namespace OHOS