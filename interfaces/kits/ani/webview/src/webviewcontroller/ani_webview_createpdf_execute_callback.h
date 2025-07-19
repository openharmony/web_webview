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
#ifndef OHOS_NWEB_ANI_WEBVIEW_CREAT_PDF_EXECUTE_CALLBACK_H
#define OHOS_NWEB_ANI_WEBVIEW_CREAT_PDF_EXECUTE_CALLBACK_H
#include <string>
#include <ani.h>
#include <new>
#include <securec.h>
#include <mutex>
#include "nweb_value_callback.h"

namespace OHOS::NWeb {
typedef void (*PdfCallbackFunc)(ani_env *env, const char* result, const long size, ani_ref callbackRef);

class WebJsArrayBufferExt {
public:
    explicit WebJsArrayBufferExt(const char* value, const long size) : size_(size)
    {
        if (value != nullptr && size > 0) {
            value_ = new (std::nothrow) char[size];
            if (value_ != nullptr) {
                if (memcpy_s(value_, size, value, size) != 0) {
                    delete[] value_;
                    value_ = nullptr;
                    size_ = 0;
                }
            }
        }
    };
    ~WebJsArrayBufferExt()
    {
        size_ = 0;
        if (value_ != nullptr) {
            delete[] value_;
            value_ = nullptr;
        }
    }

    const char* GetPDFResult() const
    {
        return value_;
    }

    long GetPDFSize() const
    {
        return size_;
    }

private:
    char* value_ = nullptr;
    long size_ = 0;
};

class WebviewCreatePDFExecuteCallback : public NWebArrayBufferValueCallback {
public:
    explicit WebviewCreatePDFExecuteCallback(ani_env *env, const PdfCallbackFunc& callback, ani_object callbackObject);
    ~WebviewCreatePDFExecuteCallback();
    void OnReceiveValue(const char* value, const long size) override;

private:
    ani_vm *aniVm_ = nullptr;
    ani_ref callbackRef_ = nullptr;
    PdfCallbackFunc callback_ = nullptr;
    char* result_;
    long size_;

    void ThreadAfterCb(ani_env *env);
    void ReleaseBuffer();
};

ani_status StsPdfDataInit(ani_env *env);
} // namespace OHOS::NWeb
#endif