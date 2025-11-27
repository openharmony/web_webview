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

#ifndef ARK_WEB_PREFERENCE_IMPL_H_
#define ARK_WEB_PREFERENCE_IMPL_H_
#pragma once

#include "include/nweb_preference.h"
#include "ohos_nweb/include/ark_web_preference.h"

namespace OHOS::ArkWeb {

class ArkWebPreferenceImpl : public ArkWebPreference {
    IMPLEMENT_REFCOUNTING(ArkWebPreferenceImpl);

public:
    ArkWebPreferenceImpl(std::shared_ptr<OHOS::NWeb::NWebPreference> nweb_preference);
    ~ArkWebPreferenceImpl() = default;

    /**
     * @brief Get the user-agent string to the nweb.
     *
     * @see PutUserAgent
     */
    ArkWebString UserAgent() override;

    /**
     * @brief Put the user-agent string to the nweb. If it is null or empty,
     *        webview will use the system default value. Changing the user-agent
     *        while loading a web page will cause the web page to reload.
     *
     * @param ua user-agent string. The value may be null.
     */
    void PutUserAgent(const ArkWebString& ua) override;

    /**
     * @brief Get the default user-agent string to the nweb. An instance of
     *        WebView could use a different User-Agent that
     *        NWebPreference#PutUserAgent(String) set to.
     *
     * @see PutUserAgent
     */
    ArkWebString DefaultUserAgent() override;

    /**
     * @brief Get cache mode
     *
     * @see PutCacheMode
     */
    int CacheMode() override;

    /**
     * @brief PutCacheMode
     */
    void PutCacheMode(int flag) override;

    /**
     * @brief Get if the WebView block loading resources from the network.
     *
     * @see PutBlockNetwork
     */
    bool IsNetworkBlocked() override;

    /**
     * @brief Put whether the WebView block loading resources from the network.
     *        The default value is false if the hap has the
     *        ohos.permission.INTERNET permission, otherwise it is true.If the hap
     *        does not have the ohos.permission.INTERNET permission, attempts to
     *        set a value of false will be failed.
     */
    void PutBlockNetwork(bool flag) override;

    /**
     * @brief Get the color of scrollbar.
     *
     * @see PutScrollbarColor
     */
    uint32_t GetScrollBarColor() override;

    /**
     * @brief Put the UX color of scrollbar.
     */
    void PutScrollBarColor(uint32_t color_value) override;

    /**
     * @brief Get over-scroll Mode.
     *
     * @see PutScrollbarColor
     */
    int GetOverscrollMode() override;

    /**
     * @brief Put over-scroll Mode.
     */
    void PutOverscrollMode(int over_scroll_mode) override;

    /**
     * @brief Get the size of default font.
     *
     * @see PutDefaultFontSize
     */
    int DefaultFontSize() override;

    /**
     * @brief Put the size of default font. The default is 16.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    void PutDefaultFontSize(int size) override;

    /**
     * @brief Get whether smooth mode is supported.
     */
    bool GetPinchSmoothMode() override;

    /**
     * @brief Put whether smooth mode is supported.
     */
    void PutPinchSmoothMode(bool flag) override;

    /**
     * @brief Get if the database storage API is supported.
     *
     * @see PutDatabaseAllowed
     */
    bool IsDataBaseEnabled() override;

    /**
     * @brief Enables or disables the database storage API. The default is false.
     *        This setting is global and effectd all WebView instances in a
     *        process. You must modify this before loading any webView page so
     *        that the changes won't be ignored.
     */
    void PutDatabaseAllowed(bool flag) override;

    /**
     * @brief Get whether support multi window.
     *
     * @see PutMultiWindowAccess
     */
    bool IsMultiWindowAccess() override;

    /**
     * @brief Put whether support multi window, default value is false.
     */
    void PutMultiWindowAccess(bool flag) override;

    /**
     * @brief Get if the WebView can execute JavaScript.
     *
     * @see PutJavaScriptEnabled
     */
    bool IsJavaScriptAllowed() override;

    /**
     * @brief Put whether the WebView can execute JavaScript. The default is
     *        false.
     */
    void PutJavaScriptEnabled(bool flag) override;

    /**
     * @brief Get whether the dark mode prefer-color-scheme is enabled for this
     *        NWeb.
     *
     * @see PutDarkSchemeEnabled
     */
    int DarkSchemeEnabled() override;

    /**
     * @brief Enables or disables the dark mode prefer-color-scheme for this NWeb.
     *
     * @param dark_scheme True if set the dark mode prefer-color-scheme enabled
     *        for this NWeb.
     */
    void PutDarkSchemeEnabled(int dark_scheme) override;

