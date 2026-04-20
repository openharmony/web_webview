/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef WEBVIEW_FFI_H
#define WEBVIEW_FFI_H

#include <cstdint>

#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "webview_utils.h"

extern "C" {
FFI_EXPORT int32_t FfiOHOSWebviewOnce(char* type, void (*callbackRef)(void));

    // WebMessagePortImpl
FFI_EXPORT int64_t FfiOHOSWebviewCtlConstructor();
FFI_EXPORT int64_t FfiOHOSWebviewCtlConstructorWithWebTag(char* webTag);
FFI_EXPORT void FfiOHOSWebviewCtlInitializeWebEngine();
FFI_EXPORT void FfiOHOSWebviewCtlSetHttpDns(int32_t secureDnsMode, char* secureDnsConfig);
FFI_EXPORT void FfiOHOSWebviewCtlSetWebDebuggingAccess(bool webDebuggingAccess);
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char* url);
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadUrlWithHeaders(int64_t id, char* url, OHOS::Webview::ArrWebHeader headers);
FFI_EXPORT int32_t FfiOHOSWebviewCtlLoadData(int64_t id, OHOS::Webview::LoadDatas loadDatas);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRefresh(int64_t id);
FFI_EXPORT int32_t FFiOHOSWebviewCtlPreFetchPage(int64_t id, char* url);
FFI_EXPORT int32_t FFiOHOSWebviewCtlPreFetchPageWithHeaders(
    int64_t id, char* url, OHOS::Webview::ArrWebHeader headers);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetAudioMuted(int64_t id, bool mute);
