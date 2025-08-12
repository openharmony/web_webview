/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "arkweb_utils.h"

#include "parameters.h"
#include "nweb_log.h"

namespace OHOS::ArkWeb {

static int g_appEngineVersion =
    OHOS::system::GetIntParameter("web.engine.debug.version", static_cast<int>(ArkWebEngineVersion::M132));

void setActiveWebEngineVersion(ArkWebEngineVersion version)
{
    g_appEngineVersion = static_cast<int>(version);
    WVLOG_I("set appEngineVersion: %{public}d", g_appEngineVersion);
}

ArkWebEngineVersion getActiveWebEngineVersion()
{
    WVLOG_I("get appEngineVersion: %{public}d", g_appEngineVersion);
    return static_cast<ArkWebEngineVersion>(g_appEngineVersion);
}

void setActiveWebEngineType(ArkWebEngineType type)
{
    g_appEngineVersion = static_cast<int>(type);
    WVLOG_I("set appEngineVersion: %{public}d", g_appEngineVersion);
}

ArkWebEngineType getActiveWebEngineType()
{
    WVLOG_I("get appEngineVersion: %{public}d", g_appEngineVersion);
    return static_cast<ArkWebEngineType>(g_appEngineVersion);
}

void LogForUnsupportedFunc(ArkWebEngineVersion version, const char* msg)
{
    WVLOG_W("%s unsupported engine version: %{public}d",
        msg, static_cast<int>(version));
}
}