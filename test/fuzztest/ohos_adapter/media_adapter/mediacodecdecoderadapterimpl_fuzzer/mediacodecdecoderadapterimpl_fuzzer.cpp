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

#include "mediacodecdecoderadapterimpl_fuzzer.h"

#include <cstring>

#include "avsharedmemory.h"
#include "avsharedmemorybase.h"
#include "media_codec_decoder_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
class DecoderCallbackAdapterMock : public DecoderCallbackAdapter {
public:
    DecoderCallbackAdapterMock() = default;

    ~DecoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(int32_t width, int32_t height, double frameRate) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag) override {}
};

class DecoderFormatAdapterMock : public DecoderFormatAdapter {
public:
    DecoderFormatAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    void SetFrameRate(double fr) override
    {
        frameRate = fr;
    }

    int32_t width;
    int32_t height;
    double frameRate;
};

bool MediaCodecDecoderAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    NWeb::MediaCodecDecoderAdapterImpl mediaCodecDecoderAdapterImpl;
    NWeb::DecoderAdapterCode code = mediaCodecDecoderAdapterImpl.CreateVideoDecoderByMime("testmimeType");
    std::shared_ptr<NWeb::DecoderFormatAdapter> format = std::make_unique<DecoderFormatAdapterMock>();
    AVCodecBufferInfo info;
    code = mediaCodecDecoderAdapterImpl.CreateVideoDecoderByName("testname");
    code = mediaCodecDecoderAdapterImpl.ConfigureDecoder(format);
    code = mediaCodecDecoderAdapterImpl.SetParameterDecoder(format);

    void* window = nullptr;
    code = mediaCodecDecoderAdapterImpl.SetOutputSurface(window);
    code = mediaCodecDecoderAdapterImpl.PrepareDecoder();
    code = mediaCodecDecoderAdapterImpl.StartDecoder();
    code = mediaCodecDecoderAdapterImpl.StopDecoder();
    code = mediaCodecDecoderAdapterImpl.FlushDecoder();
    code = mediaCodecDecoderAdapterImpl.ResetDecoder();
    code = mediaCodecDecoderAdapterImpl.ReleaseDecoder();

    code = mediaCodecDecoderAdapterImpl.QueueInputBufferDec(0, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE);

    code = mediaCodecDecoderAdapterImpl.GetOutputFormatDec(format);

    code = mediaCodecDecoderAdapterImpl.ReleaseOutputBufferDec(0, true);

    std::shared_ptr<NWeb::DecoderCallbackImpl> decoderCallback = std::make_shared<NWeb::DecoderCallbackImpl>(nullptr);

    decoderCallback->OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_INTERNAL, 0);
    decoderCallback->OnOutputFormatChanged(OHOS::MediaAVCodec::Format());
    decoderCallback->OnInputBufferAvailable(0, nullptr);
    decoderCallback->OnOutputBufferAvailable(0, OHOS::MediaAVCodec::AVCodecBufferInfo(),
        OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, nullptr);

    std::shared_ptr<NWeb::DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    std::shared_ptr<NWeb::DecoderCallbackImpl> decoderCallbackNoNull =
        std::make_shared<NWeb::DecoderCallbackImpl>(callback);
    decoderCallbackNoNull->OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_INTERNAL, 0);
    decoderCallbackNoNull->OnOutputFormatChanged(OHOS::MediaAVCodec::Format());
    decoderCallbackNoNull->OnOutputBufferAvailable(
        0, info, OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, nullptr);
    std::shared_ptr<Media::AVSharedMemory> memory = std::make_shared<Media::AVSharedMemoryBase>(1, 1.0, "test");
    decoderCallbackNoNull->OnInputBufferAvailable(1, memory);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MediaCodecDecoderAdapterImplFuzzTest(data, size);
    return 0;
}