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

import { Resource } from 'global.resource';
import type image from '@ohos.multimedia.image';
import { AsyncCallback, BusinessError, Callback } from '@ohos.base';
import { WebNetErrorList } from '@ohos.web.netErrorList';
import type cert from '@ohos.security.cert';
import print from '@ohos.print'
import {SizeOptions} from '@ohos.arkui.component';
declare type Length = string | number | Resource;

class Cleaner {
    private ptr: long = 0
    private name: string = ""
    constructor(ptr: long, name: string) {
        this.ptr = ptr
        this.name = name
    }
    native clean(): void
}

function CleanerCallback(cleaner: Cleaner): void {
    cleaner.clean()
}

let destroyRegister = new FinalizationRegistry<Cleaner>(CleanerCallback)
let unregisterToken = new object()

export default namespace webview {
    static {
        loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
    }
    export native function once(type: string, callback: Callback<void>): void;

    export enum ScrollType {
        EVENT
    }

    export enum WebDestroyMode {
        NORMAL_MODE = 0,
        FAST_MODE = 1
    }

    export enum ScrollbarMode {
        OVERLAY_LAYOUT_SCROLLBAR,
        FORCE_DISPLAY_SCROLLBAR
    };

    export enum WebSoftKeyboardBehaviorMode {
        DEFAULT = 0,
        DISABLE_AUTO_KEYBOARD_ON_ACTIVE = 1
    }

    export enum ControllerAttachState {
        UNATTACHED = 0,
        ATTACHED = 1,
    }

    export interface CacheOptions {
        responseHeaders: Array<WebHeader>;
    }

    export enum OfflineResourceType {
        IMAGE,
        CSS,
        CLASSIC_JS,
        MODULE_JS
    }

    export enum WebDownloadErrorCode {
        ERROR_UNKNOWN = 0,
        FILE_FAILED = 1,
        FILE_ACCESS_DENIED = 2,
        FILE_NO_SPACE = 3,
        FILE_NAME_TOO_LONG = 5,
        FILE_TOO_LARGE = 6,
        FILE_TRANSIENT_ERROR = 10,
        FILE_BLOCKED = 11,
        FILE_TOO_SHORT = 13,
        FILE_HASH_MISMATCH = 14,
        FILE_SAME_AS_SOURCE = 15,
        NETWORK_FAILED = 20,
        NETWORK_TIMEOUT = 21,
        NETWORK_DISCONNECTED = 22,
        NETWORK_SERVER_DOWN = 23,
        NETWORK_INVALID_REQUEST = 24,
        SERVER_FAILED = 30,
        SERVER_NO_RANGE = 31,
        SERVER_BAD_CONTENT = 33,
        SERVER_UNAUTHORIZED = 34,
        SERVER_CERT_PROBLEM = 35,
        SERVER_FORBIDDEN = 36,
        SERVER_UNREACHABLE = 37,
        SERVER_CONTENT_LENGTH_MISMATCH = 38,
        SERVER_CROSS_ORIGIN_REDIRECT = 39,
        USER_CANCELED = 40,
        USER_SHUTDOWN = 41,
        CRASH = 50
    }

    export enum WebDownloadState {
        IN_PROGRESS = 0,
        COMPLETED,
        CANCELED,
        INTERRUPTED,
        PENDING,
        PAUSED,
        UNKNOWN
    }

    export enum WebBlanklessErrorCode {
        SUCCESS = 0,
        ERR_UNKNOWN = -1,
        ERR_INVALID_PARAM = -2,
        ERR_CONTROLLER_NOT_INITED = -3,
        ERR_KEY_NOT_MATCH = -4,
        ERR_SIGNIFICANT_CHANGE = -5,
        ERR_DURATION_OUT_OF_RANGE = -6,
        ERR_EXPIRATION_TIME_OUT_OF_RANGE = -7
    }

    export enum BlanklessFrameInterpolationState {
        FRAME_INTERPOLATION_SUCCEEDED = 0,
        FRAME_INTERPOLATION_FAILED = 1,
        FRAME_INTERPOLATION_REMOVED = 2
    }

    export interface OfflineResourceMap {
        urlList: Array<string>,
        resource: Uint8Array,
        responseHeaders: Array<WebHeader>,
        type: OfflineResourceType
    }

    export interface OfflineResourceMapInternal {
        urlList: Array<string>,
        resource: ArrayBuffer,
        responseHeaders: Array<WebHeader>,
        type: OfflineResourceType
    }

    export enum WebHitTestType {
        EDIT_TEXT,
        EMAIL,
        HTTP_ANCHOR,
        HTTP_ANCHOR_IMG,
        IMG,
        MAP,
        PHONE,
        UNKNOWN
    }

    export enum SecurityLevel {
        NONE = 0,
        SECURE,
        WARNING,
        DANGEROUS
    }

    export enum RenderProcessMode{
        SINGLE = 0,
        MULTIPLE = 1
    }

    export enum SiteIsolationMode{
        PARTIAL = 0,
        STRICT = 1
    }

    export enum PressureLevel {
        MEMORY_PRESSURE_LEVEL_MODERATE = 1,
        MEMORY_PRESSURE_LEVEL_CRITICAL = 2
    }

    export enum PlaybackStatus {
        PAUSED = 0,
        PLAYING = 1
    }

    export enum NetworkState {
        EMPTY = 0,
        IDLE = 1,
        LOADING = 2,
        NETWORK_ERROR = 3
    }

    export enum ReadyState {
        HAVE_NOTHING = 0,
        HAVE_METADATA = 1,
        HAVE_CURRENT_DATA = 2,
        HAVE_FUTURE_DATA = 3,
        HAVE_ENOUGH_DATA = 4
    }

    export enum MediaError {
        NETWORK_ERROR = 1,
        FORMAT_ERROR = 2,
        DECODE_ERROR = 3
    }

    export enum MediaType {
        VIDEO = 0,
        AUDIO = 1
    }

    export enum WebResourceType{
        MAIN_FRAME = 0,
        SUB_FRAME = 1,
        STYLE_SHEET = 2,
        SCRIPT = 3,
        IMAGE = 4,
        FONT_RESOURCE = 5,
        SUB_RESOURCE = 6,
        OBJECT = 7,
        MEDIA = 8,
        WORKER = 9,
        SHARED_WORKER = 10,
        PREFETCH = 11,
        FAVICON = 12,
        XHR = 13,
        PING = 14,
        SERVICE_WORKER = 15,
        CSP_REPORT = 16,
        PLUGIN_RESOURCE = 17,
        NAVIGATION_PRELOAD_MAIN_FRAME = 19,
        NAVIGATION_PRELOAD_SUB_FRAME = 20,
    }

    export enum SourceType {
        URL = 0,
        MSE = 1
    }

    export enum Preload {
        NONE = 0,
        METADATA = 1,
        AUTO = 2
    }

    export enum SuspendType {
        ENTER_BACK_FORWARD_CACHE = 0,
        ENTER_BACKGROUND = 1,
        AUTO_CLEANUP = 2
    }

    export class MediaSourceInfo {
        type: SourceType = SourceType.URL;
        source: string = "";
        format: string = "";
    }

    export class NativeMediaPlayerSurfaceInfo {
        id: string = "";
        rect: RectEvent = {x : 0, y : 0, width : 0, height : 0};
    }

    export enum MediaPlaybackState {
        NONE = 0,
        PLAYING = 1,
        PAUSED = 2,
        STOPPED = 3
    }

    export interface RequestInfo {
        url: string;
        method: string;
        formData: string;
    }

    export interface WebHeader {
        headerKey: string;
        headerValue: string;
    }

    export class GetHeaderWebHeader implements WebHeader {
        headerKey: string;
        headerValue: string;
    }

    export interface ScrollOffset {
        x: double;
        y: double;
    }
    export class ScrollOffsetInner implements ScrollOffset {
        x: double = 0;
        y: double = 0;
    }

    export interface BlanklessInfo {
        errCode: WebBlanklessErrorCode;
        similarity: double;
        loadingTime: int;
    }
    export class BlanklessInfoInner implements BlanklessInfo {
        errCode: WebBlanklessErrorCode = WebBlanklessErrorCode.ERR_INVALID_PARAM;
        similarity: double = 0.0;
        loadingTime: int = 0;
    }

