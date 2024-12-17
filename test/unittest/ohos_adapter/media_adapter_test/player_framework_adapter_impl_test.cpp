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

#include "foundation/multimedia/player_framework/interfaces/inner_api/native/media_errors.h"
#include "gtest/gtest.h"
#define private public
#include "player_framework_adapter_impl.h"
#undef private
#include "ohos_adapter_helper.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {
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
 * @tc.require:
 */
HWTEST_F(PlayerAdapterImplTest, HandlesInvalidArguments, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(PlayerAdapterImplTest, NormalTest, TestSize.Level1)
{
    EXPECT_EQ(playerAdapter_->SetPlayerCallback(std::move(callbackTest_)), Media::MSERR_OK);
    EXPECT_NE(playerAdapter_->SetSource(-1, 0, 0), -1);
    EXPECT_NE(playerAdapter_->SetSource(sourceUrl_), -1);

    auto surfaceAdapter = OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    EXPECT_NE(surfaceAdapter, nullptr);
    EXPECT_NE(playerAdapter_->SetVideoSurface(std::move(surfaceAdapter)), -1);
    EXPECT_NE(playerAdapter_->PrepareAsync(), -1);
    EXPECT_EQ(playerAdapter_->SetVolume(0, 0), Media::MSERR_OK);
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
 * @tc.require:
 */
HWTEST_F(PlayerAdapterImplTest, ModeConvertTest, TestSize.Level1)
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
    std::shared_ptr<PlayerCallbackImpl> playerCallback_ = nullptr;
    Media::Format infoBody_;
    const std::string errorMsg_ = "test";
};

PlayerCallbackImplTest::PlayerCallbackImplTest()
{
    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    EXPECT_NE(callbackTest, nullptr);
    playerCallback_ = std::make_shared<PlayerCallbackImpl>(std::move(callbackTest));
    EXPECT_NE(playerCallback_, nullptr);
    EXPECT_NE(playerCallback_->callbackAdapter_, nullptr);
}

/**
 * @tc.name: InvalidScene.
 * @tc.desc: test invalid scene of PlayerCallbackImpl.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(PlayerCallbackImplTest, InvalidScene, TestSize.Level1)
{
    ASSERT_NE(playerCallback_, nullptr);
    auto callbackImpl = static_cast<PlayerCallbackTest*>(playerCallback_->callbackAdapter_.get());
    playerCallback_->OnInfo(Media::INFO_TYPE_ERROR_MSG, 0, infoBody_);
    EXPECT_EQ(callbackImpl->infoType_, PlayerOnInfoType::INFO_TYPE_UNSET);
    playerCallback_->OnError(Media::MSERR_EXT_OK, errorMsg_);
    EXPECT_EQ(callbackImpl->errorType_, PlayerAdapterErrorType::INVALID_CODE);

    playerCallback_->callbackAdapter_ = nullptr;
    playerCallback_->OnInfo(Media::INFO_TYPE_EOS, 0, infoBody_);
    playerCallback_->OnError(Media::MSERR_EXT_OK, errorMsg_);
}

class PlayerCallbackOnErrorParamTest : public PlayerCallbackImplTest,
                                       public testing::WithParamInterface<std::tuple<PlayerAdapterErrorType, int32_t>> {
};

/**
 * @tc.name: NormalTest.
 * @tc.desc: Value-Parameterized test of PlayerCallbackImpl::OnError().
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_P(PlayerCallbackOnErrorParamTest, NormalTest, TestSize.Level1)
{
    ASSERT_NE(playerCallback_, nullptr);
    PlayerAdapterErrorType errorType = PlayerAdapterErrorType::INVALID_CODE;
    int32_t errorCode = 0;
    playerCallback_->OnError(errorCode, errorMsg_);
    EXPECT_EQ(static_cast<PlayerCallbackTest*>(playerCallback_->callbackAdapter_.get())->errorType_, errorType);
}

INSTANTIATE_TEST_SUITE_P(UnsupportTypeTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::UNSUPPORT_TYPE),
        testing::Range(
            static_cast<int32_t>(Media::MSERR_UNSUPPORT), static_cast<int32_t>(Media::MSERR_UNSUPPORT_SOURCE) + 1)));

constexpr int32_t fatalError[] = { Media::MSERR_EXT_NO_MEMORY, Media::MSERR_EXT_SERVICE_DIED,
    Media::MSERR_CREATE_PLAYER_ENGINE_FAILED, Media::MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED,
    Media::MSERR_AUD_DEC_FAILED, Media::MSERR_VID_DEC_FAILED, Media::MSERR_FILE_ACCESS_FAILED };

INSTANTIATE_TEST_SUITE_P(FatalErrorTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::FATAL_ERROR), testing::ValuesIn(fatalError)));

INSTANTIATE_TEST_SUITE_P(InvalidTypeTest, PlayerCallbackOnErrorParamTest,
    testing::Combine(testing::Values(PlayerAdapterErrorType::INVALID_CODE),
        testing::Range(static_cast<int32_t>(Media::MSERR_START_FAILED),
            static_cast<int32_t>(Media::MSERR_DATA_SOURCE_ERROR_UNKNOWN))));

class PlayerCallbackOnInfoParamTest : public PlayerCallbackImplTest,
                                      public testing::WithParamInterface<Media::PlayerOnInfoType> {};

/**
 * @tc.name: NormalTest.
 * @tc.desc: Value-Parameterized test of PlayerCallbackImpl::OnInfo().
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_P(PlayerCallbackOnInfoParamTest, NormalTest, TestSize.Level1)
{
    ASSERT_NE(playerCallback_, nullptr);
    Media::PlayerOnInfoType infoType = GetParam();
    playerCallback_->OnInfo(infoType, 0, infoBody_);
    EXPECT_NE(static_cast<PlayerCallbackTest*>(playerCallback_->callbackAdapter_.get())->infoType_,
        PlayerOnInfoType::INFO_TYPE_UNSET);
}

INSTANTIATE_TEST_SUITE_P(NormalTest, PlayerCallbackOnInfoParamTest,
    testing::Values(Media::INFO_TYPE_SEEKDONE, Media::INFO_TYPE_EOS, Media::INFO_TYPE_STATE_CHANGE,
        Media::INFO_TYPE_POSITION_UPDATE, Media::INFO_TYPE_MESSAGE));
} // namespace OHOS::NWeb
