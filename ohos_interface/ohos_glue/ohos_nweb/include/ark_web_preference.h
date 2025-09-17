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

#ifndef ARK_WEB_PREFERENCE_H_
#define ARK_WEB_PREFERENCE_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebPreference : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Get the user-agent string to the nweb.
     *
     * @see PutUserAgent
     */
    /*--ark web()--*/
    virtual ArkWebString UserAgent() = 0;

    /**
     * @brief Put the user-agent string to the nweb. If it is null or empty,
     *        webview will use the system default value. Changing the user-agent
     *        while loading a web page will cause the web page to reload.
     *
     * @param ua user-agent string. The value may be null.
     */
    /*--ark web()--*/
    virtual void PutUserAgent(const ArkWebString& ua) = 0;

    /**
     * @brief Get the default user-agent string to the nweb. An instance of
     *        WebView could use a different User-Agent that
     *        NWebPreference#PutUserAgent(String) set to.
     *
     * @see PutUserAgent
     */
    /*--ark web()--*/
    virtual ArkWebString DefaultUserAgent() = 0;

    /**
     * @brief Get cache mode
     *
     * @see PutCacheMode
     */
    /*--ark web()--*/
    virtual int CacheMode() = 0;

    /**
     * @brief PutCacheMode
     */
    /*--ark web()--*/
    virtual void PutCacheMode(int flag) = 0;

    /**
     * @brief Get if the WebView block loading resources from the network.
     *
     * @see PutBlockNetwork
     */
    /*--ark web()--*/
    virtual bool IsNetworkBlocked() = 0;

    /**
     * @brief Put whether the WebView block loading resources from the network.
     *        The default value is false if the hap has the
     *        ohos.permission.INTERNET permission, otherwise it is true.If the hap
     *        does not have the ohos.permission.INTERNET permission, attempts to
     *        set a value of false will be failed.
     */
    /*--ark web()--*/
    virtual void PutBlockNetwork(bool flag) = 0;

    /**
     * @brief Get the color of scrollbar.
     *
     * @see PutScrollbarColor
     */
    /*--ark web()--*/
    virtual uint32_t GetScrollBarColor() = 0;

    /**
     * @brief Put the UX color of scrollbar.
     */
    /*--ark web()--*/
    virtual void PutScrollBarColor(uint32_t color_value) = 0;

    /**
     * @brief Get over-scroll Mode.
     *
     * @see PutScrollbarColor
     */
    /*--ark web()--*/
    virtual int GetOverscrollMode() = 0;

    /**
     * @brief Put over-scroll Mode.
     */
    /*--ark web()--*/
    virtual void PutOverscrollMode(int over_scroll_mode) = 0;

    /**
     * @brief Get the size of default font.
     *
     * @see PutDefaultFontSize
     */
    /*--ark web()--*/
    virtual int DefaultFontSize() = 0;

    /**
     * @brief Put the size of default font. The default is 16.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    /*--ark web()--*/
    virtual void PutDefaultFontSize(int size) = 0;

    /**
     * @brief Get whether smooth mode is supported.
     */
    /*--ark web()--*/
    virtual bool GetPinchSmoothMode() = 0;

    /**
     * @brief Put whether smooth mode is supported.
     */
    /*--ark web()--*/
    virtual void PutPinchSmoothMode(bool flag) = 0;

    /**
     * @brief Get if the database storage API is supported.
     *
     * @see PutDatabaseAllowed
     */
    /*--ark web()--*/
    virtual bool IsDataBaseEnabled() = 0;

    /**
     * @brief Enables or disables the database storage API. The default is false.
     *        This setting is global and effectd all WebView instances in a
     *        process. You must modify this before loading any webView page so
     *        that the changes won't be ignored.
     */
    /*--ark web()--*/
    virtual void PutDatabaseAllowed(bool flag) = 0;

    /**
     * @brief Get whether support multi window.
     *
     * @see PutMultiWindowAccess
     */
    /*--ark web()--*/
    virtual bool IsMultiWindowAccess() = 0;

    /**
     * @brief Put whether support multi window, default value is false.
     */
    /*--ark web()--*/
    virtual void PutMultiWindowAccess(bool flag) = 0;

    /**
     * @brief Get if the WebView can execute JavaScript.
     *
     * @see PutJavaScriptEnabled
     */
    /*--ark web()--*/
    virtual bool IsJavaScriptAllowed() = 0;

    /**
     * @brief Put whether the WebView can execute JavaScript. The default is
     *        false.
     */
    /*--ark web()--*/
    virtual void PutJavaScriptEnabled(bool flag) = 0;

    /**
     * @brief Get whether the dark mode prefer-color-scheme is enabled for this
     *        NWeb.
     *
     * @see PutDarkSchemeEnabled
     */
    /*--ark web()--*/
    virtual int DarkSchemeEnabled() = 0;

    /**
     * @brief Enables or disables the dark mode prefer-color-scheme for this NWeb.
     *
     * @param dark_scheme True if set the dark mode prefer-color-scheme enabled
     *        for this NWeb.
     */
    /*--ark web()--*/
    virtual void PutDarkSchemeEnabled(int dark_scheme) = 0;

    /**
     * @brief Get if the DOM storage API is supported.
     *
     * @see PutDomStorageEnabled
     */
    /*--ark web()--*/
    virtual bool IsDomStorageEnabled() = 0;

    /**
     * @brief Enables or disables the DOM storage API. The default value is false.
     */
    /*--ark web()--*/
    virtual void PutDomStorageEnabled(bool flag) = 0;

    /**
     * @brief Get the lower limit of the minimum font size.
     *
     * @see PutFontSizeLowerLimit
     */
    /*--ark web()--*/
    virtual int FontSizeLowerLimit() = 0;

    /**
     * @brief Put the lower limit of the minimum font size. The default is 8.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    /*--ark web()--*/
    virtual void PutFontSizeLowerLimit(int size) = 0;

    /**
     * @brief Get if the WebView can get geo location.
     *
     * @see PutGeoLocationAllowed
     */
    /*--ark web()--*/
    virtual bool GeoLocationAllowed() = 0;

    /**
     * @brief Put whether the WebView can get geo location. The default is true.
     *        To get geo location, an application must have permission to access
     *        the device location, see ohos.permission.LOCATION and
     *        ohos.permission.LOCATION_IN_BACKGROUND and implement the
     *        NWebHandler#OnGeoLocationShow callback to receive notifications of
     *        the location request via the JavaScript Geo Location API.
     */
    /*--ark web()--*/
    virtual void PutGeoLocationAllowed(bool flag) = 0;

    /**
     * @brief Get if the NWeb can be debugging.
     *
     * @see PutWebDebuggingAccess
     */
    /*--ark web()--*/
    virtual bool IsWebDebuggingAccess() = 0;

    /**
     * @brief Put whether the NWeb can be debugging, default value is false.
     */
    /*--ark web()--*/
    virtual void PutWebDebuggingAccess(bool flag) = 0;

    /**
     * @brief Get if content URL(content from a content provider installed in the
     *        system) access within WebView is supported.
     *
     * @see PutEnableContentAccess
     */
    /*--ark web()--*/
    virtual bool EnableContentAccess() = 0;

    /**
     * @brief Enables or disables content URL(content from a content provider
     *        installed in the system) access within WebView. The default is true.
     */
    /*--ark web()--*/
    virtual void PutEnableContentAccess(bool flag) = 0;

    /**
     * @brief Get if file system access within WebView is supported. Notified
     *        files in the path of AppData are always accessible.
     *
     * @see PutEnableRawFileAccess
     */
    /*--ark web()--*/
    virtual bool EnableRawFileAccess() = 0;

    /**
     * @brief Enables or disables file system access within WebView. But files in
     *        the path of AppData are still accessible. The default is false.
     */
    /*--ark web()--*/
    virtual void PutEnableRawFileAccess(bool flag) = 0;

    /**
     * @brief Get if the WebView can load image.
     *
     * @see PutImageLoadingAllowed
     */
    /*--ark web()--*/
    virtual bool IsImageLoadingAllowed() = 0;

    /**
     * @brief Put whether the WebView can load image. The default is true.
     */
    /*--ark web()--*/
    virtual void PutImageLoadingAllowed(bool flag) = 0;

    /**
     * @brief Get the fixed font family name.
     *
     * @see PutFixedFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString FixedFontFamilyName() = 0;

    /**
     * @brief Put the fixed font family name. The default is "monospace".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutFixedFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get the serif font family name.
     *
     * @see PutSerifFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString SerifFontFamilyName() = 0;

    /**
     * @brief Put the serif font family name. The default is "serif".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutSerifFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get the zoom percentage of the page text.
     *
     * @see PutZoomingForTextFactor
     */
    /*--ark web()--*/
    virtual int ZoomingForTextFactor() = 0;

    /**
     * @brief Put the zoom percentage of the page text. The default is 100.
     *
     * @param text_zoom the zoom percentage of the page text
     */
    /*--ark web()--*/
    virtual void PutZoomingForTextFactor(int text_zoom) = 0;

    /**
     * @brief Get whether the force dark mode is enabled for this NWeb.
     *
     * @see PutForceDarkModeEnabled
     */
    /*--ark web()--*/
    virtual int ForceDarkModeEnabled() = 0;

    /**
     * @brief Enables or disables the force dark mode for this NWeb.
     *
     * @param force_dark True if set the force dark mode enabled for this NWeb.
     */
    /*--ark web()--*/
    virtual void PutForceDarkModeEnabled(int force_dark) = 0;

    /**
     * @brief Get the swith for the overview mode.
     *
     * @see PutLoadWithOverviewMode
     */
    /*--ark web()--*/
    virtual bool IsLoadWithOverviewMode() = 0;

    /**
     * @brief Sets whether the WebView loads pages in overview mode, that is,
     *        zooms out the content to fit on screen by width.
     */
    /*--ark web()--*/
    virtual void PutLoadWithOverviewMode(bool flag) = 0;

    /**
     * @brief Get the size of default fixed font.
     *
     * @see PutDefaultFixedFontSize
     */
    /*--ark web()--*/
    virtual int DefaultFixedFontSize() = 0;

    /**
     * @brief Put the size of default fixed font. The default is 13.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    /*--ark web()--*/
    virtual void PutDefaultFixedFontSize(int size) = 0;

    /**
     * @brief Get the cursive font family name.
     *
     * @see PutCursiveFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString CursiveFontFamilyName() = 0;

    /**
     * @brief Put the cursive font family name. The default is "cursive".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutCursiveFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get the fantasy font family name.
     *
     * @see PutFantasyFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString FantasyFontFamilyName() = 0;

    /**
     * @brief Put the fantasy font family name. The default is "fantasy".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutFantasyFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get if the WebView supports zooming.
     *
     * @see PutZoomingFunctionEnabled
     */
    /*--ark web()--*/
    virtual bool ZoomingfunctionEnabled() = 0;

    /**
     * @brief Put whether the WebView supports zooming. The default is true.
     */
    /*--ark web()--*/
    virtual void PutZoomingFunctionEnabled(bool flag) = 0;

    /**
     * @brief Get whether media playback needs to be triggered by user gestures.
     *
     * @see PutMediaPlayGestureAccess
     */
    /*--ark web()--*/
    virtual bool GetMediaPlayGestureAccess() = 0;

    /**
     * @brief Put whether media playback needs to be triggered by user gestures,
     *        default value is false.
     */
    /*--ark web()--*/
    virtual void PutMediaPlayGestureAccess(bool flag) = 0;

    /**
     * @brief Get the standard font family name.
     *
     * @see PutStandardFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString StandardFontFamilyName() = 0;

    /**
     * @brief Put the standard font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutStandardFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get the sans-serif font family name.
     *
     * @see PutSansSerifFontFamilyName
     */
    /*--ark web()--*/
    virtual ArkWebString SansSerifFontFamilyName() = 0;

    /**
     * @brief Put the sans-serif font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    /*--ark web()--*/
    virtual void PutSansSerifFontFamilyName(const ArkWebString& font) = 0;

    /**
     * @brief Get whether enable vertical scroll bar.
     *
     * @see PutVerticalScrollBarAccess
     */
    /*--ark web()--*/
    virtual bool IsVerticalScrollBarAccess() = 0;

    /**
     * @brief Put whether enable vertical scroll bar, default value is false.
     */
    /*--ark web()--*/
    virtual void PutVerticalScrollBarAccess(bool flag) = 0;

    /**
     * @brief Get whether enable horizontal scroll bar.
     *
     * @see PutHorizontalScrollBarAccess
     */
    /*--ark web()--*/
    virtual bool IsHorizontalScrollBarAccess() = 0;

    /**
     * @brief Put whether enable horizontal scroll bar, default value is false.
     */
    /*--ark web()--*/
    virtual void PutHorizontalScrollBarAccess(bool flag) = 0;

    /**
     * @brief Get the lower limit of the minimum logical font size.
     *
     * @see PutLogicalFontSizeLowerLimit
     */
    /*--ark web()--*/
    virtual int LogicalFontSizeLowerLimit() = 0;

    /**
     * @brief Put the lower limit of the minimum logical font size. The default
     *        is 8.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    /*--ark web()--*/
    virtual void PutLogicalFontSizeLowerLimit(int size) = 0;

    /**
     * @brief Get the default text encoding format that uses to decode html pages.
     *
     * @see PutDefaultTextEncodingFormat
     */
    /*--ark web()--*/
    virtual ArkWebString DefaultTextEncodingFormat() = 0;

    /**
     * @brief Put the default text encoding format that uses to decode html pages.
     *        The default is "UTF-8".
     *
     * @param the text encoding format
     */
    /*--ark web()--*/
    virtual void PutDefaultTextEncodingFormat(const ArkWebString& encoding) = 0;

    /**
     * @brief Get if the WebView from loading image resources from the network
     *        (http and https URI schemes) is supported.
     *
     * @see PutLoadImageFromNetworkDisabled
     */
    /*--ark web()--*/
    virtual bool IsLoadImageFromNetworkDisabled() = 0;

    /**
     * @brief Put whether to block the WebView from loading image resources from
     *        the network (http and https URI schemes). This settings is invalid,
     *        if {@link #IsImageLoadingAllowed} returns false. The default is
     *        false.
     */
    /*--ark web()--*/
    virtual void PutLoadImageFromNetworkDisabled(bool flag) = 0;

    /**
     * @brief Get if JavaScript running in a file scheme URL to access content
     *        from other file scheme URLs is supported.
     *
     * @see PutEnableRawFileAccessFromFileURLs
     */
    /*--ark web()--*/
    virtual bool EnableRawFileAccessFromFileURLs() = 0;

    /**
     * @brief Put whether to allow JavaScript running in a file scheme URL to
     *        access content from other file scheme URLs. The default is false.
     */
    /*--ark web()--*/
    virtual void PutEnableRawFileAccessFromFileURLs(bool flag) = 0;

    /**
     * @brief Get if JavaScript running in a file scheme URL to access content
     *        from any origin is supported. This includes access to content from
     *        other file scheme URLs.
     *
     * @see PutEnableUniversalAccessFromFileURLs
     */
    /*--ark web()--*/
    virtual bool EnableUniversalAccessFromFileURLs() = 0;

    /**
     * @brief Put whether to allow JavaScript running in a file scheme URL to
     *        access content from any origin. This includes access to content from
     *        other file scheme URLs. See {@link
     *        #PutEnableRawFileAccessFromFileURLs}. The default is false.
     */
    /*--ark web()--*/
    virtual void PutEnableUniversalAccessFromFileURLs(bool flag) = 0;

    /**
     * @brief Get if JavaScript can open windows.
     *
     * @see PutIsCreateWindowsByJavaScriptAllowed
     */
    /*--ark web()--*/
    virtual bool IsCreateWindowsByJavaScriptAllowed() = 0;

    /**
     * @brief Put whether JavaScript can open windows by JavaScript. This applies
     *        to the JavaScript function {@code window.open()}. The default is
     *        false.
     */
    /*--ark web()--*/
    virtual void PutIsCreateWindowsByJavaScriptAllowed(bool flag) = 0;

    /**
     * @brief Get the WebView's behavior when a secure origin attempts to load a
     *        resource from an insecure origin.
     *
     * @see PutAccessModeForSecureOriginLoadFromInsecure
     */
    /*--ark web()--*/
    virtual int AccessModeForSecureOriginLoadFromInsecure() = 0;

    /**
     * @brief Put the WebView's behavior when a secure origin attempts to load a
     *        resource from an insecure origin. The default is NEVER_ALLOW.
     *
     * @param mode The mixed content mode to use.
     */
    /*--ark web()--*/
    virtual void PutAccessModeForSecureOriginLoadFromInsecure(int mode) = 0;

    /**
     * @brief Get copy-option Mode.
     *
     * @see PutCopyOptionMode
     */
    /*--ark web()--*/
    virtual int GetCopyOptionMode() = 0;

    /**
     * @brief Put copy-option.
     */
    /*--ark web()--*/
    virtual void PutCopyOptionMode(int copyOption) = 0;

    /**
     * @brief Put whether the embed mode. The default is false.
     */
    /*--ark web()--*/
    virtual void SetNativeEmbedMode(bool flag) = 0;

    /**
     * @brief Get whether the embed mode. The default is false.
     */
    /*--ark web()--*/
    virtual bool GetNativeEmbedMode() = 0;

    /**
     * @brief Register native embed rule. The default pattern is ["embed", "native/"].
     */
    /*--ark web()--*/
    virtual void RegisterNativeEmbedRule(const ArkWebString& tag, const ArkWebString& type) = 0;

    /**
     * @brief Set whether the scroll is enabled.
     */
    /*--ark web()--*/
    virtual void SetScrollable(bool enable) = 0;

    /**
     * @brief Get whether the scroll is enabled.
     */
    /*--ark web()--*/
    virtual bool GetScrollable() = 0;

    /**
     * @brief Put whether enable horizontal scroll bar, default value is false.
     */
    /*--ark web()--*/
    virtual void PutTextAutosizingEnabled(bool flag) = 0;

    /**
     * @brief Set whether to support the viewport attribute of the meta tag in the frontend page.
     */
    /*--ark web()--*/
    virtual void SetViewportEnable(bool enable) = 0;

    /**
     * @brief Set native video player config.
     */
    /*--ark web()--*/
    virtual void SetNativeVideoPlayerConfig(bool enable, bool shouldOverlay) = 0;

    /**
     * @brief Get surfaceId.
     */
    /*--ark web()--*/
    virtual ArkWebString GetSurfaceId() = 0;

    /**
     * @brief Set surfaceId.
     */
    /*--ark web()--*/
    virtual void SetSurfaceId(const ArkWebString& surfaceId) = 0;

    /**
     * @brief Set whether the scollbar is displayed.
     */
    /*--ark web()--*/
    virtual void PutOverlayScrollbarEnabled(bool enable) = 0;

    /**
     * @brief Set whether the scroll is enabled.
     */
    /*--ark web()--*/
    virtual void SetScrollable(bool enable, int32_t scrollType) = 0;

    /**
     * @brief Set whether to support the blur when the keyboard is hidden by manual.
     * @param enable set whether blur is allowed.
     */
    /*--ark web()--*/
    virtual void SetBlurOnKeyboardHideMode(int enable) = 0;

    /**
     * @brief Set whether to connect to media avsession.
     */
    /*--ark web()--*/
    virtual void PutWebMediaAVSessionEnabled(bool enable) = 0;

    /**
     * @brief Set the scale of the font-weight.The default is 1.0f.
     */
    /*--ark web()--*/
    virtual void SetFontWeightScale(float scale) = 0;

    /**
     * @brief Set whether to use the intrinsic size. The default is false.
     */
    /*--ark web()--*/
    virtual void SetIntrinsicSizeEnable(bool enable) = 0;

    /**
     * @brief Set whether to support css diplay when reporting the visibility of native embed.
     *        The default is false.
     */
    /*--ark web()--*/
    virtual void SetCssDisplayChangeEnabled(bool enable) = 0;

    /**
     * @brief Set whether to bypass vsync.
     *        The default is none.
     */
    /*--ark web()--*/
    virtual void SetBypassVsyncCondition(int32_t condition) = 0;

    /**
     * @brief Set the gesture focus mode.
     */
    /*--ark web()--*/
    virtual void SetGestureFocusMode(int32_t mode) = 0;

    /**
     * @brief Sets whether the Web supports zooming using Ctrl.
     */
    /*--ark web()--*/
    virtual void PutZoomControlAccess(bool zoomControlAccess) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PREFERENCE_H_
