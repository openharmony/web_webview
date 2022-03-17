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

#ifndef NWEB_JS_DIALOG_IMPL_TEST_H
#define NWEB_JS_DIALOG_IMPL_TEST_H

#include "nweb_handler.h"

namespace OHOS::NWeb {
class NWebJSDialogImplTest : public NWebHandler {
public:
    enum class Type {
        CONFIRM,
        CANCEL
    };
    NWebJSDialogImplTest() = default;
    explicit NWebJSDialogImplTest(Type type) : type_(type) {}
    ~NWebJSDialogImplTest() = default;
    void SetNWeb(std::shared_ptr<NWeb> nweb) override;
    bool OnAlertDialogByJS(const std::string &url,
                   const std::string &message,
                   std::shared_ptr<NWebJSDialogResult> result) override;
    bool OnBeforeUnloadByJS(const std::string &url,
                          const std::string &message,
                          std::shared_ptr<NWebJSDialogResult> result) override;
    bool OnPromptDialogByJs(const std::string &url,
                    const std::string &message,
                    const std::string &defaultValue,
                    std::shared_ptr<NWebJSDialogResult> result) override;
    bool OnConfirmDialogByJS(const std::string &url,
                     const std::string &message,
                     std::shared_ptr<NWebJSDialogResult> result) override;
    bool OnConsoleLog(const NWebConsoleLog& message) override;
private:
    std::weak_ptr<NWeb> nwebweak_;
    Type type_;
};
} // namespace OHOS::NWeb

#endif // NWEB_JS_DIALOG_IMPL_TEST_H