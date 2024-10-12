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

#include "screen_capture_adapter_impl.h"

#include "foundation/multimedia/player_framework/interfaces/inner_api/native/media_errors.h"
#include "nweb_log.h"
#include "surface_adapter_impl.h"

namespace OHOS::NWeb {
namespace {
OHOS::Media::CaptureMode GetOHCaptureMode(const CaptureModeAdapter& mode)
{
    switch (mode) {
        case CaptureModeAdapter::CAPTURE_HOME_SCREEN:
            return OHOS::Media::CaptureMode::CAPTURE_HOME_SCREEN;
        case CaptureModeAdapter::CAPTURE_SPECIFIED_SCREEN:
            return OHOS::Media::CaptureMode::CAPTURE_SPECIFIED_SCREEN;
        case CaptureModeAdapter::CAPTURE_SPECIFIED_WINDOW:
            return OHOS::Media::CaptureMode::CAPTURE_SPECIFIED_WINDOW;
        default:
            return OHOS::Media::CaptureMode::CAPTURE_INVAILD;
    }
    return OHOS::Media::CaptureMode::CAPTURE_INVAILD;
}

OHOS::Media::DataType GetOHDataType(const DataTypeAdapter& type)
{
    switch (type) {
        case DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE:
            return OHOS::Media::DataType::ORIGINAL_STREAM;
        case DataTypeAdapter::ENCODED_STREAM_DATA_TYPE:
            return OHOS::Media::DataType::ENCODED_STREAM;
        case DataTypeAdapter::CAPTURE_FILE_DATA_TYPE:
            return OHOS::Media::DataType::CAPTURE_FILE;
        default:
            return OHOS::Media::DataType::INVAILD;
    }
    return OHOS::Media::DataType::INVAILD;
}

OHOS::Media::AudioCaptureSourceType GetOHAudioCaptureSourceType(const AudioCaptureSourceTypeAdapter& type)
{
    switch (type) {
        case AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT:
            return OHOS::Media::AudioCaptureSourceType::SOURCE_DEFAULT;
        case AudioCaptureSourceTypeAdapter::MIC:
            return OHOS::Media::AudioCaptureSourceType::MIC;
        case AudioCaptureSourceTypeAdapter::ALL_PLAYBACK:
            return OHOS::Media::AudioCaptureSourceType::ALL_PLAYBACK;
        case AudioCaptureSourceTypeAdapter::APP_PLAYBACK:
            return OHOS::Media::AudioCaptureSourceType::APP_PLAYBACK;
        default:
            return OHOS::Media::AudioCaptureSourceType::SOURCE_INVALID;
    }
    return OHOS::Media::AudioCaptureSourceType::SOURCE_INVALID;
}

AudioCaptureSourceTypeAdapter GetAudioCaptureSourceTypeAdapter(const OHOS::Media::AudioCaptureSourceType& type)
{
    switch (type) {
        case OHOS::Media::AudioCaptureSourceType::SOURCE_DEFAULT:
            return AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
        case OHOS::Media::AudioCaptureSourceType::MIC:
            return AudioCaptureSourceTypeAdapter::MIC;
        case OHOS::Media::AudioCaptureSourceType::ALL_PLAYBACK:
            return AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
        case OHOS::Media::AudioCaptureSourceType::APP_PLAYBACK:
            return AudioCaptureSourceTypeAdapter::APP_PLAYBACK;
        default:
            return AudioCaptureSourceTypeAdapter::SOURCE_INVALID;
    }
    return AudioCaptureSourceTypeAdapter::SOURCE_INVALID;
}

OHOS::Media::AudioCodecFormat GetOHAudioCodecFormat(const AudioCodecFormatAdapter& format)
{
    switch (format) {
        case AudioCodecFormatAdapter::AUDIO_DEFAULT:
            return OHOS::Media::AudioCodecFormat::AUDIO_DEFAULT;
        case AudioCodecFormatAdapter::AAC_LC:
            return OHOS::Media::AudioCodecFormat::AAC_LC;
        default:
            return OHOS::Media::AudioCodecFormat::AUDIO_CODEC_FORMAT_BUTT;
    }
    return OHOS::Media::AudioCodecFormat::AUDIO_CODEC_FORMAT_BUTT;
}

OHOS::Media::VideoSourceType GetOHVideoSourceType(const VideoSourceTypeAdapter& type)
{
    switch (type) {
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_YUV:
            return OHOS::Media::VideoSourceType::VIDEO_SOURCE_SURFACE_YUV;
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_ES:
            return OHOS::Media::VideoSourceType::VIDEO_SOURCE_SURFACE_ES;
        case VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA:
            return OHOS::Media::VideoSourceType::VIDEO_SOURCE_SURFACE_RGBA;
        default:
            return OHOS::Media::VideoSourceType::VIDEO_SOURCE_BUTT;
    }
    return OHOS::Media::VideoSourceType::VIDEO_SOURCE_BUTT;
}

OHOS::Media::VideoCodecFormat GetOHVideoCodecFormat(const VideoCodecFormatAdapter& format)
{
    switch (format) {
        case VideoCodecFormatAdapter::VIDEO_DEFAULT:
            return OHOS::Media::VideoCodecFormat::VIDEO_DEFAULT;
        case VideoCodecFormatAdapter::H264:
            return OHOS::Media::VideoCodecFormat::H264;
        case VideoCodecFormatAdapter::MPEG4:
            return OHOS::Media::VideoCodecFormat::MPEG4;
        default:
            return OHOS::Media::VideoCodecFormat::VIDEO_CODEC_FORMAT_BUTT;
    }
    return OHOS::Media::VideoCodecFormat::VIDEO_CODEC_FORMAT_BUTT;
}

std::string GetOHContainerFormatType(const ContainerFormatTypeAdapter& type)
{
    switch (type) {
        case ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE:
            return std::string(OHOS::Media::ContainerFormatType::CFT_MPEG_4A);
        default:
            return std::string(OHOS::Media::ContainerFormatType::CFT_MPEG_4);
    }
    return std::string(OHOS::Media::ContainerFormatType::CFT_MPEG_4);
}

OHOS::Media::AVScreenCaptureConfig ConvertScreenCaptureConfig(const std::shared_ptr<ScreenCaptureConfigAdapter> config)
{
    OHOS::Media::AVScreenCaptureConfig avConfig;

    if (!config) {
        WVLOG_I("ConvertScreenCaptureConfig config is null");
        return avConfig;
    }

    avConfig.captureMode = GetOHCaptureMode(config->GetCaptureMode());
    avConfig.dataType = GetOHDataType(config->GetDataType());

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetMicCapInfo()) {
        avConfig.audioInfo.micCapInfo.audioSampleRate = config->GetAudioInfo()->GetMicCapInfo()->GetAudioSampleRate();
        avConfig.audioInfo.micCapInfo.audioChannels = config->GetAudioInfo()->GetMicCapInfo()->GetAudioChannels();
        avConfig.audioInfo.micCapInfo.audioSource =
            GetOHAudioCaptureSourceType(config->GetAudioInfo()->GetMicCapInfo()->GetAudioSource());
    }

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetInnerCapInfo()) {
        avConfig.audioInfo.innerCapInfo.audioSampleRate =
            config->GetAudioInfo()->GetInnerCapInfo()->GetAudioSampleRate();
        avConfig.audioInfo.innerCapInfo.audioChannels = config->GetAudioInfo()->GetInnerCapInfo()->GetAudioChannels();
        avConfig.audioInfo.innerCapInfo.audioSource =
            GetOHAudioCaptureSourceType(config->GetAudioInfo()->GetInnerCapInfo()->GetAudioSource());
    }

