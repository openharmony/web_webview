/*
* Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ani_webview_createpdf_execute_callback.h"

#include <fstream>
#include <iostream>
#include <securec.h>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"
namespace OHOS::NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace name {
const char* JS_EXT_ARR_CLASS_NAME = "L@ohos/web/webview/webview/PdfData;";
} // namespace name

WebviewCreatePDFExecuteCallback::WebviewCreatePDFExecuteCallback(ani_env *env,
    const  PdfCallbackFunc& callback, ani_object callbackObject)
{
    if (env == nullptr || callback == nullptr) {
        WVLOG_E("nullptr error");
        return;
    }
    if (env->GlobalReference_Create(callbackObject, &callbackRef_) != ANI_OK) {
        WVLOG_E("failed to create reference for callback");
        return;
    }
    ani_vm *vm = nullptr;
    env->GetVM(&vm);
    aniVm_ = vm;
    callback_ = callback;
}

WebviewCreatePDFExecuteCallback::~WebviewCreatePDFExecuteCallback()
{
    aniVm_ = nullptr;
    callback_ = nullptr;
    ReleaseBuffer();
}

void WebviewCreatePDFExecuteCallback::ReleaseBuffer()
{
    if (result_ != nullptr) {
        delete[] result_;
        result_ = nullptr;
    }
    if (callbackRef_ != nullptr && aniVm_ != nullptr) {
        ani_env *env;
        auto status = aniVm_->GetEnv(ANI_VERSION_1, &env);
        if (status != ANI_OK) {
            WVLOG_E("vm GetEnv, err: %{private}d", status);
            return;
        }
        env->GlobalReference_Delete(callbackRef_);
    }
}

void WebviewCreatePDFExecuteCallback::OnReceiveValue(const char* value, const long size)
{
    WVLOG_I("CreatePdf OnReceiveValue in");
    if (aniVm_ == nullptr || callback_ == nullptr) {
        WVLOG_E("aniVm_ or callback_ is nullptr");
        return;
    }
    ani_env *env;
    ani_options aniArgs { 0, nullptr };
    auto status = aniVm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        status = aniVm_->GetEnv(ANI_VERSION_1, &env);
        if (status != ANI_OK) {
            WVLOG_E("vm GetEnv, err: %{private}d", status);
            ReleaseBuffer();
            return;
        }
    }

    result_ = new (std::nothrow) char[size + 1];
    if (result_ == nullptr) {
        WVLOG_E("new char failed");
        ReleaseBuffer();
        return;
    }
    if (memcpy_s(result_, size + 1, value, size) != 0) {
        WVLOG_E("[CreatePDF] memcpy failed");
        ReleaseBuffer();
        return;
    }
    size_ = size;
    ThreadAfterCb(env);
    aniVm_->DetachCurrentThread();
    return;
}

void WebviewCreatePDFExecuteCallback::ThreadAfterCb(ani_env *env)
{
    if (!env) {
        WVLOG_E("[CreatePDF] env is nullptr");
        ReleaseBuffer();
        return;
    }
    
    ani_size nr_refs = 16;
    if (env->CreateLocalScope(nr_refs) != ANI_OK) {
        WVLOG_E("[CreatePDF] CreateLocalScope failed");
        ReleaseBuffer();
        return;
    }
    callback_(env, result_, size_, std::move(callbackRef_));
    env->DestroyLocalScope();
    ReleaseBuffer();
}

static ani_object GetArrayBuffer(ani_env* env, ani_object object)
{
    ani_object result = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }
    WebJsArrayBufferExt* webArrayBufferExt = reinterpret_cast<WebJsArrayBufferExt*>(AniParseUtils::Unwrap(env, object));
    if (webArrayBufferExt == nullptr) {
        WVLOG_E("unwrap webArrayBufferExt failed.");
        return result;
    }

    const char* pdfResult = webArrayBufferExt->GetPDFResult();
    const long size = webArrayBufferExt->GetPDFSize();
    if (pdfResult == nullptr || size <= 0) {
        WVLOG_E("invalid PDF result or size");
        return nullptr;
    }
    ani_arraybuffer arraybuffer;
    void* bufferData = nullptr;
    env->CreateArrayBuffer(size, &bufferData, &arraybuffer);
    if (bufferData == nullptr) {
        WVLOG_E("bufferData is null after array buffer creation");
        return nullptr;
    }

    ani_class cls;
    ani_method ctor;
    if (env->FindClass("Lescompat/ArrayBuffer;", &cls) != ANI_OK) {
        return result;
    }
    if (env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor) != ANI_OK) {
        return result;
    }

    ani_object arrayObject = nullptr;
    ani_status status = env->Object_New(cls, ctor, &arrayObject, size);
    if (status != ANI_OK) {
        WVLOG_E("Object_New failed, status is %{public}d.", status);
        return nullptr;
    }

    for (ani_int i = 0; i < size; i++) {
        ani_int value = pdfResult[i];
        status = env->Object_CallMethodByName_Void(
            arrayObject, "set", "IB:V", i, static_cast<ani_byte>(value));
        if (status != ANI_OK) {
            WVLOG_E("arrayObject set() failed, status is %{public}d.", status);
            break;
        }
    }
    return arrayObject;
}

ani_status StsPdfDataInit(ani_env* env)
{
   if (env == nullptr) {
       WVLOG_E("env is nullptr");
       return ANI_ERROR;
   }
   ani_class pdfDataCls = nullptr;
   ani_status status = env->FindClass(name::JS_EXT_ARR_CLASS_NAME, &pdfDataCls);
   if (status != ANI_OK || !pdfDataCls) {
       WVLOG_E("find %{public}s class failed, status: %{public}d", name::JS_EXT_ARR_CLASS_NAME, status);
       return ANI_ERROR;
   }
   std::array allMethods = {
       ani_native_function { "pdfArrayBufferInternal", nullptr, reinterpret_cast<void*>(GetArrayBuffer) },
   };

   status = env->Class_BindStaticNativeMethods(pdfDataCls, allMethods.data(), allMethods.size());
   if (status != ANI_OK) {
       WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
   }
   return ANI_OK;
}
} // namespace OHOS::NWeb