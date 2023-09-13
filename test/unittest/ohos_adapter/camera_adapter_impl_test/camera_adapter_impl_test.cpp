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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ui/rs_surface_node.h>
#include "nweb_adapter_helper.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "accesstoken_kit.h"

#define private public
#include "camera_manager_adapter_impl.h"
#include "nweb_surface_adapter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::CameraStandard;
using namespace OHOS::Rosen;
using namespace OHOS::Camera;

namespace OHOS::NWeb {
namespace {
sptr<SurfaceBuffer> g_surfaceBuffer = nullptr;
const uint32_t DEFAULT_WIDTH = 2560;
const uint32_t DEFAULT_HEIGHT = 1396;
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
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    EXPECT_NE(surfaceNode, nullptr);
    sptr<Surface> surface = surfaceNode->GetSurface();
    EXPECT_NE(surface, nullptr);
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    g_surfaceBuffer = surfaceAdapter.RequestBuffer(surface, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_NE(g_surfaceBuffer, nullptr);

    // set native token
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    perms[1] = "ohos.permission.CAMERA";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "web_camera_tdd",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void CameraAdapterImplTest::TearDownTestCase(void)
{}

void CameraAdapterImplTest::SetUp(void)
{}

void CameraAdapterImplTest::TearDown(void)
{}

class CameraBufferListenerAdapterMock : public CameraBufferListenerAdapter {
public:
    CameraBufferListenerAdapterMock() {}
    ~CameraBufferListenerAdapterMock() override = default;

    void OnBufferAvailable(std::shared_ptr<CameraSurfaceAdapter> surface,
        std::unique_ptr<CameraSurfaceBufferAdapter> buffer,
        CameraRotationInfo rotationInfo) override {}
};

class CameraStatusCallbackAdapterMock : public CameraStatusCallbackAdapter {
public:
    CameraStatusCallbackAdapterMock() {}
    ~CameraStatusCallbackAdapterMock() override = default;

    void OnCameraStatusChanged(CameraStatusAdapter cameraStatusAdapter) override {}
};

class CameraManagerMock : public CameraManager {
public:
    MOCK_METHOD0(CreateCaptureSession, sptr<CaptureSession>());
};

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_001
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_001, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<CameraSurfaceBufferAdapterImpl>(g_surfaceBuffer);
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
    sptr<SurfaceBuffer> buffer = adapterImpl->GetBuffer();
    EXPECT_NE(buffer, nullptr);
    uint8_t *addBuffer = adapterImpl->GetBufferAddr();
    EXPECT_NE(addBuffer, nullptr);
}

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_002
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_002, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<CameraSurfaceBufferAdapterImpl>(nullptr);
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->GetFileDescriptor();
    EXPECT_EQ(result, -1);
    result = adapterImpl->GetWidth();
    EXPECT_EQ(result, -1);
    result = adapterImpl->GetHeight();
    EXPECT_EQ(result, -1);
    result = adapterImpl->GetStride();
    EXPECT_EQ(result, -1);
    result = adapterImpl->GetFormat();
    EXPECT_EQ(result, -1);
    uint32_t surfaceBufferSize = adapterImpl->GetSize();
    EXPECT_EQ(surfaceBufferSize, 0);
    uint8_t *addBuffer = adapterImpl->GetBufferAddr();
    EXPECT_EQ(addBuffer, nullptr);
}

/**
 * @tc.name: CameraAdapterImplTest_CameraSurfaceListener_003
 * @tc.desc: CameraSurfaceListener.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_CameraSurfaceListener_003, TestSize.Level1)
{
    sptr<IConsumerSurface> surface = IConsumerSurface::Create();
    EXPECT_NE(surface, nullptr);
    auto listenerAdapter = std::make_shared<CameraBufferListenerAdapterMock>();
    EXPECT_NE(listenerAdapter, nullptr);
    auto listener = std::make_shared<CameraSurfaceListener>(SurfaceType::PREVIEW, surface, listenerAdapter);
    EXPECT_NE(listener, nullptr);
    listener->OnBufferAvailable();

    CameraRotationInfo info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_ROTATE_90);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_ROTATE_180);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_ROTATE_270);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_H);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_V);
    EXPECT_TRUE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_H_ROT90);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    EXPECT_TRUE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_H_ROT180);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    EXPECT_TRUE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_H_ROT270);
    EXPECT_FALSE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    EXPECT_TRUE(info.isFlipX);
    info = listener->GetRotationInfo(GraphicTransformType::GRAPHIC_ROTATE_BUTT);
    EXPECT_FALSE(info.isFlipX);
    listener->surface_ = nullptr;
    listener->OnBufferAvailable();
}

/**
 * @tc.name: CameraAdapterImplTest_GetFileDescriptor_004
 * @tc.desc: GetFileDescriptor.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetFileDescriptor_004, TestSize.Level1)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    VideoTransportType type = adapter.GetCameraTransportType(ConnectionType::CAMERA_CONNECTION_BUILT_IN);
    EXPECT_EQ(type, VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN);
    type = adapter.GetCameraTransportType(static_cast<ConnectionType>(-1));
    EXPECT_EQ(type, VideoTransportType::VIDEO_TRANS_TYPE_OTHER);
    VideoFacingModeAdapter modeAdapter = adapter.GetCameraFacingMode(CameraPosition::CAMERA_POSITION_UNSPECIFIED);
    EXPECT_EQ(modeAdapter, VideoFacingModeAdapter::FACING_NONE);
    modeAdapter = adapter.GetCameraFacingMode(static_cast<CameraPosition>(-1));
    EXPECT_EQ(modeAdapter, VideoFacingModeAdapter::FACING_NONE);
    VideoPixelFormatAdapter formatAdapter =
        adapter.TransToAdapterCameraFormat(CameraFormat::CAMERA_FORMAT_YCBCR_420_888);
    EXPECT_EQ(formatAdapter, VideoPixelFormatAdapter::FORMAT_YCBCR_420_888);
    formatAdapter =
        adapter.TransToAdapterCameraFormat(static_cast<CameraFormat>(0));
    EXPECT_EQ(formatAdapter, VideoPixelFormatAdapter::FORMAT_UNKNOWN);
    CameraFormat format = adapter.TransToOriCameraFormat(VideoPixelFormatAdapter::FORMAT_YCBCR_420_888);
    EXPECT_EQ(format, CameraFormat::CAMERA_FORMAT_YCBCR_420_888);
    format = adapter.TransToOriCameraFormat(static_cast<VideoPixelFormatAdapter>(-1));
    EXPECT_EQ(format, CameraFormat::CAMERA_FORMAT_INVALID);
    ExposureModeAdapter exposure = adapter.GetAdapterExposureMode(ExposureMode::EXPOSURE_MODE_LOCKED);
    EXPECT_EQ(exposure, ExposureModeAdapter::EXPOSURE_MODE_LOCKED);
    exposure = adapter.GetAdapterExposureMode(static_cast<ExposureMode>(DEFAULT_WIDTH));
    EXPECT_EQ(exposure, ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED);
}

/**
 * @tc.name: CameraAdapterImplTest_GetAdapterFocusMode_005
 * @tc.desc: GetAdapterFocusMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetAdapterFocusMode_005, TestSize.Level1)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = CameraManagerAdapterImpl::GetInstance().Create(callback);
    EXPECT_EQ(result, 0);
    FocusMode mode = adapter.GetOriFocusMode(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_EQ(mode, FocusMode::FOCUS_MODE_CONTINUOUS_AUTO);
    mode = adapter.GetOriFocusMode(static_cast<FocusModeAdapter>(-1));
    EXPECT_EQ(mode, FocusMode::FOCUS_MODE_MANUAL);
    FlashMode flashMode = adapter.GetOriFlashMode(FlashModeAdapter::FLASH_MODE_OPEN);
    EXPECT_EQ(flashMode, FlashMode::FLASH_MODE_OPEN);
    flashMode = adapter.GetOriFlashMode(static_cast<FlashModeAdapter>(-1));
    EXPECT_EQ(flashMode, FlashMode::FLASH_MODE_CLOSE);
    FocusModeAdapter focusMode = adapter.GetAdapterFocusMode(FocusMode::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_EQ(focusMode, FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    focusMode = adapter.GetAdapterFocusMode(static_cast<FocusMode>(-1));
    EXPECT_EQ(focusMode, FocusModeAdapter::FOCUS_MODE_MANUAL);
}

/**
 * @tc.name: CameraAdapterImplTest_GetOriFocusMode_006
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetOriFocusMode_006, TestSize.Level1)
{
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    int32_t result = adapter.Create(callback);
    std::vector<VideoDeviceDescriptor> devicesDiscriptor;
    adapter.GetDevicesInfo(devicesDiscriptor);
    std::shared_ptr<CameraMetadata> data = std::make_shared<CameraMetadata>(0, 0);
    dmDeviceInfo tempDmDeviceInfo;
    sptr<CameraDevice> device = new(std::nothrow) CameraDevice("test", data, tempDmDeviceInfo);
    std::string deviceId = "0";
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    sptr<ICameraDeviceService> deviceObj = nullptr;
    sptr<CameraInput> cameraInput = new(std::nothrow) CameraInput(deviceObj, device);
    adapter.cameraInput_ = cameraInput;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_EQ(result, 0);
    VideoCaptureParamsAdapter captureParams = {
        .captureFormat = {
            .width = 1,
            .height = 1,
            .frameRate = 1,
            .pixelFormat = VideoPixelFormatAdapter::FORMAT_RGBA_8888,
        },
        .enableFaceDetection = true,
    };
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
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_TransToAdapterExposureModes_007, TestSize.Level1)
{
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    std::vector<ExposureMode> exposureModes;
    exposureModes.push_back(EXPOSURE_MODE_UNSUPPORTED);
    exposureModes.push_back(EXPOSURE_MODE_LOCKED);
    exposureModes.push_back(EXPOSURE_MODE_AUTO);
    exposureModes.push_back(EXPOSURE_MODE_CONTINUOUS_AUTO);
    std::vector<ExposureModeAdapter> exposureModesAdapter;
    result = adapter.TransToAdapterExposureModes(exposureModes, exposureModesAdapter);
    EXPECT_EQ(result, 0);
    std::vector<VideoDeviceDescriptor> devicesDiscriptor;
    adapter.GetDevicesInfo(devicesDiscriptor);
    VideoCaptureParamsAdapter captureParams = {
        .captureFormat = {
            .width = 1,
            .height = 1,
            .frameRate = 1,
            .pixelFormat = VideoPixelFormatAdapter::FORMAT_RGBA_8888,
        },
        .enableFaceDetection = true,
    };
    std::shared_ptr<CameraBufferListenerAdapter> listener = std::make_shared<CameraBufferListenerAdapterMock>();
    adapter.inputInitedFlag_ = true;
    result = adapter.InitPreviewOutput(captureParams, listener);
    EXPECT_EQ(result, 0);
    result = adapter.CreateAndStartSession();
    EXPECT_NE(result, 0);
    sptr<ICaptureSession> session = nullptr;
    sptr<CaptureSession> captureSession = new(std::nothrow) CaptureSession(session);
    adapter.captureSession_ = captureSession;
    result = adapter.GetExposureModes(exposureModesAdapter);
    EXPECT_NE(result, 0);
    ExposureModeAdapter exposureModeAdapter;
    result = adapter.GetCurrentExposureMode(exposureModeAdapter);
    EXPECT_EQ(result, 0);
    VideoCaptureRangeAdapter rangeVal;
    result = adapter.GetExposureCompensation(rangeVal);
    EXPECT_EQ(result, 0);
    result = adapter.GetCaptionRangeById(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION, rangeVal);
    EXPECT_EQ(result, 0);
    adapter.GetCaptionRangeById(static_cast<RangeIDAdapter>(1), rangeVal);
    bool isMode = adapter.IsFocusModeSupported(FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO);
    EXPECT_FALSE(isMode);
    adapter.GetCurrentFocusMode();
    isMode = adapter.IsFlashModeSupported(FlashModeAdapter::FLASH_MODE_OPEN);
    EXPECT_FALSE(isMode);
}

/**
 * @tc.name: CameraAdapterImplTest_GetOriFocusMode_008
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_GetOriFocusMode_008, TestSize.Level1)
{
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    adapter.isForegound_ = true;
    adapter.isCapturing_ = true;
    result = adapter.RestartSession();
    EXPECT_NE(result, 0);
    adapter.cameraManager_ = nullptr;
    std::vector<VideoDeviceDescriptor> devicesDiscriptor;
    adapter.GetDevicesInfo(devicesDiscriptor);
    EXPECT_TRUE(devicesDiscriptor.empty());

    std::string deviceId = "0";
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    VideoCaptureParamsAdapter captureParams;
    result = adapter.InitPreviewOutput(captureParams, nullptr);
    EXPECT_NE(result, 0);
    adapter.status_ = CameraStatusAdapter::UNAVAILABLE;
    result = adapter.RestartSession();
    EXPECT_EQ(result, 0);
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
    VideoCaptureRangeAdapter rangeVal;
    result = adapter.GetExposureCompensation(rangeVal);
    EXPECT_NE(result, 0);
    result = adapter.GetCaptionRangeById(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION, rangeVal);
    EXPECT_NE(result, 0);
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
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_CameraSurfaceAdapterImpl_009, TestSize.Level1)
{
    sptr<IConsumerSurface> surface = IConsumerSurface::Create();
    EXPECT_NE(surface, nullptr);
    auto listener = std::make_shared<CameraSurfaceAdapterImpl>(surface);
    EXPECT_NE(listener, nullptr);
    listener->ReleaseBuffer(nullptr, -1);
    int32_t result = listener->ReleaseBuffer(nullptr, -1);
    EXPECT_EQ(result, -1);
    listener->cSurface_ = nullptr;
    result = listener->ReleaseBuffer(nullptr, -1);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: CameraAdapterImplTest_InitCameraInput_010
 * @tc.desc: GetOriFocusMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CameraAdapterImplTest, CameraAdapterImplTest_InitCameraInput_010, TestSize.Level1)
{
    CameraManagerAdapterImpl &adapter = CameraManagerAdapterImpl::GetInstance();
    auto callback = std::make_shared<CameraStatusCallbackAdapterMock>();
    int32_t result = adapter.Create(callback);
    EXPECT_EQ(result, 0);
    VideoCaptureParamsAdapter captureParams = {
        .captureFormat = {
            .width = 1,
            .height = 1,
            .frameRate = 1,
            .pixelFormat = VideoPixelFormatAdapter::FORMAT_RGBA_8888,
        },
        .enableFaceDetection = true,
    };
    auto listenerAdapter = std::make_shared<CameraBufferListenerAdapterMock>();
    EXPECT_NE(listenerAdapter, nullptr);
    std::string deviceId = "0";
    adapter.status_ = CameraStatusAdapter::UNAVAILABLE;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    adapter.isCapturing_ = true;
    bool isExist = adapter.IsExistCaptureTask();
    EXPECT_TRUE(isExist);
    result = adapter.StartStream(deviceId, captureParams, nullptr);
    EXPECT_NE(result, 0);
    adapter.status_ = CameraStatusAdapter::AVAILABLE;
    adapter.inputInitedFlag_ = false;
    adapter.cameraManager_ = nullptr;
    result = adapter.InitCameraInput(deviceId);
    EXPECT_NE(result, 0);
    VideoCaptureRangeAdapter rangeVal;
    result = adapter.GetCaptionRangeById(RangeIDAdapter::RANGE_ID_EXP_COMPENSATION, rangeVal);
    EXPECT_NE(result, 0);
    adapter.inputInitedFlag_ = true;
    result = adapter.InitPreviewOutput(captureParams, nullptr);
    EXPECT_NE(result, 0);
    isExist = adapter.IsExistCaptureTask();
    EXPECT_FALSE(isExist);
    result = adapter.StartStream(deviceId, captureParams, listenerAdapter);
    EXPECT_NE(result, 0);
}
} // namespace OHOS