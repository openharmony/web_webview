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

#ifndef ARK_WEB_DATE_TIME_CHOOSER_CTOCPP_H_
#define ARK_WEB_DATE_TIME_CHOOSER_CTOCPP_H_
#pragma once

#include "ohos_nweb/capi/ark_web_date_time_chooser_capi.h"
#include "ohos_nweb/include/ark_web_date_time_chooser.h"

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkWebDateTimeChooserCToCpp
    : public ArkWebCToCppRefCounted<ArkWebDateTimeChooserCToCpp, ArkWebDateTimeChooser, ark_web_date_time_chooser_t> {
public:
    ArkWebDateTimeChooserCToCpp();
    virtual ~ArkWebDateTimeChooserCToCpp();

    // ArkWebDateTimeChooser methods.
    int GetType() override;

    double GetStep() override;

    ArkWebDateTime GetMinimum() override;

    ArkWebDateTime GetMaximum() override;

    ArkWebDateTime GetDialogValue() override;

    bool GetHasSelected() override;

    size_t GetSuggestionIndex() override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DATE_TIME_CHOOSER_CTOCPP_H_