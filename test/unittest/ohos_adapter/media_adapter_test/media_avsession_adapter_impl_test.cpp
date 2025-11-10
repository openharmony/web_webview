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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "media_avsession_adapter.h"
#include "media_avsession_adapter_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AVSession;

constexpr int32_t RET_ERROR = -1;
constexpr int32_t RET_SUCCESS = 0;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<MediaAVSessionCallbackImpl> g_callback;
std::shared_ptr<MediaAVSessionKey> g_key;
std::shared_ptr<MediaAVSessionAdapterImpl> g_adapter;
} // namespace

class AVSessionMock : public AVSession::AVSession {
public:
    AVSessionMock() = default;
    MOCK_METHOD0(GetSessionId, std::string());
    MOCK_METHOD0(GetSessionType, std::string());
    MOCK_METHOD1(GetAVMetaData, int32_t(AVMetaData&));
    MOCK_METHOD1(SetAVMetaData, int32_t(const AVMetaData&));
    MOCK_METHOD1(SetAVCallMetaData, int32_t(const AVCallMetaData& meta));
    MOCK_METHOD1(SetAVCallState, int32_t(const AVCallState& avCallState));
    MOCK_METHOD1(GetAVPlaybackState, int32_t(AVPlaybackState& state));
    MOCK_METHOD1(SetAVPlaybackState, int32_t(const AVPlaybackState& state));
    MOCK_METHOD1(GetAVQueueItems, int32_t(std::vector<AVQueueItem>&));
    MOCK_METHOD1(SetAVQueueItems, int32_t(const std::vector<AVQueueItem>&));
    MOCK_METHOD1(GetAVQueueTitle, int32_t(std::string&));
    MOCK_METHOD1(SetAVQueueTitle, int32_t(const std::string&));
    MOCK_METHOD1(SetLaunchAbility, int32_t(const AbilityRuntime::WantAgent::WantAgent&));
    MOCK_METHOD1(GetExtras, int32_t(AAFwk::WantParams&));
    MOCK_METHOD1(SetExtras, int32_t(const AAFwk::WantParams&));
    MOCK_METHOD0(GetController, std::shared_ptr<AVSessionController>());
    MOCK_METHOD1(RegisterCallback, int32_t(const std::shared_ptr<AVSessionCallback>&));
    MOCK_METHOD0(Activate, int32_t());
    MOCK_METHOD0(Deactivate, int32_t());
    MOCK_METHOD0(IsActive, bool());
    MOCK_METHOD0(Destroy, int32_t());
    MOCK_METHOD1(AddSupportCommand, int32_t(const int32_t));
    MOCK_METHOD1(DeleteSupportCommand, int32_t(const int32_t));
    MOCK_METHOD2(SetSessionEvent, int32_t(const std::string&, const AAFwk::WantParams&));
#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    MOCK_METHOD0(GetAVCastController, std::shared_ptr<AVCastController>());
    MOCK_METHOD0(ReleaseCast, int32_t());
    MOCK_METHOD0(StartCastDisplayListener, int32_t());
    MOCK_METHOD0(StopCastDisplayListener, int32_t());
    MOCK_METHOD1(GetAllCastDisplays, int32_t(std::vector<CastDisplayInfo>&));
#endif
};

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() {};
    void Pause() {};
    void Stop() {};
    void SeekTo(int64_t millisTime) {};
};

class MediaAVSessionAdapterImplMock : public MediaAVSessionAdapterImpl {
public:
    MediaAVSessionAdapterImplMock() = default;
    ~MediaAVSessionAdapterImplMock() = default;
    MOCK_METHOD0(Activate, bool());
};

class MetaDataMock : public OHOS::NWeb::MediaAVSessionMetadataAdapter {
public:
    MOCK_METHOD0(GetTitle, std::string());
    MOCK_METHOD1(SetTitle, void(const std::string& title));
    MOCK_METHOD0(GetArtist, std::string());
    MOCK_METHOD1(SetArtist, void(const std::string& artist));
    MOCK_METHOD0(GetAlbum, std::string());
    MOCK_METHOD1(SetAlbum, void(const std::string& album));
};
class PositionMock : public OHOS::NWeb::MediaAVSessionPositionAdapter {
public:
    MOCK_METHOD0(GetDuration, int64_t());
    MOCK_METHOD1(SetDuration, void(int64_t duration));
    MOCK_METHOD0(GetElapsedTime, int64_t());
    MOCK_METHOD1(SetElapsedTime, void(int64_t elapsedTime));
    MOCK_METHOD0(GetUpdateTime, int64_t());
    MOCK_METHOD1(SetUpdateTime, void(int64_t updateTime));
};

