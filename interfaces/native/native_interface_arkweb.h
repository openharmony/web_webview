/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
 * @brief Provides APIs to use javascript proxy and run javascirpt code.
 * @since 11
 */
/**
 * @file native_interface_arkweb.h
 *
 * @brief Declares the APIs to use javascript proxy and run javascirpt code.
 * @library libohweb.so
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
#ifndef NATIVE_INTERFACE_ARKWEB_H
#define NATIVE_INTERFACE_ARKWEB_H

#include <cstdint>

#include "arkweb_error_code.h"
#include "arkweb_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Defines the javascript callback of the web component.
*
* @since 11
*/
typedef void (*NativeArkWeb_OnJavaScriptCallback)(const char*);

/**
* @brief Defines the javascript proxy callback of the web component.
*
* @since 11
*/
typedef char* (*NativeArkWeb_OnJavaScriptProxyCallback)(const char** argv, int32_t argc);

/**
* @brief Defines the valid callback of the web component.
*
* @since 11
*/
typedef void (*NativeArkWeb_OnValidCallback)(const char*);

/**
* @brief Defines the destroy callback of the web component.
*
* @since 11
*/
typedef void (*NativeArkWeb_OnDestroyCallback)(const char*);

/**
 * @brief Loads a piece of code and execute JS code in the context of the currently displayed page.
 *
 * @param webTag The name of the web component.
 * @param jsCode a piece of javascript code.
 * @param callback Callbacks execute JavaScript script results.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
void OH_NativeArkWeb_RunJavaScript(const char* webTag, const char* jsCode, NativeArkWeb_OnJavaScriptCallback callback);

/**
 * @brief Registers the JavaScript object and method list.
 *
 * @param webTag The name of the web component.
 * @param objName The name of the registered object.
 * @param methodList The method of the application side JavaScript object participating in the registration.
 * @param callback The callback function registered by developer is called back when HTML side uses.
 * @param size The size of the callback.
 * @param needRefresh if web need refresh.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
void OH_NativeArkWeb_RegisterJavaScriptProxy(const char* webTag, const char* objName, const char** methodList,
    NativeArkWeb_OnJavaScriptProxyCallback* callback, int32_t size, bool isNeedRefresh);

/**
 * @brief Deletes the registered object which th given name.
 *
 * @param webTag The name of the web component.
 * @param objName The name of the registered object.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
void OH_NativeArkWeb_UnregisterJavaScriptProxy(const char* webTag, const char* objName);

/**
 * @brief Registers the valid callback.
 *
 * @param webTag The name of the web component.
 * @param callback The callback in which we can register object.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
void OH_NativeArkWeb_SetJavaScriptProxyValidCallback(const char* webTag, NativeArkWeb_OnValidCallback callback);

/**
 * @brief Get the valid callback.
 *
 * @param webTag The name of the web component.
 * @return return the valid callback function registered.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
NativeArkWeb_OnValidCallback OH_NativeArkWeb_GetJavaScriptProxyValidCallback(const char* webTag);

/**
 * @brief Registers the destroy callback.
 *
 * @param webTag The name of the web component.
 * @param callback the destroy callback.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
void OH_NativeArkWeb_SetDestroyCallback(const char* webTag, NativeArkWeb_OnDestroyCallback callback);

/**
 * @brief Get the destroy callback.
 *
 * @param webTag The name of the web component.
 * @return return the destroy callback function registered.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 11
 */
NativeArkWeb_OnDestroyCallback OH_NativeArkWeb_GetDestroyCallback(const char* webTag);

/**
 * @brief Loads the data or URL.
 *        This function should be called on main thread.
 *
 * @param webTag The name of the web component.
 * @param data A string encoded according to "Base64" or "URL", should not be NULL.
 * @param mimeType Media type. For example: "text/html", should not be NULL.
 * @param encoding Encoding type. For example: "UTF-8", should not be NULL.
 * @param baseUrl A specified URL path ("http"/"https"/"data" protocol),
 *                which is assigned to window.origin by the Web component.
 * @param historyUrl History URL. When it is not empty, it can be managed by
 *                   history records to realize the back and forth function.
 * @return LoadData result code.
 *         {@link ARKWEB_SUCCESS} load data success.
 *         {@link ARKWEB_INVALID_PARAM} Mandatory parameters are left unspecified or
 *                                      Incorrect parameter types or Parameter verification failed.
 *         {@link ARKWEB_INIT_ERROR} Initialization error, can't get a valid Web for the webTag.
 *         {@link ARKWEB_LIBRARY_OPEN_FAILURE} Failed to open the library.
 *         {@link ARKWEB_LIBRARY_SYMBOL_NOT_FOUND} The required symbol was not found in the library.
 *
 * @syscap SystemCapability.Web.Webview.Core
 * @since 15
 */
ArkWeb_ErrorCode OH_NativeArkWeb_LoadData(const char* webTag,
                                          const char* data,
                                          const char* mimeType,
                                          const char* encoding,
                                          const char* baseUrl,
                                          const char* historyUrl);

/**
 * @brief Register a JavaScript object with callback methods, which may return values. This object will be injected
 *        into all frames of the current page, including all iframes, and will be accessible using the specified
 *        name in ArkWeb_ProxyObjectWithResult. The object will only be available in JavaScript after the next
 *        load or reload.
 *        These methods will be executed in the ArkWeb worker thread.
 *
 * @param webTag The name of the web component.
 * @param proxyObject The JavaScript object to register, the object has callback functions with return value.
 * @param permission The JSON string, which defaults to null, is used to configure the permission control for
 *                   JSBridge, allowing for the definition of URL whitelists at the object and method levels.
 *
 * @since 20
 */
void OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(const char* webTag,
    const ArkWeb_ProxyObjectWithResult* proxyObject, const char* permission);

#ifdef __cplusplus
};
#endif
#endif // NATIVE_INTERFACE_ARKWEB_H
