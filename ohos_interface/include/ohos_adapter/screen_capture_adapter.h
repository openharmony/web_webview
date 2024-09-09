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

#ifndef SCREEN_CAPTURE_ADAPTER_H
#define SCREEN_CAPTURE_ADAPTER_H

#include <list>
#include <memory>
#include <string>

#include "graphic_adapter.h"

namespace OHOS::NWeb {

enum class CaptureModeAdapter : int32_t {
    /* capture home screen */
    CAPTURE_HOME_SCREEN = 0,
    /* capture a specified screen */
    CAPTURE_SPECIFIED_SCREEN = 1,
    /* capture a specified window */
    CAPTURE_SPECIFIED_WINDOW = 2,
    CAPTURE_INVAILD = -1
};

enum class DataTypeAdapter : int32_t {
    /* YUV/RGBA/PCM, etc. original stream */
    ORIGINAL_STREAM_DATA_TYPE = 0,
    /* h264/AAC, etc. encoded stream */
    ENCODED_STREAM_DATA_TYPE = 1,
    /* mp4 file */
    CAPTURE_FILE_DATA_TYPE = 2,
    INVAILD_DATA_TYPE = -1
};

enum class AudioCaptureSourceTypeAdapter : int32_t {
    /* Invalid audio source */
    SOURCE_INVALID = -1,
    /* Default audio source */
    SOURCE_DEFAULT = 0,
    /* Microphone */
    MIC = 1,
    /* inner all PlayBack */
    ALL_PLAYBACK = 2,
    /* inner app PlayBack */
    APP_PLAYBACK = 3,
};

enum class AudioCodecFormatAdapter : int32_t {
    /* Default format */
    AUDIO_DEFAULT = 0,
    /* Advanced Audio Coding Low Complexity (AAC-LC) */
    AAC_LC = 3,
    /* Invalid value */
    AUDIO_CODEC_FORMAT_BUTT,
};

enum class VideoSourceTypeAdapter : int32_t {
    /* Unsupported App Usage. */
    /* YUV video data provided through graphic */
    VIDEO_SOURCE_SURFACE_YUV = 0,
    /* Raw encoded data provided through graphic */
    VIDEO_SOURCE_SURFACE_ES,
    /* RGBA video data provided through graphic */
    VIDEO_SOURCE_SURFACE_RGBA,
    /* Invalid value */
    VIDEO_SOURCE_BUTT
};

enum class VideoCodecFormatAdapter : int32_t {
    /* Default format */
    VIDEO_DEFAULT = 0,
    /* H.264 */
    H264 = 2,
    /* H.265/HEVC */
    H265 = 4,
    /* MPEG4 */
    MPEG4 = 6,
    /* VP8 */
    VP8 = 8,
    /* VP9 */
    VP9 = 10,
    /* Invalid format */
    VIDEO_CODEC_FORMAT_BUTT,
};

enum class ContainerFormatTypeAdapter : int32_t {
    /* Audio format type -- m4a */
    CFT_MPEG_4A_TYPE = 0,
    /* Video format type -- mp4 */
    CFT_MPEG_4_TYPE = 1,
    /* Invalid format */
    CFT_BUTT,
};

enum class ScreenCaptureStateCodeAdapter : int32_t {
    /* Screen capture state INVALID */
    SCREEN_CAPTURE_STATE_INVLID = -1,
    /* Screen capture started by user */
    SCREEN_CAPTURE_STATE_STARTED = 0,
    /* Screen capture canceled by user */
    SCREEN_CAPTURE_STATE_CANCELED = 1,
    /* ScreenCapture stopped by user */
    SCREEN_CAPTURE_STATE_STOPPED_BY_USER = 2,
    /* ScreenCapture interrupted by other screen capture */
    SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER = 3,
    /* ScreenCapture stopped by SIM call */
    SCREEN_CAPTURE_STATE_STOPPED_BY_CALL = 4,
    /* Microphone is temporarily unavailable */
    SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE = 5,
    /* Microphone is muted by user */
    SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER = 6,
    /* Microphone is unmuted by user */
    SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER = 7,
    /* Current captured screen has private window */
    SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE = 8,
    /* Private window disappeared on current captured screen*/
    SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE = 9,
};

class AudioCaptureInfoAdapter {
public:
    AudioCaptureInfoAdapter() = default;

    virtual ~AudioCaptureInfoAdapter() = default;

    /* Audio capture sample rate info */
    virtual int32_t GetAudioSampleRate() = 0;

    /* Audio capture channel info */
    virtual int32_t GetAudioChannels() = 0;

    /* Audio capture source type */
    virtual AudioCaptureSourceTypeAdapter GetAudioSource() = 0;
};

class AudioEncInfoAdapter {
public:
    AudioEncInfoAdapter() = default;