class MediaAVSessionMetadataAdapterMock : public MediaAVSessionMetadataAdapter {
public:
    MediaAVSessionMetadataAdapterMock() = default;

    ~MediaAVSessionMetadataAdapterMock() = default;

    MOCK_METHOD1(SetTitle, void(const std::string& title));
    MOCK_METHOD0(GetTitle, std::string());
    MOCK_METHOD1(SetArtist, void(const std::string& artist));
    MOCK_METHOD0(GetArtist, std::string());
    MOCK_METHOD1(SetAlbum, void(const std::string& album));
    MOCK_METHOD0(GetAlbum, std::string());
};

class AVPlaybackStateMock : public AVPlaybackState {
public:
    AVPlaybackStateMock() = default;
    ~AVPlaybackStateMock() = default;
    MOCK_METHOD0(GetState, int32_t());
    MOCK_METHOD1(SetState, void(int32_t));
};

class AVMetaDataMock : public AVMetaData {
public:
    AVMetaDataMock() = default;

    ~AVMetaDataMock() = default;

    MOCK_METHOD1(SetTitle, void(const std::string& title));
    MOCK_METHOD0(GetTitle, std::string());

    MOCK_METHOD1(SetArtist, void(const std::string& artist));
    MOCK_METHOD0(GetArtist, std::string());

    MOCK_METHOD1(SetAlbum, void(const std::string& album));
    MOCK_METHOD0(GetAlbum, std::string());

    MOCK_METHOD1(SetDuration, void(const int64_t& duration));
    MOCK_METHOD0(GetDuration, int64_t());
};

class MediaAVSessionPositionAdapterMock : public MediaAVSessionPositionAdapter {
public:
    MediaAVSessionPositionAdapterMock() = default;

    ~MediaAVSessionPositionAdapterMock() = default;

    MOCK_METHOD1(SetDuration, void(int64_t duration));
    MOCK_METHOD0(GetDuration, int64_t());

    MOCK_METHOD1(SetElapsedTime, void(int64_t elapsedTime));
    MOCK_METHOD0(GetElapsedTime, int64_t());

    MOCK_METHOD1(SetUpdateTime, void(int64_t updateTime));
    MOCK_METHOD0(GetUpdateTime, int64_t());
};

class MediaAVSessionKeyMock : public MediaAVSessionKey {
public:
    MediaAVSessionKeyMock() = default;

    ~MediaAVSessionKeyMock() = default;

    MOCK_METHOD0(GetElement, std::shared_ptr<AppExecFwk::ElementName>());
};

class MediaAVSessionCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class MediaAVSessionKeyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class MediaAVSessionAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MediaAVSessionCallbackImplTest::SetUpTestCase(void) {}

void MediaAVSessionCallbackImplTest::TearDownTestCase(void) {}

void MediaAVSessionCallbackImplTest::SetUp(void)
{
    std::shared_ptr<MediaAVSessionCallbackAdapterMock> mediaAvSessionCB;
    g_callback = std::make_shared<MediaAVSessionCallbackImpl>(mediaAvSessionCB);
    ASSERT_NE(g_callback, nullptr);
}

void MediaAVSessionCallbackImplTest::TearDown(void)
{
    g_callback = nullptr;
}

void MediaAVSessionKeyTest::SetUpTestCase(void) {}

void MediaAVSessionKeyTest::TearDownTestCase(void) {}

void MediaAVSessionKeyTest::SetUp(void)
{
    g_key = std::make_shared<MediaAVSessionKey>();
    ASSERT_NE(g_key, nullptr);
}

void MediaAVSessionKeyTest::TearDown(void)
{
    g_key = nullptr;
}

void MediaAVSessionAdapterImplTest::SetUpTestCase(void) {}

void MediaAVSessionAdapterImplTest::TearDownTestCase(void) {}

void MediaAVSessionAdapterImplTest::SetUp(void)
{
    g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
    ASSERT_NE(g_adapter, nullptr);
}