    if (config->GetAudioInfo() && config->GetAudioInfo()->GetAudioEncInfo()) {
        avConfig.audioInfo.audioEncInfo.audioBitrate = config->GetAudioInfo()->GetAudioEncInfo()->GetAudioBitrate();
        avConfig.audioInfo.audioEncInfo.audioCodecformat =
            GetOHAudioCodecFormat(config->GetAudioInfo()->GetAudioEncInfo()->GetAudioCodecformat());
    }

    if (config->GetVideoInfo() && config->GetVideoInfo()->GetVideoCapInfo()) {
        avConfig.videoInfo.videoCapInfo.displayId = config->GetVideoInfo()->GetVideoCapInfo()->GetDisplayId();
        avConfig.videoInfo.videoCapInfo.taskIDs = config->GetVideoInfo()->GetVideoCapInfo()->GetTaskIDs();
        avConfig.videoInfo.videoCapInfo.videoFrameWidth =
            config->GetVideoInfo()->GetVideoCapInfo()->GetVideoFrameWidth();
        avConfig.videoInfo.videoCapInfo.videoFrameHeight =
            config->GetVideoInfo()->GetVideoCapInfo()->GetVideoFrameHeight();
        avConfig.videoInfo.videoCapInfo.videoSource =
            GetOHVideoSourceType(config->GetVideoInfo()->GetVideoCapInfo()->GetVideoSourceType());
    }

