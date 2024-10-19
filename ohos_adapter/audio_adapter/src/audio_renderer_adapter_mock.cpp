/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "audio_renderer_adapter_impl.h"

namespace OHOS::NWeb {
AudioRendererAdapterImpl::~AudioRendererAdapterImpl() {}

int32_t AudioRendererAdapterImpl::Create(
    const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions, std::string cachePath)
{
    return -1;
}

bool AudioRendererAdapterImpl::Start()
{
    return false;
}

bool AudioRendererAdapterImpl::Pause()
{
    return false;
}

bool AudioRendererAdapterImpl::Stop()
{
    return false;
}

bool AudioRendererAdapterImpl::Release()
{
    return false;
}

int32_t AudioRendererAdapterImpl::Write(uint8_t* buffer, size_t bufferSize)
{
    return -1;
}

int32_t AudioRendererAdapterImpl::GetLatency(uint64_t& latency)
{
    return -1;
}

int32_t AudioRendererAdapterImpl::SetVolume(float volume)
{
    return -1;
}

float AudioRendererAdapterImpl::GetVolume()
{
    return -1;
}

int32_t AudioRendererAdapterImpl::SetAudioRendererCallback(
    const std::shared_ptr<AudioRendererCallbackAdapter>& callback)
{
    return -1;
}

void AudioRendererAdapterImpl::SetInterruptMode(bool audioExclusive) {}

bool AudioRendererAdapterImpl::IsRendererStateRunning()
{
    return false;
}
} // namespace OHOS::NWeb
