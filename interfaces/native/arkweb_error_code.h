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

/**
 * @addtogroup Web
 * @{
 *
 * @brief Provides APIs for the ArkWeb errors.
 * @since 12
 */
/**
 * @file arkweb_error_code.h
 *
 * @brief Declares the APIs for the ArkWeb errors.
 * @kit ArkWeb
 * @library libohweb.so
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
#ifndef ARKWEB_ERROR_CODE_H
#define ARKWEB_ERROR_CODE_H

typedef enum ArkWeb_ErrorCode {
/*
 * @brief Success.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_SUCCESS = 0,

/*
 * @brief Init error. The WebviewController must be associated with a Web component.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_INIT_ERROR = 17100001,

/*
 * @brief Unknown error.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_ERROR_UNKNOWN = 17100100,

/*
 * @brief Invalid param.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_INVALID_PARAM = 17100101,

/*
 * @brief Register custom schemes should be called before create any ArkWeb.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_SCHEME_REGISTER_FAILED = 17100102,

/*
 * @brief Invalid url.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_INVALID_URL = 17100103,
 
/*
 * @brief Invalid cookie value.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */
ARKWEB_INVALID_COOKIE_VALUE = 17100104,

/*
 * @brief Failed to open the library.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 15
 */
ARKWEB_LIBRARY_OPEN_FAILURE = 17100105,

/*
 * @brief The required symbol was not found in the library.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 15
 */
ARKWEB_LIBRARY_SYMBOL_NOT_FOUND = 17100106,
} ArkWeb_ErrorCode;

#endif // ARKWEB_ERROR_CODE_H