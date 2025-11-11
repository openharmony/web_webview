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
#include <fuzzer/FuzzedDataProvider.h>
#include "mediacodecencoderadapterimpl_fuzzer.h"

#include "avsharedmemory.h"
#include "avsharedmemorybase.h"
#include "media_codec_encoder_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 255;

class EncoderCallbackAdapterMock : public CodecCallbackAdapter {
public:
    EncoderCallbackAdapterMock() = default;

    ~EncoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(const std::shared_ptr<CodecFormatAdapter> formatApadter_) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag,
        std::shared_ptr<OhosBufferAdapter> buffer) override
    {}
};

class CodecConfigParaAdapterMock : public CodecConfigParaAdapter {
public:
    CodecConfigParaAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    int64_t GetBitRate() override
    {
        return bitRate;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    int32_t width;
    int32_t height;
    int64_t bitRate;
    double frameRate;
};

bool CreateLocationProxyAdapterFuzzTest_01(const uint8_t* data, size_t size)
{
    NWeb::MediaCodecEncoderAdapterImpl mediaCodecEncoderAdapterImpl;
    std::shared_ptr<NWeb::CodecCallbackAdapter> callbackImpl = std::make_shared<EncoderCallbackAdapterMock>();
    std::shared_ptr<NWeb::CodecConfigParaAdapter> config = std::make_shared<CodecConfigParaAdapterMock>();

    FuzzedDataProvider dataProvider(data, size);
    std::string mimeParam = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    mediaCodecEncoderAdapterImpl.CreateVideoCodecByMime(mimeParam);
    mediaCodecEncoderAdapterImpl.SetCodecCallback(nullptr);
    mediaCodecEncoderAdapterImpl.SetCodecCallback(callbackImpl);
    mediaCodecEncoderAdapterImpl.Configure(nullptr);
    mediaCodecEncoderAdapterImpl.Configure(config);
    mediaCodecEncoderAdapterImpl.Prepare();
    mediaCodecEncoderAdapterImpl.Start();
    mediaCodecEncoderAdapterImpl.Stop();
    mediaCodecEncoderAdapterImpl.Reset();
    mediaCodecEncoderAdapterImpl.Release();
    mediaCodecEncoderAdapterImpl.CreateInputSurface();
    mediaCodecEncoderAdapterImpl.ReleaseOutputBuffer(0, true);
    mediaCodecEncoderAdapterImpl.RequestKeyFrameSoon();
    mediaCodecEncoderAdapterImpl.GetErrorType(AVCodecErrorType::AVCODEC_ERROR_INTERNAL);
    mediaCodecEncoderAdapterImpl.GetBufferFlag(AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE);

    std::shared_ptr<NWeb::CodecCallbackAdapter> cb = nullptr;
    std::shared_ptr<Media::AVSharedMemory> memory = std::make_shared<Media::AVSharedMemoryBase>(1, 1.0, "test");

    NWeb::EncoderCallbackImpl encoderCallback(cb);

    encoderCallback.OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_INTERNAL, 0);
    encoderCallback.OnOutputFormatChanged(OHOS::MediaAVCodec::Format());
    encoderCallback.OnInputBufferAvailable(0, nullptr);
    encoderCallback.OnInputBufferAvailable(1, memory);
    encoderCallback.OnOutputBufferAvailable(0, OHOS::MediaAVCodec::AVCodecBufferInfo(),
        OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, nullptr);
    mediaCodecEncoderAdapterImpl.CreateVideoCodecByName("video/avc");
    return true;
}

bool CreateLocationProxyAdapterFuzzTest_02(const uint8_t* data, size_t size)
{
    NWeb::MediaCodecEncoderAdapterImpl mediaCodecEncoderAdapterImpl;
    std::shared_ptr<NWeb::CodecCallbackAdapter> callbackImpl = std::make_shared<EncoderCallbackAdapterMock>();
    std::shared_ptr<NWeb::CodecConfigParaAdapter> config = std::make_shared<CodecConfigParaAdapterMock>();

    FuzzedDataProvider dataProvider(data, size);
    std::string mimeParam = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    mediaCodecEncoderAdapterImpl.CreateVideoCodecByMime(mimeParam);
    mediaCodecEncoderAdapterImpl.CreateVideoCodecByMime("video/avc");
    mediaCodecEncoderAdapterImpl.SetCodecCallback(nullptr);
    mediaCodecEncoderAdapterImpl.SetCodecCallback(callbackImpl);
    mediaCodecEncoderAdapterImpl.Configure(nullptr);
    mediaCodecEncoderAdapterImpl.Configure(config);
    mediaCodecEncoderAdapterImpl.Prepare();
    mediaCodecEncoderAdapterImpl.Start();
    mediaCodecEncoderAdapterImpl.Stop();
    mediaCodecEncoderAdapterImpl.Reset();
    mediaCodecEncoderAdapterImpl.Release();
    mediaCodecEncoderAdapterImpl.CreateInputSurface();
    mediaCodecEncoderAdapterImpl.ReleaseOutputBuffer(0, true);
    mediaCodecEncoderAdapterImpl.RequestKeyFrameSoon();
    mediaCodecEncoderAdapterImpl.GetErrorType(AVCodecErrorType::AVCODEC_ERROR_INTERNAL);
    mediaCodecEncoderAdapterImpl.GetBufferFlag(AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE);

    std::shared_ptr<Media::AVSharedMemory> memory = std::make_shared<Media::AVSharedMemoryBase>(1, 1.0, "test");
    NWeb::EncoderCallbackImpl encoderCallback(callbackImpl);
    encoderCallback.OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_INTERNAL, 0);
    encoderCallback.OnOutputFormatChanged(OHOS::MediaAVCodec::Format());
    encoderCallback.OnInputBufferAvailable(0, nullptr);
    encoderCallback.OnInputBufferAvailable(1, memory);
    encoderCallback.OnOutputBufferAvailable(0, OHOS::MediaAVCodec::AVCodecBufferInfo(),
        OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, nullptr);
    mediaCodecEncoderAdapterImpl.CreateVideoCodecByName("video/avc");
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CreateLocationProxyAdapterFuzzTest_01(data, size);
    OHOS::CreateLocationProxyAdapterFuzzTest_02(data, size);
    return 0;
}