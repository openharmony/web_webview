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

#include "base/functional/bind.h"
#include "base/test/launcher/unit_test_launcher.h"
#include "ohos_adapter/test/ohos_adapter_test_suite.h"

int main(int argc, char** argv) {
  OHOS::NWeb::OhosAdapterTestSuite test_suite(argc, argv);

  return base::LaunchUnitTests(
      argc, argv,
      base::BindOnce(&OHOS::NWeb::OhosAdapterTestSuite::Run, base::Unretained(&test_suite)));
}