    virtual ~AudioEncInfoAdapter() = default;

    /* Audio encoder bitrate */
    virtual int32_t GetAudioBitrate() = 0;

    /* Audio codec format */
    virtual AudioCodecFormatAdapter GetAudioCodecformat() = 0;
};

class AudioInfoAdapter {
public:
    AudioInfoAdapter() = default;

    virtual ~AudioInfoAdapter() = default;

    /* Audio capture info of microphone */
    virtual std::shared_ptr<AudioCaptureInfoAdapter> GetMicCapInfo() = 0;

    /* Audio capture info of inner */
    virtual std::shared_ptr<AudioCaptureInfoAdapter> GetInnerCapInfo() = 0;

    /* Audio encoder info, no need to set, while dataType = ORIGINAL_STREAM_DATA_TYPE */
    virtual std::shared_ptr<AudioEncInfoAdapter> GetAudioEncInfo() = 0;
};

class VideoCaptureInfoAdapter {
public:
    VideoCaptureInfoAdapter() = default;

    virtual ~VideoCaptureInfoAdapter() = default;

    /* Display id, should be set while captureMode = CAPTURE_SPECIFIED_SCREEN */
    virtual uint64_t GetDisplayId() = 0;

    /* The ids of mission, should be set while captureMode = CAPTURE_SPECIFIED_WINDOW */
    virtual std::list<int32_t> GetTaskIDs() = 0;

    /* Video frame width of avscreeencapture */
    virtual int32_t GetVideoFrameWidth() = 0;

    /* Video frame height of avscreeencapture */
    virtual int32_t GetVideoFrameHeight() = 0;

    /* Video source type of avscreeencapture */
    virtual VideoSourceTypeAdapter GetVideoSourceType() = 0;
};

class VideoEncInfoAdapter {
public:
    VideoEncInfoAdapter() = default;

    virtual ~VideoEncInfoAdapter() = default;

    /* Video encoder format */
    virtual VideoCodecFormatAdapter GetVideoCodecFormat() = 0;

    /* Video encoder bitrate */
    virtual int32_t GetVideoBitrate() = 0;

    /* Video encoder frame rate */
    virtual int32_t GetVideoFrameRate() = 0;
};

class VideoInfoAdapter {
public:
    VideoInfoAdapter() = default;

    virtual ~VideoInfoAdapter() = default;

    /* Video capture info */
    virtual std::shared_ptr<VideoCaptureInfoAdapter> GetVideoCapInfo() = 0;

    /* Video encoder info */
    virtual std::shared_ptr<VideoEncInfoAdapter> GetVideoEncInfo() = 0;
};

class RecorderInfoAdapter {
public:
    RecorderInfoAdapter() = default;

    virtual ~RecorderInfoAdapter() = default;

    /* Recorder file url */
    virtual std::string GetUrl() = 0;

    /* Recorder file format */
    virtual ContainerFormatTypeAdapter GetFileFormat() = 0;
};

class ScreenCaptureConfigAdapter {
public:
    ScreenCaptureConfigAdapter() = default;

    virtual ~ScreenCaptureConfigAdapter() = default;

    virtual CaptureModeAdapter GetCaptureMode() = 0;

    virtual DataTypeAdapter GetDataType() = 0;

    virtual std::shared_ptr<AudioInfoAdapter> GetAudioInfo() = 0;

    virtual std::shared_ptr<VideoInfoAdapter> GetVideoInfo() = 0;

    virtual std::shared_ptr<RecorderInfoAdapter> GetRecorderInfo() = 0;
};

class ScreenCaptureCallbackAdapter {
public:
    ScreenCaptureCallbackAdapter() = default;

    virtual ~ScreenCaptureCallbackAdapter() = default;

    virtual void OnError(int32_t errorCode) = 0;

    virtual void OnAudioBufferAvailable(bool isReady, AudioCaptureSourceTypeAdapter type) = 0;

    virtual void OnVideoBufferAvailable(bool isReady) = 0;

    virtual void OnStateChange(ScreenCaptureStateCodeAdapter stateCode) {}
};

class ScreenCaptureAdapter {
public:
    ScreenCaptureAdapter() = default;

    virtual ~ScreenCaptureAdapter() = default;

    virtual int32_t Init(const std::shared_ptr<ScreenCaptureConfigAdapter> config) = 0;

    virtual int32_t SetMicrophoneEnable(bool enable) = 0;

    virtual int32_t StartCapture() = 0;

    virtual int32_t StopCapture() = 0;

    virtual int32_t SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback) = 0;

    virtual std::shared_ptr<SurfaceBufferAdapter> AcquireVideoBuffer() = 0;

    virtual int32_t ReleaseVideoBuffer() = 0;
};

} // namespace OHOS::NWeb

#endif // SCREEN_CAPTURE_ADAPTER_H