    if (config->GetVideoInfo() && config->GetVideoInfo()->GetVideoEncInfo()) {
        avConfig.videoInfo.videoEncInfo.videoCodec =
            GetOHVideoCodecFormat(config->GetVideoInfo()->GetVideoEncInfo()->GetVideoCodecFormat());
        avConfig.videoInfo.videoEncInfo.videoBitrate = config->GetVideoInfo()->GetVideoEncInfo()->GetVideoBitrate();
        avConfig.videoInfo.videoEncInfo.videoFrameRate = config->GetVideoInfo()->GetVideoEncInfo()->GetVideoFrameRate();
    }

    if (config->GetDataType() == DataTypeAdapter::CAPTURE_FILE_DATA_TYPE && config->GetRecorderInfo()) {
        avConfig.recorderInfo.url = config->GetRecorderInfo()->GetUrl();
        avConfig.recorderInfo.fileFormat = GetOHContainerFormatType(config->GetRecorderInfo()->GetFileFormat());
    }

    return avConfig;
}

ScreenCaptureStateCodeAdapter GetScreenCaptureStateCodeAdapter(const OHOS::Media::AVScreenCaptureStateCode& stateCode)
{
    switch (stateCode) {
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STARTED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_CANCELED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;
        default:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
    }
    return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
}
} // namespace

void OHScreenCaptureCallback::OnError(OHOS::Media::ScreenCaptureErrorType errorType, int32_t errorCode)
{
    WVLOG_I("OnError() is called, errorType %{public}d, errorCode %{public}d", errorType, errorCode);
    if (callback_) {
        callback_->OnError(errorCode);
    }
}

void OHScreenCaptureCallback::OnAudioBufferAvailable(bool isReady, OHOS::Media::AudioCaptureSourceType type)
{
    if (callback_) {
        callback_->OnAudioBufferAvailable(isReady, GetAudioCaptureSourceTypeAdapter(type));
    }
}

void OHScreenCaptureCallback::OnVideoBufferAvailable(bool isReady)
{
    if (callback_) {
        callback_->OnVideoBufferAvailable(isReady);
    }
}

void OHScreenCaptureCallback::OnStateChange(OHOS::Media::AVScreenCaptureStateCode stateCode)
{
    WVLOG_I("OnStateChange() is called, stateCode %{public}d", stateCode);
    if (callback_) {
        callback_->OnStateChange(GetScreenCaptureStateCodeAdapter(stateCode));
    }
}

ScreenCaptureAdapterImpl::~ScreenCaptureAdapterImpl()
{
    Release();
}

