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

#ifndef ARK_WEB_FILE_SELECTOR_PARAMS_IMPL_H_
#define ARK_WEB_FILE_SELECTOR_PARAMS_IMPL_H_
#pragma once

#include "include/nweb_file_selector_params.h"
#include "ohos_nweb/include/ark_web_file_selector_params.h"

namespace OHOS::ArkWeb {

class ArkWebFileSelectorParamsImpl : public ArkWebFileSelectorParams {
    IMPLEMENT_REFCOUNTING(ArkWebFileSelectorParamsImpl);

public:
    ArkWebFileSelectorParamsImpl(std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> nweb_file_selector_params);
    ~ArkWebFileSelectorParamsImpl() = default;

    int Mode() override;

    const ArkWebString Title() override;

    bool IsCapture() override;

    const ArkWebStringVector AcceptType() override;

    const ArkWebString DefaultFilename() override;

    const ArkWebStringVector MimeType() override;

    const ArkWebString DefaultPath() override;

    const ArkWebStringVector Descriptions() override;

    bool IsAcceptAllOptionExcluded() override;
private:
    std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> nweb_file_selector_params_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FILE_SELECTOR_PARAMS_IMPL_H_
