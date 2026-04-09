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

/**
 * @brief The CookieManager is not initialized.
 *
 * @since 20
 */
ARKWEB_COOKIE_MANAGER_NOT_INITIALIZED = 17100107,

/**
 * @brief The CookieManager initialization failed.
 *
 * @since 20
 */
ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED = 17100108,

/**
 * @brief Save cookie failed.
 *
 * @since 20
 */
ARKWEB_COOKIE_SAVE_FAILED = 17100109,
} ArkWeb_ErrorCode;

/**
 * @brief Defines an enum for the error codes of the white screen optimization solution.
 *
 * @since 20
 */
typedef enum ArkWeb_BlanklessErrorCode {
/** @error The operation is successful. */
ARKWEB_BLANKLESS_SUCCESS = 0,

/** @error Unidentified error. */
ARKWEB_BLANKLESS_ERR_UNKNOWN = -1,

/** @error Invalid parameter. */
ARKWEB_BLANKLESS_ERR_INVALID_ARGS = -2,

/** @error The web controller is not bound to a component. */
ARKWEB_BLANKLESS_ERR_CONTROLLER_NOT_INITED = -3,

/**
 * @error The key value is not matched. The OH_NativeArkWeb_SetBlanklessLoadingWithKey
 * and OH_NativeArkWeb_GetBlanklessInfoWithKey APIs must be used in pair and use the same key value.
 */
ARKWEB_BLANKLESS_ERR_KEY_NOT_MATCH = -4,

/**
 * @error If the similarity is low, the system determines that the change is too large.
 * As a result, the OH_NativeArkWeb_SetBlanklessLoadingWithKey API fails to enable frame interpolation.
 */
ARKWEB_BLANKLESS_ERR_SIGNIFICANT_CHANGE = -5,

/** @error The device does not support this feature. */
ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT = 801,
} ArkWeb_BlanklessErrorCode;

#endif // ARKWEB_ERROR_CODE_H