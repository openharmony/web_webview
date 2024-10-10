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
 * @brief Provide the definition of the C interface for the native ArkWeb.
 * @since 12
 */
/**
 * @file arkweb_type.h
 *
 * @brief Defines the common types for the native ArkWeb.
 * @kit ArkWeb
 * @library libohweb.so
 * @syscap SystemCapability.Web.Webview.Core
 * @since 12
 */

#ifndef ARKWEB_TYPE_H
#define ARKWEB_TYPE_H

#include <stddef.h>
#include <stdint.h>

#include "arkweb_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the javascript bridge data type.
 *
 * @since 12
 */
typedef struct {
    /** A buffer that contains data. */
    const uint8_t* buffer;
    /** The size of the buffer. */
    size_t size;
} ArkWeb_JavaScriptBridgeData;

/**
 * @brief Defines the data type carried in a ArkWeb_WebMessage.
 *
 * @since 12
 */
typedef enum ArkWeb_WebMessageType {
    /** Represent error data */
    ARKWEB_NONE = 0,
    /** The data carried in the ArkWeb_WebMessage is string. */
    ARKWEB_STRING,
    /** The data carried in the ArkWeb_WebMessage is buffer(uint8_t). */
    ARKWEB_BUFFER
} ArkWeb_WebMessageType;

/**
 * @brief Defines the ArkWeb_WebMessage.
 *
 * @since 12
 */
typedef struct ArkWeb_WebMessage* ArkWeb_WebMessagePtr;

/**
 * @brief Defines the javascript callback of the native ArkWeb.
 *
 * @since 12
 */
typedef void (*ArkWeb_OnJavaScriptCallback)(
    const char* webTag, const ArkWeb_JavaScriptBridgeData* data, void* userData);

/**
 * @brief Defines the javascript proxy callback of the native ArkWeb.
 *
 * @since 12
 */
typedef void (*ArkWeb_OnJavaScriptProxyCallback)(
    const char* webTag, const ArkWeb_JavaScriptBridgeData* dataArray, size_t arraySize, void* userData);

/**
 * @brief Defines the component callback of the native ArkWeb.
 *
 * @since 12
 */
typedef void (*ArkWeb_OnComponentCallback)(const char* webTag, void* userData);

/**
 * @brief Defines the ArkWeb_WebMessagePort that represent a HTML5 message port.
 *
 * @since 12
 */
typedef struct ArkWeb_WebMessagePort* ArkWeb_WebMessagePortPtr;

/**
 * @brief Defines the callback to receive message from HTML.
 *
 * @param webTag The name of the web component.
 * @param port The ArkWeb_WebMessagePort for registering the ArkWeb_OnMessageEventHandler.
 * @param message The received ArkWeb_WebMessage.
 * @param userData The data set by user.
 *
 * @since 12
 */
typedef void (*ArkWeb_OnMessageEventHandler)(
    const char* webTag, const ArkWeb_WebMessagePortPtr port, const ArkWeb_WebMessagePtr message, void* userData);

/**
 * @brief Defines the javascript object.
 *
 * @since 12
 */
typedef struct {
    /** A piece of javascript code. */
    const uint8_t* buffer;
    /** The size of the javascript code. */
    size_t size;
    /** Callbacks execute JavaScript script results. */
    ArkWeb_OnJavaScriptCallback callback;
    /** The user data to set. */
    void* userData;
} ArkWeb_JavaScriptObject;

/**
 * @brief Defines the javascript proxy registered method object.
 *
 * @since 12
 */
typedef struct {
    /** The method of the application side JavaScript object participating in the registration. */
    const char* methodName;
    /** The callback function registered by developer is called back when HTML side uses. */
    ArkWeb_OnJavaScriptProxyCallback callback;
    /** The user data to set. */
    void* userData;
} ArkWeb_ProxyMethod;

/**
 * @brief Defines the javascript proxy registered object.
 *
 * @since 12
 */
typedef struct {
    /** The name of the registered object. */
    const char* objName;
    /** The javascript proxy registered method object list */
    const ArkWeb_ProxyMethod* methodList;
    /** The size of the methodList. */
    size_t size;
} ArkWeb_ProxyObject;

/**
 * @brief Defines the controller API for native ArkWeb.
 * Before invoking an API, you are advised to use ARKWEB_MEMBER_MISSING to check
 * whether the function structure has a corresponding function pointer to avoid crash
 * caused by mismatch between the SDK and the device ROM.
 *
 * @since 12
 */
