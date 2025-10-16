/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "mediaavsessioncallback_fuzzer.h"

#define private public
#include <cstdlib>
#include <ctime>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "media_avsession_adapter.h"
#include "media_avsession_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() {};
    void Pause() {};
    void Stop() {};
    void SeekTo(int64_t millisTime) {};
};

bool MediaAVSessionAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    std::shared_ptr<MediaAVSessionCallbackAdapterMock> mediaAvSessionCB;
    std::shared_ptr<MediaAVSessionCallbackImpl> callbackImpl =
        std::make_shared<MediaAVSessionCallbackImpl>(mediaAvSessionCB);
    FuzzedDataProvider fdp(data, size);

    std::shared_ptr<MediaAVSessionKey> key = std::make_shared<MediaAVSessionKey>();
    key->Init();
    key->GetPID();
    key->GetElement();
    key->GetType();
    key->ToString();

    AVSession::AVControlCommand cmd;
    callbackImpl->OnPlay(cmd);
    callbackImpl->OnPause();
    callbackImpl->OnStop();

    callbackImpl->OnPlayNext(cmd);
    callbackImpl->OnPlayPrevious(cmd);

    int64_t time = fdp.ConsumeIntegralInRange<int64_t>(0, MAX_SET_NUMBER);
    callbackImpl->OnSeek(time);
    callbackImpl->OnFastForward(time, cmd);
    callbackImpl->OnRewind(time, cmd);

    auto speed = fdp.ConsumeFloatingPoint<float>();
    callbackImpl->OnSetSpeed(speed);

    auto loopMode = fdp.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    callbackImpl->OnSetLoopMode(loopMode);

    std::string assertId = fdp.ConsumeRandomLengthString(32);
    callbackImpl->OnToggleFavorite(assertId);

    auto itemId = fdp.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    callbackImpl->OnSkipToQueueItem(itemId);
    callbackImpl->OnAVCallAnswer();
    callbackImpl->OnAVCallHangUp();
    callbackImpl->OnAVCallToggleCallMute();

    auto assetId = fdp.ConsumeIntegralInRange<int64_t>(0, MAX_SET_NUMBER);
    callbackImpl->OnPlayFromAssetId(assetId);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::MediaAVSessionAdapterImplFuzzTest(data, size);
    return 0;
}