    export interface BlanklessFrameInterpolationInfo {
        key: string;
        state: BlanklessFrameInterpolationState;
        timestamp: int;
        reason: string;
    }

    export class BlanklessFrameInterpolationInfoInner implements BlanklessFrameInterpolationInfo {
        key: string;
        state: BlanklessFrameInterpolationState;
        timestamp: int;
        reason: string;
    }

    export interface BlanklessLoadingParam {
        enable: boolean;
        duration?: int;
        expirationTime?: int;
        callback?: Callback<BlanklessFrameInterpolationInfo>;
    }

    export class BlanklessLoadingParamInner implements BlanklessLoadingParam {
        enable: boolean;
        duration?: int;
        expirationTime?: int;
        callback?: Callback<BlanklessFrameInterpolationInfo>;
    }

    export interface HitTestValue {
        type: WebHitTestType;
        extra: string;
    }
    export class HitTestValueInner implements HitTestValue {
        type: WebHitTestType = WebHitTestType.UNKNOWN;
        extra: string = "";
    }

    enum SecureDnsMode {
        OFF = 0,
        AUTO,
        SECURE_ONLY
    }

    export interface HistoryItem {
        icon: image.PixelMap;
        historyUrl: string;
        historyRawUrl: string;
        title: string;
    }

    export class HistoryIteminner implements HistoryItem {
        icon: image.PixelMap;
        historyUrl: string;
        historyRawUrl: string;
        title: string;
    }

    export interface SnapshotResult {
        id?: string;
        status?: boolean;
        imagePixelMap?: image.PixelMap;
        size: SizeOptions;
    }

    export class SnapshotResultClass implements SnapshotResult {
        id?: string;
        status?: boolean;
        imagePixelMap?: image.PixelMap;
        size: SizeOptions;
    }

    export interface SnapshotInfo {
        id?: string;
        size: SizeOptions;
    }

    export class SnapshotInfoClass implements SnapshotInfo {
        id?: string;
        size: SizeOptions;
    }

    export class SizeOptionsClass implements SizeOptions{
        width?: Length;
        height?: Length;
    }

    export interface WebCustomScheme {
        schemeName: string;
        isSupportCORS: boolean;
        isSupportFetch: boolean;
        isStandard?: boolean;
        isLocal?: boolean;
        isDisplayIsolated?: boolean;
        isSecure?: boolean;
        isCspBypassing?: boolean;
        isCodeCacheSupported?: boolean;
  }

    export class WebCustomSchemeClass implements WebCustomScheme {
        schemeName: string;
        isSupportCORS: boolean;
        isSupportFetch: boolean;
        isStandard?: boolean;
        isLocal?: boolean;
        isDisplayIsolated?: boolean;
        isSecure?: boolean;
        isCspBypassing?: boolean;
        isCodeCacheSupported?: boolean;
  }

    export interface RectEvent {
        x: double;
        y: double;
        width: double;
        height: double;
    }

    export class RectEventinner implements RectEvent {
        x: double = 0;
        y: double = 0;
        width: double = 0;
        height: double = 0;
    }

    export interface MediaInfo {
        embedID: string;
        mediaType: MediaType;
        mediaSrcList: MediaSourceInfo[];
        surfaceInfo: NativeMediaPlayerSurfaceInfo;
        controlsShown: boolean;
        controlList: string[];
        muted: boolean;
        posterUrl: string;
        preload: Preload;
        headers: Record<string, string>;
        attributes: Record<string, string>;
    }

    export class MediaInfoinner implements MediaInfo {
        embedID: string = "";
        mediaType: MediaType = MediaType.VIDEO;
        mediaSrcList: MediaSourceInfo[] = [];
        surfaceInfo: NativeMediaPlayerSurfaceInfo = {};
        controlsShown: boolean = false;
        controlList: string[] = [];
        muted: boolean = false;
        posterUrl: string = "";
        preload: Preload = Preload.NONE;
        headers: Record<string, string> = {};
        attributes: Record<string, string> = {};
    }

    export interface NativeMediaPlayerHandler {
        handleStatusChanged(status: PlaybackStatus): void
        handleVolumeChanged(volume: double): void
        handleMutedChanged(muted: boolean): void
        handlePlaybackRateChanged(playbackRate: double): void
        handleDurationChanged(duration: double): void
        handleTimeUpdate(currentPlayTime: double): void
        handleBufferedEndTimeChanged(bufferedEndTime: double): void
        handleEnded(): void
        handleNetworkStateChanged(state: NetworkState): void
        handleReadyStateChanged(state: ReadyState): void
        handleFullscreenChanged(fullscreen: boolean): void
        handleSeeking(): void
        handleSeekFinished(): void
        handleError(error: MediaError, errorMessage: string): void
        handleVideoSizeChanged(width: double, height: double): void
    }

    export class NativeMediaPlayerHandlerinner implements NativeMediaPlayerHandler {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "NativeMediaPlayerHandlerinner")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        constructor(){};

        native handleStatusChanged(status: PlaybackStatus): void
        native handleVolumeChanged(volume: double): void
        native handleMutedChanged(muted: boolean): void
        native handlePlaybackRateChanged(playbackRate: double): void
        native handleDurationChanged(duration: double): void
        native handleTimeUpdate(currentPlayTime: double): void
        native handleBufferedEndTimeChanged(bufferedEndTime: double): void
        native handleEnded(): void
        native handleNetworkStateChanged(state: NetworkState): void
        native handleReadyStateChanged(state: ReadyState): void
        native handleFullscreenChanged(fullscreen: boolean): void
        native handleSeeking(): void
        native handleSeekFinished(): void
        native handleError(error: MediaError, errorMessage: string): void
        native handleVideoSizeChanged(width: double, height: double): void


        static transferNativeMediaPlayerHandlerToStatic(input: Any): Object {
            let nmphObject: NativeMediaPlayerHandlerinner = new NativeMediaPlayerHandlerinner();
            if (input == undefined || input == null) {
                throw new Error("input object is null or undefined");
            }
            let dyESValue = ESValue.wrap(input);

            if (!NativeMediaPlayerHandlerinner.transferNativeMediaPlayerHandlerToStaticInner(nmphObject, dyESValue)) {
                throw new Error("transfer failed");
            }
            return nmphObject as Object;
        }
        private static native transferNativeMediaPlayerHandlerToStaticInner(newDemo: NativeMediaPlayerHandlerinner, input: ESValue): boolean;

