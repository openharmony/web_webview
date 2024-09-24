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

#include "nweb_log.h"
#include <cstring>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "ohcamera/camera_device.h"
#include "ohcamera/camera_manager.h"
#include "camera_manager_adapter_impl.h"
#include "camera_manager_adapter.h"

using namespace testing;

namespace OHOS::NWeb {
namespace {
OH_NativeImage* g_nativeImage = nullptr;
Camera_Manager* g_cameraManager = nullptr;
OHNativeWindowBuffer*  g_windowBuffer;
int g_fenceFd = -1;
const uint32_t DEFAULT_WIDTH = 2560;
const uint32_t DEFAULT_HEIGHT = 1396;
const std::string deviceId = "device/1";
} // namespace

class CameraAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void CameraAdapterImplTest::SetUpTestCase(void)
{
    WVLOG_I("SetUpTestCase");
    g_nativeImage = OH_NativeImage_Create(0, 0);
    EXPECT_NE(g_nativeImage, nullptr);
    OHNativeWindow* window = OH_NativeImage_AcquireNativeWindow(g_nativeImage);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_BUFFER_GEOMETRY, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_STRIDE, 0x8);
    EXPECT_EQ(OH_NativeWindow_NativeWindowRequestBuffer(window, &g_windowBuffer, &g_fenceFd), 0);
    EXPECT_EQ(OH_Camera_GetCameraManager(&g_cameraManager), 0);
    EXPECT_NE(g_cameraManager, nullptr);
}

void CameraAdapterImplTest::TearDownTestCase(void) {
    OH_NativeWindow_DestroyNativeWindowBuffer(g_windowBuffer);
    OH_NativeImage_Destroy(&g_nativeImage);
    OH_Camera_DeleteCameraManager(g_cameraManager);
}

void CameraAdapterImplTest::SetUp(void) {}

void CameraAdapterImplTest::TearDown(void) {}

class CameraBufferListenerAdapterMock : public CameraBufferListenerAdapter {
public:
    CameraBufferListenerAdapterMock() {}
    ~CameraBufferListenerAdapterMock() override = default;

    void OnBufferAvailable(std::shared_ptr<CameraSurfaceAdapter> adapter,
        std::shared_ptr<CameraSurfaceBufferAdapter> buffer,
        std::shared_ptr<CameraRotationInfoAdapter> rotationInfo) override
    {}
};

class CameraStatusCallbackAdapterMock : public CameraStatusCallbackAdapter {
public:
    CameraStatusCallbackAdapterMock() {}
    ~CameraStatusCallbackAdapterMock() override = default;

    void OnCameraStatusChanged(CameraStatusAdapter cameraStatusAdapter, std::string callBackDeviceId) override {
        m_cameraStatusAdapter = cameraStatusAdapter;
    }
private:
    CameraStatusAdapter m_cameraStatusAdapter;
};

class VideoCaptureParamsAdapterMock : public VideoCaptureParamsAdapter {
public:
    VideoCaptureParamsAdapterMock() = default;

    uint32_t GetWidth() override
    {
        return width;
    }

    uint32_t GetHeight() override
    {
        return height;
    }

    float GetFrameRate() override
    {
        return frameRate;
    }

    VideoPixelFormatAdapter GetPixelFormat() override
    {
        return pixelFormat;
    }

    bool GetEnableFaceDetection() override
    {
        return enableFaceDetection;
    }

