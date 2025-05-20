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

#include "ohos_nweb/bridge/ark_web_preference_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebPreferenceWrapper::ArkWebPreferenceWrapper(ArkWebRefPtr<ArkWebPreference> ark_web_preference)
    : ark_web_preference_(ark_web_preference)
{}

std::string ArkWebPreferenceWrapper::UserAgent()
{
    ArkWebString stUserAgent = ark_web_preference_->UserAgent();

    std::string objUserAgent = ArkWebStringStructToClass(stUserAgent);
    ArkWebStringStructRelease(stUserAgent);
    return objUserAgent;
}

void ArkWebPreferenceWrapper::PutUserAgent(const std::string& ua)
{
    ArkWebString stUserAgent = ArkWebStringClassToStruct(ua);

    ark_web_preference_->PutUserAgent(stUserAgent);

    ArkWebStringStructRelease(stUserAgent);
}

std::string ArkWebPreferenceWrapper::DefaultUserAgent()
{
    ArkWebString stUserAgent = ark_web_preference_->DefaultUserAgent();

    std::string objUserAgent = ArkWebStringStructToClass(stUserAgent);
    ArkWebStringStructRelease(stUserAgent);
    return objUserAgent;
}

ArkWebCacheModeFlag ArkWebPreferenceWrapper::CacheMode()
{
    return static_cast<ArkWebCacheModeFlag>(ark_web_preference_->CacheMode());
}

void ArkWebPreferenceWrapper::PutCacheMode(ArkWebCacheModeFlag flag)
{
    ark_web_preference_->PutCacheMode(static_cast<int>(flag));
}

bool ArkWebPreferenceWrapper::IsNetworkBlocked()
{
    return ark_web_preference_->IsNetworkBlocked();
}

void ArkWebPreferenceWrapper::PutBlockNetwork(bool flag)
{
    ark_web_preference_->PutBlockNetwork(flag);
}

uint32_t ArkWebPreferenceWrapper::GetScrollBarColor()
{
    return ark_web_preference_->GetScrollBarColor();
}

void ArkWebPreferenceWrapper::PutScrollBarColor(uint32_t color_value)
{
    ark_web_preference_->PutScrollBarColor(color_value);
}

int ArkWebPreferenceWrapper::GetOverscrollMode()
{
    return ark_web_preference_->GetOverscrollMode();
}

void ArkWebPreferenceWrapper::PutOverscrollMode(int over_scroll_mode)
{
    ark_web_preference_->PutOverscrollMode(over_scroll_mode);
}

int ArkWebPreferenceWrapper::DefaultFontSize()
{
    return ark_web_preference_->DefaultFontSize();
}

void ArkWebPreferenceWrapper::PutDefaultFontSize(int size)
{
    ark_web_preference_->PutDefaultFontSize(size);
}

bool ArkWebPreferenceWrapper::GetPinchSmoothMode()
{
    return ark_web_preference_->GetPinchSmoothMode();
}

void ArkWebPreferenceWrapper::PutPinchSmoothMode(bool flag)
{
    ark_web_preference_->PutPinchSmoothMode(flag);
}

bool ArkWebPreferenceWrapper::IsDataBaseEnabled()
{
    return ark_web_preference_->IsDataBaseEnabled();
}

void ArkWebPreferenceWrapper::PutDatabaseAllowed(bool flag)
{
    ark_web_preference_->PutDatabaseAllowed(flag);
}

bool ArkWebPreferenceWrapper::IsMultiWindowAccess()
{
    return ark_web_preference_->IsMultiWindowAccess();
}

void ArkWebPreferenceWrapper::PutMultiWindowAccess(bool flag)
{
    ark_web_preference_->PutMultiWindowAccess(flag);
}

bool ArkWebPreferenceWrapper::IsJavaScriptAllowed()
{
    return ark_web_preference_->IsJavaScriptAllowed();
}

void ArkWebPreferenceWrapper::PutJavaScriptEnabled(bool flag)
{
    ark_web_preference_->PutJavaScriptEnabled(flag);
}

int ArkWebPreferenceWrapper::DarkSchemeEnabled()
{
    return ark_web_preference_->DarkSchemeEnabled();
}

void ArkWebPreferenceWrapper::PutDarkSchemeEnabled(int dark_scheme)
{
    ark_web_preference_->PutDarkSchemeEnabled(dark_scheme);
}

