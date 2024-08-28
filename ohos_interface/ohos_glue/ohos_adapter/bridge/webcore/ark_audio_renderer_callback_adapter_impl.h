/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_AUDIO_RENDERER_CALLBACK_ADAPTER_IMPL_H
#define ARK_AUDIO_RENDERER_CALLBACK_ADAPTER_IMPL_H
#pragma once

#include "audio_renderer_adapter.h"
#include "ohos_adapter/include/ark_audio_renderer_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioRendererCallbackAdapterImpl : public ArkAudioRendererCallbackAdapter {
public:
    explicit ArkAudioRendererCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioRendererCallbackAdapter>);

    void OnSuspend() override;

    void OnResume() override;

    int32_t OnWriteDataCallback(void* buffer, int32_t length) override;

private:
    std::shared_ptr<OHOS::NWeb::AudioRendererCallbackAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioRendererCallbackAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_RENDERER_CALLBACK_ADAPTER_IMPL_H