void MediaAVSessionAdapterImplTest::TearDown(void)
{
    g_adapter = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackImpl_001.
 * @tc.desc: test of MediaAVSessionCallbackImpl :: Onplay()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionCallbackImplTest, NWebMediaAdapterTest_MediaAVSessionCallbackImpl_001, TestSize.Level1)
{
    g_callback->callbackAdapter_ = nullptr;
    AVControlCommand cmd;
    g_callback->OnPlay(cmd);
    EXPECT_EQ(g_callback->callbackAdapter_, nullptr);

    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback->callbackAdapter_ = callbackMock;
    g_callback->OnPlay(cmd);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackImpl_002.
 * @tc.desc: test of MediaAVSessionCallbackImpl :: OnPause()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionCallbackImplTest, NWebMediaAdapterTest_MediaAVSessionCallbackImpl_002, TestSize.Level1)
{
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnPause();
    EXPECT_EQ(g_callback->callbackAdapter_, nullptr);

    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback->callbackAdapter_ = callbackMock;
    g_callback->OnPause();
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackImpl_003.
 * @tc.desc: test of MediaAVSessionCallbackImpl :: OnStop()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionCallbackImplTest, NWebMediaAdapterTest_MediaAVSessionCallbackImpl_003, TestSize.Level1)
{
    AVControlCommand cmd;
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStop();
    g_callback->OnPlayNext(cmd);
    g_callback->OnPlayPrevious(cmd);
    g_callback->OnFastForward(0, cmd);
    g_callback->OnRewind(0, cmd);
    g_callback->OnSetSpeed(0);
    g_callback->OnSetLoopMode(0);
    g_callback->OnToggleFavorite("");
    g_callback->OnSkipToQueueItem(0);
    g_callback->OnAVCallAnswer();
    g_callback->OnAVCallHangUp();
    g_callback->OnAVCallToggleCallMute();
    g_callback->OnPlayFromAssetId(0);
    AAFwk::WantParams param;
    AVSession::CastDisplayInfo castDisplayInfo;
    AVSession::OutputDeviceInfo info;
    g_callback->OnOutputDeviceChange(0, info);
    g_callback->OnCommonCommand("test", param);
    g_callback->OnCastDisplayChange(castDisplayInfo);

    EXPECT_EQ(g_callback->callbackAdapter_, nullptr);

    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback->callbackAdapter_ = callbackMock;
    g_callback->OnStop();
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
}
/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackImpl_004s.
 * @tc.desc: test of MediaAVSessionCallbackImpl :: OnSeek()
 * @tc.type: FUNC.
 * @tc.require:
 */

HWTEST_F(MediaAVSessionCallbackImplTest, NWebMediaAdapterTest_MediaAVSessionCallbackImpl_004, TestSize.Level1)
{
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnSeek(100);
    EXPECT_EQ(g_callback->callbackAdapter_, nullptr);

    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback->callbackAdapter_ = callbackMock;
    g_callback->OnSeek(100);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_001.
 * @tc.desc: test of MediaAVSessionKey :: GetType()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_001, TestSize.Level1)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    g_key->type_ = type;
    EXPECT_EQ(g_key->GetType(), type);
    g_key->Init();
    EXPECT_NE(g_key->GetType(), type);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_002.
 * @tc.desc: test of MediaAVSessionKey :: GetPID()
 * @tc.type: FUNC.
 * @tc.require:
 */

HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_002, TestSize.Level1)
{
    auto pid = 100;
    g_key->pid_ = pid;
    auto ret = g_key->GetPID();
    EXPECT_EQ(ret, pid);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_003.
 * @tc.desc: test of MediaAVSessionKey :: GetElement()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_003, TestSize.Level1)
{
    OHOS::AppExecFwk::ElementName element;
    g_key->element_ = element;
    auto ret = g_key->GetElement();
    EXPECT_EQ(ret, element);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_004.
 * @tc.desc: test of MediaAVSessionKey :: GetType()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_004, TestSize.Level1)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    g_key->type_ = type;
    auto ret = g_key->GetType();
    EXPECT_EQ(ret, type);
}
/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_005.
 * @tc.desc: test of MediaAVSessionKey :: SetType()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_005, TestSize.Level1)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    g_key->SetType(type);
    auto ret = g_key->GetType();
    EXPECT_EQ(ret, type);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_006.
 * @tc.desc: test of MediaAVSessionKey :: ToString()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_006, TestSize.Level1)
{
    auto pid = 100;
    OHOS::AppExecFwk::ElementName element;
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    auto Str = (std::to_string(pid) + "_" + element.GetBundleName() + "_" + element.GetAbilityName());
    g_key->pid_ = pid;
    g_key->element_ = element;
    g_key->type_ = type;
    auto ret = g_key->ToString();
    EXPECT_EQ(ret, Str);
}
/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_001.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: CreateAVSession()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_001, TestSize.Level1)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    auto avSessionKey = std::make_shared<MediaAVSessionKey>();
    g_adapter->avSessionKey_ = avSessionKey;
    g_adapter->avSessionKey_->Init();
    auto avSessionMock = std::make_shared<AVSessionMock>();
    auto avSessionMock_dummy = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       g_adapter->avSession_));

    bool ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, false);

    type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    g_adapter->avSessionKey_->SetType(type);
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, false);

    g_adapter->avSessionKey_->SetType(MediaAVSessionType::MEDIA_TYPE_INVALID);
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    g_adapter->avSessionKey_->SetType(MediaAVSessionType::MEDIA_TYPE_VIDEO);
    ret = g_adapter->CreateAVSession(type);

    g_adapter->avSession_ = nullptr;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    g_adapter->avSession_ = avSessionMock;
    type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    g_adapter->avSessionMap.clear();
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    g_adapter->avSession_ = avSessionMock;
    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       avSessionMock_dummy));
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       g_adapter->avSession_));
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, false);

    g_adapter->avSession_ = avSessionMock;
    g_adapter->avSessionMap.clear();
    type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_002.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DestroyAVSession()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_002, TestSize.Level1)
{
    g_adapter->avSession_ = nullptr;
    g_adapter->DestroyAVSession();
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, Destroy()).WillOnce(::testing::Return(RET_ERROR));
    g_adapter->DestroyAVSession();
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, Destroy()).WillOnce(::testing::Return(RET_SUCCESS));
    g_adapter->DestroyAVSession();
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    g_adapter->avSession_ = nullptr;
    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       g_adapter->avSession_));
    g_adapter->DestroyAVSession();

    g_adapter->avSessionMap.clear();
    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_003.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: RegistCallback()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_003, TestSize.Level1)
{
    auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_adapter->avSession_ = nullptr;
    bool ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, false);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_ERROR));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, false);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, AddSupportCommand(_)).WillRepeatedly(::testing::Return(RET_SUCCESS));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, AddSupportCommand(_)).WillRepeatedly(::testing::Return(RET_ERROR));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_ERROR));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, false);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_ERROR));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, false);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, AddSupportCommand(_)).WillRepeatedly(::testing::Return(RET_SUCCESS));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);
    g_adapter->avSession_ = nullptr;

    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, RegisterCallback(_)).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, AddSupportCommand(_)).WillRepeatedly(::testing::Return(RET_ERROR));
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_004.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: IsActivated()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_004, TestSize.Level1)
{
    g_adapter->avSession_ = nullptr;
    bool ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(true));
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, true);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_005.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: Activate()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_005, TestSize.Level1)
{
    g_adapter->avSession_ = nullptr;
    bool ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(true));
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_SUCCESS));
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*avSessionMock, IsActive()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*avSessionMock, Activate()).WillOnce(::testing::Return(RET_ERROR));
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_006.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DeActivate()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_006, TestSize.Level1)
{
    g_adapter->avSession_ = nullptr;
    g_adapter->DeActivate();
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(false));
    g_adapter->DeActivate();
    EXPECT_NE(g_adapter->avSession_, nullptr);

    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, Deactivate()).WillRepeatedly(::testing::Return(RET_SUCCESS));
    g_adapter->DeActivate();
    EXPECT_NE(g_adapter->avSession_, nullptr);

    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, Deactivate()).WillRepeatedly(::testing::Return(RET_ERROR));
    g_adapter->DeActivate();
    EXPECT_NE(g_adapter->avSession_, nullptr);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_007.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetMetadata()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_007, TestSize.Level1)
{
    auto avSessionMock = std::make_shared<AVSessionMock>();
    auto metadataMock = std::make_shared<MetaDataMock>();
    g_adapter->avSession_ = avSessionMock;

    EXPECT_CALL(*avSessionMock, SetAVMetaData(_)).WillOnce(
        ::testing::Return(RET_ERROR));
    g_adapter->SetMetadata(metadataMock);

    EXPECT_CALL(*avSessionMock, SetAVMetaData(_)).WillOnce(
        ::testing::Return(RET_SUCCESS));
    g_adapter->SetMetadata(metadataMock);

    g_adapter->avSession_ = nullptr;
    g_adapter->SetMetadata(metadataMock);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_008.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackState()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_008, TestSize.Level1)
{
    int32_t state = AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY;
    MediaAVSessionPlayState playState = static_cast<MediaAVSessionPlayState>(state);
    auto avPlaybackState = std::make_shared<AVSession::AVPlaybackState>();
    g_adapter->avSession_ = nullptr;
    g_adapter->avPlaybackState_ = avPlaybackState;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE);
    g_adapter->SetPlaybackState(playState);
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY);
    g_adapter->SetPlaybackState(playState);
    EXPECT_NE(g_adapter->avSession_, nullptr);

    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE);
    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, SetAVPlaybackState(_)).WillOnce(::testing::Return(RET_SUCCESS));
    g_adapter->SetPlaybackState(playState);
    EXPECT_NE(g_adapter->avSession_, nullptr);

    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE);
    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*avSessionMock, SetAVPlaybackState(_)).WillOnce(::testing::Return(RET_ERROR));
    g_adapter->SetPlaybackState(playState);
    EXPECT_NE(g_adapter->avSession_, nullptr);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_009.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackPosition()_1
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_009, TestSize.Level1)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();
    auto avPlaybackState = std::make_shared<AVSession::AVPlaybackState>();
    auto avmetadata = std::make_shared<AVSession::AVMetaData>();
    g_adapter->avMetadata_ = avmetadata;
    g_adapter->avPlaybackState_ = avPlaybackState;
    g_adapter->avSession_ = nullptr;
    g_adapter->avMetadata_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);
    EXPECT_EQ(g_adapter->avSession_, nullptr);

    g_adapter->avSession_ = nullptr;
    g_adapter->avMetadata_->SetDuration(temp2);
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);

    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;
    g_adapter->avMetadata_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(
        ::testing::Return(temp1));
    g_adapter->SetPlaybackPosition(positionMock);
    EXPECT_NE(g_adapter->avSession_, nullptr);

    g_adapter->avSession_ = avSessionMock;
    g_adapter->avMetadata_->SetDuration(temp1);
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_016.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackPosition()_2
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_016, TestSize.Level1)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();
    auto avPlaybackState = std::make_shared<AVSession::AVPlaybackState>();
    auto avmetadata = std::make_shared<AVSession::AVMetaData>();
    g_adapter->avMetadata_ = avmetadata;
    g_adapter->avPlaybackState_ = avPlaybackState;
    auto avSessionMock = std::make_shared<AVSessionMock>();

    g_adapter->avMetadata_->SetDuration(temp1);
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*avSessionMock, SetAVMetaData(_)).WillOnce(::testing::Return(RET_SUCCESS));
    EXPECT_CALL(*avSessionMock, SetAVPlaybackState(_)).WillOnce(::testing::Return(RET_SUCCESS));
    g_adapter->SetPlaybackPosition(positionMock);
    EXPECT_NE(g_adapter->avSession_, nullptr);

    g_adapter->avMetadata_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    g_adapter->avSession_ = avSessionMock;
    EXPECT_CALL(*avSessionMock, IsActive()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*avSessionMock, SetAVMetaData(_)).WillOnce(::testing::Return(RET_ERROR));
    EXPECT_CALL(*avSessionMock, SetAVPlaybackState(_)).WillOnce(::testing::Return(RET_ERROR));
    g_adapter->SetPlaybackPosition(positionMock);
    EXPECT_NE(g_adapter->avSession_, nullptr);
    g_adapter->avSession_ = nullptr;
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0010.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdateMetaDataCache()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0010, TestSize.Level1)
{
    auto mediaAVSessionMetadataAdapterMock = std::make_shared<MediaAVSessionMetadataAdapterMock>();
    auto avMetaDataMock = std::make_shared<AVMetaDataMock>();
    g_adapter->avMetadata_ = avMetaDataMock;

    g_adapter->avMetadata_->SetTitle("1");
    g_adapter->avMetadata_->SetAlbum("1");
    g_adapter->avMetadata_->SetArtist("1");

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillOnce(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillOnce(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillOnce(::testing::Return("1"));
    bool ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillRepeatedly(::testing::Return("2"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillRepeatedly(::testing::Return("2"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillRepeatedly(::testing::Return("2"));
    ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillRepeatedly(::testing::Return("2"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillRepeatedly(::testing::Return("1"));
    ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillRepeatedly(::testing::Return("2"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillRepeatedly(::testing::Return("1"));
    ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillRepeatedly(::testing::Return("2"));
    ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0011.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdateMetaDataCache()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0011, TestSize.Level1)
{
    auto positionMock = std::make_shared<PositionMock>();
    auto avmetadata = std::make_shared<AVSession::AVMetaData>();
    g_adapter->avMetadata_ = avmetadata;
    g_adapter->avMetadata_->SetDuration(1);

    EXPECT_CALL(*positionMock, GetDuration()).WillOnce(::testing::Return(1));
    bool ret = g_adapter->UpdateMetaDataCache(positionMock);
    EXPECT_EQ(ret, false);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(2));
    ret = g_adapter->UpdateMetaDataCache(positionMock);
    EXPECT_EQ(ret, true);
    g_adapter->avMetadata_ = nullptr;

    g_adapter->avMetadata_ = avmetadata;
    g_adapter->avMetadata_->SetDuration(3);
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(INT64_MAX));
    ret = g_adapter->UpdateMetaDataCache(positionMock);
    EXPECT_EQ(ret, true);

    std::shared_ptr<MediaAVSessionPositionAdapter> p_null = std::shared_ptr<MediaAVSessionPositionAdapter>();
    ret = g_adapter->UpdateMetaDataCache(p_null);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0012.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdatePlaybackStateCache()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0012, TestSize.Level1)
{
    MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
    auto avPlaybackState = std::make_shared<AVSession::AVPlaybackState>();
    g_adapter->avPlaybackState_ = avPlaybackState;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY);
    bool ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, false);

    playState = MediaAVSessionPlayState::STATE_PLAY;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE);
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_PAUSE;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE);
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, false);

    playState = MediaAVSessionPlayState::STATE_PAUSE;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY);
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_INITIAL;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_INITIAL);
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, false);

    playState = MediaAVSessionPlayState::STATE_INITIAL;
    g_adapter->avPlaybackState_->SetState(AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY);
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0013.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdatePlaybackStateCache();
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0013, TestSize.Level1)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();
    auto avPlaybackState = std::make_shared<AVSession::AVPlaybackState>();
    g_adapter->avPlaybackState_ = avPlaybackState;
    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    bool ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, false);

    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);

    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);

    g_adapter->avPlaybackState_->SetDuration(temp1);
    g_adapter->avPlaybackState_->SetPosition({ temp1, temp1 });
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(
        ::testing::Return(temp2));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0014.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DestroyAndEraseSession();
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest,
         NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0014,
         TestSize.Level1)
{
    auto avSessionKey = std::make_shared<MediaAVSessionKey>();
    g_adapter->avSessionKey_ = avSessionKey;
    g_adapter->avSessionKey_->Init();
    auto avSessionMock = std::make_shared<AVSessionMock>();
    g_adapter->avSession_ = avSessionMock;

    g_adapter->avSessionMap.clear();
    g_adapter->DestroyAndEraseSession();

    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       nullptr));
    g_adapter->DestroyAndEraseSession();

    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       g_adapter->avSession_));
    g_adapter->DestroyAndEraseSession();

    g_adapter->avSessionMap.insert(
        std::make_pair(g_adapter->avSessionKey_->ToString(),
                       g_adapter->avSession_));
    EXPECT_CALL(*avSessionMock, Destroy()).WillRepeatedly(
        ::testing::Return(RET_ERROR));
    g_adapter->DestroyAndEraseSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0015.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: CreateNewSession();
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaAVSessionAdapterImplTest,
        NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0015,
        TestSize.Level1)
{
    auto avSessionKeyMock = std::make_shared<MediaAVSessionKeyMock>();
    g_adapter->avSessionKey_ = avSessionKeyMock;
    g_adapter->avSessionKey_->Init();
    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;

    bool ret = g_adapter->CreateNewSession(type);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*avSessionKeyMock, GetElement()).WillRepeatedly(
        ::testing::Return(nullptr));
    ret = g_adapter->CreateNewSession(type);
    EXPECT_EQ(ret, false);
}
} // namespace OHOS::NWeb