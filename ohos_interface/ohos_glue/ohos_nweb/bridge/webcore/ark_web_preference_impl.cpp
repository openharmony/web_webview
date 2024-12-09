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

#include "ohos_nweb/bridge/ark_web_preference_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebAccessMode = OHOS::NWeb::NWebPreference::AccessMode;
using ArkWebCacheModeFlag = OHOS::NWeb::NWebPreference::CacheModeFlag;
using ArkWebCopyOptionMode = OHOS::NWeb::NWebPreference::CopyOptionMode;

ArkWebPreferenceImpl::ArkWebPreferenceImpl(std::shared_ptr<OHOS::NWeb::NWebPreference> nweb_preference)
    : nweb_preference_(nweb_preference)
{}

ArkWebString ArkWebPreferenceImpl::UserAgent()
{
    return ArkWebStringClassToStruct(nweb_preference_->UserAgent());
}

void ArkWebPreferenceImpl::PutUserAgent(const ArkWebString& ua)
{
    nweb_preference_->PutUserAgent(ArkWebStringStructToClass(ua));
}

ArkWebString ArkWebPreferenceImpl::DefaultUserAgent()
{
    return ArkWebStringClassToStruct(nweb_preference_->DefaultUserAgent());
}

int ArkWebPreferenceImpl::CacheMode()
{
    return static_cast<int>(nweb_preference_->CacheMode());
}

void ArkWebPreferenceImpl::PutCacheMode(int flag)
{
    nweb_preference_->PutCacheMode(static_cast<ArkWebCacheModeFlag>(flag));
}

bool ArkWebPreferenceImpl::IsNetworkBlocked()
{
    return nweb_preference_->IsNetworkBlocked();
}

void ArkWebPreferenceImpl::PutBlockNetwork(bool flag)
{
    nweb_preference_->PutBlockNetwork(flag);
}

uint32_t ArkWebPreferenceImpl::GetScrollBarColor()
{
    return nweb_preference_->GetScrollBarColor();
}

void ArkWebPreferenceImpl::PutScrollBarColor(uint32_t color_value)
{
    nweb_preference_->PutScrollBarColor(color_value);
}

int ArkWebPreferenceImpl::GetOverscrollMode()
{
    return nweb_preference_->GetOverscrollMode();
}

void ArkWebPreferenceImpl::PutOverscrollMode(int over_scroll_mode)
{
    nweb_preference_->PutOverscrollMode(over_scroll_mode);
}

int ArkWebPreferenceImpl::DefaultFontSize()
{
    return nweb_preference_->DefaultFontSize();
}

void ArkWebPreferenceImpl::PutDefaultFontSize(int size)
{
    nweb_preference_->PutDefaultFontSize(size);
}

bool ArkWebPreferenceImpl::GetPinchSmoothMode()
{
    return nweb_preference_->GetPinchSmoothMode();
}

void ArkWebPreferenceImpl::PutPinchSmoothMode(bool flag)
{
    nweb_preference_->PutPinchSmoothMode(flag);
}

bool ArkWebPreferenceImpl::IsDataBaseEnabled()
{
    return nweb_preference_->IsDataBaseEnabled();
}

void ArkWebPreferenceImpl::PutDatabaseAllowed(bool flag)
{
    nweb_preference_->PutDatabaseAllowed(flag);
}

bool ArkWebPreferenceImpl::IsMultiWindowAccess()
{
    return nweb_preference_->IsMultiWindowAccess();
}

void ArkWebPreferenceImpl::PutMultiWindowAccess(bool flag)
{
    nweb_preference_->PutMultiWindowAccess(flag);
}

bool ArkWebPreferenceImpl::IsJavaScriptAllowed()
{
    return nweb_preference_->IsJavaScriptAllowed();
}

void ArkWebPreferenceImpl::PutJavaScriptEnabled(bool flag)
{
    nweb_preference_->PutJavaScriptEnabled(flag);
}

