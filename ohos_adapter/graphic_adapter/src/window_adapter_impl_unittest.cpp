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

#include "testing/gtest/include/gtest/gtest.h"
#include "ohos_adapter_helper.h"
#include "nweb_log.h"
#include <native_window/external_window.h>

#define private public
#include "window_adapter_impl.h"
#include "native_window_adapter_impl.h"
#undef private

class WindowAdapterImplTest : public testing::Test {
protected:
    WindowAdapterImplTest() {}

    ~WindowAdapterImplTest() override {}

    void SetUp() override {}

    void TearDown() override {}
};

class AshmemAdapterImplTest : public testing::Test {
protected:
    AshmemAdapterImplTest() {}

    ~AshmemAdapterImplTest() override {}

    void SetUp() override {}

    void TearDown() override {}
};

namespace OHOS::NWeb {
class BufferConsumerListenerTest : public IBufferConsumerListenerAdapter {
public:
    BufferConsumerListenerTest() = default;
    ~BufferConsumerListenerTest() override = default;
    void OnBufferAvailable(std::shared_ptr<SurfaceBufferAdapter> buffer) override
    {
        WVLOG_I("test buffer is available, the format=%{public}d", buffer->GetFormat());
    }
};

/**
 * @tc.name: NormalTest.
 * @tc.desc: test WindowAdapterImpl class function.
 * @tc.type: FUNC.
 */
TEST_F(WindowAdapterImplTest, WindowAdapterImplTest_NormalTest_001)
{
    std::unique_ptr<ConsumerNativeAdapterImpl> nativeAdapter = std::make_unique<ConsumerNativeAdapterImpl>();
    std::unique_ptr<IBufferConsumerListenerAdapter> listenerTest = std::make_unique<BufferConsumerListenerTest>();
    EXPECT_EQ(nativeAdapter->RegisterConsumerListener(std::move(listenerTest)), 0);
    EXPECT_EQ(nativeAdapter->SetUserData("testkey", "testval"), 0);
    EXPECT_EQ(nativeAdapter->SetQueueSize(1), 0);
    OH_NativeImage* image = nativeAdapter->GetConsumerSurface();
    OHNativeWindow* nativeWin = OH_NativeImage_AcquireNativeWindow(image);
    NWebNativeWindow window = reinterpret_cast<NWebNativeWindow>(nativeWin);
    EXPECT_EQ(OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSetBufferGeometry(
        window, 0, 0), GSERROR_OK);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSurfaceCleanCache(window);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSurfaceCleanCacheWithPara(window, false);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}

/**
 * @tc.name: NormalTest.
 * @tc.desc: test AshmemAdapterImplTest class function.
 * @tc.type: FUNC.
 */
TEST_F(AshmemAdapterImplTest, AshmemAdapterImplTest_NormalTest_001)
{
    size_t size = 1024;
    EXPECT_GE(AshmemAdapter::AshmemCreate("test", size), 0);
}
} // namespace OHOS::NWeb
