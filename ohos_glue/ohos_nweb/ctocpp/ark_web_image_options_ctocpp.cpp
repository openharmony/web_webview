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

#include "ohos_nweb/ctocpp/ark_web_image_options_ctocpp.h"

#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
size_t ArkWebImageOptionsCToCpp::GetWidth()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_image_options_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_width, 0);

    // Execute
    return _struct->get_width(_struct);
}

ARK_WEB_NO_SANITIZE
size_t ArkWebImageOptionsCToCpp::GetHeight()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_image_options_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_height, 0);

    // Execute
    return _struct->get_height(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebImageOptionsCToCpp::GetAlphaType()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_image_options_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_alpha_type, 0);

    // Execute
    return _struct->get_alpha_type(_struct);
}

ARK_WEB_NO_SANITIZE
int ArkWebImageOptionsCToCpp::GetColorType()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_web_image_options_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_color_type, 0);

    // Execute
    return _struct->get_color_type(_struct);
}

ArkWebImageOptionsCToCpp::ArkWebImageOptionsCToCpp() {}

ArkWebImageOptionsCToCpp::~ArkWebImageOptionsCToCpp() {}

template<>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkWebImageOptionsCToCpp, ArkWebImageOptions, ark_web_image_options_t>::kBridgeType =
        ARK_WEB_IMAGE_OPTIONS;

} // namespace OHOS::ArkWeb