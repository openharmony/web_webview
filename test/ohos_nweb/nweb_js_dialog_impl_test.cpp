/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_js_dialog_impl_test.h"
#include "nweb_test_log.h"

namespace OHOS::NWeb {
void NWebJSDialogImplTest::SetNWeb(std::shared_ptr<NWeb> nweb)
{
    nwebweak_ = nweb;
}

bool NWebJSDialogImplTest::OnAlertDialogByJS(const std::string &url,
                                             const std::string &message,
                                             std::shared_ptr<NWebJSDialogResult> result)
{
    TESTLOG_I("OnAlertDialogByJS message:%{public}s", message.c_str());
    type_ == Type::CONFIRM ? result->Confirm() : result->Cancel();
    return true;
}

bool NWebJSDialogImplTest::OnBeforeUnloadByJS(const std::string &url,
                                              const std::string &message,
                                              std::shared_ptr<NWebJSDialogResult> result)
{
    TESTLOG_I("OnBeforeUnloadByJS message:%{public}s", message.c_str());
    type_ == Type::CONFIRM ? result->Confirm() : result->Cancel();
    return true;
}

bool NWebJSDialogImplTest::OnPromptDialogByJs(const std::string &url,
                                              const std::string &message,
                                              const std::string &defaultValue,
                                              std::shared_ptr<NWebJSDialogResult> result)
{
    TESTLOG_I("OnPromptDialogByJs message:%{public}s", message.c_str());
    TESTLOG_I("OnPromptDialogByJs defaultValue:%{public}s", defaultValue.c_str());
    type_ == Type::CONFIRM ? result->Confirm("my input") : result->Cancel();
    return true;
}

bool NWebJSDialogImplTest::OnConfirmDialogByJS(const std::string &url,
                                               const std::string &message,
                                               std::shared_ptr<NWebJSDialogResult> result)
{
    TESTLOG_I("OnConfirmDialogByJS message:%{public}s", message.c_str());
    type_ == Type::CONFIRM ? result->Confirm() : result->Cancel();
    return true;
}

bool NWebJSDialogImplTest::OnConsoleLog(const NWebConsoleLog& message)
{
    TESTLOG_I("message:%{public}s", message.Log().c_str());
    TESTLOG_I("lineNumer:%{public}d", message.LineNumer());
    TESTLOG_I("messageLevel:%{public}d", message.LogLevel());
    TESTLOG_I("sourceId:%{public}s", message.SourceId().c_str());
    return true;
}
}