bool ArkWebPreferenceWrapper::IsDomStorageEnabled()
{
    return ark_web_preference_->IsDomStorageEnabled();
}

void ArkWebPreferenceWrapper::PutDomStorageEnabled(bool flag)
{
    ark_web_preference_->PutDomStorageEnabled(flag);
}

int ArkWebPreferenceWrapper::FontSizeLowerLimit()
{
    return ark_web_preference_->FontSizeLowerLimit();
}

void ArkWebPreferenceWrapper::PutFontSizeLowerLimit(int size)
{
    ark_web_preference_->PutFontSizeLowerLimit(size);
}

bool ArkWebPreferenceWrapper::GeolocationAllowed()
{
    return ark_web_preference_->GeoLocationAllowed();
}

void ArkWebPreferenceWrapper::PutGeolocationAllowed(bool flag)
{
    ark_web_preference_->PutGeoLocationAllowed(flag);
}

bool ArkWebPreferenceWrapper::IsWebDebuggingAccess()
{
    return ark_web_preference_->IsWebDebuggingAccess();
}

void ArkWebPreferenceWrapper::PutWebDebuggingAccess(bool flag)
{
    ark_web_preference_->PutWebDebuggingAccess(flag);
}

bool ArkWebPreferenceWrapper::EnableContentAccess()
{
    return ark_web_preference_->EnableContentAccess();
}

void ArkWebPreferenceWrapper::PutEnableContentAccess(bool flag)
{
    ark_web_preference_->PutEnableContentAccess(flag);
}

bool ArkWebPreferenceWrapper::EnableRawFileAccess()
{
    return ark_web_preference_->EnableRawFileAccess();
}

void ArkWebPreferenceWrapper::PutEnableRawFileAccess(bool flag)
{
    ark_web_preference_->PutEnableRawFileAccess(flag);
}

bool ArkWebPreferenceWrapper::IsImageLoadingAllowed()
{
    return ark_web_preference_->IsImageLoadingAllowed();
}

void ArkWebPreferenceWrapper::PutImageLoadingAllowed(bool flag)
{
    ark_web_preference_->PutImageLoadingAllowed(flag);
}

std::string ArkWebPreferenceWrapper::FixedFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->FixedFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutFixedFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutFixedFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

std::string ArkWebPreferenceWrapper::SerifFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->SerifFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutSerifFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutSerifFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

int ArkWebPreferenceWrapper::ZoomingForTextFactor()
{
    return ark_web_preference_->ZoomingForTextFactor();
}

void ArkWebPreferenceWrapper::PutZoomingForTextFactor(int text_zoom)
{
    ark_web_preference_->PutZoomingForTextFactor(text_zoom);
}

int ArkWebPreferenceWrapper::ForceDarkModeEnabled()
{
    return ark_web_preference_->ForceDarkModeEnabled();
}

void ArkWebPreferenceWrapper::PutForceDarkModeEnabled(int force_dark)
{
    ark_web_preference_->PutForceDarkModeEnabled(force_dark);
}

bool ArkWebPreferenceWrapper::IsLoadWithOverviewMode()
{
    return ark_web_preference_->IsLoadWithOverviewMode();
}

void ArkWebPreferenceWrapper::PutLoadWithOverviewMode(bool flag)
{
    ark_web_preference_->PutLoadWithOverviewMode(flag);
}

int ArkWebPreferenceWrapper::DefaultFixedFontSize()
{
    return ark_web_preference_->DefaultFixedFontSize();
}

void ArkWebPreferenceWrapper::PutDefaultFixedFontSize(int size)
{
    ark_web_preference_->PutDefaultFixedFontSize(size);
}

std::string ArkWebPreferenceWrapper::CursiveFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->CursiveFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutCursiveFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutCursiveFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

std::string ArkWebPreferenceWrapper::FantasyFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->FantasyFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutFantasyFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutFantasyFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

bool ArkWebPreferenceWrapper::ZoomingfunctionEnabled()
{
    return ark_web_preference_->ZoomingfunctionEnabled();
}

void ArkWebPreferenceWrapper::PutZoomingFunctionEnabled(bool flag)
{
    ark_web_preference_->PutZoomingFunctionEnabled(flag);
}