FFI_EXPORT char* FfiOHOSWebviewCtlGetUserAgent(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetConnectionTimeout(int32_t timeout);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetWebId(int64_t id, int32_t* errCode);
FFI_EXPORT bool FfiOHOSWebviewCtlAccessForward(int64_t id, int32_t* errCode);
FFI_EXPORT bool FfiOHOSWebviewCtlAccessBackward(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetCustomUserAgent(int64_t id, char* cUserAgent);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetCustomUserAgent(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRunJavaScript(
    int64_t id, char* cScript, void (*callbackRef)(RetDataCString infoRef));
FFI_EXPORT int32_t FfiOHOSWebviewCtlRunJavaScriptExt(int64_t id, char* cScript, void (*callbackRef)(RetDataI64));
FFI_EXPORT int32_t FfiOHOSWebviewCtlRunJavaScriptExtArr(
    int64_t id, CArrUI8 cScript, void (*callbackRef)(RetDataI64));
FFI_EXPORT int32_t FfiOHOSWebviewCtlRegisterJavaScriptProxy(
    int64_t id, CArrI64 cFuncIds, const char* cName, CArrString cMethodList);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRegisterJavaScriptProxyEx(
    int64_t id, CArrI64 cFuncIds, const char* cName, CArrString cMethodList, char* cPermission);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetOriginalUrl(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPageUp(int64_t id, bool top);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPageDown(int64_t id, bool bottom);
FFI_EXPORT int32_t FfiOHOSWebviewCtlScrollTo(int64_t id, float x, float y);
FFI_EXPORT int32_t FfiOHOSWebviewCtlScrollBy(int64_t id, float deltaX, float deltaY);
FFI_EXPORT int32_t FfiOHOSWebviewCtlScrollToWithAnime(int64_t id, float x, float y, int32_t duration);
FFI_EXPORT int32_t FfiOHOSWebviewCtlScrollByWithAnime(int64_t id, float deltaX, float deltaY, int32_t duration);
FFI_EXPORT int32_t FfiOHOSWebviewCtlForward(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlBackward(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlBackOrForward(int64_t id, int32_t step);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetProgress(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetPageHeight(int64_t id, int32_t* errCode);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetTitle(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlZoom(int64_t id, float factor);
FFI_EXPORT int32_t FfiOHOSWebviewCtlZoomIn(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlZoomOut(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRequestFocus(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearHistory(int64_t id);
FFI_EXPORT bool FfiOHOSWebviewCtlAccessStep(int64_t id, int32_t* errCode, int32_t step);
FFI_EXPORT int32_t FfiOHOSWebviewCtlOnActive(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlOnInactive(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetHitTest(int64_t id, int32_t* errCode);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetHitTestValue(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStoreWebArchive(
    int64_t id, const char* cBaseName, bool autoName, void (*callbackRef)(RetDataCString infoRef));
FFI_EXPORT int32_t FfiOHOSWebviewCtlEnableSafeBrowsing(int64_t id, bool enable);
FFI_EXPORT bool FfiOHOSWebviewCtlIsSafeBrowsingEnabled(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetSecurityLevel(int64_t id, int32_t* errCode);
FFI_EXPORT bool FfiOHOSWebviewCtlIsIncognitoMode(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRemoveCache(int64_t id, bool clearRom);
FFI_EXPORT int64_t FfiOHOSWebviewCtlGetBackForwardEntries(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStop(int64_t id);
FFI_EXPORT int64_t FfiOHOSWebviewCtlGetFavicon(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPrepareForPageLoad(char* url, bool preconnectable, int32_t numSockets);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSlideScroll(int64_t id, float vx, float vy);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetNetworkAvailable(int64_t id, bool enable);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearClientAuthenticationCache(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearSslChache(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSearchNext(int64_t id, bool forward);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearMatches(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSearchAllAsync(int64_t id, char* searchString);
FFI_EXPORT int32_t FfiOHOSWebviewCtlDeleteJavaScriptRegister(int64_t id, char* name);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPostUrl(int64_t id, char* url, CArrUI8 buffer);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetDownloadDelegate(int64_t id, int64_t delegateId);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStartDownload(int64_t id, char* url);
FFI_EXPORT CArrI64 FfiOHOSWebviewCtlCreateWebMessagePorts(int64_t id, bool isExtentionType, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPostMessage(int64_t id, char* name, CArrI64 ports, char* uri);
FFI_EXPORT CArrUI8 FfiOHOSWebviewCtlSerializeWebState(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlRestoreWebState(int64_t id, CArrUI8 cState);
FFI_EXPORT CArrString FfiOHOSWebviewCtlGetCertificate(int64_t id, int32_t* errCode);
FFI_EXPORT OHOS::Webview::CCertByteData FfiOHOSWebviewCtlGetCertificateByte(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlHasImage(int64_t id, void (*callbackRef)(RetDataBool));
FFI_EXPORT int32_t FfiWebviewCtlCustomizeSchemes(OHOS::Webview::CArrScheme schemes);
FFI_EXPORT bool FfiOHOSWebviewCtlTerminateRenderProcess(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlCloseAllMediaPresentations(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPauseAllMedia(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlResumeAllMedia(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStopAllMedia(int64_t id);
FFI_EXPORT void FfiOHOSWebviewCtlResumeAllTimers();
FFI_EXPORT void FfiOHOSWebviewCtlPauseAllTimers();
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetPrintBackground(int64_t id, bool enable);
FFI_EXPORT bool FfiOHOSWebviewCtlGetPrintBackground(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetScrollable(int64_t id, bool enable);
FFI_EXPORT bool FfiOHOSWebviewCtlGetScrollable(int64_t id, int32_t* errCode);
FFI_EXPORT void FfiOHOSWebviewCtlEnableAdsBlock(int64_t id, bool enable);
FFI_EXPORT bool FfiOHOSWebviewCtlIsAdsBlockEnabled(int64_t id);
FFI_EXPORT bool FfiOHOSWebviewCtlIsAdsBlockEnabledForCurPage(int64_t id);
FFI_EXPORT bool FfiOHOSWebviewCtlIsIntelligentTrackingPreventionEnabled(int64_t id, int32_t* errorCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlEnableIntelligentTrackingPrevention(int64_t id, bool enable);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetMediaPlaybackState(int64_t id, int32_t* errorCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlGetRenderProcessMode();
FFI_EXPORT void FfiOHOSWebviewCtlSetRenderProcessMode(int32_t mode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlWarmupServiceWorker(char* url);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetHostIP(char* hostname, char* address, int32_t aliveTime);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearHostIP(char* hostname);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetLastJavascriptProxyCallingFrameUrl(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlEnableWholeWebPageDrawing();
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearPrefetchedResource(CArrString cacheKeyList);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStartCamera(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlStopCamera(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlCloseCamera(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlAvoidVisibleViewportBottom(int64_t id, int32_t avoidHeight);

FFI_EXPORT int32_t FfiOHOSWebviewCtlAddIntelligentTrackingPreventionBypassingList(CArrString hostList);
FFI_EXPORT void FfiOHOSWebviewCtlClearIntelligentTrackingPreventionBypassingList();
FFI_EXPORT void FfiOHOSWebviewCtlEnableBackForwardCache(bool nativeEmbed, bool mediaTakeOver);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetSurfaceId(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlInjectOfflineResources(int64_t id, OHOS::Webview::CArrOfflineResourceMap maps);
FFI_EXPORT void FfiOHOSWebviewCtlRemoveIntelligentTrackingPreventionBypassingList(CArrString hostList);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetBackForwardCacheOptions(int64_t id, int32_t size, int32_t timeToLive);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetPathAllowingUniversalAccess(int64_t id, CArrString pathList);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetUrlTrustList(int64_t id, char* cUrl);
FFI_EXPORT int32_t FfiOHOSWebviewCtlPrefetchResource(OHOS::Webview::CRequestInfo request,
    OHOS::Webview::ArrWebHeader additionalHttpHeaders, char* cKey, int32_t cacheValidTime);
FFI_EXPORT void FfiOHOSWebviewCtlSetWebSchemeHandler(int64_t id, char* cScheme, int64_t classId, int32_t *errCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearWebSchemeHandler(int64_t id);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetServiceWorkerWebSchemeHandler(char* cScheme, int64_t classId);
FFI_EXPORT int32_t FfiOHOSWebviewCtlClearServiceWorkerWebSchemeHandler();
FFI_EXPORT void FfiOHOSWebviewCtlOnCreateNativeMediaPlayer(int64_t id,
    int64_t (* callback)(int64_t, OHOS::Webview::CMediaInfo));
FFI_EXPORT int32_t FfiOHOSWebviewCtlPrecompileJavaScript(int64_t id,
    char* url, char* script, OHOS::Webview::CacheOptions cacheOptions);
FFI_EXPORT int32_t FfiOHOSWebviewCtlWebPageSnapshot(int64_t id, OHOS::Webview::CSnapshotInfo snapshot,
    void (*callbackRef)(OHOS::Webview::RetDataCSnapshotResult infoRef));
FFI_EXPORT OHOS::Webview::CScrollOffset FfiOHOSWebviewCtlgetScrollOffset(int64_t id, int32_t* errorCode);
FFI_EXPORT int32_t FfiOHOSWebviewCtlSetScrollableEx(int64_t id, bool enable, int32_t scrollType);
FFI_EXPORT bool FfiOHOSWebviewCtlScrollByWithResult(int64_t id, float deltaX, float deltaY, int32_t* errorCode);

    // BackForwardList
FFI_EXPORT int32_t FfiOHOSBackForwardListCurrentIndex(int64_t id, int32_t* errCode);
FFI_EXPORT int32_t FfiOHOSBackForwardListSize(int64_t id, int32_t* errCode);
FFI_EXPORT OHOS::Webview::CHistoryItem FfiOHOSGetItemAtIndex(int64_t id, int32_t index, int32_t* errCode);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetLastHitTest(int64_t id, int32_t* errCode);
FFI_EXPORT RetDataCString FfiOHOSWebviewCtlGetLastPostMessageURL(int64_t id);
}

#endif // WEBVIEW_FFI_H