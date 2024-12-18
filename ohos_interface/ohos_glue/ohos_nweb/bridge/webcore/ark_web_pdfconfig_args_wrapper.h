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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_PDFCONFIG_ARGS_WRAPPER_H_
#define ARK_WEB_PDFCONFIG_ARGS_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_pdfconfig_args.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebPDFConfigArgsWrapper : public OHOS::NWeb::NWebPDFConfigArgs {
public:
    ArkWebPDFConfigArgsWrapper(ArkWebRefPtr<ArkWebPDFConfigArgs> ark_web_pdfconfig_args);
    ~ArkWebPDFConfigArgsWrapper() = default;

    double GetWidth() override;
    double GetHeight() override;
    double GetScale() override;
    double GetMarginTop() override;
    double GetMarginBottom() override;
    double GetMarginRight() override;
    double GetMarginLeft() override;
    bool GetShouldPrintBackground() override;
private:
    ArkWebRefPtr<ArkWebPDFConfigArgs> ark_web_pdfconfig_args_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PDFCONFIG_ARGS_WRAPPER_H_
