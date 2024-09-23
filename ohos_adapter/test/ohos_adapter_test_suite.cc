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

#include "ohos_adapter/test/ohos_adapter_test_suite.h"

#include "base/base_paths.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/test/task_environment.h"

#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace OHOS::NWeb {
std::unique_ptr<base::test::TaskEnvironment> OhosAdapterTestSuite::task_environment_;

OhosAdapterTestSuite::OhosAdapterTestSuite(int argc, char** argv)
    : base::TestSuite(argc, argv) {}

OhosAdapterTestSuite::~OhosAdapterTestSuite() = default;

// static
void OhosAdapterTestSuite::RunUntilIdle() {
  CHECK(task_environment_);
  task_environment_->RunUntilIdle();
}

void OhosAdapterTestSuite::Initialize() {
  base::TestSuite::Initialize();

  CHECK(!task_environment_);
  task_environment_ = std::make_unique<base::test::TaskEnvironment>();
}

void OhosAdapterTestSuite::Shutdown() {
  CHECK(task_environment_);
  task_environment_ = nullptr;

  base::TestSuite::Shutdown();
}
}  // namespace content