int ArkWebPreferenceImpl::DarkSchemeEnabled()
{
    return nweb_preference_->DarkSchemeEnabled();
}

void ArkWebPreferenceImpl::PutDarkSchemeEnabled(int dark_scheme)
{
    nweb_preference_->PutDarkSchemeEnabled(dark_scheme);
}

bool ArkWebPreferenceImpl::IsDomStorageEnabled()
{
    return nweb_preference_->IsDomStorageEnabled();
}

void ArkWebPreferenceImpl::PutDomStorageEnabled(bool flag)
{
    nweb_preference_->PutDomStorageEnabled(flag);
}

int ArkWebPreferenceImpl::FontSizeLowerLimit()
{
    return nweb_preference_->FontSizeLowerLimit();
}

void ArkWebPreferenceImpl::PutFontSizeLowerLimit(int size)
{
    nweb_preference_->PutFontSizeLowerLimit(size);
}

bool ArkWebPreferenceImpl::GeoLocationAllowed()
{
    return nweb_preference_->GeolocationAllowed();
}

void ArkWebPreferenceImpl::PutGeoLocationAllowed(bool flag)
{
    nweb_preference_->PutGeolocationAllowed(flag);
}

bool ArkWebPreferenceImpl::IsWebDebuggingAccess()
{
    return nweb_preference_->IsWebDebuggingAccess();
}

void ArkWebPreferenceImpl::PutWebDebuggingAccess(bool flag)
{
    nweb_preference_->PutWebDebuggingAccess(flag);
}

bool ArkWebPreferenceImpl::EnableContentAccess()
{
    return nweb_preference_->EnableContentAccess();
}

void ArkWebPreferenceImpl::PutEnableContentAccess(bool flag)
{
    nweb_preference_->PutEnableContentAccess(flag);
}

bool ArkWebPreferenceImpl::EnableRawFileAccess()
{
    return nweb_preference_->EnableRawFileAccess();
}

void ArkWebPreferenceImpl::PutEnableRawFileAccess(bool flag)
{
    nweb_preference_->PutEnableRawFileAccess(flag);
}

bool ArkWebPreferenceImpl::IsImageLoadingAllowed()
{
    return nweb_preference_->IsImageLoadingAllowed();
}

void ArkWebPreferenceImpl::PutImageLoadingAllowed(bool flag)
{
    nweb_preference_->PutImageLoadingAllowed(flag);
}

ArkWebString ArkWebPreferenceImpl::FixedFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->FixedFontFamilyName());
}

void ArkWebPreferenceImpl::PutFixedFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutFixedFontFamilyName(ArkWebStringStructToClass(font));
}

ArkWebString ArkWebPreferenceImpl::SerifFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->SerifFontFamilyName());
}

void ArkWebPreferenceImpl::PutSerifFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutSerifFontFamilyName(ArkWebStringStructToClass(font));
}

int ArkWebPreferenceImpl::ZoomingForTextFactor()
{
    return nweb_preference_->ZoomingForTextFactor();
}

void ArkWebPreferenceImpl::PutZoomingForTextFactor(int text_zoom)
{
    nweb_preference_->PutZoomingForTextFactor(text_zoom);
}

int ArkWebPreferenceImpl::ForceDarkModeEnabled()
{
    return nweb_preference_->ForceDarkModeEnabled();
}

void ArkWebPreferenceImpl::PutForceDarkModeEnabled(int force_dark)
{
    nweb_preference_->PutForceDarkModeEnabled(force_dark);
}

bool ArkWebPreferenceImpl::IsLoadWithOverviewMode()
{
    return nweb_preference_->IsLoadWithOverviewMode();
}

void ArkWebPreferenceImpl::PutLoadWithOverviewMode(bool flag)
{
    nweb_preference_->PutLoadWithOverviewMode(flag);
}

int ArkWebPreferenceImpl::DefaultFixedFontSize()
{
    return nweb_preference_->DefaultFixedFontSize();
}

