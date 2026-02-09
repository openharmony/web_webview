/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef TEST_MOCK_ANI_BUSINESS_ERROR_H
#define TEST_MOCK_ANI_BUSINESS_ERROR_H

#include <ani.h>
#include <string>

namespace OHOS {
namespace NWebError {

// Mock AniBusinessError class
class AniBusinessError {
public:
    static ani_status ThrowError(ani_env* env, int32_t errorCode, const std::string& error_message)
    {
        g_lastErrorCode = errorCode;
        g_lastErrorMessage = error_message;
        g_errorWasThrown = true;
        return ANI_OK;
    }

    static ani_status ThrowErrorByErrCode(ani_env* env, int32_t errorCode)
    {
        g_lastErrorCode = errorCode;
        g_errorWasThrown = true;
        return ANI_OK;
    }

    static ani_ref CreateError(ani_env* env, int32_t err)
    {
        return nullptr;
    }

    // Test helper variables
    static bool g_errorWasThrown;
    static int32_t g_lastErrorCode;
    static std::string g_lastErrorMessage;

    static void Reset()
    {
        g_errorWasThrown = false;
        g_lastErrorCode = 0;
        g_lastErrorMessage = "";
    }
};

// Static member initialization
bool AniBusinessError::g_errorWasThrown = false;
int32_t AniBusinessError::g_lastErrorCode = 0;
std::string AniBusinessError::g_lastErrorMessage = "";

} // namespace NWebError
} // namespace OHOS

#endif // TEST_MOCK_ANI_BUSINESS_ERROR_H
