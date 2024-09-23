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

#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "player_framework_adapter_impl.h"
#undef private
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
static void PlayerInfoCallback(OH_AVPlayer *player, AVPlayerOnInfoType type, OH_AVFormat* infoBody, void *userData) {
    if (userData == nullptr) {
        return;
    }
    int32_t state = 0;
    int32_t hintValue = -1;
    PlayerCallbackAdapter* callback = (PlayerCallbackAdapter*)(userData);
    callback->OnInfo(PlayerOnInfoType::INFO_TYPE_UNSET, state, hintValue);
}
static void PlayerErrorCallback(OH_AVPlayer *player, int32_t errorCode, const char *errorMsg, void *userData) {
    if (userData == nullptr) {
        return;
    }
    auto errorType = PlayerAdapterErrorType::INVALID_CODE;
    PlayerCallbackAdapter* callback = (PlayerCallbackAdapter*)(userData);
    callback->OnError(errorType);
}
class PlayerCallbackTest : public PlayerCallbackAdapter {
public:
    PlayerCallbackTest() = default;
    ~PlayerCallbackTest() override = default;
    void OnInfo(PlayerOnInfoType type, int32_t extra, int32_t value) override
    {
        infoType_ = type;
    }
    void OnError(PlayerAdapterErrorType errorType) override
    {
        errorType_ = errorType;
    }
    PlayerOnInfoType infoType_ = PlayerOnInfoType::INFO_TYPE_UNSET;
    PlayerAdapterErrorType errorType_ = PlayerAdapterErrorType::INVALID_CODE;
};

class PlayerAdapterImplTest : public testing::Test {
protected:
    void SetUp() override;
    std::unique_ptr<PlayerAdapterImpl> playerAdapter_ = nullptr;
    std::unique_ptr<PlayerCallbackTest> callbackTest_ = nullptr;
    const std::string sourceUrl_ = "sourcetest.mp3";
    int32_t currentTime_ = 0;
    int32_t duration_ = 0;
};

void PlayerAdapterImplTest::SetUp()
{
    EXPECT_EQ(playerAdapter_, nullptr);
    playerAdapter_ = std::make_unique<PlayerAdapterImpl>();
    ASSERT_NE(playerAdapter_, nullptr);
    ASSERT_NE(playerAdapter_->player_, nullptr);
    callbackTest_ = std::make_unique<PlayerCallbackTest>();
    EXPECT_NE(callbackTest_, nullptr);
}

/**
 * @tc.name: HandlesInvalidArguments.
 * @tc.desc: test invalid scene of PlayerCallbackImpl.
 * @tc.type: FUNC.
 */
TEST_F(PlayerAdapterImplTest, HandlesInvalidArguments)
{
    EXPECT_EQ(playerAdapter_->SetPlayerCallback(nullptr), -1);
    EXPECT_EQ(playerAdapter_->SetVideoSurface(nullptr), -1);

    playerAdapter_->player_ = nullptr;
    EXPECT_EQ(playerAdapter_->SetPlayerCallback(nullptr), -1);
    EXPECT_EQ(playerAdapter_->SetSource(sourceUrl_), -1);
    EXPECT_EQ(playerAdapter_->SetSource(0, 0, 0), -1);
    EXPECT_EQ(playerAdapter_->SetVideoSurface(nullptr), -1);
    EXPECT_EQ(playerAdapter_->SetVolume(0, 0), -1);
    EXPECT_EQ(playerAdapter_->Seek(0, PlayerSeekMode::SEEK_CLOSEST_SYNC), -1);
    EXPECT_EQ(playerAdapter_->Play(), -1);
    EXPECT_EQ(playerAdapter_->Pause(), -1);
    EXPECT_EQ(playerAdapter_->PrepareAsync(), -1);
    EXPECT_EQ(playerAdapter_->GetCurrentTime(currentTime_), -1);
    EXPECT_EQ(playerAdapter_->GetDuration(duration_), -1);
    EXPECT_EQ(playerAdapter_->SetPlaybackSpeed(PlaybackRateMode::SPEED_FORWARD_1_00_X), -1);
}

