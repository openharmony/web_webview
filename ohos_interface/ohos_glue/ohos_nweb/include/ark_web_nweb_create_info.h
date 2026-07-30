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

#ifndef ARK_WEB_NWEB_CREATE_INFO_H_
#define ARK_WEB_NWEB_CREATE_INFO_H_
#pragma once

#include "ohos_nweb/include/ark_web_engine_init_args.h"
#include "ohos_nweb/include/ark_web_output_frame_callback.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebNWebCreateInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual uint32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual bool GetIsIncognitoMode() = 0;

    /*--ark web()--*/
    virtual void* GetProducerSurface() = 0;

    /*--ark web()--*/
    virtual void* GetEnhanceSurfaceInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebEngineInitArgs> GetEngineInitArgs() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebOutputFrameCallback> GetOutputFrameCallback() = 0;

    /*--ark web()--*/
    virtual uint64_t GetAsyncStackId() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NWEB_CREATE_INFO_H_