bool ArkWebPreferenceWrapper::GetMediaPlayGestureAccess()
{
    return ark_web_preference_->GetMediaPlayGestureAccess();
}

void ArkWebPreferenceWrapper::PutMediaPlayGestureAccess(bool flag)
{
    ark_web_preference_->PutMediaPlayGestureAccess(flag);
}

std::string ArkWebPreferenceWrapper::StandardFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->StandardFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutStandardFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutStandardFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

std::string ArkWebPreferenceWrapper::SansSerifFontFamilyName()
{
    ArkWebString stName = ark_web_preference_->SansSerifFontFamilyName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

void ArkWebPreferenceWrapper::PutSansSerifFontFamilyName(const std::string& font)
{
    ArkWebString stName = ArkWebStringClassToStruct(font);

    ark_web_preference_->PutSansSerifFontFamilyName(stName);

    ArkWebStringStructRelease(stName);
}

bool ArkWebPreferenceWrapper::IsVerticalScrollBarAccess()
{
    return ark_web_preference_->IsVerticalScrollBarAccess();
}

void ArkWebPreferenceWrapper::PutVerticalScrollBarAccess(bool flag)
{
    ark_web_preference_->PutVerticalScrollBarAccess(flag);
}

bool ArkWebPreferenceWrapper::IsHorizontalScrollBarAccess()
{
    return ark_web_preference_->IsHorizontalScrollBarAccess();
}

void ArkWebPreferenceWrapper::PutHorizontalScrollBarAccess(bool flag)
{
    ark_web_preference_->PutHorizontalScrollBarAccess(flag);
}

int ArkWebPreferenceWrapper::LogicalFontSizeLowerLimit()
{
    return ark_web_preference_->LogicalFontSizeLowerLimit();
}

void ArkWebPreferenceWrapper::PutLogicalFontSizeLowerLimit(int size)
{
    ark_web_preference_->PutLogicalFontSizeLowerLimit(size);
}

std::string ArkWebPreferenceWrapper::DefaultTextEncodingFormat()
{
    ArkWebString stFormat = ark_web_preference_->DefaultTextEncodingFormat();

    std::string objFormat = ArkWebStringStructToClass(stFormat);
    ArkWebStringStructRelease(stFormat);
    return objFormat;
}

void ArkWebPreferenceWrapper::PutDefaultTextEncodingFormat(const std::string& encoding)
{
    ArkWebString stFormat = ArkWebStringClassToStruct(encoding);

    ark_web_preference_->PutDefaultTextEncodingFormat(stFormat);

    ArkWebStringStructRelease(stFormat);
}

bool ArkWebPreferenceWrapper::IsLoadImageFromNetworkDisabled()
{
    return ark_web_preference_->IsLoadImageFromNetworkDisabled();
}

void ArkWebPreferenceWrapper::PutLoadImageFromNetworkDisabled(bool flag)
{
    ark_web_preference_->PutLoadImageFromNetworkDisabled(flag);
}

bool ArkWebPreferenceWrapper::EnableRawFileAccessFromFileURLs()
{
    return ark_web_preference_->EnableRawFileAccessFromFileURLs();
}

void ArkWebPreferenceWrapper::PutEnableRawFileAccessFromFileURLs(bool flag)
{
    ark_web_preference_->PutEnableRawFileAccessFromFileURLs(flag);
}

bool ArkWebPreferenceWrapper::EnableUniversalAccessFromFileURLs()
{
    return ark_web_preference_->EnableUniversalAccessFromFileURLs();
}

void ArkWebPreferenceWrapper::PutEnableUniversalAccessFromFileURLs(bool flag)
{
    ark_web_preference_->PutEnableUniversalAccessFromFileURLs(flag);
}

bool ArkWebPreferenceWrapper::IsCreateWindowsByJavaScriptAllowed()
{
    return ark_web_preference_->IsCreateWindowsByJavaScriptAllowed();
}

void ArkWebPreferenceWrapper::PutIsCreateWindowsByJavaScriptAllowed(bool flag)
{
    ark_web_preference_->PutIsCreateWindowsByJavaScriptAllowed(flag);
}

ArkWebAccessMode ArkWebPreferenceWrapper::AccessModeForSecureOriginLoadFromInsecure()
{
    return static_cast<ArkWebAccessMode>(ark_web_preference_->AccessModeForSecureOriginLoadFromInsecure());
}

void ArkWebPreferenceWrapper::PutAccessModeForSecureOriginLoadFromInsecure(ArkWebAccessMode mode)
{
    ark_web_preference_->PutAccessModeForSecureOriginLoadFromInsecure(static_cast<int>(mode));
}

ArkWebCopyOptionMode ArkWebPreferenceWrapper::GetCopyOptionMode()
{
    return static_cast<ArkWebCopyOptionMode>(ark_web_preference_->GetCopyOptionMode());
}

void ArkWebPreferenceWrapper::PutCopyOptionMode(ArkWebCopyOptionMode copyOption)
{
    ark_web_preference_->PutCopyOptionMode(static_cast<int>(copyOption));
}

void ArkWebPreferenceWrapper::SetNativeEmbedMode(bool flag)
{
    ark_web_preference_->SetNativeEmbedMode(flag);
}

bool ArkWebPreferenceWrapper::GetNativeEmbedMode()
{
    return ark_web_preference_->GetNativeEmbedMode();
}

void ArkWebPreferenceWrapper::RegisterNativeEmbedRule(const std::string& tag, const std::string& type)
{
    ark_web_preference_->RegisterNativeEmbedRule(ArkWebStringClassToStruct(tag), ArkWebStringClassToStruct(type));
}

void ArkWebPreferenceWrapper::SetScrollable(bool enable)
{
    ark_web_preference_->SetScrollable(enable);
}

bool ArkWebPreferenceWrapper::GetScrollable()
{
    return ark_web_preference_->GetScrollable();
}

void ArkWebPreferenceWrapper::PutTextAutosizingEnabled(bool flag)
{
    if (ark_web_preference_)
        ark_web_preference_->PutTextAutosizingEnabled(flag);
}

void ArkWebPreferenceWrapper::SetViewportEnable(bool enable)
{
    if (ark_web_preference_) {
        ark_web_preference_->SetViewportEnable(enable);
    }
}

void ArkWebPreferenceWrapper::SetNativeVideoPlayerConfig(bool enable, bool shouldOverlay)
{
    ark_web_preference_->SetNativeVideoPlayerConfig(enable, shouldOverlay);
}

std::string ArkWebPreferenceWrapper::GetSurfaceId()
{
    ArkWebString stSurfaceId = ark_web_preference_->GetSurfaceId();

    std::string surfaceId = ArkWebStringStructToClass(stSurfaceId);
    ArkWebStringStructRelease(stSurfaceId);
    return surfaceId;
}

void ArkWebPreferenceWrapper::SetSurfaceId(const std::string& surfaceId)
{
    ArkWebString stSurfaceId = ArkWebStringClassToStruct(surfaceId);

    ark_web_preference_->SetSurfaceId(stSurfaceId);

    ArkWebStringStructRelease(stSurfaceId);
}

void ArkWebPreferenceWrapper::PutOverlayScrollbarEnabled(bool enable)
{
    ark_web_preference_->PutOverlayScrollbarEnabled(enable);
}

void ArkWebPreferenceWrapper::SetScrollable(bool enable, int32_t scrollType)
{
    ark_web_preference_->SetScrollable(enable, scrollType);
}

void ArkWebPreferenceWrapper::SetBlurOnKeyboardHideMode(int enable)
{
    if (ark_web_preference_) {
        ark_web_preference_->SetBlurOnKeyboardHideMode(enable);
    }
}

void ArkWebPreferenceWrapper::PutWebMediaAVSessionEnabled(bool enable)
{
    ark_web_preference_->PutWebMediaAVSessionEnabled(enable);
}

void ArkWebPreferenceWrapper::SetFontWeightScale(float scale)
{
    if (ark_web_preference_) {
        ark_web_preference_->SetFontWeightScale(scale);
    }
}
void ArkWebPreferenceWrapper::SetIntrinsicSizeEnable(bool enable)
{
    if (ark_web_preference_) {
        ark_web_preference_->SetIntrinsicSizeEnable(enable);
    }
}

void ArkWebPreferenceWrapper::SetCssDisplayChangeEnabled(bool enable)
{
    if (ark_web_preference_) {
        ark_web_preference_->SetCssDisplayChangeEnabled(enable);
    }
}
} // namespace OHOS::ArkWeb
