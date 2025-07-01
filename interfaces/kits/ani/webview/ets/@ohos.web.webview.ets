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
    export enum ScrollType {
        EVENT
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
        EditText,
        Email,
        HttpAnchor,
        HttpAnchorImg,
        Img,
        Map,
        Phone,
        Unknown
    }

    export enum RenderProcessMode{
        SINGLE = 0,
        MULTIPLE = 1
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

    export interface ScrollOffset {
        x: number;
        y: number;
    }
    export class ScrollOffsetInner implements ScrollOffset {
        x: number = 0;
        y: number = 0;
    }

    export interface HitTestValue {
        type: WebHitTestType;
        extra: string;
    }
    export class HitTestValueInner implements HitTestValue {
        type: WebHitTestType = WebHitTestType.Unknown;
        extra: string = "";
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
    }
 
    export class SnapshotResultClass implements SnapshotResult {
        id?: string;
        status?: boolean;
        imagePixelMap?: image.PixelMap;
    }
 
    export interface SnapshotInfo {
        id?: string;
    }
 
    export class SnapshotInfoClass implements SnapshotInfo {
        id?: string;
    }

    export interface RectEvent {
        x: number;
        y: number;
        width: number;
        height: number;
    }

    export class RectEventinner implements RectEvent {
        x: number = 0;
        y: number = 0;
        width: number = 0;
        height: number = 0;
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
        handleVolumeChanged(volume: number): void
        handleMutedChanged(muted: boolean): void
        handlePlaybackRateChanged(playbackRate: number): void
        handleDurationChanged(duration: number): void
        handleTimeUpdate(currentPlayTime: number): void
        handleBufferedEndTimeChanged(bufferedEndTime: number): void
        handleEnded(): void
        handleNetworkStateChanged(state: NetworkState): void
        handleReadyStateChanged(state: ReadyState): void
        handleFullscreenChanged(fullscreen: boolean): void
        handleSeeking(): void
        handleSeekFinished(): void
        handleError(error: MediaError, errorMessage: string): void
        handleVideoSizeChanged(width: number, height: number): void
    }

    export class NativeMediaPlayerHandlerinner implements NativeMediaPlayerHandler {
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
        native handleVolumeChanged(volume: number): void
        native handleMutedChanged(muted: boolean): void
        native handlePlaybackRateChanged(playbackRate: number): void
        native handleDurationChanged(duration: number): void
        native handleTimeUpdate(currentPlayTime: number): void
        native handleBufferedEndTimeChanged(bufferedEndTime: number): void
        native handleEnded(): void
        native handleNetworkStateChanged(state: NetworkState): void
        native handleReadyStateChanged(state: ReadyState): void
        native handleFullscreenChanged(fullscreen: boolean): void
        native handleSeeking(): void
        native handleSeekFinished(): void
        native handleError(error: MediaError, errorMessage: string): void
        native handleVideoSizeChanged(width: number, height: number): void
    }

    export interface NativeMediaPlayerBridge {
        updateRect(x: number, y: number, width: number, height: number): void
        play(): void
        pause(): void
        seek(targetTime: number): void
        setVolume(volume: number): void
        setMuted(muted: boolean): void
        setPlaybackRate(playbackRate: number): void
        release(): void
        enterFullscreen(): void
        exitFullscreen(): void
        resumePlayer?: ResumePlayerFn;
        suspendPlayer?: SuspendPlayerFn;
    }

    type SuspendPlayerFn = (type: SuspendType) => void
    type ResumePlayerFn = () => void

    export interface BackForwardList {
        currentIndex: int;
        size: int;
        getItemAtIndex(index: int): HistoryItem;
    }

    export class BackForwardListinner implements BackForwardList {
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
    }

    export class WebviewController {
        static {
            loadLibrary("webview_ani.z");
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
        native _setNWebId(nwebId: number): void;
        native _setHapPath(hapPath: string): void;

        native static initializeWebEngine(): void;
        native loadUrl(url: string | Resource, headers?: Array<WebHeader>): void;

        native onActive(): void;
        native onInactive(): void;
        native getWebId(): number;
        native getScrollable(): boolean;
        native requestFocus(): void;
        native clearClientAuthenticationCache(): void;
        native scrollByWithResult(deltaX: number, deltaY: number): boolean;
        native setScrollable(enable: boolean, type?: ScrollType): void;
        native scrollTo(x: number, y: number, duration?: number): void;
        native scrollBy(deltaX: number, deltaY: number, duration?: number): void;
        native getScrollOffset(): ScrollOffset;
        native slideScroll(vx: number, vy: number): void;
        native zoom(factor: number): void;
        native isSafeBrowsingEnabled(): boolean;
        native static prepareForPageLoad(url: string, preconnectable: boolean, numSockets: number): void;
        native getFavicon(): image.PixelMap;
        native pageDown(bottom: boolean): void;
        native pageUp(top: boolean): void;
        native isAdsBlockEnabledForCurPage(): boolean;
        native isIntelligentTrackingPreventionEnabled(): boolean;
        native static addIntelligentTrackingPreventionBypassingList(hostList: Array<string>): void;
        native static removeIntelligentTrackingPreventionBypassingList(hostList: Array<string>): void;
        native static clearIntelligentTrackingPreventionBypassingList(): void;
        native static setHostIP(hostName: string, address: string, aliveTime: number): void;
        native static clearHostIP(hostName: string): void;
        native static warmupServiceWorker(url: string): void;
        native zoomOut(): void;
        native zoomIn(): void;
        native getLastHitTest(): HitTestValue;
        native getPageHeight(): number;
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
        native accessStep(step: number): boolean;
        native isAdsBlockEnabled(): boolean;
        native enableAdsBlock(enable: boolean): void;
        native postUrl(url: string, postData: ArrayBuffer): void;
        native getUrl(): string;
        native getTitle(): string;
        native getOriginalUrl(): string;
        native getUserAgent(): string;
        native getCustomUserAgent(): string;
        native getLastJavascriptProxyCallingFrameUrl(): string;
        native forward(): void;
        native backward(): void;
        native accessForward(): boolean;
        native accessBackward(): boolean;
        native loadData(data: string, mimeType: string, encoding: string, baseUrl?: string, historyUrl?: string): void;
        native clearHistory(): void;
        native clearWebSchemeHandler(): void;
        native terminateRenderProcess(): boolean;
        native static setConnectionTimeout(timeout: number): void;
        native backOrForward(step: number): void;
        native static setWebDebuggingAccess(webDebuggingAccess: boolean): void;
        native enableSafeBrowsing(enable: boolean): void;
        native static setRenderProcessMode(mode: RenderProcessMode): void;
        native static getRenderProcessMode(): RenderProcessMode;
        native static pauseAllTimers(): void;
        native static resumeAllTimers(): void;
        native setCustomUserAgent(userAgent: string): void;
        native removeCache(clearRom: boolean): void;
        native setNetworkAvailable(enable: boolean): void;
        native isIncognitoMode(): boolean;
        native serializeWebState(): Uint8Array;
        native static trimMemoryByPressureLevel(level: PressureLevel): void;
        native setPathAllowingUniversalAccess(pathList: Array<string>): void;
        native onCreateNativeMediaPlayer(callback: (handler: NativeMediaPlayerHandler, mediaInfo: MediaInfo) => NativeMediaPlayerBridge): void;

        native getSurfaceId(): string;
        native static enableWholeWebPageDrawing(): void;
        native setPrintBackground(enable: boolean): void;
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
        native precompileJavaScriptInternal(url: string, script: string | ArrayBuffer, cacheOptions: CacheOptions): number;

        precompileJavaScript(url: string, script: string | Uint8Array, cacheOptions: CacheOptions): Promise<number> {
            let resScript: string | ArrayBuffer
            if (typeof script === 'string') {
                resScript = script as string;
            } else if (script instanceof Uint8Array) {
                resScript = script.buffer as ArrayBuffer;
            }
            let p = new Promise<number>((resolve: (v: number) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(this.precompileJavaScriptInternal, url, resScript, cacheOptions);
                p1.then((e : NullishType) => {
                    let r = e as number;
                    resolve(r);
                }).catch((error: Error): number => {
                    reject(error);
                });
            });
            return p;
        }
        native static prefetchResource(request: RequestInfo, additionalHeaders?: Array<WebHeader>, cacheKey?: string, cacheValidTime?: number): void;
        native startCamera(): void;
        native closeAllMediaPresentations(): void;
        native stopAllMedia(): void;
        native stopCamera(): void;
        native closeCamera(): void;
        native pauseAllMedia(): void;
        native resumeAllMedia(): void;
        native setAudioMuted(mute: boolean): void;
        native getMediaPlaybackState(): MediaPlaybackState;
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

        /**
         * Set the resolved URL after redirects or changed as a result of HSTS.
         *
         * @param { string } url - Set response url for redirects.
         * @throws { BusinessError } 401 - Parameter error. Possible causes: 1. Incorrect parameter types.
         * @syscap SystemCapability.Web.Webview.Core
         * @atomicservice
         * @since 20
         */
        native setUrl(url: string): void;

        /**
         * Get the resolved URL after redirects or changed as a result of HSTS.
         *
         * @returns { string } Return response url for redirects.
         * @syscap SystemCapability.Web.Webview.Core
         * @atomicservice
         * @since 20
         */
        native getUrl(): string;
    }

    export class WebDownloadItem {
        private url: string;

        constructor(url: string) {
            this.url = url;
        }

        /**
         * Get url of the web download request.
         * @returns { string } - Returns the url.
         * @syscap SystemCapability.Web.Webview.Core
         * @crossplatform
         * @atomicservice
         * @since 20
         */
        getUrl(): string {
            return this.url;
        }
    }

    export class WebDownloadDelegate {
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

        /**
         * Callback will be triggered before web download start.
         * @param { Callback<WebDownloadItem> } callback - The callback of download will be start.
         * @syscap SystemCapability.Web.Webview.Core
         * @crossplatform
         * @atomicservice
         * @since 20
         */
        native onBeforeDownload(callback : (item: WebDownloadItem) => void): void;
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
        /**
         * Set the maximum size of pages that can cache.
         * Default is 1, max is 50.
         *
         * @type { number }
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        size: number;
        /**
         * Set the lifetime in seconds in the BackForwardCache.
         * Default is 600.
         *
         * @type { number }
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        timeToLive: number;
        /**
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        constructor() {};
    }

    export class WebCookieManager {
        /**
         * Save the cookies synchronously.
         * @syscap SystemCapability.Web.Webview.Core
         * @since 20
         */
        native static saveCookieSync(): void;

        native static configCookieSync(url: string, value: string, incognito: boolean, includeHttpOnly: boolean): void;

        native static configCookieSyncInternal(url: string, value: string, incognito?: boolean): void;

        static configCookieSync(url: string, value: string, incognito?: boolean): void {
            WebCookieManager.configCookieSyncInternal(url, value, incognito);
        }

        static configCookie(url: string, value: string, callback: AsyncCallback<void>): void {
            let pCallback = taskpool.execute((): undefined => {
                WebCookieManager.configCookieSync(url, value, false, false);
                return undefined
        });
            pCallback.then((e :NullishType)=>{
                let resultCode: Int  = 0;
                let r: BusinessError = { code: resultCode.toInt(), data: undefined };
                callback(r, undefined);
            }, (err: Object): void => {
            });
        }



        static configCookie(url: string, value: string): Promise<void> {
            let pPromise = new Promise<void>((resolve: (v: PromiseLike<void>) => void,
                                              reject: (error: Error) => void): void => {
                let pPromise1 = taskpool.execute(():undefined => {
                    WebCookieManager.configCookieSync(url, value, false, false);
                    return undefined});
                pPromise1.then((e :NullishType): void=>{
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
                pPromise1.then((e :NullishType): void=>{
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
                pPromise1.then((e :NullishType): void=>{
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
            pCallback.then((e :NullishType)=>{
                let resultCode: Int  = 0;
                let r: BusinessError = { code: resultCode.toInt(), data: undefined };
                callback(r, undefined);
            }, (err:Error): void => {
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
                pPromise1.then((e :NullishType): void=>{
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
            pCallback.then((e :NullishType)=>{
                let resultCode: Int  = 0;
                let r: BusinessError = { code: resultCode.toInt(), data: undefined };
                callback(r, undefined);
            }, (err:Error): void => {
            });
        }

        native static fetchCookieSync(url: string, incognito?: boolean): string;
        static fetchCookie(url: string): Promise<string> {
            let pPromise = new Promise<string>((resolve: (v: string) => void, reject: (error: Error) => void) => {
                let pPromise1 = taskpool.execute((): string => {
                    return WebCookieManager.fetchCookieSync(url);
                });
                pPromise1.then((e: NullishType) => {
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
            pCallback.then((e: NullishType) => {
                let data: string = e as string;
                let resultCode: Int = 0;
                let error: BusinessError = { code: resultCode.toInt(), data: undefined };
                callback(error, data);
            }, (err: Error): void => {
            });
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
            this.cleaner = new Cleaner(ptr, "ProxyConfig")
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
    }

    export interface WebStorageOrigin {
        origin: string;
        usage: number;
        quota: number;
    }

    export class WebStorageOriginInner implements WebStorageOrigin {
        origin: string = "";
        usage: number = 0;
        quota: number = 0;
    }

    export class WebStorage {
        static {
            loadLibrary("webview_ani.z");
        }
        native static getOriginsSync(): Array<WebStorageOrigin>;
        native static getOriginQuotaSync(origin: string): number;
        native static getOriginUsageSync(origin: string): number;
        native static deleteAllData(incognito?: boolean): void;
        native static deleteOrigin(origin: string): void;
        static getOrigins(): Promise<Array<WebStorageOrigin>> {
            let p = new Promise<Array<WebStorageOrigin>>((resolve: (v: Array<WebStorageOrigin>) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginsSync);
                p1.then((e : NullishType) => {
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
            p.then((e : NullishType) => {
                let r = e as Array<WebStorageOrigin>;
                let err : BusinessError<void> = new BusinessError<void>();
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                let data = new Array<WebStorageOrigin>;
                callback(err, data);
            });
        }

        static getOriginQuota(origin: string): Promise<number> {
            let p = new Promise<number>((resolve: (v: number) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginQuotaSync, origin);
                p1.then((e : NullishType) => {
                    let r = e as number;
                    resolve(r);
                }).catch((error: Error): number => {
                    reject(error);
                });
            });
            return p;
        }

        static getOriginQuota(origin: string, callback: AsyncCallback<number>): void {
            let p = taskpool.execute(WebStorage.getOriginQuotaSync, origin);
            p.then((e : NullishType) => {
                let r = e as number;
                let err : BusinessError<void> = new BusinessError<void>();
                callback(err, r);
            }).catch((error: Error) => {
                let err = error as BusinessError<void>;
                callback(err, -1);
            });
        }

        static getOriginUsage(origin: string): Promise<number> {
            let p = new Promise<number>((resolve: (v: number) => void,
            reject: (error: Error) => void) => {
                let p1 = taskpool.execute(WebStorage.getOriginUsageSync, origin);
                p1.then((e : NullishType) => {
                    let r = e as number;
                    resolve(r);
                }).catch((error: Error): number => {
                    reject(error);
                });
            });
            return p;
        }

        static getOriginUsage(origin: string, callback: AsyncCallback<number>): void {
            let p = taskpool.execute(WebStorage.getOriginUsageSync, origin);
            p.then((e : NullishType) => {
                let r = e as number;
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
            loadLibrary("webview_ani.z");
        }

        native static clearAdsBlockAllowedList(): void;
        native static clearAdsBlockDisallowedList(): void;
        native static removeAdsBlockDisallowedList(domainSuffixes: Array<string>): void;
        native static addAdsBlockDisallowedList(domainSuffixes: Array<string>): void;
        native static removeAdsBlockAllowedList(domainSuffixes: Array<string>): void;
        native static addAdsBlockAllowedList(domainSuffixes: Array<string>): void;
    }

    export class WebDataBase {
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
            loadLibrary("webview_ani.z");
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
                p1.then((e : NullishType) => {
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
            p.then((e : NullishType) => {
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
                p1.then((e : NullishType) => {
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
            p.then((e : NullishType) => {
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

    export class PdfData {
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

        native constructor();

        native static pdfArrayBuffer(): Uint8Array;
    }
}