typedef struct {
    /** The ArkWeb_ControllerAPI struct size. */
    size_t size;
    /** Load a piece of code and execute JS code in the context of the currently displayed page. */
    void (*runJavaScript)(const char* webTag, const ArkWeb_JavaScriptObject* javascriptObject);
    /** Register the JavaScript object and method list. */
    void (*registerJavaScriptProxy)(const char* webTag, const ArkWeb_ProxyObject* proxyObject);
    /** Deletes the registered object which th given name. */
    void (*deleteJavaScriptRegister)(const char* webTag, const char* objName);
    /** Refresh the current web page. */
    void (*refresh)(const char* webTag);
    /** Register the JavaScript object and async method list. */
    void (*registerAsyncJavaScriptProxy)(const char* webTag, const ArkWeb_ProxyObject* proxyObject);
    /**
     * @brief Creates a message channel to communicate with HTML and returns
     *        the message ports representing the message channel endpoints.
     *
     * @param webTag The name of the web component.
     * @param size The quantity of message ports.
     */
    ArkWeb_WebMessagePortPtr* (*createWebMessagePorts)(const char* webTag, size_t* size);

    /**
     * @brief Destroy message ports.
     *
     * @param ports Address of the message ports array pointer.
     * @param size The quantity of message ports.
     */
    void (*destroyWebMessagePorts)(ArkWeb_WebMessagePortPtr** ports, size_t size);

    /**
     * @brief Post message ports to main frame.
     *
     * @param webTag The name of the web component.
     * @param name Name of the message to be sent.
     * @param size The quantity of message ports.
     * @param url Indicates the URI for receiving the message.
     * @return Post web message result code.
     *         {@link ARKWEB_SUCCESS} post web message success.
     *         {@link ARKWEB_INVALID_PARAM} the parameter verification fails.
     *         {@link ARKWEB_INIT_ERROR} no web associated with this webTag.
     */
    ArkWeb_ErrorCode (*postWebMessage)(
        const char* webTag, const char* name, ArkWeb_WebMessagePortPtr* webMessagePorts, size_t size, const char* url);
} ArkWeb_ControllerAPI;

/**
 * @brief Defines the component API for native ArkWeb.
 *
 * @since 12
 */
typedef struct {
    /** The ArkWeb_ComponentAPI struct size. */
    size_t size;
    /** Register the OnControllerAttached callback. */
    void (*onControllerAttached)(const char* webTag, ArkWeb_OnComponentCallback callback, void* userData);
    /** Register the OnPageBegin callback. */
    void (*onPageBegin)(const char* webTag, ArkWeb_OnComponentCallback callback, void* userData);
    /** Register the OnPageEnd callback. */
    void (*onPageEnd)(const char* webTag, ArkWeb_OnComponentCallback callback, void* userData);
    /** Register the OnDestroy callback. */
    void (*onDestroy)(const char* webTag, ArkWeb_OnComponentCallback callback, void* userData);
} ArkWeb_ComponentAPI;

/**
 * @brief Defines the web message API for native ArkWeb.
 * Before invoking an API, you are advised to use ARKWEB_MEMBER_MISSING to check
 * whether the function structure has a corresponding function pointer to avoid crash
 * caused by mismatch between the SDK and the device ROM.
 *
 * @since 12
 */
typedef struct {
    /** The ArkWeb_WebMessagePortAPI struct size. */
    size_t size;
    /**
     * @brief Post message to HTML.
     *
     * @param webMessagePort The ArkWeb_WebMessagePort.
     * @param webTag The name of the web component.
     * @param webMessage The ArkWeb_WebMessage to send.
     * @return Post message result code.
     *         {@link ARKWEB_SUCCESS} post message success.
     *         {@link ARKWEB_INVALID_PARAM} the parameter verification fails.
     *         {@link ARKWEB_INIT_ERROR} no web associated with this webTag.
     */
    ArkWeb_ErrorCode (*postMessage)(
        const ArkWeb_WebMessagePortPtr webMessagePort, const char* webTag, const ArkWeb_WebMessagePtr webMessage);
    /**
     * @brief Close the message port.
     *
     * @param webMessagePort The ArkWeb_WebMessagePort.
     * @param webTag The name of the web component.
     */
    void (*close)(const ArkWeb_WebMessagePortPtr webMessagePort, const char* webTag);
    /**
     * @brief Set a callback to receive message from HTML.
     *
     * @param webMessagePort The ArkWeb_WebMessagePort.
     * @param webTag The name of the web component.
     * @param messageEventHandler The handler to receive message from HTML.
     * @param userData The data set by user.
     */
    void (*setMessageEventHandler)(const ArkWeb_WebMessagePortPtr webMessagePort, const char* webTag,
        ArkWeb_OnMessageEventHandler messageEventHandler, void* userData);
} ArkWeb_WebMessagePortAPI;

/**
 * @brief Defines the web message data API for native ArkWeb.
 * Before invoking an API, you are advised to use ARKWEB_MEMBER_MISSING to check
 * whether the function structure has a corresponding function pointer to avoid crash
 * caused by mismatch between the SDK and the device ROM.
 *
 * @since 12
 */
