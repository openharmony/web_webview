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

#include <testing/gtest/include/gtest/gtest.h>
#include <testing/gmock/include/gmock/gmock.h>

#define private public
#include "media_avsession_adapter.h"
#include "media_avsession_adapter_impl.h"

using namespace testing;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<MediaAVSessionCallbackAdapter> g_callback;
std::shared_ptr<MediaAVSessionKey> g_key;
std::shared_ptr<MediaAVSessionAdapterImpl> g_adapter;
} // namespace

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
public:
    MediaAVSessionCallbackAdapterMock() = default;
    void Play() {}
    void Pause() {}
    void Stop() {}
    void SeekTo(int64_t millisTime) {}
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

class MediaAVSessionCallbackAdapterTest : public testing::Test {
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

void MediaAVSessionCallbackAdapterTest::SetUpTestCase(void) {}

void MediaAVSessionCallbackAdapterTest::TearDownTestCase(void) {}

void MediaAVSessionCallbackAdapterTest::SetUp(void)
{
    g_callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    ASSERT_NE(g_callback, nullptr);
}

void MediaAVSessionCallbackAdapterTest::TearDown(void)
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
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackAdapter_001.
 * @tc.desc: test of MediaAVSessionCallbackAdapter :: Onplay()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionCallbackAdapterTest, NWebMediaAdapterTest_MediaAVSessionCallbackAdapter_001)
{
    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback = callbackMock;
    g_callback->Play();
    EXPECT_NE(g_callback, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackAdapter_002.
 * @tc.desc: test of MediaAVSessionCallbackAdapter :: OnPause()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionCallbackAdapterTest, NWebMediaAdapterTest_MediaAVSessionCallbackAdapter_002)
{
    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback->Pause();
    EXPECT_NE(g_callback, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackAdapter_003.
 * @tc.desc: test of MediaAVSessionCallbackAdapter :: OnStop()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionCallbackAdapterTest, NWebMediaAdapterTest_MediaAVSessionCallbackAdapter_003)
{
    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    g_callback = callbackMock;
    g_callback->Stop();
    EXPECT_NE(g_callback, nullptr);
}
/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionCallbackAdapter_004s.
 * @tc.desc: test of MediaAVSessionCallbackAdapter :: OnSeek()
 * @tc.type: FUNC.
 */

TEST_F(MediaAVSessionCallbackAdapterTest, NWebMediaAdapterTest_MediaAVSessionCallbackAdapter_004)
{
    auto callbackMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    int64_t millisTime = 100;
    g_callback = callbackMock;
    g_callback->SeekTo(millisTime);
    EXPECT_NE(g_callback, nullptr);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_001.
 * @tc.desc: test of MediaAVSessionKey :: GetType()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_001)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    g_key->Init();
    g_key->SetType(type);
    EXPECT_EQ(g_key->GetType(), type);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_002.
 * @tc.desc: test of MediaAVSessionKey :: GetPID()
 * @tc.type: FUNC.
 */

TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_002)
{
    g_key->Init();
    auto ret = g_key->GetPID();
    EXPECT_EQ(ret, getpid());
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_003.
 * @tc.desc: test of MediaAVSessionKey :: GetElement()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_003)
{
    g_key->Init();
    auto ret = g_key->GetBundleName();
    EXPECT_EQ(ret, "unittest");
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_004.
 * @tc.desc: test of MediaAVSessionKey :: GetType()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_004)
{
    g_key->Init();
    auto ret = g_key->GetAbilityName();
    EXPECT_EQ(ret, "unittest");
}
/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_005.
 * @tc.desc: test of MediaAVSessionKey :: SetType()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_005)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    g_key->Init();
    g_key->SetType(type);
    auto ret = g_key->GetType();
    EXPECT_EQ(ret, type);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionKey_006.
 * @tc.desc: test of MediaAVSessionKey :: ToString()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionKeyTest, NWebMediaAdapterTest_MediaAVSessionKey_006)
{
    g_key->Init();
    auto str = (std::to_string(g_key->GetPID()) + "_" + g_key->GetBundleName() + "_" + g_key->GetAbilityName());
    auto ret = g_key->ToString();
    EXPECT_EQ(ret, str);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_001.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: CreateAVSession()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_001)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
    bool ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, false);

    type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    g_adapter->DeActivate();
    g_adapter->DestroyAVSession();
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);

    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_002.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DestroyAVSession()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_002)
{
    g_adapter->DestroyAVSession();
    auto ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);

    g_adapter->DeActivate();
    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_003.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: RegistCallback()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_003)
{
    auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
    bool ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    g_adapter->DeActivate();
    EXPECT_EQ(g_adapter->IsActivated(), false);
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);

    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, false);
    g_adapter->Activate();
    EXPECT_EQ(g_adapter->IsActivated(), true);
    ret = g_adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
    EXPECT_EQ(ret, true);

    g_adapter->DeActivate();
    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_004.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: IsActivated()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_004)
{
    bool ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, true);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, true);
    g_adapter->DestroyAVSession();

    ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_005.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: Activate()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_005)
{
    bool ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
    ret = g_adapter->Activate();
    EXPECT_EQ(ret, true);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, true);
    g_adapter->DeActivate();
    g_adapter->DestroyAVSession();

    ret = g_adapter->Activate();
    EXPECT_EQ(ret, false);
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_006.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DeActivate()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_006)
{
    g_adapter->DeActivate();
    auto ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    ret = g_adapter->Activate();
    EXPECT_EQ(ret, true);

    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, true);

    g_adapter->DeActivate();
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);

    g_adapter->DestroyAVSession();

    g_adapter->DeActivate();
    ret = g_adapter->IsActivated();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_007.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetMetadata()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_007)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    auto ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    auto metadataMock = std::make_shared<MetaDataMock>();
    EXPECT_CALL(*metadataMock, GetTitle()).WillOnce(
        ::testing::Return(std::string("title")));
    EXPECT_CALL(*metadataMock, GetArtist()).WillOnce(
        ::testing::Return(std::string("artist")));
    EXPECT_CALL(*metadataMock, GetAlbum()).WillOnce(
        ::testing::Return(std::string("album")));
    g_adapter->SetMetadata(metadataMock);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_008.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackState()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_008)
{
    MediaAVSessionPlayState playState = static_cast<MediaAVSessionPlayState>(MediaAVSessionPlayState::STATE_PLAY);
    g_adapter->SetPlaybackState(playState);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    auto ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    auto metadataMock = std::make_shared<MetaDataMock>();
    EXPECT_CALL(*metadataMock, GetTitle()).WillOnce(
        ::testing::Return(std::string("title")));
    EXPECT_CALL(*metadataMock, GetArtist()).WillOnce(
        ::testing::Return(std::string("artist")));
    EXPECT_CALL(*metadataMock, GetAlbum()).WillOnce(
        ::testing::Return(std::string("album")));
    g_adapter->SetMetadata(metadataMock);
    g_adapter->SetPlaybackState(playState);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_009.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackPosition()_1
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_009)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    auto ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(
        ::testing::Return(temp1));
    g_adapter->SetPlaybackPosition(positionMock);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0010.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdateMetaDataCache()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0010)
{
    auto mediaAVSessionMetadataAdapterMock = std::make_shared<MediaAVSessionMetadataAdapterMock>();
    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    auto ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetTitle()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetArtist()).WillRepeatedly(::testing::Return("1"));
    EXPECT_CALL(*mediaAVSessionMetadataAdapterMock, GetAlbum()).WillRepeatedly(::testing::Return("1"));
    ret = g_adapter->UpdateMetaDataCache(mediaAVSessionMetadataAdapterMock);
    EXPECT_EQ(ret, true);

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

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0011.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdateMetaDataCache()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0011)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();

    EXPECT_CALL(*positionMock, GetDuration()).WillOnce(::testing::Return(temp1));
    bool ret = g_adapter->UpdateMetaDataCache(positionMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    ret = g_adapter->UpdateMetaDataCache(positionMock);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0012.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdatePlaybackStateCache()
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0012)
{
    MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
    bool ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    playState = MediaAVSessionPlayState::STATE_PLAY;
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_PAUSE;
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_PAUSE;
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_INITIAL;
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    playState = MediaAVSessionPlayState::STATE_INITIAL;
    ret = g_adapter->UpdatePlaybackStateCache(playState);
    EXPECT_EQ(ret, true);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0013.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: UpdatePlaybackStateCache();
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0013)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    bool ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, false);

    auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    ret = g_adapter->CreateAVSession(type);
    EXPECT_EQ(ret, true);
    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(
        ::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(
        ::testing::Return(temp2));
    ret = g_adapter->UpdatePlaybackStateCache(positionMock);
    EXPECT_EQ(ret, true);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0014.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: DestroyAndEraseSession();
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0014)
{
    auto avSessionKey = std::make_shared<MediaAVSessionKey>();
    g_adapter->avSessionKey_ = avSessionKey;
    avSessionKey->Init();

    g_adapter->DestroyAndEraseSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_0015.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: CreateNewSession();
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_0015)
{
    auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;

    bool ret = g_adapter->CreateNewSession(MediaAVSessionType::MEDIA_TYPE_INVALID);
    EXPECT_EQ(ret, false);

    type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    ret = g_adapter->CreateNewSession(type);
    EXPECT_EQ(ret, true);

    g_adapter->DestroyAVSession();
}

/**
 * @tc.name: MediaAVSessionAdapterImplTest_MediaAVSessionAdapterImpl_016.
 * @tc.desc: test of MediaAVSessionAdapterImpl :: SetPlaybackPosition()_2
 * @tc.type: FUNC.
 */
TEST_F(MediaAVSessionAdapterImplTest, NWebMediaAdapterTest_MediaAVSessionAdapterImpl_016)
{
    int64_t temp1 = 1;
    int64_t temp2 = 2;
    auto positionMock = std::make_shared<PositionMock>();

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp1));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp1));
    g_adapter->SetPlaybackPosition(positionMock);

    EXPECT_CALL(*positionMock, GetDuration()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetElapsedTime()).WillRepeatedly(::testing::Return(temp2));
    EXPECT_CALL(*positionMock, GetUpdateTime()).WillRepeatedly(::testing::Return(temp2));
    g_adapter->SetPlaybackPosition(positionMock);
}

} // namespace OHOS::NWeb