void ArkWebPreferenceImpl::PutDefaultFixedFontSize(int size)
{
    nweb_preference_->PutDefaultFixedFontSize(size);
}

ArkWebString ArkWebPreferenceImpl::CursiveFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->CursiveFontFamilyName());
}

void ArkWebPreferenceImpl::PutCursiveFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutCursiveFontFamilyName(ArkWebStringStructToClass(font));
}

ArkWebString ArkWebPreferenceImpl::FantasyFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->FantasyFontFamilyName());
}

void ArkWebPreferenceImpl::PutFantasyFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutFantasyFontFamilyName(ArkWebStringStructToClass(font));
}

bool ArkWebPreferenceImpl::ZoomingfunctionEnabled()
{
    return nweb_preference_->ZoomingfunctionEnabled();
}

void ArkWebPreferenceImpl::PutZoomingFunctionEnabled(bool flag)
{
    nweb_preference_->PutZoomingFunctionEnabled(flag);
}

bool ArkWebPreferenceImpl::GetMediaPlayGestureAccess()
{
    return nweb_preference_->GetMediaPlayGestureAccess();
}

void ArkWebPreferenceImpl::PutMediaPlayGestureAccess(bool flag)
{
    nweb_preference_->PutMediaPlayGestureAccess(flag);
}

ArkWebString ArkWebPreferenceImpl::StandardFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->StandardFontFamilyName());
}

void ArkWebPreferenceImpl::PutStandardFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutStandardFontFamilyName(ArkWebStringStructToClass(font));
}

ArkWebString ArkWebPreferenceImpl::SansSerifFontFamilyName()
{
    return ArkWebStringClassToStruct(nweb_preference_->SansSerifFontFamilyName());
}

void ArkWebPreferenceImpl::PutSansSerifFontFamilyName(const ArkWebString& font)
{
    nweb_preference_->PutSansSerifFontFamilyName(ArkWebStringStructToClass(font));
}

bool ArkWebPreferenceImpl::IsVerticalScrollBarAccess()
{
    return nweb_preference_->IsVerticalScrollBarAccess();
}

void ArkWebPreferenceImpl::PutVerticalScrollBarAccess(bool flag)
{
    nweb_preference_->PutVerticalScrollBarAccess(flag);
}

bool ArkWebPreferenceImpl::IsHorizontalScrollBarAccess()
{
    return nweb_preference_->IsHorizontalScrollBarAccess();
}

void ArkWebPreferenceImpl::PutHorizontalScrollBarAccess(bool flag)
{
    nweb_preference_->PutHorizontalScrollBarAccess(flag);
}

int ArkWebPreferenceImpl::LogicalFontSizeLowerLimit()
{
    return nweb_preference_->LogicalFontSizeLowerLimit();
}

void ArkWebPreferenceImpl::PutLogicalFontSizeLowerLimit(int size)
{
    nweb_preference_->PutLogicalFontSizeLowerLimit(size);
}

ArkWebString ArkWebPreferenceImpl::DefaultTextEncodingFormat()
{
    return ArkWebStringClassToStruct(nweb_preference_->DefaultTextEncodingFormat());
}

void ArkWebPreferenceImpl::PutDefaultTextEncodingFormat(const ArkWebString& encoding)
{
    nweb_preference_->PutDefaultTextEncodingFormat(ArkWebStringStructToClass(encoding));
}

bool ArkWebPreferenceImpl::IsLoadImageFromNetworkDisabled()
{
    return nweb_preference_->IsLoadImageFromNetworkDisabled();
}

void ArkWebPreferenceImpl::PutLoadImageFromNetworkDisabled(bool flag)
{
    nweb_preference_->PutLoadImageFromNetworkDisabled(flag);
}

bool ArkWebPreferenceImpl::EnableRawFileAccessFromFileURLs()
{
    return nweb_preference_->EnableRawFileAccessFromFileURLs();
}

void ArkWebPreferenceImpl::PutEnableRawFileAccessFromFileURLs(bool flag)
{
    nweb_preference_->PutEnableRawFileAccessFromFileURLs(flag);
}