/**
 * @tc.name: NormalTest.
 * @tc.desc: test normal scene of PlayerCallbackImpl.
 * @tc.type: FUNC.
 */
TEST_F(PlayerAdapterImplTest, NormalTest)
{
    EXPECT_EQ(playerAdapter_->SetPlayerCallback(std::move(callbackTest_)), 0);
    EXPECT_NE(playerAdapter_->SetSource(-1, 0, 0), -1);
    EXPECT_NE(playerAdapter_->SetSource(sourceUrl_), -1);

    auto surfaceAdapter = OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    EXPECT_NE(surfaceAdapter, nullptr);
    EXPECT_NE(playerAdapter_->SetVideoSurface(std::move(surfaceAdapter)), -1);
    EXPECT_NE(playerAdapter_->PrepareAsync(), -1);
    EXPECT_EQ(playerAdapter_->SetVolume(0, 0), 0);
    EXPECT_NE(playerAdapter_->Seek(0, PlayerSeekMode::SEEK_CLOSEST_SYNC), -1);
    EXPECT_NE(playerAdapter_->Play(), -1);
    EXPECT_NE(playerAdapter_->Pause(), -1);
    EXPECT_NE(playerAdapter_->GetCurrentTime(currentTime_), -1);
    EXPECT_NE(playerAdapter_->GetDuration(duration_), -1);
    EXPECT_NE(playerAdapter_->SetPlaybackSpeed(PlaybackRateMode::SPEED_FORWARD_1_00_X), -1);
}

/**
 * @tc.name: ModeConvertTest.
 * @tc.desc: test seek() and SetPlaybackSpeed() in different mode.
 * @tc.type: FUNC.
 */
TEST_F(PlayerAdapterImplTest, ModeConvertTest)
{
    const std::array seekModeArray = { PlayerSeekMode::SEEK_NEXT_SYNC, PlayerSeekMode::SEEK_PREVIOUS_SYNC,
        PlayerSeekMode::SEEK_CLOSEST_SYNC, PlayerSeekMode::SEEK_CLOSEST };
    const std::array rateModeArray = { PlaybackRateMode::SPEED_FORWARD_0_75_X, PlaybackRateMode::SPEED_FORWARD_1_00_X,
        PlaybackRateMode::SPEED_FORWARD_1_25_X, PlaybackRateMode::SPEED_FORWARD_1_75_X,
        PlaybackRateMode::SPEED_FORWARD_2_00_X };

    for (PlayerSeekMode mode : seekModeArray) {
        EXPECT_NE(playerAdapter_->Seek(0, mode), -1);
    }
    for (PlaybackRateMode mode : rateModeArray) {
        EXPECT_NE(playerAdapter_->SetPlaybackSpeed(mode), -1);
    }
}

class PlayerCallbackImplTest : public testing::Test {
protected:
    PlayerCallbackImplTest();
    std::shared_ptr<PlayerCallbackTest> callbackAdapter_ = std::make_unique<PlayerCallbackTest>();
    const std::string errorMsg_ = "test";
};

PlayerCallbackImplTest::PlayerCallbackImplTest()
{
    EXPECT_NE(callbackAdapter_, nullptr);
}

/**
 * @tc.name: InvalidScene.
 * @tc.desc: test invalid scene of PlayerCallbackImpl.
 * @tc.type: FUNC.
 */
TEST_F(PlayerCallbackImplTest, InvalidScene)
{
    ASSERT_NE(callbackAdapter_, nullptr);
    OH_AVFormat* format = OH_AVFormat_Create();
    PlayerInfoCallback(nullptr, (AVPlayerOnInfoType)(-1), format, callbackAdapter_.get());
    EXPECT_EQ(callbackAdapter_->infoType_, PlayerOnInfoType::INFO_TYPE_UNSET);
    
    PlayerErrorCallback(nullptr, 0, errorMsg_.c_str(), callbackAdapter_.get());
    EXPECT_EQ(callbackAdapter_->errorType_, PlayerAdapterErrorType::INVALID_CODE);

    format = OH_AVFormat_Create();
    PlayerInfoCallback(nullptr, AVPlayerOnInfoType::AV_INFO_TYPE_SEEKDONE, format, nullptr);
    PlayerErrorCallback(nullptr, 0, errorMsg_.c_str(), nullptr);
}

