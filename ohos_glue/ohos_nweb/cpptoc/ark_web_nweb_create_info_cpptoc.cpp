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

#include "ohos_nweb/cpptoc/ark_web_nweb_create_info_cpptoc.h"

#include "ohos_nweb/cpptoc/ark_web_engine_init_args_cpptoc.h"
#include "ohos_nweb/cpptoc/ark_web_output_frame_callback_cpptoc.h"

#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

uint32_t ARK_WEB_CALLBACK ark_web_nweb_create_info_get_width(struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

    // Execute
    return ArkWebNWebCreateInfoCppToC::Get(self)->GetWidth();
}

uint32_t ARK_WEB_CALLBACK ark_web_nweb_create_info_get_height(struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

    // Execute
    return ArkWebNWebCreateInfoCppToC::Get(self)->GetHeight();
}

bool ARK_WEB_CALLBACK ark_web_nweb_create_info_get_is_incognito_mode(struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

    // Execute
    return ArkWebNWebCreateInfoCppToC::Get(self)->GetIsIncognitoMode();
}

void* ARK_WEB_CALLBACK ark_web_nweb_create_info_get_producer_surface(struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

    // Execute
    return ArkWebNWebCreateInfoCppToC::Get(self)->GetProducerSurface();
}

void* ARK_WEB_CALLBACK ark_web_nweb_create_info_get_enhance_surface_info(struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

    // Execute
    return ArkWebNWebCreateInfoCppToC::Get(self)->GetEnhanceSurfaceInfo();
}

ark_web_engine_init_args_t* ARK_WEB_CALLBACK ark_web_nweb_create_info_get_engine_init_args(
    struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

    // Execute
    ArkWebRefPtr<ArkWebEngineInitArgs> _retval = ArkWebNWebCreateInfoCppToC::Get(self)->GetEngineInitArgs();

    // Return type: refptr_same
    return ArkWebEngineInitArgsCppToC::Invert(_retval);
}

ark_web_output_frame_callback_t* ARK_WEB_CALLBACK ark_web_nweb_create_info_get_output_frame_callback(
    struct _ark_web_nweb_create_info_t* self)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

    // Execute
    ArkWebRefPtr<ArkWebOutputFrameCallback> _retval = ArkWebNWebCreateInfoCppToC::Get(self)->GetOutputFrameCallback();

    // Return type: refptr_same
    return ArkWebOutputFrameCallbackCppToC::Invert(_retval);
}

} // namespace

ArkWebNWebCreateInfoCppToC::ArkWebNWebCreateInfoCppToC()
{
    GetStruct()->get_width = ark_web_nweb_create_info_get_width;
    GetStruct()->get_height = ark_web_nweb_create_info_get_height;
    GetStruct()->get_is_incognito_mode = ark_web_nweb_create_info_get_is_incognito_mode;
    GetStruct()->get_producer_surface = ark_web_nweb_create_info_get_producer_surface;
    GetStruct()->get_enhance_surface_info = ark_web_nweb_create_info_get_enhance_surface_info;
    GetStruct()->get_engine_init_args = ark_web_nweb_create_info_get_engine_init_args;
    GetStruct()->get_output_frame_callback = ark_web_nweb_create_info_get_output_frame_callback;
}

ArkWebNWebCreateInfoCppToC::~ArkWebNWebCreateInfoCppToC() {}

template<>
ArkWebBridgeType
    ArkWebCppToCRefCounted<ArkWebNWebCreateInfoCppToC, ArkWebNWebCreateInfo, ark_web_nweb_create_info_t>::kBridgeType =
        ARK_WEB_NWEB_CREATE_INFO;

} // namespace OHOS::ArkWeb