typedef struct {
    /** The ArkWeb_WebMessageAPI struct size. */
    size_t size;
    /**
     *  @brief Used to create a ArkWeb_WebMessage.
     *
     *  @return The created ArkWeb_WebMessage, destroy it through
     *  destroyWebMessage after it is no longer used.
     */
    ArkWeb_WebMessagePtr (*createWebMessage)();
    /**
     *  @brief Used to destroy a ArkWeb_WebMessage.
     *
     *  @param webMessage The ArkWeb_WebMessage to destroy.
     */
    void (*destroyWebMessage)(ArkWeb_WebMessagePtr* webMessage);
    /**
     *  @brief Set the type of ArkWeb_WebMessage.
     *
     *  @param webMessage The ArkWeb_WebMessage.
     *  @param type The type of ArkWeb_WebMessage.
     */
    void (*setType)(ArkWeb_WebMessagePtr webMessage, ArkWeb_WebMessageType type);
    /**
     *  @brief Get the type of ArkWeb_WebMessage.
     *
     *  @param webMessage The ArkWeb_WebMessage.
     *  @return The type of ArkWeb_WebMessage.
     */
    ArkWeb_WebMessageType (*getType)(ArkWeb_WebMessagePtr webMessage);
    /**
     *  @brief Set the data of ArkWeb_WebMessage.
     *
     *  @param webMessage The ArkWeb_WebMessage.
     *  @param data The data of ArkWeb_WebMessage.
     *  @param dataLength The length of data.
     */
    void (*setData)(ArkWeb_WebMessagePtr webMessage, void* data, size_t dataLength);
    /**
     *  @brief Get the data of ArkWeb_WebMessage.
     *
     *  @param webMessage The ArkWeb_WebMessage.
     *  @param dataLength The length of data.
     *  @return The data of ArkWeb_WebMessage.
     */
    void* (*getData)(ArkWeb_WebMessagePtr webMessage, size_t* dataLength);
} ArkWeb_WebMessageAPI;

/**
 * @brief Defines the native CookieManager API for ArkWeb.
 * Before invoking an API, you are advised to use ARKWEB_MEMBER_MISSING to check
 * whether the function structure has a corresponding function pointer to avoid crash
 * caused by mismatch between the SDK and the device ROM.
 *
 * @since 12
 */
typedef struct {
    /** The ArkWeb_CookieManagerAPI struct size. */
    size_t size;
 
    /**
     * @brief Obtains the cookie value corresponding to a specified URL.
     *
     * @param url URL to which the cookie to be obtained belongs. A complete URL is recommended.
     * @param incognito True indicates that the memory cookies of the webview in privacy mode are obtained,
     *                  and false indicates that cookies in non-privacy mode are obtained.
     * @param includeHttpOnly If true HTTP-only cookies will also be included in the cookieValue.
     * @param cookieValue Get the cookie value corresponding to the URL.
     * @return Fetch cookie result code.
     *         {@link ARKWEB_SUCCESS} fetch cookie success.
     *         {@link ARKWEB_INVALID_URL} invalid url.
     *         {@link ARKWEB_INVALID_PARAM} cookieValue is nullptr.
     */
    ArkWeb_ErrorCode (*fetchCookieSync)(const char* url, bool incognito, bool includeHttpOnly, char** cookieValue);

    /**
     * @brief Sets the cookie value for a specified URL.
     *
     * @param url Specifies the URL to which the cookie belongs. A complete URL is recommended.
     * @param cookieValue The value of the cookie to be set.
     * @param incognito True indicates that cookies of the corresponding URL are set in privacy mode,
     *                  and false indicates that cookies of the corresponding URL are set in non-privacy mode.
     * @param includeHttpOnly If true, HTTP-only cookies can also be overwritten.
     * @return Config cookie result code.
     *         {@link ARKWEB_SUCCESS} config cookie success.
     *         {@link ARKWEB_INVALID_URL} invalid url.
     *         {@link ARKWEB_INVALID_COOKIE_VALUE} invalid cookie value.
     */
    ArkWeb_ErrorCode (*configCookieSync)(const char* url,
        const char* cookieValue, bool incognito, bool includeHttpOnly);

    /**
     * @brief Check whether cookies exist.
     *
     * @param incognito True indicates whether cookies exist in privacy mode,
     *                  and false indicates whether cookies exist in non-privacy mode.
     * @return True indicates that the cookie exists, and false indicates that the cookie does not exist.
     */
    bool (*existCookies)(bool incognito);

    /**
     * @brief Clear all cookies.
     *
     * @param incognito True indicates that all memory cookies of the webview are cleared in privacy mode,
     *                  and false indicates that persistent cookies in non-privacy mode are cleared.
     */
    void (*clearAllCookiesSync)(bool incognito);

    /**
     * @brief Clear all session cookies.
     */
    void (*clearSessionCookiesSync)();
} ArkWeb_CookieManagerAPI;

/**
 * @brief Check whether the member variables of the current struct exist.
 *
 * @since 12
 */
#define ARKWEB_MEMBER_EXISTS(s, f) \
    ((intptr_t) & ((s)->f) - (intptr_t)(s) + sizeof((s)->f) <= *reinterpret_cast<size_t*>(s))

/**
 * @brief Return false if the struct member does not exist, otherwise true.
 *
 * @since 12
 */
#define ARKWEB_MEMBER_MISSING(s, f) (!ARKWEB_MEMBER_EXISTS(s, f) || !((s)->f))

#ifdef __cplusplus
}
#endif
#endif // ARKWEB_TYPE_H