class PlayerCallbackOnErrorParamTest : public PlayerCallbackImplTest,
                                       public testing::WithParamInterface<std::tuple<PlayerAdapterErrorType, int32_t>> {
};

/**
 * @tc.name: NormalTest.
 * @tc.desc: Value-Parameterized test of PlayerCallbackImpl::OnError().
 * @tc.type: FUNC.
 */
TEST_P(PlayerCallbackOnErrorParamTest, NormalTest)
{
    ASSERT_NE(callbackAdapter_, nullptr);
    PlayerAdapterErrorType errorType = PlayerAdapterErrorType::INVALID_CODE;
    int32_t errorCode = -1;
    PlayerErrorCallback(nullptr, errorCode, errorMsg_.c_str(), callbackAdapter_.get());
    EXPECT_EQ(static_cast<PlayerCallbackTest*>(callbackAdapter_.get())->errorType_, errorType);
}

INSTANTIATE_TEST_SUITE_P(UnsupportTypeTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::UNSUPPORT_TYPE),
        testing::Range(
            static_cast<int32_t>(PlayerAdapterErrorType::UNSUPPORT_TYPE),
            static_cast<int32_t>(PlayerAdapterErrorType::UNSUPPORT_TYPE))));

constexpr int32_t fatalError[] = { OH_AVErrCode::AV_ERR_NO_MEMORY, OH_AVErrCode::AV_ERR_SERVICE_DIED,
    OH_AVErrCode::AV_ERR_IO};

INSTANTIATE_TEST_SUITE_P(FatalErrorTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::FATAL_ERROR), testing::ValuesIn(fatalError)));

INSTANTIATE_TEST_SUITE_P(InvalidTypeTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::INVALID_CODE),
        testing::Range(static_cast<int32_t>(OH_AVErrCode::AV_ERR_NO_MEMORY),
            static_cast<int32_t>(OH_AVErrCode::AV_ERR_VIDEO_UNSUPPORTED_COLOR_SPACE_CONVERSION))));

class PlayerCallbackOnInfoParamTest : public PlayerCallbackImplTest,
                                      public testing::WithParamInterface<AVPlayerOnInfoType> {};

/**
 * @tc.name: NormalTest.
 * @tc.desc: Value-Parameterized test of PlayerCallbackImpl::OnInfo().
 * @tc.type: FUNC.
 */
TEST_P(PlayerCallbackOnInfoParamTest, NormalTest)
{
    ASSERT_NE(callbackAdapter_, nullptr);
    OH_AVFormat* format = OH_AVFormat_Create();
    PlayerInfoCallback(nullptr, AVPlayerOnInfoType::AV_INFO_TYPE_SEEKDONE, format, callbackAdapter_.get());
    EXPECT_EQ(static_cast<PlayerCallbackTest*>(callbackAdapter_.get())->infoType_,
        PlayerOnInfoType::INFO_TYPE_UNSET);
}

INSTANTIATE_TEST_SUITE_P(NormalTest, PlayerCallbackOnInfoParamTest,
    testing::Values(AVPlayerOnInfoType::AV_INFO_TYPE_SEEKDONE, AVPlayerOnInfoType::AV_INFO_TYPE_EOS,
    AVPlayerOnInfoType::AV_INFO_TYPE_STATE_CHANGE, AVPlayerOnInfoType::AV_INFO_TYPE_POSITION_UPDATE,
    AVPlayerOnInfoType::AV_INFO_TYPE_MESSAGE, AVPlayerOnInfoType::AV_INFO_TYPE_INTERRUPT_EVENT));
} // namespace OHOS::NWeb
