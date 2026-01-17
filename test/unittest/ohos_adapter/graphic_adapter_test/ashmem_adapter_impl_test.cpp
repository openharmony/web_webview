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
#include "iconsumer_surface.h"
#include "ohos_adapter_helper.h"
#include "window.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {
/**
 * @tc.name: NormalTest.
 * @tc.desc: test AshmemAdapterImplTest class function.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(AshmemAdapterImplTest, NormalTest, TestSize.Level1)
{
    size_t size = 1024;
    EXPECT_GE(AshmemAdapter::AshmemCreate("test", size), 0);
}
} // namespace OHOS::NWeb