int32_t ScreenCaptureAdapterImpl::Init(const std::shared_ptr<ScreenCaptureConfigAdapter> config)
{
    if (screenCapture_) {
        return 0;
    }
    screenCapture_ = OHOS::Media::ScreenCaptureFactory::CreateScreenCapture();
    if (!screenCapture_) {
        WVLOG_E("CreateScreenCapture create failed");
        return -1;
    }

    if (!config) {
        WVLOG_E("config is null");
        return -1;
    }

    int32_t ret = screenCapture_->Init(ConvertScreenCaptureConfig(config));
    if (ret != Media::MSERR_OK) {
        WVLOG_E("OH_AVScreenCapture init failed, ret = %{public}d", ret);
        Release();
        return -1;
    }
    return 0;
}

void ScreenCaptureAdapterImpl::Release()
{
    if (!screenCapture_) {
        return;
    }
    screenCaptureCallback_ = nullptr;
    int32_t ret = screenCapture_->Release();
    if (ret != Media::MSERR_OK) {
        WVLOG_E("OH_AVScreenCapture release failed, ret = %{public}d", ret);
    }
    screenCapture_ = nullptr;
}

int32_t ScreenCaptureAdapterImpl::SetMicrophoneEnable(bool enable)
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = screenCapture_->SetMicrophoneEnabled(enable);
    if (ret != Media::MSERR_OK) {
        WVLOG_E("set microphone enabled failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::StartCapture()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = screenCapture_->SetPrivacyAuthorityEnabled();
    if (ret != Media::MSERR_OK) {
        WVLOG_E("start capture SetPrivacyAuthorityEnabled failed, ret = %{public}d", ret);
        return -1;
    }
    ret = screenCapture_->SetCanvasRotation(true);
    if (ret != Media::MSERR_OK) {
        WVLOG_E("start capture SetCanvasRotation failed, ret = %{public}d", ret);
    }
    ret = screenCapture_->StartScreenCapture();
    if (ret != Media::MSERR_OK) {
        WVLOG_E("start capture failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::StopCapture()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = screenCapture_->StopScreenCapture();
    if (ret != Media::MSERR_OK) {
        WVLOG_E("stop capture failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}

int32_t ScreenCaptureAdapterImpl::SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback)
{
    if (!screenCapture_ || !callback || screenCaptureCallback_) {
        WVLOG_E("not init or param error");
        return -1;
    }
    screenCaptureCallback_ = std::make_shared<OHScreenCaptureCallback>(callback);
    if (!screenCaptureCallback_) {
        WVLOG_E("make_shared failed");
        return -1;
    }
    int32_t ret = screenCapture_->SetScreenCaptureCallback(screenCaptureCallback_);
    if (ret != Media::MSERR_OK) {
        WVLOG_E("set callback failed, ret = %{public}d", ret);
        screenCaptureCallback_ = nullptr;
        return -1;
    }
    return 0;
}

std::shared_ptr<SurfaceBufferAdapter> ScreenCaptureAdapterImpl::AcquireVideoBuffer()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return nullptr;
    }
    int32_t fence;
    int64_t timestamp;
    OHOS::Rect region;
    sptr<OHOS::SurfaceBuffer> surfaceBuffer = screenCapture_->AcquireVideoBuffer(fence, timestamp, region);
    if (surfaceBuffer == nullptr) {
        WVLOG_E("acquire video buffer failed");
        return nullptr;
    }
    auto surfaceBufferImpl = std::make_shared<SurfaceBufferAdapterImpl>(surfaceBuffer);
    if (!surfaceBufferImpl) {
        WVLOG_E("make_unique failed");
        (void)ReleaseVideoBuffer();
        return nullptr;
    }
    return std::move(surfaceBufferImpl);
}

int32_t ScreenCaptureAdapterImpl::ReleaseVideoBuffer()
{
    if (!screenCapture_) {
        WVLOG_E("not init");
        return -1;
    }
    int32_t ret = screenCapture_->ReleaseVideoBuffer();
    if (ret != Media::MSERR_OK) {
        WVLOG_E("release video buffer failed, ret = %{public}d", ret);
        return -1;
    }
    return 0;
}
} // namespace OHOS::NWeb
