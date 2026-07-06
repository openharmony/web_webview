/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "ohos_adapter_helper.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {

HWTEST(SurfaceControlAdapterImplTest, SurfaceControlAdapter_GetInstance, TestSize.Level1)
{
    auto& instance1 = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    auto& instance2 = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    EXPECT_EQ(&instance1, &instance2);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceControlAdapter_CreateSurfaceControlFromNativeWindow_NullParent,
    TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    void* result = adapter.CreateSurfaceControlFromNativeWindow(nullptr, "test_surface");
    EXPECT_EQ(result, nullptr);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceControlAdapter_Create, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    void* result = adapter.Create("test_create");
    EXPECT_EQ(result, nullptr);
    if (result != nullptr) {
        adapter.Release(result);
    }
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceControlAdapter_Release_NullSurfaceControl, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    adapter.Release(nullptr);
    auto& adapterAfter = OhosAdapterHelper::GetInstance().GetSurfaceControlAdapter();
    EXPECT_EQ(&adapter, &adapterAfter);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_GetInstance, TestSize.Level1)
{
    auto& instance1 = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    auto& instance2 = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    EXPECT_EQ(&instance1, &instance2);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_CreateSurfaceTransaction_NullNativeWindow,
    TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    void* result = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(result, nullptr);
    adapter.DeleteTransaction(result);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_DeleteTransaction_NullTransaction, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.DeleteTransaction(nullptr);
    auto& adapterAfter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    EXPECT_EQ(&adapter, &adapterAfter);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_Commit_NullTransaction, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.Commit(nullptr);
    auto& adapterAfter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    EXPECT_EQ(&adapter, &adapterAfter);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetOnComplete_NullTransaction, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetOnComplete(nullptr, nullptr, nullptr);
    auto& adapterAfter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    EXPECT_EQ(&adapter, &adapterAfter);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetOnComplete_NullCallback, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetOnComplete(transaction, nullptr, nullptr);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetOnComplete_ValidCallback, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    SurfaceControlOnCompleteCallback callback = [](void* context, uint64_t timestamp) {};
    adapter.SetOnComplete(transaction, nullptr, callback);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_Reparent_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.Reparent(nullptr, nullptr, nullptr);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.Reparent(transaction, nullptr, nullptr);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetVisibility_InvalidValue, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetVisibility(nullptr, nullptr, 2);
    adapter.SetVisibility(nullptr, nullptr, -1);
    adapter.SetVisibility(nullptr, nullptr, 0);
    adapter.SetVisibility(nullptr, nullptr, 1);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetVisibility_NullTransaction, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetVisibility(transaction, nullptr, 1);
    adapter.SetVisibility(transaction, nullptr, 0);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBufferTransform_InvalidValue, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBufferTransform(nullptr, nullptr, -1);
    adapter.SetBufferTransform(nullptr, nullptr, 99);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBufferTransform_NullTransaction, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBufferTransform(nullptr, nullptr, 0);
    adapter.SetBufferTransform(nullptr, nullptr, 1);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetZOrder_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetZOrder(nullptr, nullptr, 0);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetZOrder(transaction, nullptr, 0);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBuffer_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBuffer(nullptr, nullptr, nullptr, -1, nullptr, nullptr);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBuffer(transaction, nullptr, nullptr, -1, nullptr, nullptr);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBounds_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBounds(nullptr, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBounds(transaction, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetPivot_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetPivot(nullptr, nullptr, 0.5f, 0.5f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetPivot(transaction, nullptr, 0.5f, 0.5f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetTranslate_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetTranslate(nullptr, nullptr, 0.0f, 0.0f, 0.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetTranslate(transaction, nullptr, 0.0f, 0.0f, 0.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetDamageRegion_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetDamageRegion(nullptr, nullptr, nullptr, 0);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetDamageRegion(transaction, nullptr, nullptr, 0);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBufferAlpha_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBufferAlpha(nullptr, nullptr, 1.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBufferAlpha(transaction, nullptr, 1.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetForegroundColor_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetForegroundColor(nullptr, nullptr, 1.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetForegroundColor(transaction, nullptr, 1.0f, 0.0f, 0.0f, 1.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBackgroundColor_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBackgroundColor(nullptr, nullptr, 0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBackgroundColor(transaction, nullptr, 0.0f, 0.0f, 0.0f, 1.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBorderColor_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBorderColor(nullptr, nullptr, 0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBorderColor(transaction, nullptr, 0.0f, 0.0f, 0.0f, 1.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBorderWidth_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBorderWidth(nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBorderWidth(transaction, nullptr, 0.0f, 0.0f, 0.0f, 0.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetBorderStyle_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetBorderStyle(nullptr, nullptr, 0, 0, 0, 0);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetBorderStyle(transaction, nullptr, 0, 0, 0, 0);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetHardwareEnableHint_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetHardwareEnableHint(nullptr, nullptr, true);
    adapter.SetHardwareEnableHint(nullptr, nullptr, false);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetHardwareEnableHint(transaction, nullptr, true);
    adapter.SetHardwareEnableHint(transaction, nullptr, false);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetName_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetName(nullptr, nullptr, "test");
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetName(transaction, nullptr, "test");
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetFrameGravity_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetFrameGravity(nullptr, nullptr, 0);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetFrameGravity(transaction, nullptr, 0);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetSrcRect_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetSrcRect(nullptr, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetSrcRect(transaction, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_SetDisplayRect_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.SetDisplayRect(nullptr, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.SetDisplayRect(transaction, nullptr, 0.0f, 0.0f, 100.0f, 100.0f);
    adapter.DeleteTransaction(transaction);
}

HWTEST(SurfaceControlAdapterImplTest, SurfaceTransactionAdapter_ClearBufferQueueCache_NullParams, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter();
    adapter.ClearBufferQueueCache(nullptr, nullptr, true);
    adapter.ClearBufferQueueCache(nullptr, nullptr, false);
    EXPECT_EQ(&adapter, &OhosAdapterHelper::GetInstance().GetSurfaceTransactionAdapter());
    void* transaction = adapter.CreateSurfaceTransaction(nullptr);
    ASSERT_NE(transaction, nullptr);
    adapter.ClearBufferQueueCache(transaction, nullptr, true);
    adapter.ClearBufferQueueCache(transaction, nullptr, false);
    adapter.DeleteTransaction(transaction);
}

} // namespace OHOS::NWeb