        static transferNativeMediaPlayerHandlerToDynamic(input: Object): Any {
            const staticRef = input as NativeMediaPlayerHandlerinner;
            if (staticRef) {
                let addr = staticRef.nativePtr;
                let module = ESValue.load("@ohos.web.webview");
                let initFunc = module.getProperty("__createNativeMediaPlayerHandlerTransfer__");
                if (initFunc) {
                    let dynamicRef = initFunc.invoke(ESValue.wrap(addr));
                    return dynamicRef.unwrap();
                }
            } else {
                console.error("invoke func not exit");
            }
            return undefined;
        }
    }

    export interface NativeMediaPlayerBridge {
        updateRect: UpdateRectFn;
        play: ZeroParamFn<>;
        pause: ZeroParamFn<>;
        seek: OneParamFn<double>;
        setVolume: OneParamFn<double>;
        setMuted: OneParamFn<boolean>;
        setPlaybackRate: OneParamFn<double>;
        release: ZeroParamFn<>;
        enterFullscreen: ZeroParamFn<>;
        exitFullscreen: ZeroParamFn<>;
        resumePlayer?: ResumePlayerFn;
        suspendPlayer?: SuspendPlayerFn;
    }

    type ZeroParamFn<V=void> = () => V;
    type OneParamFn<T,V=void> = (param: T) => V;
    type UpdateRectFn = (x: double, y: double, width: double, height: double) => void;
    type SuspendPlayerFn = (type: SuspendType) => void
    type ResumePlayerFn = () => void

    export interface BackForwardList {
        currentIndex: int;
        size: int;
        getItemAtIndex(index: int): HistoryItem;
    }

    export class BackForwardListinner implements BackForwardList {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;
        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebHistoryList")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        currentIndex: int = 0;
        size: int = 0;
        native getItemAtIndex(index: int): HistoryItem;

        static transferBackForwardListToStatic(input: Any): Object {
            let bflObject: BackForwardListinner = new BackForwardListinner();
            if (input === undefined || input === null) {
                throw new Error("input object is null or undefined");
            }
            let dyESValue = ESValue.wrap(input);
            bflObject.currentIndex = dyESValue.getProperty('currentIndex').toNumber().toInt();
            bflObject.size = dyESValue.getProperty('size').toNumber().toInt();
            if (!BackForwardListinner.transferBackForwardListToStaticInner(bflObject, dyESValue)) {
                throw new Error("transfer failed");
            }
            return bflObject as Object;
        }
        private static native transferBackForwardListToStaticInner(newDemo: BackForwardListinner, input: ESValue): boolean;

        static transferBackForwardListToDynamic(input: Object): Any {
            const staticRef = input as BackForwardListinner;
            if (staticRef) {
                let addr = staticRef.nativePtr;
                let module = ESValue.load("@ohos.web.webview");
                let initFunc = module.getProperty("__createBackForwardListTransfer__");
                if (initFunc) {
                    let dynamicRef = initFunc.invoke(
                        ESValue.wrap(staticRef.currentIndex), ESValue.wrap(staticRef.size), ESValue.wrap(addr));
                    return dynamicRef.unwrap();
                } else {
                    console.error("invoke func not exit");
                }
            }
            return undefined;
        }
    }

    export class AsyncCallbackWrapper<T> {
        myFun_:AsyncCallback<T, void> = (err: BusinessError<void> | null, data: T | undefined) => {}

        constructor(myFun:AsyncCallback<T, void>){
            this.myFun_ = myFun;
        }

        invoke(err: BusinessError<void> | null, data: T | undefined) : void {
            this.myFun_(err, data);
        }
    }

    export interface PdfConfiguration {
        width: double;
        height: double;
        marginTop: double;
        marginBottom: double;
        marginRight: double;
        marginLeft: double;
        scale?: double;
        shouldPrintBackground?: boolean;
    }

    export class PdfData {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "PdfData")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native pdfArrayBufferInternal(): ArrayBuffer;
        pdfArrayBuffer(): Uint8Array{
            let buffer = this.pdfArrayBufferInternal();
            let uint8Array = new Uint8Array(buffer);
            return uint8Array;
        }
    }

    export enum ArkWebEngineVersion {
        SYSTEM_DEFAULT = 0,
        M114 = 1,
        M132 = 2,
        ARKWEB_EVERGREEN = 99999
    }

    export class WebviewController {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebviewController")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor(webTag?: string);
        native _setNWebId(nwebId: int): void;
        native _setHapPath(hapPath: string): void;

        native static initializeWebEngine(): void;
        native loadUrl(url: string | Resource, headers?: Array<WebHeader>): void;
        native createWebMessagePorts(isExtentionType?: boolean): Array<WebMessagePort>;
        native onActive(): void;
        native onInactive(): void;
        native getWebId(): int;
        native innerGetWebId(): int;
        native getScrollable(): boolean;
        native requestFocus(): void;
        native clearClientAuthenticationCache(): void;
        native scrollByWithResult(deltaX: double, deltaY: double): boolean;
        native setScrollable(enable: boolean, type?: ScrollType): void;
        native scrollTo(x: double, y: double, duration?: int): void;
        native scrollBy(deltaX: double, deltaY: double, duration?: int): void;
        native getScrollOffset(): ScrollOffset;
        native getPageOffset(): ScrollOffset;
        native slideScroll(vx: double, vy: double): void;
        native zoom(factor: double): void;
        native isSafeBrowsingEnabled(): boolean;
        native static prepareForPageLoad(url: string, preconnectable: boolean, numSockets: int): void;
        native getFavicon(): image.PixelMap;
        native pageDown(bottom: boolean): void;
        native pageUp(top: boolean): void;
        native isAdsBlockEnabledForCurPage(): boolean;
        native isIntelligentTrackingPreventionEnabled(): boolean;
        native static addIntelligentTrackingPreventionBypassingList(hostList: Array<string>): void;
        native static removeIntelligentTrackingPreventionBypassingList(hostList: Array<string>): void;
        native static clearIntelligentTrackingPreventionBypassingList(): void;
        native createPdfNative(configuration: PdfConfiguration, callback: AsyncCallbackWrapper<PdfData>): void;
        createPdf(configuration: PdfConfiguration, callback: AsyncCallback<PdfData, void>): void {
            let myCall = new AsyncCallbackWrapper<PdfData>(callback);
            taskpool.execute((): void => {
                this.createPdfNative(configuration, myCall);
            })
        }
        createPdf(configuration: PdfConfiguration): Promise<PdfData> {
            let p:Promise<PdfData> =
                new Promise<PdfData>((resolve: (data: PdfData)=>void, reject:(err: Error)=>void):void => {
                let myCall = new AsyncCallbackWrapper<PdfData>((err: BusinessError<void> | null, data: PdfData | undefined)=>{
                    if (err === null || err.code == 0) {
                        resolve(data!);
                    } else {
                        reject(err);
                    }
                });
                taskpool.execute((): void => {
                    this.createPdfNative(configuration, myCall);
                });
            });
            return p;
        }
        native static setHostIP(hostName: string, address: string, aliveTime: int): void;
        native static clearHostIP(hostName: string): void;
        native static warmupServiceWorker(url: string): void;
        native zoomOut(): void;
        native zoomIn(): void;
        native getLastHitTest(): HitTestValue;
        native getPageHeight(): int;
        native refresh(): void;
        native stop(): void;
        native clearSslCache(): void;
        native clearMatches(): void;
        native startDownload(url: string): void;
        native setDownloadDelegate(delegate: WebDownloadDelegate): void;
        native getBackForwardEntries(): BackForwardList;
        native setUrlTrustList(urlTrustList: string): void;
        native enableIntelligentTrackingPrevention(enable: boolean): void;
        native searchNext(forward: boolean): void;
        native static getDefaultUserAgent(): string;
        native static removeAllCache(clearRom: boolean): void;
        native accessStep(step: int): boolean;
        native isAdsBlockEnabled(): boolean;
        native enableAdsBlock(enable: boolean): void;
        native postUrl(url: string, postData: ArrayBuffer): void;
        native postMessage(name: string, ports: Array<WebMessagePort>, uri: string): void;
        native getUrl(): string;
        native getTitle(): string;
        native getProgress(): int;
        native getOriginalUrl(): string;
        native getUserAgent(): string;
        native getCustomUserAgent(): string;
        native getLastJavascriptProxyCallingFrameUrl(): string;
        native getSecurityLevel(): SecurityLevel;
        native static setHttpDns(secureDnsMode: SecureDnsMode, secureDnsConfig: string): void;
        native searchAllAsync(searchString: string): void;
        native static clearServiceWorkerWebSchemeHandler(): void;
        native forward(): void;
        native backward(): void;
        native accessForward(): boolean;
        native accessBackward(): boolean;
        native loadData(data: string, mimeType: string, encoding: string, baseUrl?: string, historyUrl?: string): void;
        native clearHistory(): void;
        native setWebSchemeHandler(scheme: string, handler: WebSchemeHandler): void;
        native static customizeSchemes(schemes: Array<WebCustomScheme>): void;
        native static setServiceWorkerWebSchemeHandler(scheme: string, handler: WebSchemeHandler): void;
        native clearWebSchemeHandler(): void;
        native terminateRenderProcess(): boolean;
        native static setConnectionTimeout(timeout: int): void;
        native backOrForward(step: int): void;
        native static SetWebDebuggingAccess(webDebuggingAccess: boolean): void;
        static setWebDebuggingAccess(webDebuggingAccess: boolean): void {
            WebviewController.SetWebDebuggingAccess(webDebuggingAccess);
        }
        native static setWebDebuggingAccessAndPort(webDebuggingAccess: boolean, port: int): void;
        static setWebDebuggingAccess(webDebuggingAccess: boolean, port: int): void {
            WebviewController.setWebDebuggingAccessAndPort(webDebuggingAccess, port);
        }
        native enableSafeBrowsing(enable: boolean): void;
        native static setRenderProcessMode(mode: RenderProcessMode): void;
        native static getRenderProcessMode(): RenderProcessMode;
        native static pauseAllTimers(): void;
        native static resumeAllTimers(): void;
        native setCustomUserAgent(userAgent: string): void;
        native removeCache(clearRom: boolean): void;
        native setNetworkAvailable(enable: boolean): void;
        native isIncognitoMode(): boolean;
        native serializeWebStateInternal(): ArrayBuffer;
        serializeWebState(): Uint8Array{
            let buffer = this.serializeWebStateInternal();
            let uint8Array = new Uint8Array(buffer);
            return uint8Array;
        }
        native restoreWebState(state: Uint8Array): void;
        native static trimMemoryByPressureLevel(level: PressureLevel): void;
        native setPathAllowingUniversalAccess(pathList: Array<string>): void;
        native onCreateNativeMediaPlayer(callback: (handler: NativeMediaPlayerHandler, mediaInfo: MediaInfo) => NativeMediaPlayerBridge): void;

        native getSurfaceId(): string;
        native static enableWholeWebPageDrawing(): void;
        native setPrintBackground(enable: boolean): void;
        native createWebPrintDocumentAdapter(jobName: string): print.PrintDocumentAdapter;
        native getPrintBackground(): boolean;
        native webPageSnapshot(info: SnapshotInfo, callback: AsyncCallback<SnapshotResult>): void;
        native injectOfflineResourcesInternal(resourceMaps: Array<OfflineResourceMapInternal>) : void;
        injectOfflineResources(resourceMaps: Array<OfflineResourceMap>): void {
            let res = resourceMaps.map((map) => ({
                urlList: map.urlList,
                resource: map.resource.buffer,
                responseHeaders: map.responseHeaders,
                type: map.type} as OfflineResourceMapInternal));
            this.injectOfflineResourcesInternal(res);
        }
        native static clearPrefetchedResource(cacheKeyList: Array<string>): void;
        native static prefetchResource(request: RequestInfo, additionalHeaders?: Array<WebHeader>, cacheKey?: string, cacheValidTime?: int): void;
        native startCamera(): void;
        native closeAllMediaPresentations(): void;
        native stopAllMedia(): void;
        native stopCamera(): void;
        native closeCamera(): void;
        native resumeMicrophone(): void;
        native stopMicrophone(): void;
        native pauseMicrophone(): void;
        native pauseAllMedia(): void;
        native resumeAllMedia(): void;
        native setAudioMuted(mute: boolean): void;
        native getMediaPlaybackState(): MediaPlaybackState;
        native avoidVisibleViewportBottom(avoidHeight: int): void;
        native innerCompleteWindowNew(parentNwebId:int): void;
        innerGetThisVar(): long {
            return this.nativePtr;
        }
        native precompileJavaScriptInternal(url: string, script: string | ArrayBuffer, cacheOptions: CacheOptions): Promise<int>;
        precompileJavaScript(url: string, script: string | Uint8Array, cacheOptions: CacheOptions): Promise<int> {
            let resScript: string | ArrayBuffer
            if (typeof script === 'string') {
                resScript = script as string;
            } else if (script instanceof Uint8Array) {
                resScript = script.buffer as ArrayBuffer;
            }
            return this.precompileJavaScriptInternal(url, resScript, cacheOptions);
        }
        native registerJavaScriptProxy(object: object, name: string, methodList: Array<string>,
                                        asyncMethodList?: Array<string>, permission?: string): void;
        native deleteJavaScriptRegister(name: string): void;
        native runJavaScriptCallback(script: string, callback: AsyncCallback<string>): void;
        runJavaScript(script: string, callback: AsyncCallback<string>): void {
            this.runJavaScriptCallback(script, callback);
        }
        native runJavaScriptPromise(script: string): Promise<string>;
        runJavaScript(script: string): Promise<string> {
            return this.runJavaScriptPromise(script);
        }
        native runJavaScriptCallbackExt(script: string | ArrayBuffer, callback: AsyncCallback<JsMessageExt>): void;
        runJavaScriptExt(script: string | ArrayBuffer, callback: AsyncCallback<JsMessageExt>): void {
            this.runJavaScriptCallbackExt(script, callback);
        }
        native runJavaScriptPromiseExt(script: string | ArrayBuffer): Promise<string>;
        runJavaScriptExt(script: string | ArrayBuffer): Promise<string> {
            return this.runJavaScriptPromiseExt(script);
        }
        native static enableBackForwardCache(features?: BackForwardCacheSupportedFeatures): void;
        native setBackForwardCacheOptions(options?: BackForwardCacheOptions): void;
        native static setAppCustomUserAgent(userAgent: string): void;
        native static setUserAgentForHosts(userAgent: string, hosts: Array<string>): void
        native storeWebArchiveCallback(baseName: string, autoName: boolean, callback: AsyncCallback<string>): void;
        storeWebArchive(baseName: string, autoName: boolean, callback: AsyncCallback<string>): void {
            this.storeWebArchiveCallback(baseName, autoName, callback);
        }
        native storeWebArchivePromise(baseName: string, autoName: boolean): Promise<string>;
        storeWebArchive(baseName: string, autoName: boolean): Promise<string> {
            return this.storeWebArchivePromise(baseName, autoName);
        }
        native hasImageCallback(callback: AsyncCallback<boolean>): void;
        hasImage(callback: AsyncCallback<boolean>): void {
            this.hasImageCallback(callback);
        }
        native hasImagePromise(): Promise<boolean>;
        hasImage(): Promise<boolean> {
            return this.hasImagePromise();
        }

        native getCertificateSync(): Array<ArrayBuffer>;
        getCertificate(): Promise<Array<cert.X509Cert>> {
            let certChainData = this.getCertificateSync();
            return new Promise<Array<cert.X509Cert>>((resolve, reject) => {
                let taskPromise = taskpool.execute(():undefined => {
                    return undefined});
                taskPromise.then(async (results: Any) => {
                    const certs: Array<cert.X509Cert> = [];
                    for (let i = 0; i < certChainData.length; i++) {
                        let certsUint8Array = new Uint8Array(certChainData[i]);
                        const encodingBlob: cert.EncodingBlob = {
                            data: certsUint8Array,
                            encodingFormat: cert.EncodingFormat.FORMAT_DER
                        };
                        try {
                            const x509Cert = await cert.createX509Cert(encodingBlob);
                            certs.push(x509Cert);
                        } catch (e) {
                            console.error("Failed to parse certificate", e);
                        }
                    }
                    resolve(certs);
                })
                .catch((error: Error) => {
                    reject(error);
                });
            });
        }
        getCertificate(callback: AsyncCallback<Array<cert.X509Cert>>): void {
            let certChainData = this.getCertificateSync();
            let p = taskpool.execute(():undefined => {
                    return undefined});
            p.then((results : Any) => {
                const certs: Array<cert.X509Cert> = [];
                for (let i = 0; i < certChainData.length; i++) {
                    let certsUint8Array = new Uint8Array(certChainData[i]);
                    const encodingBlob: cert.EncodingBlob = {
                        data: certsUint8Array,
                        encodingFormat: cert.EncodingFormat.FORMAT_DER
                    };
                    try {
                        const x509Cert = await cert.createX509Cert(encodingBlob);
                        certs.push(x509Cert);
                    } catch (e) {
                        console.error("Failed to parse certificate", e);
                    }
                }
                callback(null, certs);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, undefined);
            });
        }

        native setSocketIdleTimeout(timeout: int): void;
        native static enablePrivateNetworkAccess(enable: boolean): void;
        native static isPrivateNetworkAccessEnabled(): boolean;
        native setErrorPageEnabled(enable: boolean): void;
        native getErrorPageEnabled(): boolean;
        native getBlanklessInfoWithKey(key: string): BlanklessInfo;
        native setBlanklessLoadingWithKey(key: string, is_start: boolean): WebBlanklessErrorCode;
        native setBlanklessLoadingWithParams(key: string, param: BlanklessLoadingParam): WebBlanklessErrorCode;
        native static clearBlanklessLoadingCache(keys?: Array<string>): void;
        native static setBlanklessLoadingCacheCapacity(capacity: int): int;
        native static setSiteIsolationMode(mode: SiteIsolationMode): void;
        native static getSiteIsolationMode(): SiteIsolationMode;
        native static setWebDestroyMode(mode: WebDestroyMode): void;
        native static setScrollbarMode(mode: ScrollbarMode): void;
        native setSoftKeyboardBehaviorMode(mode: WebSoftKeyboardBehaviorMode): void;
        native getAttachState(): ControllerAttachState;
        native onControllerAttachStateChange(callback: Callback<ControllerAttachState>): void;
        native offControllerAttachStateChange(callback?: Callback<ControllerAttachState>): void;
        native waitForAttachedPromise(timeout: int): Promise<ControllerAttachState>;
        waitForAttached(timeout: int): Promise<ControllerAttachState> {
            if (timeout < 0 || timeout > 300000) {
                let error = new Error('BusinessError 401: Parameter error. The value of \'timeout\' is invalid.');
                throw new BusinessError<void>(401, error);
            }

            return this.waitForAttachedPromise(timeout);
        }
        native static isAutoPreconnectEnabled(): boolean;
        native static setAutoPreconnect(enabled: boolean): void;
        native prefetchPageUrl(url: string): void;
        native prefetchPageUrlHeaders(url: string, additionalHeaders: Array<WebHeader>): void;
        native prefetchPageUrlHeadersOptions(url: string, additionalHeaders: Array<WebHeader>, prefetchOptions: PrefetchOptions): void;
        prefetchPage(url: string): void {
            this.prefetchPageUrl(url);
        }
        prefetchPage(url: string, additionalHeaders: Array<WebHeader>): void {
            this.prefetchPageUrlHeaders(url, additionalHeaders);
        }
        prefetchPage(url: string, additionalHeaders: Array<WebHeader>, prefetchOptions: PrefetchOptions): void {
            this.prefetchPageUrlHeadersOptions(url, additionalHeaders, prefetchOptions);
        }
        native setWebDetach(nwebId: int): void;
        native static getActiveWebEngineVersion(): ArkWebEngineVersion;
        native static setActiveWebEngineVersion(engineVersion: ArkWebEngineVersion): void;
        native static isActiveWebEngineEvergreen(): boolean;
        native jsProxy(obj: Object, name: string, methodList: Array<string>, asyncMethodList?: Array<string>, permission?: string): void;
        jsProxyInvokeMethod(obj: Object, name: string, args: Array<Object>): Any {
            let clsVal = Type.of(obj) as ClassType;
            let method = clsVal.getMethodByName(name);
            if(args.length <= 0) {
                return method.invoke(obj, []);
            }
            const methodArgs: FixedArray<Object> = new Object[args.length];
            for (let argIdx = 0; argIdx < args.length; argIdx++) {
                const arg = args[argIdx];
                methodArgs[argIdx] = arg;
            }
            return method.invoke(obj, methodArgs);
        }
        getReferenceMap(): Map<string, Object> {
            let claMap: Map<string, Object> = new Map<string, Object>;
            return claMap;
        }
        setFieldForObject(target: Map<string, Object>, name: string, val: Object): Map<string, Object> {
            try {
                target.set(name, val);
            } catch (e: Error) {
                console.error(e);
            }
            return target;
        }
    }
    
    export class jsProxyCallbackWrapper {
        myFun_: (...args: Array<Object|string|number|undefined|null>) => Any = (...args: Array<Object|string|number|undefined|null>) => { return "" };
        constructor(myFun: (...args: Array<Object|string|number|undefined|null>)=> Any) {
            this.myFun_ = myFun;
        }

        native callNativeFunc(ptr: long, argv: double, args: Array<Object>): void;
        getCallback(ptr: long): (...args: Array<Object|string|number|undefined|null>) => Any {
            this.myFun_ = (...args: Array<Object|string|number|undefined|null>) => {
                let objArgs: Array<Object> = new Array<Object>;
                for (const arg of args) {
                    if (arg instanceof Object) {
                       let obj = arg as Object;
                       objArgs.push(obj);
                    }
                }
                this.callNativeFunc(ptr, args.length, objArgs);
                return "";
            };
            return this.myFun_;
        }
    }

    export class WebSchemeHandlerResponse {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebSchemeHandlerResponse")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();

        native setUrl(url: string): void;
        native getUrl(): string;
        native setMimeType(type: string): void;
        native getMimeType(): string;
        native setEncoding(encoding: string): void;
        native getEncoding(): string;
        native setStatusText(text: string): void;
        native getStatusText(): string;
        native setStatus(code: int): void;
        native getStatus(): int;
        native setHeaderByName(name: string, value: string, overwrite: boolean): void;
        native getHeaderByName(name: string): string;
        native setNetErrorCode(code: WebNetErrorList): void;
        native getNetErrorCode(): WebNetErrorList;
    }

     export class WebSchemeHandler {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebSchemeHandler");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }

        native constructor();
        native onRequestStart(callback: (request: WebSchemeHandlerRequest, handler: WebResourceHandler) => boolean): void;
        native onRequestStop(callback : (request: WebSchemeHandlerRequest) => void): void;
    }

    export class WebSchemeHandlerRequest {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebSchemeHandlerRequest");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }

        native constructor();
        native hasGesture(): boolean;
        native getReferrer(): string;
        native getFrameUrl(): string;
        native getHeader(): Array<WebHeader>;
        native getRequestResourceType(): WebResourceType;
        native getRequestUrl(): string;
        native getHttpBodyStream(): WebHttpBodyStream | null;
        native isMainFrame(): boolean;
        native getRequestMethod(): string;

        static transferWebSchemeHandlerRequestToStatic(input: Any): Object {
            let wshrObject: WebSchemeHandlerRequest = new WebSchemeHandlerRequest();
            if (input === undefined || input === null) {
                throw new Error("input object is null or undefined");
            }
            let dyESValue = ESValue.wrap(input);
            if (!WebSchemeHandlerRequest.transferWebSchemeHandlerRequestToStaticInner(wshrObject, dyESValue)) {
                throw new Error("transfer failed");
            }
            return wshrObject as Object;
        }
        private static native transferWebSchemeHandlerRequestToStaticInner(newDemo: WebSchemeHandlerRequest, input: ESValue): boolean;

        static transferWebSchemeHandlerRequestToDynamic(input: Object): Any {
            const staticRef = input as WebSchemeHandlerRequest;
            if (staticRef) {
                let addr = staticRef.nativePtr;
                let module = ESValue.load("@ohos.web.webview");
                let initFunc = module.getProperty("__createWebSchemeHandlerRequestTransfer__");
                if (initFunc) {
                    let dynamicRef = initFunc.invoke(ESValue.wrap(addr));
                    return dynamicRef.unwrap();
                } else {
                    console.error("invoke func not exit");
                }
            }
            return undefined;
        }
    }

    export class WebResourceHandler {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebResourceHandler");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }
        native constructor();
        native didReceiveResponse(response: WebSchemeHandlerResponse): void;
        native didFinish(): void;
        native didFailv0(code: WebNetErrorList): void;
        didFail(code: WebNetErrorList): void {
            this.didFailv0(code);
        }
        native didFailv1(code: WebNetErrorList, completeIfNoResponse: boolean): void
        didFail(code: WebNetErrorList, completeIfNoResponse: boolean): void {
            this.didFailv1(code, completeIfNoResponse);
        }
        native didReceiveResponseBody(data: ArrayBuffer): void;

        static transferWebResourceHandlerToStatic(input: Any): Object {
            let wrhObject: WebResourceHandler = new WebResourceHandler();
            if (input === undefined || input === null) {
                throw new Error("input object is null or undefined");
            }
            let dyESValue = ESValue.wrap(input);
            if (!WebResourceHandler.transferWebResourceHandlerToStaticInner(wrhObject, dyESValue)) {
                throw new Error("transfer failed");
            }
            return wrhObject as Object;
        }
        private static native transferWebResourceHandlerToStaticInner(newDemo: WebResourceHandler, input: ESValue): boolean;

        static transferWebResourceHandlerToDynamic(input: Object): Any {
            const staticRef = input as WebResourceHandler;
            if (staticRef) {
                let addr = staticRef.nativePtr;
                let module = ESValue.load("@ohos.web.webview");
                let initFunc = module.getProperty("__createWebResourceHandlerTransfer__");
                if (initFunc) {
                    let dynamicRef = initFunc.invoke(ESValue.wrap(addr));
                    return dynamicRef.unwrap();
                } else {
                    console.error("invoke func not exit");
                }
            }
            return undefined;
        }
    }

    export class WebHttpBodyStream {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebHttpBodyStream");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }

        native constructor();
        native isChunked(): boolean;
        native isInMemory(): boolean;
        native isEof(): boolean;
        native getSize():long;
	native getPosition():long;
        native initialize(): Promise<void>;
        native read(size: int): Promise<ArrayBuffer>;
    }

    export class WebDownloadItem {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebDownloadItem")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();

        native getUrl(): string;
        native getGuid(): string;
        native getPercentComplete(): int;
        native getSuggestedFileName(): string;
        native getMethod(): string;
        native getMimeType(): string;
        native getCurrentSpeed(): int;
        native getReceivedBytes(): long;
        native getFullPath(): string;
        native getTotalBytes(): long;
        native getLastErrorCode(): WebDownloadErrorCode;
        native getState(): WebDownloadState;

        native start(downloadPath: string): void;
        native cancel(): void;
        native pause(): void;
        native resume(): void;

        native serializeInternal(): ArrayBuffer;
        serialize(): Uint8Array{
            let buffer = this.serializeInternal();
            let uint8Array = new Uint8Array(buffer);
            return uint8Array;
        }

        native static deserializeInternal(arrayBuffer: ArrayBuffer): WebDownloadItem;
        static deserialize(serializedData: Uint8Array): WebDownloadItem {
            let buffer = serializedData.buffer;
            let item = WebDownloadItem.deserializeInternal(buffer);
            return item;
        }
    }

    export class WebDownloadDelegate {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebDownloadDelegate")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();

        native onBeforeDownload(callback : (item: WebDownloadItem) => void): void;
        native onDownloadFinish(callback : (item: WebDownloadItem) => void): void;
        native onDownloadUpdated(callback : (item: WebDownloadItem) => void): void;
        native onDownloadFailed(callback : (item: WebDownloadItem) => void): void;
    }

    export class WebDownloadManager {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebDownloadManager")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native static resumeDownload(webDownloadItem: WebDownloadItem): void;
        native static setDownloadDelegate(delegate: WebDownloadDelegate): void;
    }

    export class BackForwardCacheSupportedFeatures {
        /**
         * Whether cache the pages that use native embed.
         * Default is false;
         *
         * @type { boolean }
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        nativeEmbed: boolean;
        /**
         * Whether cache the pages that use media take over.
         * Default is false;
         *
         * @type { boolean }
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        mediaTakeOver: boolean;
        /**
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        constructor() {};
    }

    export class BackForwardCacheOptions {

        size: int = 1;
        timeToLive: int = 600;

        constructor() {};
    }

    export class WebCookieManager {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        /**
         * Save the cookies synchronously.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        native static saveCookieSync(): void;

        native static existCookie(incognito?: boolean): boolean;
        native static isCookieAllowed(): boolean;
        native static clearSessionCookieSync(): void;
        native static configCookieSync(url: string, value: string, incognito: boolean, includeHttpOnly: boolean): void;
        native static putAcceptCookieEnabled(accept: boolean): void;
        native static putAcceptThirdPartyCookieEnabled(accept: boolean): void;
        native static isThirdPartyCookieAllowed(): boolean;

        native static configCookieSyncInternal(url: string, value: string, incognito?: boolean): void;
        native static setLazyInitializeWebEngine(lazy: boolean): void;

        static configCookieSync(url: string, value: string, incognito?: boolean): void {
            WebCookieManager.configCookieSyncInternal(url, value, incognito);
        }

        static configCookie(url: string, value: string, callback: AsyncCallback<void>): void {
            let pCallback = taskpool.execute((): undefined => {
                WebCookieManager.configCookieSync(url, value, false, false);
                return undefined
        });
            pCallback.then((e :Any)=>{
                callback(null, undefined);
            }, (err: Error): void => {
                let error: BusinessError = err as BusinessError;
                callback(error, undefined);
            });
        }



        static configCookie(url: string, value: string): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.configCookieSync(url, value, false, false);
                    return undefined});
                pPromise1.then((e :Any): void=>{
                    console.log("in Promise then. e is ", e)
                    resolve(Promise.resolve());
                }, (err:Error): void => {
                    reject(err);
                });
            });
            return pPromise;
        }

        static configCookie(url: string, value: string, incognito: boolean, includeHttpOnly: boolean): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.configCookieSync(url, value, incognito, includeHttpOnly);
                    return undefined});
                pPromise1.then((e :Any): void=>{
                    console.log("in Promise then. e is ", e)
                    resolve(Promise.resolve());
                }, (err:Error): void => {
                    reject(err);
                });
            });
            return pPromise;
        }

        /**
         * Save the cookies Asynchronously.
         * @returns { Promise<void> } - A promise resolved after the cookies have been saved.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
         * <br>2. Incorrect parameter types. 3.Parameter verification failed.
         * @syscap SystemCapability.Web.Webview.Core
         * @atomicservice
         * @since 20
         */
        static saveCookieAsync(): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: undefined) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.saveCookieSync();  return undefined});
                pPromise1.then((e :Any): void=>{
                    console.log("in Promise then. e is ", e)
                    resolve(undefined);
                }, (err:Error): void => {
                    reject(err);
                });
            });
            return pPromise;
        }

        /**
         * Save the cookies Asynchronously.
         * @param { AsyncCallback<void> } callback - Called after the cookies have been saved.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
         * <br>2. Incorrect parameter types. 3.Parameter verification failed.
         * @syscap SystemCapability.Web.Webview.Core
         * @atomicservice
         * @since 20
         */
        static saveCookieAsync(callback: AsyncCallback<void>): void {
            let pCallback = taskpool.execute(():undefined => {WebCookieManager.saveCookieSync();  return undefined});
            pCallback.then((e :Any)=>{
                callback(null, undefined);
            }, (err: Error): void => {
                let error: BusinessError = err as BusinessError;
                callback(error, undefined);
            });
        }

        /**
         * Remove all cookies.
         *
         * @param { boolean } incognito - {@code true} remove all cookies in incognito mode;
         *                                {@code false} otherwise.
         * @syscap SystemCapability.Web.Webview.Core
         * @atomicservice
         * @since 20
         */
        native static clearAllCookiesSync(incognito?: boolean): void;

        /**
         * Remove all cookies Asynchronously.
         * @returns { Promise<void> } - A promise resolved after the cookies have been deleted.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
         * @syscap SystemCapability.Web.Webview.Core
         * @crossplatform
         * @atomicservice
         * @since 20
         */
        static clearAllCookies(): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: undefined) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.clearAllCookiesSync();  return undefined});
                pPromise1.then((e :Any): void=>{
                    console.log("in Promise then. e is ", e)
                    resolve(undefined);
                }, (err:Error): void => {
                    reject(err);
                });
            });
            return pPromise;
        }

        /**
         * Remove all cookies Asynchronously.
         * @param { AsyncCallback<void> } callback - Called after the cookies have been deleted.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
         * <br>2. Incorrect parameter types.
         * @syscap SystemCapability.Web.Webview.Core
         * @crossplatform
         * @atomicservice
         * @since 20
         */
        static clearAllCookies(callback: AsyncCallback<void>): void {
            let pCallback = taskpool.execute(():undefined => {
                WebCookieManager.clearAllCookiesSync();  return undefined});
            pCallback.then((e :Any)=>{
                callback(null, undefined);
            }, (err: Error): void => {
                let error: BusinessError = err as BusinessError;
                callback(error, undefined);
            });
        }

        native static fetchCookieSync(url: string, incognito?: boolean): string;
        static fetchCookie(url: string): Promise<string> {
            let pPromise = new Promise<string>((resolve: (v: string) => void, reject: (error: Error) => void) => {
                let pPromise1 = taskpool.execute((): string => {
                    return WebCookieManager.fetchCookieSync(url);
                });
                pPromise1.then((e: Any) => {
                    let r: string = e as string;
                    resolve(r);
                }, (error: Error): void => {
                });
            });
            return pPromise;
        }
        static fetchCookie(url: string, callback: AsyncCallback<string>): void {
            let pCallback = taskpool.execute((): string => {
                return WebCookieManager.fetchCookieSync(url);
            });
            pCallback.then((e: Any) => {
                let data: string = e as string;
                callback(null, data);
            }, (err: Error): void => {
                let error: BusinessError = err as BusinessError;
                callback(error, undefined);
            });
        }

        static clearSessionCookie(callback: AsyncCallback<void>): void {
            let pCallback = taskpool.execute(():undefined => {WebCookieManager.clearSessionCookieSync(); return undefined});
            pCallback.then((e :Any) => {
                callback(null, undefined);
            }, (err: Error): void => {
                let error: BusinessError = err as BusinessError;
                callback(error, undefined);
            });
        }

        static clearSessionCookie(): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: undefined) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.clearSessionCookieSync();  return undefined});
                pPromise1.then((e: Any): void => {
                    console.log("in Promise then. e is ", e)
                    resolve(undefined);
                }, (err: Error): void => {
                    reject(err);
                });
            });
            return pPromise;
        }

        static fetchCookie(url: string, incognito: boolean): Promise<string> {
            let pPromise = new Promise<string>((resolve: (v: string) => void, reject: (error: Error) => void) => {
                let pPromise1 = taskpool.execute((): string => {
                    return WebCookieManager.fetchCookieSync(url, incognito);
                });
                pPromise1.then((e: Any) => {
                    let r: string = e as string;
                    resolve(r);
                }, (error: Error): void => {
                });
            });
            return pPromise;
        }
    }

    enum ProxySchemeFilter {
        /**
         * This indicates all the schemes will use the proxy.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        MATCH_ALL_SCHEMES = 0,
        /**
         * This indicates only the HTTP requests will use the proxy.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        MATCH_HTTP = 1,
        /**
         * This indicates only the HTTPS requests will use the proxy.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        MATCH_HTTPS = 2
    }

    export class ProxyRule {
        private url: string;
        private filter: ProxySchemeFilter;

        constructor(url: string, filter: ProxySchemeFilter) {
            this.url = url;
            this.filter = filter;
        }

        /**
         * Returns the scheme filter used for this rule.
         *
         * @returns { ProxySchemeFilter } The scheme filter used for this rule.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        getSchemeFilter(): ProxySchemeFilter {
            return this.filter;
        }

        /**
         * Returns the proxy URL.
         *
         * @returns { string } The proxy URL.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        getUrl(): string {
            return this.url;
        }
    }

    export class ProxyConfig {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "ProxyConfig");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }

        native constructor();
        /**
         * Insert a proxy rule which indicates that requests matching the schemeFilter should use an override proxy, all requests will
         * use the proxy rule if schemeFilter is null.
         *
         * The format for proxy is [scheme://]host[:port]. Scheme is optional and must be HTTP, HTTPS, or SOCKS if present. Scheme defaults to HTTP.
         * Host is an IPv6 literal with brackets, an IPv4 literal or one or more labels seperated by a period. Port number is optional and defaults
         * to 80 for HTTP, 443 for HTTPS and 1080 for SOCKS.
         *
         * e.g. example.com host: example.com
         *      https://example.com  scheme: https  host: example.com
         *      example.com:8888     host: example.com  port: 8888
         *      https://example.com:8888  scheme:https  host: example.com  port:8888
         *      192.168.1.1  host: 192.168.1.1
         *      192.168.1.1:8888  host:192.168.1.1 port: 8888
         *      [10:20:30:40:50:60:70:80]
         *
         * @param { string } proxyRule - The proxy rule.
         * @param { ProxySchemeFilter } schemeFilter - The scheme filter for this rule.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Mandatory parameters are left unspecified.
         * <br>2. Incorrect parameter types.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        native insertProxyRule(proxyRule: string, schemeFilter?: ProxySchemeFilter): void;
        /**
         * Returns the proxy rules.
         *
         * @returns { Array<ProxyRule> } The proxy rules.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        native getProxyRules(): Array<ProxyRule>;
        native insertBypassRule(bypassRule: string): void;
        native insertDirectRule(schemeFilter?: ProxySchemeFilter): void;
        native bypassHostnamesWithoutPeriod(): void;
        native clearImplicitRules(): void;
        native enableReverseBypass(reverse: boolean): void;
        native getBypassRules(): Array<string>;
        native isReverseBypassEnabled(): boolean;
    }

    type WebMessage = ArrayBuffer | string;
    export interface WebMessagePort {
        isExtentionType?: boolean;
        close(): void;
        postMessageEvent(message: WebMessage): void;
        onMessageEvent(callback: (result: WebMessage) => void): void;
        postMessageEventExt(message: WebMessageExt): void;
        onMessageEventExt(callback: (result: WebMessageExt) => void): void;
    }
    export class WebMessagePortInner implements WebMessagePort {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;
        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebMessagePort")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }
        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        constructor(){};
        isExtentionType?: boolean;
        native close(): void;
        native postMessageEvent(message: WebMessage): void;
        native onMessageEvent(callback: (result: WebMessage) => void): void;
        native postMessageEventExt(message: WebMessageExt): void;
        native onMessageEventExt(callback: (result: WebMessageExt) => void): void;

        static transferWebMessagePortToStatic(input: Any): Object {
            let wmpObject: WebMessagePortInner = new WebMessagePortInner();
            if (input === undefined || input === null) {
                throw new Error("input object is null or undefined");
            }
            let dyESValue = ESValue.wrap(input);
            let isExtentionType = dyESValue.getProperty('isExtentionType').toBoolean();
            if (!WebMessagePortInner.transferWebMessagePortToStaticInner(wmpObject, dyESValue, isExtentionType)) {
                throw new Error("transfer failed");
            }
            return wmpObject as Object;
        }
        private static native transferWebMessagePortToStaticInner(newDemo: WebMessagePortInner, input: ESValue, extType: boolean): boolean;

        static transferWebMessagePortToDynamic(input: Object): Any {
            const staticRef = input as WebMessagePortInner;
            let extType: boolean = false;
            if (staticRef.isExtentionType === true) {
                extType = true;
            }
            if (staticRef) {
                let addr = staticRef.nativePtr;
                let module = ESValue.load("@ohos.web.webview");
                let initFunc = module.getProperty("__createWebMessagePortTransfer__");
                if (initFunc) {
                    let dynamicRef = initFunc.invoke(ESValue.wrap(extType), ESValue.wrap(addr));
                    return dynamicRef.unwrap();
                } else {
                    console.error("invoke func not exit");
                }
            }
            return undefined;
        }
    }

    export interface WebStorageOrigin {
        origin: string;
        usage: double;
        quota: double;
    }

    export class WebStorageOriginInner implements WebStorageOrigin {
        origin: string = "";
        usage: double = 0;
        quota: double = 0;
    }

    export class WebStorage {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        native static getOriginsSync(): Array<WebStorageOrigin>;
        native static getOriginQuotaSync(origin: string): double;
        native static getOriginUsageSync(origin: string): double;
        native static deleteAllData(incognito?: boolean): void;
        native static deleteOrigin(origin: string): void;
        static getOrigins(): Promise<Array<WebStorageOrigin>> {
            let p = new Promise<Array<WebStorageOrigin>>((resolve: (v: Array<WebStorageOrigin>) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginsSync);
                p1.then((e : Any) => {
                    let r = e as Array<WebStorageOrigin>;
                    resolve(r);
                }).catch((error: Error): Array<WebStorageOrigin> => {
                    reject(error);
                });
            });
            return p;
        }

        static getOrigins(callback: AsyncCallback<Array<WebStorageOrigin>>): void {
            let p = taskpool.execute(WebStorage.getOriginsSync);
            p.then((e : Any) => {
                let r = e as Array<WebStorageOrigin>;
                let err : BusinessError<void> = new BusinessError<void>();
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                let data = new Array<WebStorageOrigin>;
                callback(err, data);
            });
        }

        static getOriginQuota(origin: string): Promise<double> {
            let p = new Promise<double>((resolve: (v: double) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginQuotaSync, origin);
                p1.then((e : Any) => {
                    let r = e as double;
                    resolve(r);
                }).catch((error: Error): double => {
                    reject(error);
                });
            });
            return p;
        }

        static getOriginQuota(origin: string, callback: AsyncCallback<double>): void {
            let p = taskpool.execute(WebStorage.getOriginQuotaSync, origin);
            p.then((e : Any) => {
                let r = e as double;
                let err : BusinessError<void> = new BusinessError<void>();
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, -1);
            });
        }

        static getOriginUsage(origin: string): Promise<double> {
            let p = new Promise<double>((resolve: (v: double) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginUsageSync, origin);
                p1.then((e : Any) => {
                    let r = e as double;
                    resolve(r);
                }).catch((error: Error): double => {
                    reject(error);
                });
            });
            return p;
        }

        static getOriginUsage(origin: string, callback: AsyncCallback<double>): void {
            let p = taskpool.execute(WebStorage.getOriginUsageSync, origin);
            p.then((e : Any) => {
                let r = e as double;
                let err : BusinessError<void> = new BusinessError<void>();
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, -1);
            });
        }
    }

    export class AdsBlockManager {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }

        native static clearAdsBlockAllowedList(): void;
        native static clearAdsBlockDisallowedList(): void;
        native static removeAdsBlockDisallowedList(domainSuffixes: Array<string>): void;
        native static addAdsBlockDisallowedList(domainSuffixes: Array<string>): void;
        native static removeAdsBlockAllowedList(domainSuffixes: Array<string>): void;
        native static addAdsBlockAllowedList(domainSuffixes: Array<string>): void;
        native static setAdsBlockRules(rulesFile: string, replace: boolean): void;
    }

    export class WebDataBase {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebDataBase")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();
        native static existHttpAuthCredentials(): boolean;
        native static deleteHttpAuthCredentials(): void;
        native static getHttpAuthCredentials(host: string, realm: string): Array<string>;
        native static saveHttpAuthCredentials(host: string, realm: string, username: string, password: string): void;
    }

    export class GeolocationPermissions {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "GeolocationPermissions")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();
        native static deleteAllGeolocation_inner(incognito: boolean): void;
        native static getOriginsSync(incognito:boolean): Array<string>;
        native static deleteGeolocation_inner(origin: string, incognito: boolean): void;
        native static allowGeolocation_inner(origin: string, incognito: boolean): void;
        native static getAccessibleGeolocationSync(origin: string, incognito:boolean): boolean;
        static deleteAllGeolocation(incognito?: boolean): void{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            GeolocationPermissions.deleteAllGeolocation_inner(incognitoMode);
        }

        static deleteGeolocation(origin: string, incognito?: boolean): void{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            GeolocationPermissions.deleteGeolocation_inner(origin,incognitoMode);
        }

        static allowGeolocation(origin: string, incognito?: boolean): void{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            GeolocationPermissions.allowGeolocation_inner(origin,incognitoMode);
        }

        static getStoredGeolocation(incognito?: boolean):Promise<Array<string>>{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            let p = new Promise<Array<string>>((resolve: (v: Array<string>) => void,
                reject: (error: Error) => void) => {
                let p1 = taskpool.execute(GeolocationPermissions.getOriginsSync,incognitoMode);
                p1.then((e : Any) => {
                    let r = e as Array<string>;
                    resolve(r);
                }).catch((error: Error): Array<string> => {
                    reject(error);
                });
            });
            return p;
        }

        static getStoredGeolocation(callback:AsyncCallback<Array<string>>,incognito?: boolean):void{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            let p = taskpool.execute(GeolocationPermissions.getOriginsSync,incognitoMode);
            p.then((e : Any) => {
                let r = e as Array<string>;
                let err:  BusinessError = {};
                err.code = 0;
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, []);
            });
        }


        static getAccessibleGeolocation(origin: string, incognito?: boolean):Promise<boolean>{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            let p = new Promise<boolean>((resolve: (v: boolean) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(GeolocationPermissions.getAccessibleGeolocationSync, origin, incognitoMode);
                p1.then((e : Any) => {
                    let r = e as boolean;
                    resolve(r);
                }).catch((error: Error): boolean => {
                    reject(error);
                });
            });
            return p;
        }

        static getAccessibleGeolocation(origin: string, callback:AsyncCallback<boolean>,incognito?: boolean):void{
            let incognitoMode:boolean = false;
            if(incognito !== undefined)
            {
                incognitoMode = incognito;
            }
            let p = taskpool.execute(GeolocationPermissions.getAccessibleGeolocationSync, origin, incognitoMode);
            p.then((e : Any) => {
                let r = e as boolean;
                let err:  BusinessError = {};
                err.code = 0;
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, false);
            });
        }
    }

    export enum WebMessageType {
        NOT_SUPPORT,
        STRING,
        NUMBER,
        BOOLEAN,
        ARRAY_BUFFER,
        ARRAY,
        ERROR
    }

    export class WebMessageExt {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "WebMessageExt")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native constructor();

        native setType(type: WebMessageType): void;
        native setString(message: string): void;
        native setNumber(message: double): void;
        native setBoolean(message: boolean): void;
        native setArrayBuffer(message: ArrayBuffer): void;
        native setArray(message: Array<string | double | long | boolean>): void;
        native setError(message: Error): void;

        native getType(): WebMessageType;
        native getString(): string;
        native getNumber(): double | long;
        native getBoolean(): boolean;
        native getArrayBuffer(): ArrayBuffer;
        native getArray(): Array<string | double | long | boolean>;
        native getError(): Error;
    }

    export enum JsMessageType {
        NOT_SUPPORT,
        STRING,
        NUMBER,
        BOOLEAN,
        ARRAY_BUFFER,
        ARRAY
    }
    export class JsMessageExt {
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }

        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "JsMessageExt")
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr)
            }
        }

        native getType(): JsMessageType;
        native getString(): string;
        native getNumber(): double | long;
        native getBoolean(): boolean;
        native getArrayBuffer(): ArrayBuffer;
        native getArray(): Array<string | double | long | boolean>;
    }

    export class PrefetchOptions {
        minTimeBetweenPrefetchesMs: int;
        ignoreCacheControlNoStore: boolean;

        constructor() {};
    }
    type OnProxyConfigChangeCallback = () => void;
    export class ProxyController {
        private nativePtr: long = 0;
        private cleaner?: Cleaner;

        registerCleaner(ptr: long): void {
            this.cleaner = new Cleaner(ptr, "ProxyController");
            destroyRegister.register(this, this.cleaner!, unregisterToken);
        }
        unregisterCleaner(): void {
            destroyRegister.unregister(unregisterToken);
        }

        bindNativePtr(ptr: long): void {
            if (this.nativePtr == 0) {
                this.nativePtr = ptr;
                this.registerCleaner(this.nativePtr);
            }
        }

        native constructor();

        native static applyProxyOverride(proxyConfig: ProxyConfig, callback: OnProxyConfigChangeCallback): void;
        native static removeProxyOverride(callback: OnProxyConfigChangeCallback): void;
    }

    export class PrintDocumentAdapterInner implements print.PrintDocumentAdapter{
        static {
            loadLibraryWithPermissionCheck("webview_ani.z", "@ohos.web.webview");
        }
        private nativePtr: long = 0;
            private cleaner?: Cleaner;

            registerCleaner(ptr: long): void {
                this.cleaner = new Cleaner(ptr, "PrintDocumentAdapterInner");
                destroyRegister.register(this, this.cleaner!, unregisterToken);
            }
            unregisterCleaner(): void {
                destroyRegister.unregister(unregisterToken);
            }

            bindNativePtr(ptr: long): void {
                if (this.nativePtr == 0) {
                    this.nativePtr = ptr;
                    this.registerCleaner(this.nativePtr)
                }
            }

        native onStartLayoutWrite(jobId: string, oldAttrs: print.PrintAttributes, newAttrs: print.PrintAttributes, fd: int,
                    writeResultCallback: (jobId: string, writeResult: print.PrintFileCreationState) => void): void;

        native onJobStateChanged(jobId: string, state: print.PrintDocumentAdapterState): void;
    }
}
