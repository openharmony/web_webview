/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef ARK_SCREEN_CAPTURE_ADAPTER_H
#define ARK_SCREEN_CAPTURE_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {
/*--ark web(source=webcore)--*/
class ArkAudioCaptureInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetAudioSampleRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetAudioChannels() = 0;

    /*--ark web()--*/
    virtual int32_t GetAudioSource() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioEncInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetAudioBitrate() = 0;

    /*--ark web()--*/
    virtual int32_t GetAudioCodecformat() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioCaptureInfoAdapter> GetMicCapInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioCaptureInfoAdapter> GetInnerCapInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioEncInfoAdapter> GetAudioEncInfo() = 0;
};

/*--ark web(source=webcore)--*/
class ArkVideoCaptureInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint64_t GetDisplayId() = 0;

    /*--ark web()--*/
    virtual ArkWebInt32List GetTaskIDs() = 0;

    /*--ark web()--*/
    virtual int32_t GetVideoFrameWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetVideoFrameHeight() = 0;

    /*--ark web()--*/
    virtual int32_t GetVideoSourceType() = 0;
};

/*--ark web(source=webcore)--*/
class ArkVideoEncInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetVideoCodecFormat() = 0;

    /*--ark web()--*/
    virtual int32_t GetVideoBitrate() = 0;

    /*--ark web()--*/
    virtual int32_t GetVideoFrameRate() = 0;
};

/*--ark web(source=webcore)--*/
class ArkVideoInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVideoCaptureInfoAdapter> GetVideoCapInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVideoEncInfoAdapter> GetVideoEncInfo() = 0;
};

/*--ark web(source=webcore)--*/
class ArkRecorderInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebString GetUrl() = 0;

    /*--ark web()--*/
    virtual int32_t GetFileFormat() = 0;
};

/*--ark web(source=webcore)--*/
class ArkScreenCaptureConfigAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetCaptureMode() = 0;

    /*--ark web()--*/
    virtual int32_t GetDataType() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioInfoAdapter> GetAudioInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVideoInfoAdapter> GetVideoInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkRecorderInfoAdapter> GetRecorderInfo() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint8_t* GetBuffer() = 0;

    /*--ark web()--*/
    virtual int32_t GetLength() = 0;

    /*--ark web()--*/
    virtual int64_t GetTimestamp() = 0;

    /*--ark web()--*/
    virtual int32_t GetSourcetype() = 0;

    /*--ark web()--*/
    virtual void SetBuffer(uint8_t* buffer) = 0;

    /*--ark web()--*/
    virtual void SetLength(int32_t length) = 0;

    /*--ark web()--*/
    virtual void SetTimestamp(int64_t timestamp) = 0;

    /*--ark web()--*/
    virtual void SetSourcetype(int32_t sourcetype) = 0;
};

/*--ark web(source=webcore)--*/
class ArkScreenCaptureCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnError(int32_t errorCode) = 0;

    /*--ark web()--*/
    virtual void OnAudioBufferAvailable(bool isReady, int32_t type) = 0;

    /*--ark web()--*/
    virtual void OnVideoBufferAvailable(bool isReady) = 0;

    /*--ark web()--*/
    virtual void OnStateChange(int32_t stateCode) = 0;
};

/*--ark web(source=webview)--*/
class ArkScreenCaptureAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t Init(const ArkWebRefPtr<ArkScreenCaptureConfigAdapter> config) = 0;

    /*--ark web()--*/
    virtual int32_t SetMicrophoneEnable(bool enable) = 0;

    /*--ark web()--*/
    virtual int32_t StartCapture() = 0;

    /*--ark web()--*/
    virtual int32_t StopCapture() = 0;

    /*--ark web()--*/
    virtual int32_t SetCaptureCallback(const ArkWebRefPtr<ArkScreenCaptureCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkSurfaceBufferAdapter> AcquireVideoBuffer() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseVideoBuffer() = 0;

    /*--ark web()--*/
    virtual int32_t AcquireAudioBuffer(
        ArkWebRefPtr<ArkAudioBufferAdapter> audiobuffer, int32_t type) = 0;
    
    /*--ark web()--*/
    virtual int32_t ReleaseAudioBuffer(int32_t type) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_SCREEN_CAPTURE_ADAPTER_H