    /**
     * @brief Get if the DOM storage API is supported.
     *
     * @see PutDomStorageEnabled
     */
    bool IsDomStorageEnabled() override;

    /**
     * @brief Enables or disables the DOM storage API. The default value is false.
     */
    void PutDomStorageEnabled(bool flag) override;

    /**
     * @brief Get the lower limit of the minimum font size.
     *
     * @see PutFontSizeLowerLimit
     */
    int FontSizeLowerLimit() override;

    /**
     * @brief Put the lower limit of the minimum font size. The default is 8.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    void PutFontSizeLowerLimit(int size) override;

    /**
     * @brief Get if the WebView can get geo location.
     *
     * @see PutGeoLocationAllowed
     */
    bool GeoLocationAllowed() override;

    /**
     * @brief Put whether the WebView can get geo location. The default is true.
     *        To get geo location, an application must have permission to access
     *        the device location, see ohos.permission.LOCATION and
     *        ohos.permission.LOCATION_IN_BACKGROUND and implement the
     *        NWebHandler#OnGeoLocationShow callback to receive notifications of
     *        the location request via the JavaScript Geo Location API.
     */
    void PutGeoLocationAllowed(bool flag) override;

    /**
     * @brief Get if the NWeb can be debugging.
     *
     * @see PutWebDebuggingAccess
     */
    bool IsWebDebuggingAccess() override;

    /**
     * @brief Put whether the NWeb can be debugging, default value is false.
     */
    void PutWebDebuggingAccess(bool flag) override;

    /**
     * @brief Get if content URL(content from a content provider installed in the
     *        system) access within WebView is supported.
     *
     * @see PutEnableContentAccess
     */
    bool EnableContentAccess() override;

    /**
     * @brief Enables or disables content URL(content from a content provider
     *        installed in the system) access within WebView. The default is true.
     */
    void PutEnableContentAccess(bool flag) override;

    /**
     * @brief Get if file system access within WebView is supported. Notified
     *        files in the path of AppData are always accessible.
     *
     * @see PutEnableRawFileAccess
     */
    bool EnableRawFileAccess() override;

    /**
     * @brief Enables or disables file system access within WebView. But files in
     *        the path of AppData are still accessible. The default is false.
     */
    void PutEnableRawFileAccess(bool flag) override;

    /**
     * @brief Get if the WebView can load image.
     *
     * @see PutImageLoadingAllowed
     */
    bool IsImageLoadingAllowed() override;

    /**
     * @brief Put whether the WebView can load image. The default is true.
     */
    void PutImageLoadingAllowed(bool flag) override;

    /**
     * @brief Get the fixed font family name.
     *
     * @see PutFixedFontFamilyName
     */
    ArkWebString FixedFontFamilyName() override;

    /**
     * @brief Put the fixed font family name. The default is "monospace".
     *
     * @param font a font family name
     */
    void PutFixedFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get the serif font family name.
     *
     * @see PutSerifFontFamilyName
     */
    ArkWebString SerifFontFamilyName() override;

    /**
     * @brief Put the serif font family name. The default is "serif".
     *
     * @param font a font family name
     */
    void PutSerifFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get the zoom percentage of the page text.
     *
     * @see PutZoomingForTextFactor
     */
    int ZoomingForTextFactor() override;

    /**
     * @brief Put the zoom percentage of the page text. The default is 100.
     *
     * @param text_zoom the zoom percentage of the page text
     */
    void PutZoomingForTextFactor(int text_zoom) override;

    /**
     * @brief Get whether the force dark mode is enabled for this NWeb.
     *
     * @see PutForceDarkModeEnabled
     */
    int ForceDarkModeEnabled() override;

    /**
     * @brief Enables or disables the force dark mode for this NWeb.
     *
     * @param force_dark True if set the force dark mode enabled for this NWeb.
     */
    void PutForceDarkModeEnabled(int force_dark) override;

    /**
     * @brief Get the swith for the overview mode.
     *
     * @see PutLoadWithOverviewMode
     */
    bool IsLoadWithOverviewMode() override;

    /**
     * @brief Sets whether the WebView loads pages in overview mode, that is,
     *        zooms out the content to fit on screen by width.
     */
    void PutLoadWithOverviewMode(bool flag) override;

    /**
     * @brief Get the size of default fixed font.
     *
     * @see PutDefaultFixedFontSize
     */
    int DefaultFixedFontSize() override;

    /**
     * @brief Put the size of default fixed font. The default is 13.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    void PutDefaultFixedFontSize(int size) override;

    /**
     * @brief Get the cursive font family name.
     *
     * @see PutCursiveFontFamilyName
     */
    ArkWebString CursiveFontFamilyName() override;