bool ArkWebPreferenceImpl::EnableUniversalAccessFromFileURLs()
{
    return nweb_preference_->EnableUniversalAccessFromFileURLs();
}

void ArkWebPreferenceImpl::PutEnableUniversalAccessFromFileURLs(bool flag)
{
    nweb_preference_->PutEnableUniversalAccessFromFileURLs(flag);
}

bool ArkWebPreferenceImpl::IsCreateWindowsByJavaScriptAllowed()
{
    return nweb_preference_->IsCreateWindowsByJavaScriptAllowed();
}

void ArkWebPreferenceImpl::PutIsCreateWindowsByJavaScriptAllowed(bool flag)
{
    nweb_preference_->PutIsCreateWindowsByJavaScriptAllowed(flag);
}

int ArkWebPreferenceImpl::AccessModeForSecureOriginLoadFromInsecure()
{
    return static_cast<int>(nweb_preference_->AccessModeForSecureOriginLoadFromInsecure());
}

void ArkWebPreferenceImpl::PutAccessModeForSecureOriginLoadFromInsecure(int mode)
{
    nweb_preference_->PutAccessModeForSecureOriginLoadFromInsecure(static_cast<ArkWebAccessMode>(mode));
}

int ArkWebPreferenceImpl::GetCopyOptionMode()
{
    return static_cast<int>(nweb_preference_->GetCopyOptionMode());
}

void ArkWebPreferenceImpl::PutCopyOptionMode(int copyOption)
{
    nweb_preference_->PutCopyOptionMode(static_cast<ArkWebCopyOptionMode>(copyOption));
}

void ArkWebPreferenceImpl::SetNativeEmbedMode(bool flag)
{
    nweb_preference_->SetNativeEmbedMode(flag);
}

bool ArkWebPreferenceImpl::GetNativeEmbedMode()
{
    return nweb_preference_->GetNativeEmbedMode();
}

void ArkWebPreferenceImpl::RegisterNativeEmbedRule(const ArkWebString& tag, const ArkWebString& type)
{
    nweb_preference_->RegisterNativeEmbedRule(ArkWebStringStructToClass(tag), ArkWebStringStructToClass(type));
}

void ArkWebPreferenceImpl::SetScrollable(bool enable)
{
    nweb_preference_->SetScrollable(enable);
}

bool ArkWebPreferenceImpl::GetScrollable()
{
    return nweb_preference_->GetScrollable();
}

void ArkWebPreferenceImpl::PutTextAutosizingEnabled(bool flag)
{
    nweb_preference_->PutTextAutosizingEnabled(flag);
}

void ArkWebPreferenceImpl::SetViewportEnable(bool enable)
{
    nweb_preference_->SetViewportEnable(enable);
}

void ArkWebPreferenceImpl::SetNativeVideoPlayerConfig(bool enable, bool shouldOverlay)
{
    nweb_preference_->SetNativeVideoPlayerConfig(enable, shouldOverlay);
}

ArkWebString ArkWebPreferenceImpl::GetSurfaceId()
{
    std::string surfaceId = nweb_preference_->GetSurfaceId();
    return ArkWebStringClassToStruct(surfaceId);
}

void ArkWebPreferenceImpl::SetSurfaceId(const ArkWebString& surfaceId)
{
    nweb_preference_->SetSurfaceId(ArkWebStringStructToClass(surfaceId));
}

void ArkWebPreferenceImpl::PutOverlayScrollbarEnabled(bool enable)
{
    nweb_preference_->PutOverlayScrollbarEnabled(enable);
}

void ArkWebPreferenceImpl::SetScrollable(bool enable, int32_t scrollType)
{
    nweb_preference_->SetScrollable(enable, scrollType);
}

void ArkWebPreferenceImpl::SetBlurOnKeyboardHideMode(int enable)
{
    nweb_preference_->SetBlurOnKeyboardHideMode(enable);
}

} // namespace OHOS::ArkWeb
