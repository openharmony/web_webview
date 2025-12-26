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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "napi_native_mediaplayer_handler_impl.h"

using namespace testing;
using namespace OHOS::NWeb;
using namespace testing::ext;

namespace {

class MockNWebNativeMediaPlayerHandler : public NWebNativeMediaPlayerHandler {
public:
    MOCK_METHOD(void, HandleStatusChanged, (PlaybackStatus), (override));
    MOCK_METHOD(void, HandleVolumeChanged, (double), (override));
    MOCK_METHOD(void, HandleMutedChanged, (bool), (override));
    MOCK_METHOD(void, HandlePlaybackRateChanged, (double), (override));
    MOCK_METHOD(void, HandleDurationChanged, (double), (override));
    MOCK_METHOD(void, HandleTimeUpdate, (double), (override));
    MOCK_METHOD(void, HandleBufferedEndTimeChanged, (double), (override));
    MOCK_METHOD(void, HandleEnded, (), (override));
    MOCK_METHOD(void, HandleNetworkStateChanged, (NetworkState), (override));
    MOCK_METHOD(void, HandleReadyStateChanged, (ReadyState), (override));
    MOCK_METHOD(void, HandleFullScreenChanged, (bool), (override));
    MOCK_METHOD(void, HandleSeeking, (), (override));
    MOCK_METHOD(void, HandleSeekFinished, (), (override));
    MOCK_METHOD(void, HandleError, (MediaError, const std::string&), (override));
    MOCK_METHOD(void, HandleVideoSizeChanged, (double, double), (override));
};

class NapiNativeMediaPlayerHandlerImplTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        mockHandler_ = std::make_shared<StrictMock<MockNWebNativeMediaPlayerHandler>>();
        impl_ = std::make_unique<NapiNativeMediaPlayerHandlerImpl>(NWEB_ID, mockHandler_);
    }

    void TearDown() override
    {
        impl_.reset();
        mockHandler_.reset();
    }

    static constexpr int32_t NWEB_ID = 100;

    std::shared_ptr<MockNWebNativeMediaPlayerHandler> mockHandler_;
    std::unique_ptr<NapiNativeMediaPlayerHandlerImpl> impl_;
};

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleStatusChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleStatusChanged(PlaybackStatus::PLAYING)).Times(1);
    impl_->HandleStatusChanged(PlaybackStatus::PLAYING);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleVolumeChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleVolumeChanged(0.5)).Times(1);
    impl_->HandleVolumeChanged(0.5);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleMutedChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleMutedChanged(true)).Times(1);
    impl_->HandleMutedChanged(true);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandlePlaybackRateChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandlePlaybackRateChanged(1.25)).Times(1);
    impl_->HandlePlaybackRateChanged(1.25);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleDurationChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleDurationChanged(100.0)).Times(1);
    impl_->HandleDurationChanged(100.0);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleTimeUpdate_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleTimeUpdate(20.0)).Times(1);
    impl_->HandleTimeUpdate(20.0);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleBufferedEndTimeChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleBufferedEndTimeChanged(30.0)).Times(1);
    impl_->HandleBufferedEndTimeChanged(30.0);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleEnded_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleEnded()).Times(1);
    impl_->HandleEnded();
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleNetworkStateChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleNetworkStateChanged(NetworkState::EMPTY)).Times(1);
    impl_->HandleNetworkStateChanged(NetworkState::EMPTY);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleReadyStateChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleReadyStateChanged(ReadyState::HAVE_ENOUGH_DATA)).Times(1);
    impl_->HandleReadyStateChanged(ReadyState::HAVE_ENOUGH_DATA);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleFullScreenChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleFullScreenChanged(true)).Times(1);
    impl_->HandleFullScreenChanged(true);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleSeeking_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleSeeking()).Times(1);
    impl_->HandleSeeking();
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleSeekFinished_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleSeekFinished()).Times(1);
    impl_->HandleSeekFinished();
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleError_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_,
        HandleError(MediaError::NETWORK_ERROR, "test error")).Times(1);
    impl_->HandleError(MediaError::NETWORK_ERROR, "test error");
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleVideoSizeChanged_CallHandler, TestSize.Level1)
{
    EXPECT_CALL(*mockHandler_, HandleVideoSizeChanged(1920, 1080)).Times(1);
    impl_->HandleVideoSizeChanged(1920, 1080);
}

HWTEST_F(NapiNativeMediaPlayerHandlerImplTest, HandleMethods_HandlerNull_NotCrash, TestSize.Level1)
{
    NapiNativeMediaPlayerHandlerImpl impl(NWEB_ID, nullptr);
    EXPECT_NO_THROW(impl.HandleStatusChanged(PlaybackStatus::PLAYING));
    EXPECT_NO_THROW(impl.HandleVolumeChanged(1.0));
    EXPECT_NO_THROW(impl.HandleMutedChanged(false));
    EXPECT_NO_THROW(impl.HandlePlaybackRateChanged(1.0));
    EXPECT_NO_THROW(impl.HandleDurationChanged(10.0));
    EXPECT_NO_THROW(impl.HandleTimeUpdate(5.0));
    EXPECT_NO_THROW(impl.HandleBufferedEndTimeChanged(8.0));
    EXPECT_NO_THROW(impl.HandleEnded());
    EXPECT_NO_THROW(impl.HandleNetworkStateChanged(NetworkState::NETWORK_ERROR));
    EXPECT_NO_THROW(impl.HandleReadyStateChanged(ReadyState::HAVE_NOTHING));
    EXPECT_NO_THROW(impl.HandleFullScreenChanged(false));
    EXPECT_NO_THROW(impl.HandleSeeking());
    EXPECT_NO_THROW(impl.HandleSeekFinished());
    EXPECT_NO_THROW(impl.HandleError(MediaError::NETWORK_ERROR, "err"));
    EXPECT_NO_THROW(impl.HandleVideoSizeChanged(0, 0));
}
} // namespace
