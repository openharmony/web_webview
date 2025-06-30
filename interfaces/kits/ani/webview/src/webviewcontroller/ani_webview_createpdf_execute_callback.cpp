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
        WVLOG_E("[CreatePDF] invalid PDF result or size");
        return nullptr;
    }
    ani_arraybuffer arraybuffer;
    void* bufferData = nullptr;
    env->CreateArrayBuffer(size, &bufferData, &arraybuffer);
    if (bufferData == nullptr) {
        WVLOG_E("[CreatePDF] bufferData is null after array buffer creation");
        return nullptr;
    }
    if (memcpy_s(bufferData, size, pdfResult, size) != 0) {
        WVLOG_E("[CreatePDF] memcpy failed");
        return nullptr;
    }

    ani_class cls;
    ani_method ctor;
    if (env->FindClass("Lstd/core/ArrayBuffer;", &cls) != ANI_OK) {
        return result;
    }
    if (env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor) != ANI_OK) {
        return result;
    }

    ani_object arrayObject;
    env->Object_New(cls, ctor, &arrayObject, arraybuffer);
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
       ani_native_function { "pdfArrayBuffer", nullptr, reinterpret_cast<void*>(GetArrayBuffer) },
   };

   status = env->Class_BindNativeMethods(pdfDataCls, allMethods.data(), allMethods.size());
   if (status != ANI_OK) {
       WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
   }
   return ANI_OK;
}
} // namespace OHOS::NWeb