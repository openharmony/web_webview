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

#include "cpptoc/ark_producer_surface_adapter_cpptoc.h"

#include "cpptoc/ark_surface_buffer_adapter_cpptoc.h"
#include "cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

ark_surface_buffer_adapter_t* ARK_WEB_CALLBACK ark_producer_surface_adapter_request_buffer(
    struct _ark_producer_surface_adapter_t* self, int32_t* fence, ArkBufferRequestConfigAdapter* config)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, NULL);

    ARK_WEB_CPPTOC_CHECK_PARAM(fence, NULL);

    ARK_WEB_CPPTOC_CHECK_PARAM(config, NULL);

    // Execute
    ArkWebRefPtr<ArkSurfaceBufferAdapter> _retval =
        ArkProducerSurfaceAdapterCppToC::Get(self)->RequestBuffer(*fence, *config);

    // Return type: refptr_same
    return ArkSurfaceBufferAdapterCppToC::Invert(_retval);
}

int32_t ARK_WEB_CALLBACK ark_producer_surface_adapter_flush_buffer(struct _ark_producer_surface_adapter_t* self,
    ark_surface_buffer_adapter_t* buffer, int32_t fence, ArkBufferFlushConfigAdapter* flushConfig)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, 0);

    ARK_WEB_CPPTOC_CHECK_PARAM(flushConfig, 0);

    // Execute
    return ArkProducerSurfaceAdapterCppToC::Get(self)->FlushBuffer(
        ArkSurfaceBufferAdapterCppToC::Revert(buffer), fence, *flushConfig);
}

} // namespace

ArkProducerSurfaceAdapterCppToC::ArkProducerSurfaceAdapterCppToC()
{
    GetStruct()->request_buffer = ark_producer_surface_adapter_request_buffer;
    GetStruct()->flush_buffer = ark_producer_surface_adapter_flush_buffer;
}

ArkProducerSurfaceAdapterCppToC::~ArkProducerSurfaceAdapterCppToC() {}

template<>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkProducerSurfaceAdapterCppToC, ArkProducerSurfaceAdapter,
    ark_producer_surface_adapter_t>::kBridgeType = ARK_PRODUCER_SURFACE_ADAPTER;

} // namespace OHOS::ArkWeb