    /**
     * @brief Put the cursive font family name. The default is "cursive".
     *
     * @param font a font family name
     */
    void PutCursiveFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get the fantasy font family name.
     *
     * @see PutFantasyFontFamilyName
     */
    ArkWebString FantasyFontFamilyName() override;

    /**
     * @brief Put the fantasy font family name. The default is "fantasy".
     *
     * @param font a font family name
     */
    void PutFantasyFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get if the WebView supports zooming.
     *
     * @see PutZoomingFunctionEnabled
     */
    bool ZoomingfunctionEnabled() override;

    /**
     * @brief Put whether the WebView supports zooming. The default is true.
     */
    void PutZoomingFunctionEnabled(bool flag) override;

    /**
     * @brief Get whether media playback needs to be triggered by user gestures.
     *
     * @see PutMediaPlayGestureAccess
     */
    bool GetMediaPlayGestureAccess() override;

    /**
     * @brief Put whether media playback needs to be triggered by user gestures,
     *        default value is false.
     */
    void PutMediaPlayGestureAccess(bool flag) override;

    /**
     * @brief Get the standard font family name.
     *
     * @see PutStandardFontFamilyName
     */
    ArkWebString StandardFontFamilyName() override;

    /**
     * @brief Put the standard font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    void PutStandardFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get the sans-serif font family name.
     *
     * @see PutSansSerifFontFamilyName
     */
    ArkWebString SansSerifFontFamilyName() override;

    /**
     * @brief Put the sans-serif font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    void PutSansSerifFontFamilyName(const ArkWebString& font) override;

    /**
     * @brief Get whether enable vertical scroll bar.
     *
     * @see PutVerticalScrollBarAccess
     */
    bool IsVerticalScrollBarAccess() override;

    /**
     * @brief Put whether enable vertical scroll bar, default value is false.
     */
    void PutVerticalScrollBarAccess(bool flag) override;

    /**
     * @brief Get whether enable horizontal scroll bar.
     *
     * @see PutHorizontalScrollBarAccess
     */
    bool IsHorizontalScrollBarAccess() override;

    /**
     * @brief Put whether enable horizontal scroll bar, default value is false.
     */
    void PutHorizontalScrollBarAccess(bool flag) override;

    /**
     * @brief Get the lower limit of the minimum logical font size.
     *
     * @see PutLogicalFontSizeLowerLimit
     */
    int LogicalFontSizeLowerLimit() override;

    /**
     * @brief Put the lower limit of the minimum logical font size. The default
     *        is 8.
     *
     * @param size A positive integer that ranges from 1 to 72. Any number outside
     *        the specified range will be pinned.
     */
    void PutLogicalFontSizeLowerLimit(int size) override;

    /**
     * @brief Get the default text encoding format that uses to decode html pages.
     *
     * @see PutDefaultTextEncodingFormat
     */
    ArkWebString DefaultTextEncodingFormat() override;

    /**
     * @brief Put the default text encoding format that uses to decode html pages.
     *        The default is "UTF-8".
     *
     * @param the text encoding format
     */
    void PutDefaultTextEncodingFormat(const ArkWebString& encoding) override;

    /**
     * @brief Get if the WebView from loading image resources from the network
     *        (http and https URI schemes) is supported.
     *
     * @see PutLoadImageFromNetworkDisabled
     */
    bool IsLoadImageFromNetworkDisabled() override;

    /**
     * @brief Put whether to block the WebView from loading image resources from
     *        the network (http and https URI schemes). This settings is invalid,
     *        if {@link #IsImageLoadingAllowed} returns false. The default is
     *        false.
     */
    void PutLoadImageFromNetworkDisabled(bool flag) override;

    /**
     * @brief Get if JavaScript running in a file scheme URL to access content
     *        from other file scheme URLs is supported.
     *
     * @see PutEnableRawFileAccessFromFileURLs
     */
    bool EnableRawFileAccessFromFileURLs() override;

    /**
     * @brief Put whether to allow JavaScript running in a file scheme URL to
     *        access content from other file scheme URLs. The default is false.
     */
    void PutEnableRawFileAccessFromFileURLs(bool flag) override;

    /**
     * @brief Get if JavaScript running in a file scheme URL to access content
     *        from any origin is supported. This includes access to content from
     *        other file scheme URLs.
     *
     * @see PutEnableUniversalAccessFromFileURLs
     */
    bool EnableUniversalAccessFromFileURLs() override;

    /**
     * @brief Put whether to allow JavaScript running in a file scheme URL to
     *        access content from any origin. This includes access to content from
     *        other file scheme URLs. See {@link
     *        #PutEnableRawFileAccessFromFileURLs}. The default is false.
     */
    void PutEnableUniversalAccessFromFileURLs(bool flag) override;