    uint32_t width;
    uint32_t height;
    float frameRate;
    VideoPixelFormatAdapter pixelFormat;
    bool enableFaceDetection;
};

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_001
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_001)
{
    auto adapterImpl = std::make_shared<CameraSurfaceBufferAdapterImpl>(g_windowBuffer, g_fenceFd);
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->GetFileDescriptor();
    EXPECT_NE(result, -1);
    result = adapterImpl->GetWidth();
    EXPECT_NE(result, -1);
    result = adapterImpl->GetHeight();
    EXPECT_NE(result, -1);
    result = adapterImpl->GetStride();
    EXPECT_NE(result, -1);
    result = adapterImpl->GetFormat();
    EXPECT_NE(result, -1);
    uint32_t surfaceBufferSize = adapterImpl->GetSize();
    EXPECT_NE(surfaceBufferSize, 0);
    OHNativeWindowBuffer* buffer = adapterImpl->GetWindowBuffer();
    EXPECT_NE(buffer, nullptr);
    uint8_t* addBuffer = adapterImpl->GetBufferAddr();
    EXPECT_NE(addBuffer, nullptr);
}

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_002
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_002)
{
    auto adapterImpl = std::make_shared<CameraSurfaceBufferAdapterImpl>(g_windowBuffer, g_fenceFd);
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->GetFileDescriptor();
    EXPECT_GE(result, -1);
    result = adapterImpl->GetWidth();
    EXPECT_GE(result, -1);
    result = adapterImpl->GetHeight();
    EXPECT_GE(result, -1);
    result = adapterImpl->GetStride();
    EXPECT_GE(result, -1);
    result = adapterImpl->GetFormat();
    EXPECT_GE(result, -1);
    uint32_t surfaceBufferSize = adapterImpl->GetSize();
    EXPECT_GE(surfaceBufferSize, 0);
    uint8_t* addBuffer = adapterImpl->GetBufferAddr();
    EXPECT_NE(addBuffer, nullptr);
}

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_004
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_004)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    VideoTransportType type = adapter.GetCameraTransportType(Camera_Connection::CAMERA_CONNECTION_BUILT_IN);
    EXPECT_EQ(type, VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN);
    type = adapter.GetCameraTransportType(static_cast<Camera_Connection>(-1));
    EXPECT_EQ(type, VideoTransportType::VIDEO_TRANS_TYPE_OTHER);
    VideoFacingModeAdapter modeAdapter = adapter.GetCameraFacingMode(Camera_Position::CAMERA_POSITION_UNSPECIFIED);
    EXPECT_EQ(modeAdapter, VideoFacingModeAdapter::FACING_NONE);
    modeAdapter = adapter.GetCameraFacingMode(static_cast<Camera_Position>(-1));
    EXPECT_EQ(modeAdapter, VideoFacingModeAdapter::FACING_NONE);
    VideoPixelFormatAdapter formatAdapter =
        adapter.TransToAdapterCameraFormat(Camera_Format::CAMERA_FORMAT_YUV_420_SP);
    EXPECT_EQ(formatAdapter, VideoPixelFormatAdapter::FORMAT_YUV_420_SP);
    formatAdapter = adapter.TransToAdapterCameraFormat(static_cast<Camera_Format>(0));
    EXPECT_EQ(formatAdapter, VideoPixelFormatAdapter::FORMAT_UNKNOWN);
    Camera_Format format = adapter.TransToOriCameraFormat(VideoPixelFormatAdapter::FORMAT_YUV_420_SP);
    EXPECT_EQ(format, Camera_Format::CAMERA_FORMAT_YUV_420_SP);
    format = adapter.TransToOriCameraFormat(static_cast<VideoPixelFormatAdapter>(-1));
    EXPECT_EQ(format, Camera_Format::CAMERA_FORMAT_RGBA_8888);
    ExposureModeAdapter exposure = adapter.GetAdapterExposureMode(Camera_ExposureMode::EXPOSURE_MODE_LOCKED);
    EXPECT_EQ(exposure, ExposureModeAdapter::EXPOSURE_MODE_LOCKED);
    exposure = adapter.GetAdapterExposureMode(static_cast<Camera_ExposureMode>(DEFAULT_WIDTH));
    EXPECT_EQ(exposure, ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED);
}

