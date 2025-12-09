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

#ifndef ARK_WEB_FILE_SELECTOR_PARAMS_WRAPPER_H_
#define ARK_WEB_FILE_SELECTOR_PARAMS_WRAPPER_H_
#pragma once

#include "include/nweb_file_selector_params.h"
#include "ohos_nweb/include/ark_web_file_selector_params.h"

namespace OHOS::ArkWeb {

using ArkWebFileSelectorMode = OHOS::NWeb::NWebFileSelectorParams::FileSelectorMode;

class ArkWebFileSelectorParamsWrapper : public OHOS::NWeb::NWebFileSelectorParams {
public:
    ArkWebFileSelectorParamsWrapper(ArkWebRefPtr<ArkWebFileSelectorParams> ark_web_file_selector_params);
    ~ArkWebFileSelectorParamsWrapper() = default;

    ArkWebFileSelectorMode Mode() override;

    const std::string Title() override;

    bool IsCapture() override;

    const std::vector<std::string> AcceptType() override;

    const std::string DefaultFilename() override;

    const std::vector<std::string> MimeType() override;

    const std::string DefaultPath() override;

    const std::vector<std::string> Descriptions() override;

    bool IsAcceptAllOptionExcluded() override;

    const std::string Accepts() override;
private:
    ArkWebRefPtr<ArkWebFileSelectorParams> ark_web_file_selector_params_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FILE_SELECTOR_PARAMS_WRAPPER_H_