    /**
     * @brief Get if JavaScript can open windows.
     *
     * @see PutIsCreateWindowsByJavaScriptAllowed
     */
    bool IsCreateWindowsByJavaScriptAllowed() override;

    /**
     * @brief Put whether JavaScript can open windows by JavaScript. This applies
     *        to the JavaScript function {@code window.open()}. The default is
     *        false.
     */
    void PutIsCreateWindowsByJavaScriptAllowed(bool flag) override;

    /**
     * @brief Get the WebView's behavior when a secure origin attempts to load a
     *        resource from an insecure origin.
     *
     * @see PutAccessModeForSecureOriginLoadFromInsecure
     */
    int AccessModeForSecureOriginLoadFromInsecure() override;

    /**
     * @brief Put the WebView's behavior when a secure origin attempts to load a
     *        resource from an insecure origin. The default is NEVER_ALLOW.
     *
     * @param mode The mixed content mode to use.
     */
    void PutAccessModeForSecureOriginLoadFromInsecure(int mode) override;

    /**
     * @brief Get copy-option Mode.
     *
     * @see PutCopyOptionMode
     */
    int GetCopyOptionMode() override;

    /**
     * @brief Put copy-option.
     */
    void PutCopyOptionMode(int copyOption) override;

    /**
     * @brief Put whether the embed mode. The default is false.
     */
    void SetNativeEmbedMode(bool flag) override;

    /**
     * @brief Get whether the embed mode. The default is false.
     */
    bool GetNativeEmbedMode() override;

    /**
     * @brief Register native embed rule. The default pattern is ["embed",
     * "native/"].
     */
    void RegisterNativeEmbedRule(const ArkWebString& tag, const ArkWebString& type) override;

    /**
     * @brief Set whether the scroll is enabled.
     */
    void SetScrollable(bool enable) override;

    /**
     * @brief Get whether the scroll is enabled.
     */
    bool GetScrollable() override;

    /**
     * @brief Put whether enable horizontal scroll bar, default value is false.
     */
    /*--ark web()--*/
    virtual void PutTextAutosizingEnabled(bool flag) override;

    /**
     * @brief Set whether to support the viewport attribute of the meta tag in the
     * frontend page.
     */
    void SetViewportEnable(bool enable) override;

    /**
     * @brief Put whether enable horizontal scroll bar, default value is false.
     */
    void SetNativeVideoPlayerConfig(bool enable, bool shouldOverlay) override;

    /**
     * @brief Get surfaceId.
     */
    /*--ark web()--*/
    ArkWebString GetSurfaceId() override;

    /**
     * @brief Set surfaceId.
     */
    /*--ark web()--*/
    void SetSurfaceId(const ArkWebString& surfaceId) override;

    /**
     * Set whether the scrollbar is displayed.
     */
    void PutOverlayScrollbarEnabled(bool enable) override;

    /**
     * @brief Set whether the scroll is enabled.
     */
    void SetScrollable(bool enable, int32_t scrollType) override;

    /**
     * @brief Set whether to support the blur when the keyboard is hidden by manual.
     */
    void SetBlurOnKeyboardHideMode(int enable) override;

    /**
     * Set whether to connect to media avsession.
     */
    void PutWebMediaAVSessionEnabled(bool enable) override;

    /**
     * @brief Set the scale of the font-weight.The default is 1.0f.
     */
    void SetFontWeightScale(float scale) override;

    /**
     * @brief Set whether to use the intrinsic size. The default is false.
     */
    void SetIntrinsicSizeEnable(bool enable) override;

    /**
     * @brief Set whether to support css diplay when reporting the visibility of native embed.
     *        The default is false.
     */
    void SetCssDisplayChangeEnabled(bool enable) override;

    /**
     * @brief Set whether to bypass vsync.
     *        The default is none.
     */
    void SetBypassVsyncCondition(int32_t condition) override;

    /**
     * @brief Set the gesture focus mode.
     */
    void SetGestureFocusMode(int32_t mode) override;

    /**
     * @brief Sets whether the Web supports zooming using Ctrl.
     */
    void PutZoomControlAccess(bool zoomControlAccess) override;

    /**
     * @brief Set whether to enable image analyzer.
     */
    void PutImageAnalyzerEnabled(bool enabled) override;

    /**
     * @brief Get whether to enable image analyzer.
     */
    bool GetImageAnalyzerEnabled() override;

    /**
     * @brief Sets whether enable auto fill or not.
     *
     * @param enable bool: Indicates the flag whether autofill is enabled.
     */
    void SetEnableAutoFill(bool enable) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebPreference> nweb_preference_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PREFERENCE_IMPL_H_