/**
 * @tc.name: CameraAdapterImplTest_GetAdapterFocusMode_005
 * @tc.desc: GetAdapterFocusMode.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetAdapterFocusMode_005)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    Camera_FocusMode mode = adapter.GetOriFocusMode(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_EQ(mode, Camera_FocusMode::FOCUS_MODE_CONTINUOUS_AUTO);
    mode = adapter.GetOriFocusMode(static_cast<FocusModeAdapter>(-1));
    EXPECT_EQ(mode, Camera_FocusMode::FOCUS_MODE_MANUAL);
    Camera_FlashMode flashMode = adapter.GetOriFlashMode(FlashModeAdapter::FLASH_MODE_OPEN);
    EXPECT_EQ(flashMode, Camera_FlashMode::FLASH_MODE_OPEN);
    flashMode = adapter.GetOriFlashMode(static_cast<FlashModeAdapter>(-1));
    EXPECT_EQ(flashMode, Camera_FlashMode::FLASH_MODE_CLOSE);
    FocusModeAdapter focusMode = adapter.GetAdapterFocusMode(Camera_FocusMode::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_EQ(focusMode, FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    focusMode = adapter.GetAdapterFocusMode(static_cast<Camera_FocusMode>(-1));
    EXPECT_EQ(focusMode, FocusModeAdapter::FOCUS_MODE_MANUAL);
    std::string errnoTypeString;
    EXPECT_EQ(CameraManagerAdapterImpl::GetInstance().ErrorTypeToString(
        static_cast<CameraErrorType>(-1), errnoTypeString), CAMERA_ERROR);
    EXPECT_EQ(CameraManagerAdapterImpl::GetInstance().ErrorTypeToString(
        CameraErrorType::CAMERA_NOT_CLOSE, errnoTypeString), CAMERA_OK);
}

/**
 * @tc.name: CameraAdapterImplTest_GetOriFocusMode_006
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetOriFocusMode_006)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = adapter.Create(callback);
    std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devicesDiscriptor = adapter.GetDevicesInfo();

    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);

    std::shared_ptr<VideoCaptureParamsAdapterMock> captureParams = std::make_shared<VideoCaptureParamsAdapterMock>();
    EXPECT_NE(captureParams, nullptr);
    captureParams->width = 1;
    captureParams->height = 1;
    captureParams->frameRate = 1;
    captureParams->pixelFormat = VideoPixelFormatAdapter::FORMAT_RGBA_8888;
    captureParams->enableFaceDetection = true;
    std::shared_ptr<CameraBufferListenerAdapter> listener = std::make_shared<CameraBufferListenerAdapterMock>();
    adapter.inputInitedFlag_ = true;
    result = adapter.InitPreviewOutput(captureParams, listener);
    EXPECT_EQ(result, 0);
    result = adapter.CreateAndStartSession();
    EXPECT_NE(result, 0);
    result = adapter.RestartSession();
    EXPECT_EQ(result, 0);
    adapter.status_ = CameraStatusAdapter::UNAVAILABLE;
    result = adapter.CreateAndStartSession();
    EXPECT_NE(result, 0);
    result = adapter.StopSession(CameraStopType::NORMAL);
    EXPECT_EQ(result, 0);
    result = adapter.StopSession(CameraStopType::TO_BACK);
    EXPECT_EQ(result, 0);
    result = adapter.ReleaseSession();
    EXPECT_EQ(result, 0);
    result = adapter.ReleaseSessionResource("test");
    EXPECT_EQ(result, 0);
    result = adapter.ReleaseSessionResource(deviceId);
    EXPECT_EQ(result, 0);
    adapter.ReleaseCameraManger();
}

/**
 * @tc.name: CameraAdapterImplTest_TransToAdapterExposureModes_007
 * @tc.desc: TransToAdapterExposureModes.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_TransToAdapterExposureModes_007)
{
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    std::vector<Camera_ExposureMode> exposureModes;
    exposureModes.push_back(EXPOSURE_MODE_LOCKED);
    exposureModes.push_back(EXPOSURE_MODE_AUTO);
    exposureModes.push_back(EXPOSURE_MODE_CONTINUOUS_AUTO);
    std::vector<ExposureModeAdapter> exposureModesAdapter;

    result = OH_CameraManager_CreateCaptureSession(adapter.cameraManager_, &adapter.captureSession_);
    EXPECT_EQ(result, 0);
    result = adapter.GetExposureModes(exposureModesAdapter);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(exposureModesAdapter.size(), 0);
}

/**
 * @tc.name: CameraAdapterImplTest_GetOriFocusMode_008
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetOriFocusMode_008)
{
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    adapter.isForegound_ = true;
    adapter.isCapturing_ = true;
    result = adapter.RestartSession();
    EXPECT_NE(result, 0);
    result = adapter.RestartSession();
    EXPECT_NE(result, 0);
    adapter.cameraManager_ = nullptr;
    result = adapter.RestartSession();
    EXPECT_NE(result, 0);
    std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devicesDiscriptor = adapter.GetDevicesInfo();
    EXPECT_TRUE(devicesDiscriptor.empty());
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    std::shared_ptr<VideoCaptureParamsAdapterMock> captureParams = std::make_shared<VideoCaptureParamsAdapterMock>();
    EXPECT_NE(captureParams, nullptr);
    result = adapter.InitPreviewOutput(captureParams, nullptr);
    EXPECT_NE(result, 0);
    adapter.status_ = CameraStatusAdapter::AVAILABLE;
    adapter.isCapturing_ = false;
    result = adapter.RestartSession();
    EXPECT_EQ(result, 0);
    adapter.status_ = CameraStatusAdapter::UNAVAILABLE;
    result = adapter.RestartSession();
    EXPECT_EQ(result, 0);
    result = adapter.ReleaseSession();
    EXPECT_EQ(result, 0);
    std::vector<ExposureModeAdapter> exposureModesAdapter;
    result = adapter.GetExposureModes(exposureModesAdapter);
    EXPECT_NE(result, 0);
    ExposureModeAdapter exposureModeAdapter;
    result = adapter.GetCurrentExposureMode(exposureModeAdapter);
    EXPECT_NE(result, 0);
    std::shared_ptr<VideoCaptureRangeAdapter> rangeVal = adapter.GetExposureCompensation();
    EXPECT_EQ(rangeVal, nullptr);
    rangeVal = nullptr;
    rangeVal = adapter.GetCaptionRangeById(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION);
    EXPECT_EQ(rangeVal, nullptr);
    bool isMode = adapter.IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_FALSE(isMode);
    FocusModeAdapter focusMode = adapter.GetCurrentFocusMode();
    EXPECT_EQ(focusMode, FocusModeAdapter::FOCUS_MODE_MANUAL);
    isMode = adapter.IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_OPEN);
    EXPECT_FALSE(isMode);
}

/**
 * @tc.name: CameraAdapterImplTest_CameraSurfaceAdapterImpl_009
 * @tc.desc: CameraSurfaceAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_CameraSurfaceAdapterImpl_009)
{
    auto listener = std::make_shared<CameraSurfaceAdapterImpl>();
    EXPECT_NE(listener, nullptr);
    listener->ReleaseBuffer(nullptr, -1);
    int32_t result = listener->ReleaseBuffer(nullptr, -1);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: CameraAdapterImplTest_InitCameraInput_010
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_InitCameraInput_010)
{
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    std::shared_ptr<VideoCaptureParamsAdapterMock> captureParams = std::make_shared<VideoCaptureParamsAdapterMock>();
    EXPECT_NE(captureParams, nullptr);
    captureParams->width = 1;
    captureParams->height = 1;
    captureParams->frameRate = 1;
    captureParams->pixelFormat = VideoPixelFormatAdapter::FORMAT_RGBA_8888;
    captureParams->enableFaceDetection = true;
    auto listenerAdapter = std::make_shared<CameraBufferListenerAdapterMock>();
    EXPECT_NE(listenerAdapter, nullptr);
    adapter.status_ = CameraStatusAdapter::UNAVAILABLE;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    adapter.isCapturing_ = true;
    bool isExist = adapter.IsExistCaptureTask();
    EXPECT_TRUE(isExist);
    result = adapter.StartStream(deviceId, nullptr, nullptr);
    EXPECT_NE(result, 0);
    result = adapter.StartStream(deviceId, captureParams, nullptr);
    EXPECT_NE(result, 0);
    adapter.status_ = CameraStatusAdapter::AVAILABLE;
    adapter.inputInitedFlag_ = true;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    adapter.inputInitedFlag_ = false;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    adapter.cameraManager_ = nullptr;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);

    std::shared_ptr<VideoCaptureRangeAdapter> rangeVal =
        adapter.GetCaptionRangeById(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION);
    EXPECT_EQ(rangeVal, nullptr);
    adapter.inputInitedFlag_ = true;
    result = adapter.InitPreviewOutput(captureParams, nullptr);
    EXPECT_NE(result, 0);
    isExist = adapter.IsExistCaptureTask();
    EXPECT_FALSE(isExist);
    result = adapter.StartStream(deviceId, captureParams, listenerAdapter);
    EXPECT_NE(result, 0);
}

/**
 * @tc.name: CameraAdapterImplTest_CameraManagerAdapterCallback_011
 * @tc.desc: CameraManagerAdapterCallback.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_CameraManagerAdapterCallback_011)
{
    auto adapter = std::make_shared<CameraStatusCallbackAdapterMock>();
    EXPECT_NE(adapter, nullptr);
    adapter->OnCameraStatusChanged(CameraStatusAdapter::APPEAR, deviceId);
    EXPECT_EQ(adapter->m_cameraStatusAdapter, CameraStatusAdapter::APPEAR);
    adapter->OnCameraStatusChanged(CameraStatusAdapter::DISAPPEAR, deviceId);
    EXPECT_EQ(adapter->m_cameraStatusAdapter, CameraStatusAdapter::DISAPPEAR);
    adapter->OnCameraStatusChanged(CameraStatusAdapter::AVAILABLE, deviceId);
    EXPECT_EQ(adapter->m_cameraStatusAdapter, CameraStatusAdapter::AVAILABLE);
    adapter->OnCameraStatusChanged(CameraStatusAdapter::UNAVAILABLE, deviceId);
    EXPECT_EQ(adapter->m_cameraStatusAdapter, CameraStatusAdapter::UNAVAILABLE);
}

/**
 * @tc.name: CameraAdapterImplTest_CameraStatus_012
 * @tc.desc: CameraManagerAdapterCallback.
 * @tc.type: FUNC
 */
TEST_F(CameraAdapterImplTest, CameraAdapterImplTest_CameraStatus_012)
{
    CameraManagerAdapterImpl& adapter = CameraManagerAdapterImpl::GetInstance();
    EXPECT_EQ(adapter.GetCameraStatus(), CameraStatusAdapter::AVAILABLE);
    adapter.SetCameraStatus(CameraStatusAdapter::APPEAR);
    EXPECT_EQ(adapter.GetCameraStatus(), CameraStatusAdapter::APPEAR);
}

} // namespace OHOS::NWeb
