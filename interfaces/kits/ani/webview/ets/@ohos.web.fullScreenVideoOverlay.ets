/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

'use static'

import {
  Flex,
  Text,
  Column,
  Row,
  RowOptions,
  Scroll,
  Image,
  SymbolGlyph,
  Slider,
  Progress,
  ProgressType,
  ProgressStyleOptions,
  Span,
  Stack,
  RelativeContainer,
  XComponent,
  XComponentType,
  XComponentParameters,
  XComponentOptions,
  XComponentController,
  GestureGroup,
  GestureMode,
  TapGesture,
  PanGesture,
  PanDirection,
  DismissDialogAction,
  DismissReason,
  DialogAlignment,
  SliderChangeMode,
  Visibility,
  Divider,
  SideBarContainer,
  SideBarContainerType,
  SideBarPosition,
  ScrollDirection,
  BarState,
  EdgeEffect,
  GradientDirection,
  Direction,
  TextSelectableMode,
  TextOverflow,
  RenderFit,
  GesturePriority,
  GestureJudgeResult,
  LengthMetrics,
  FlexAlign,
  FlexDirection,
  ItemAlign,
  VerticalAlign,
  HorizontalAlign,
  FontWeight,
  TextAlign,
  TransitionEffect,
  Color,
  Curve,
  $r,
  GestureEvent,
  ForEach,
  If,
  Builder,
  Rectangle,
  Component,
  CommonMethod,
  HitTestMode,
  WordBreak,
  TextHeightAdaptivePolicy,
  AlignRuleOption,
  Area
} from '@ohos.arkui.component'
import { BusinessError } from '@ohos.base'
import display from '@ohos.display'
import i18n from '@ohos.i18n'
import util from '@ohos.util'
import resourceManager from '@ohos.resourceManager'
import commonEventManager from '@ohos.commonEventManager'
import { UIContext } from '@ohos.arkui.UIContext'
import { Observed, State, PropRef } from '@ohos.arkui.stateManagement'
import MeasureText from '@ohos.measure'
import ApplicationStateChangeCallback from '@ohos.app.ability.ApplicationStateChangeCallback'


export class ArkwebVideoInfo {
  title: string = ''
  duration: number = 0
  curTime: number = 0
  playbackrate: number = 1
  isPlaying: boolean = false
  muted: number = 0
  width: number = 0
  height: number = 0
}

class OverlayCleaner {
    private ptr: long = 0
    private name: string = ""
    constructor(ptr: long, name: string) {
        this.ptr = ptr
        this.name = name
    }
    native clean(): void
}

function OverlayCleanerCallback(cleaner: OverlayCleaner): void {
    cleaner.clean()
}

let overlayDestroyRegister = new FinalizationRegistry<OverlayCleaner>(OverlayCleanerCallback)
let overlayUnregisterToken = new object()

export class FullScreenVideoOverlayHandlerInner {
    private nativePtr: long = 0;
    private cleaner?: OverlayCleaner;

    registerCleaner(ptr: long): void {
        this.cleaner = new OverlayCleaner(ptr, "FullScreenVideoOverlayHandlerInner")
        overlayDestroyRegister.register(this, this.cleaner!, overlayUnregisterToken);
    }
    unregisterCleaner(): void {
        overlayDestroyRegister.unregister(overlayUnregisterToken);
    }

    bindNativePtr(ptr: long): void {
        if (this.nativePtr == 0) {
            this.nativePtr = ptr;
            this.registerCleaner(this.nativePtr)
        }
    }

    constructor() {};

    native setVideoSurface(surfaceId: string): void;
    native requestMediaControl(action: int, param: string): void;
    native addListener(listener: VideoPlayerListenerInner): void;

    static transferFullScreenVideoOverlayHandlerToStatic(input: Any): Object {
        let handlerObj: FullScreenVideoOverlayHandlerInner = new FullScreenVideoOverlayHandlerInner();
        if (input == undefined || input == null) {
            throw new Error("input object is null or undefined");
        }
        let dyESValue = ESValue.wrap(input);
        if (!FullScreenVideoOverlayHandlerInner.transferFullScreenVideoOverlayHandlerToStaticInner(handlerObj, dyESValue)) {
            throw new Error("transfer failed");
        }
        return handlerObj;
    }
    private static native transferFullScreenVideoOverlayHandlerToStaticInner(
        newDemo: FullScreenVideoOverlayHandlerInner, input: ESValue): boolean;
}

export interface VideoPlayerCallbacks {
    onStatusChanged?: (status: number) => void
    onMutedChanged?: (muted: boolean) => void
    onPlaybackRateChanged?: (playbackRate: number) => void
    onDurationChanged?: (duration: number) => void
    onTimeUpdate?: (currentPlayTime: number) => void
    onBufferedEndTimeChanged?: (bufferedEndTime: number) => void
    onEnded?: () => void
    onVideoSizeChanged?: (width: number, height: number) => void
}

export class VideoPlayerListenerInner {
    private onStatusChangedCallback: ((status: number) => void) | undefined
    private onMutedChangedCallback: ((muted: boolean) => void) | undefined
    private onPlaybackRateChangedCallback: ((playbackRate: number) => void) | undefined
    private onDurationChangedCallback: ((duration: number) => void) | undefined
    private onTimeUpdateCallback: ((currentPlayTime: number) => void) | undefined
    private onBufferedEndTimeChangedCallback: ((bufferedEndTime: number) => void) | undefined
    private onEndedCallback: (() => void) | undefined
    private onVideoSizeChangedCallback: ((width: number, height: number) => void) | undefined

    constructor(callbacks: VideoPlayerCallbacks) {
        this.onStatusChangedCallback = callbacks.onStatusChanged
        this.onMutedChangedCallback = callbacks.onMutedChanged
        this.onPlaybackRateChangedCallback = callbacks.onPlaybackRateChanged
        this.onDurationChangedCallback = callbacks.onDurationChanged
        this.onTimeUpdateCallback = callbacks.onTimeUpdate
        this.onBufferedEndTimeChangedCallback = callbacks.onBufferedEndTimeChanged
        this.onEndedCallback = callbacks.onEnded
        this.onVideoSizeChangedCallback = callbacks.onVideoSizeChanged
    }

    onStatusChanged(status: int): void {
        let cb = this.onStatusChangedCallback
        if (cb !== undefined) { cb(status) }
    }
    onMutedChanged(muted: boolean): void {
        let cb = this.onMutedChangedCallback
        if (cb !== undefined) { cb(muted) }
    }
    onPlaybackRateChanged(playbackRate: number): void {
        let cb = this.onPlaybackRateChangedCallback
        if (cb !== undefined) { cb(playbackRate) }
    }
    onDurationChanged(duration: number): void {
        let cb = this.onDurationChangedCallback
        if (cb !== undefined) { cb(duration) }
    }
    onTimeUpdate(currentPlayTime: number): void {
        let cb = this.onTimeUpdateCallback
        if (cb !== undefined) { cb(currentPlayTime) }
    }
    onBufferedEndTimeChanged(bufferedEndTime: number): void {
        let cb = this.onBufferedEndTimeChangedCallback
        if (cb !== undefined) { cb(bufferedEndTime) }
    }
    onEnded(): void {
        let cb = this.onEndedCallback
        if (cb !== undefined) { cb() }
    }
    onVideoSizeChanged(width: int, height: int): void {
        let cb = this.onVideoSizeChangedCallback
        if (cb !== undefined) { cb(width, height) }
    }
}

const PLAY_RATE_LIST: number[] = [0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 3.0]
const INVALID_PARAM: string = 'invalid param'

enum PlaybackStatus {
  PAUSED = 0,
  PLAYING = 1,
  BUFFERING = 2,
  BUFFERED = 3,
}

enum MediaControlAction {
  PLAY = 0,
  PAUSE = 1,
  SEEK = 2,
  SET_MUTED = 3,
  SET_PLAYBACK_RATE = 4,
  EXIT = 5,
}

enum MutedStatus {
  MUTED = 0,
  UNMUTED = 1,
  DISABLE = 2,
}

enum DialogComponentType {
  None = 0,
  SeekComponent = 1,
}

enum VideoPlayerComponentDisplayType {
  HIDDEN = 0,
  VISIBLE = 1,
  PLAYBACK_RATE_MENU = 2,
}

enum VideoStateGestureManagerType {
  SEEK_MANAGER = 0,
}

function parseSecTimeToLocalTime(secTime: number): string {
  let minute = Math.floor(secTime / 60);
  let secondLeft = Math.floor(secTime % 60);
  let hour = Math.floor(minute / 60);
  let minuteLeft = Math.floor(minute % 60);
  if (hour === 0) {
    return `${minuteLeft.toString().padStart(2, '0')}:${secondLeft.toString().padStart(2, '0')}`;
  } else {
    return `${hour.toString().padStart(2, '0')}:${minuteLeft.toString().padStart(2, '0')}:${secondLeft.toString().padStart(2, '0')}`;
  }
}

function formatRateNumber(rate: number, valuePattern: string): string {
  try {
    let locale = i18n.System.getSystemLocaleInstance().baseName;
    let latnLocale = locale + '-u-nu-latn';
    let formatNum = new Intl.NumberFormat(latnLocale, {
      minimumFractionDigits: 1,
      maximumFractionDigits: 2,
    }).format(rate);
    return util.format(valuePattern, formatNum);
  } catch (e) {
    return rate.toString();
  }
}

function isZhLocale(): boolean {
  let lang = i18n.System.getSystemLanguage();
  return lang.split('-')[0].toLowerCase() === 'zh';
}

// Obtain the actual content displayed at the speedup ratio.
function getSpeedContent(currentPlaybackRate: number, speedValuePattern: string, speedButtonText: string): string {
  if (currentPlaybackRate === 1.0) {
    return speedButtonText;
  }
  return formatRateNumber(currentPlaybackRate, speedValuePattern);
}

function scaleTransition() {
  return TransitionEffect.scale({ x: 0.95, y: 0.95 })
      .animation({ curve: Curve.Friction, delay: 75, duration: 415 })
      .combine(TransitionEffect.opacity(0));
}

@Observed
export class VideoPlayerState {
  isPlaying: boolean = false
  currentSecond: number = 0
  totalSecond: number = 0
  volume: number = 0
  isMuted: number = MutedStatus.UNMUTED
  currentPlaybackRate: number = 1
  title: string = ''
  bufferedEndTime: number = 0
  displayLoading: boolean = false
  dialogComponent: number = DialogComponentType.None
  isShowDownload: boolean = true
  isShowPlaybackSpeed: boolean = true
  width: number = 0
  height: number = 0
  displayWidthVp: number = 0
  isPortrait: boolean = false
  displayState: number = VideoPlayerComponentDisplayType.HIDDEN
}

class SeekManager {
  private _state: VideoPlayerState;
  private _player: FullScreenVideoOverlayHandlerInner | null;
  private _gestureStartValue: number = 0;

  constructor(state: VideoPlayerState, player: FullScreenVideoOverlayHandlerInner | null) {
    this._state = state;
    this._player = player;
  }

  gestureStart(): void {
    this._gestureStartValue = this._state.currentSecond;
    this._state.dialogComponent = DialogComponentType.SeekComponent;
  }

  gestureUpdate(offsetX: number, width: number): void {
    const minReference = 120;
    const DAMPING = this._state.totalSecond <= minReference ? 0.2 : 0.4;
    const referenceDuration = Math.max(this._state.totalSecond, minReference);
    const delta = Math.floor((offsetX * DAMPING / width) * referenceDuration);
    let newVal = this._gestureStartValue + delta;
    newVal = Math.max(0, Math.min(this._state.totalSecond, newVal));
    this._state.currentSecond = newVal;
  }

  gestureEnd(): void {
    console.info(`[FullscreenVideoOverlay] seek gesture end:${this._state.currentSecond}.`);
    if (this._player) {
      this._player!.requestMediaControl(MediaControlAction.SEEK, String(this._state.currentSecond));
    }
  }

  gestureRestore(): void {}
}

class VideoGestureManager {
  private _managerMap: Map<int, SeekManager> = new Map<int, SeekManager>();
  private _currentManager: SeekManager | null = null;
  private _displayTimer: int | null = null;
  private _state: VideoPlayerState;

  constructor(state: VideoPlayerState, player: FullScreenVideoOverlayHandlerInner | null) {
    this._state = state;
    this._managerMap.set(VideoStateGestureManagerType.SEEK_MANAGER, new SeekManager(state, player));
  }

  startGesture(type: int): void {
    this._currentManager = this._managerMap.get(type) ?? null;
    this._currentManager?.gestureStart();
    if (this._displayTimer !== null) {
      clearTimeout(this._displayTimer);
    }
  }

  updateGesture(offset: number, total: number): void {
    this._currentManager?.gestureUpdate(offset, total);
  }

  endGesture(): void {
    this._currentManager?.gestureEnd();
    this._displayTimer = setTimeout(() => {
      this._state.dialogComponent = DialogComponentType.None
    }, 1000);
    this._currentManager = null;
  }

  restoreGesture(): void {
    this._managerMap.forEach(manager => {
      manager.gestureRestore();
    });
  }
}

export interface DialogController {
  close: () => void
}

export class VideoPlayerLogic {
  private state: VideoPlayerState;
  private handler: FullScreenVideoOverlayHandlerInner | null;
  private gestureManager: VideoGestureManager;
  private seekDisplayTimer: int | null = null;
  private allBarsDisplayTimer: int | null = null;
  dialogController: DialogController | null = null;

  constructor(state: VideoPlayerState, handler: FullScreenVideoOverlayHandlerInner | null) {
    this.state = state;
    this.handler = handler;
    this.gestureManager = new VideoGestureManager(state, handler);
  }

  switchPlayingStatus(): void {
    console.info(`[FullscreenVideoOverlay] switchPlayingStatus isPlaying:${this.state.isPlaying}.`)
    if (!this.handler) {
      console.warn(`[FullscreenVideoOverlay] switchPlayingStatus handler is null.`);
      return;
    }
    if (this.state.isPlaying) {
      this.handler!.requestMediaControl(MediaControlAction.PAUSE, INVALID_PARAM);
      this.state.isPlaying = false;
      if (this.allBarsDisplayTimer !== null) {
        clearTimeout(this.allBarsDisplayTimer);
      }
    } else {
      this.handler!.requestMediaControl(MediaControlAction.PLAY, INVALID_PARAM);
      this.state.isPlaying = true;
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        this.resetBarsTimer();
      }
    }
  }

  setMute(mutedStatus: number): void {
    if (mutedStatus === MutedStatus.DISABLE) {
      return;
    }
    if (this.handler) {
      let param = mutedStatus === MutedStatus.MUTED ? '1' : '0';
      this.handler!.requestMediaControl(MediaControlAction.SET_MUTED, param);
      this.state.isMuted = mutedStatus;
    }
  }

  exitFullScreen(): void {
    if (this.handler) {
      this.handler!.requestMediaControl(MediaControlAction.EXIT, INVALID_PARAM);
      const dc = this.dialogController;
      if (dc) {
        dc.close()
      }
      console.info(`[FullscreenVideoOverlay] exitfullscreen`);
    } else {
      console.warn(`[FullscreenVideoOverlay] request to exitfullscreen failed.`);
    }
  }

  playbackProgressChange(currentSecond: number, mode: SliderChangeMode): void {
    this.state.currentSecond = currentSecond;
    if (mode === SliderChangeMode.Begin) {
      this.showSeekComponent();
    } else if (mode === SliderChangeMode.End || mode === SliderChangeMode.Click) {
      this.hideSeekComponent();
      if (this.handler) {
        this.handler!.requestMediaControl(MediaControlAction.SEEK, String(currentSecond));
      }
      this.resetBarsTimer();
    }
  }

  showSeekComponent(): void {
    if (this.seekDisplayTimer !== null) {
      clearTimeout(this.seekDisplayTimer);
    }
    this.state.dialogComponent = DialogComponentType.SeekComponent;
  }

  hideSeekComponent(): void {
    this.seekDisplayTimer = setTimeout(() => {
      this.state.dialogComponent = DialogComponentType.None;
    }, 1000);
  }

  showBars(): void {
    if (this.allBarsDisplayTimer !== null) {
      clearTimeout(this.allBarsDisplayTimer);
    }
    this.state.displayState = VideoPlayerComponentDisplayType.VISIBLE;
    if (this.state.isPlaying) {
      this.allBarsDisplayTimer = setTimeout(() => {
        this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
      }, 5000);
    }
  }

  hideBars(): void {
    if (this.allBarsDisplayTimer !== null) {
      clearTimeout(this.allBarsDisplayTimer);
    }
    this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
  }

  resetBarsTimer(): void {
    if (this.state.isPlaying) {
      if (this.allBarsDisplayTimer !== null) {
        clearTimeout(this.allBarsDisplayTimer);
      }
      this.allBarsDisplayTimer = setTimeout(() => {
        this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
      }, 5000);
    }
  }

  openPlayRateMenu(): void {
    if (this.allBarsDisplayTimer !== null) {
      clearTimeout(this.allBarsDisplayTimer);
    }
    this.state.displayState = VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU;
  }

  closePlaybackRateMenu(): void {
    if (this.allBarsDisplayTimer !== null) {
      clearTimeout(this.allBarsDisplayTimer);
    }
    this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
  }

  switchPlaybackRate(rate: number): void {
    if (this.state.currentPlaybackRate === rate) {
      return;
    }
    if (this.handler) {
      this.handler!.requestMediaControl(MediaControlAction.SET_PLAYBACK_RATE, String(rate));
      this.state.currentPlaybackRate = rate;
    }
  }

  gestureStart(mode: int): void {
    this.gestureManager.startGesture(mode);
  }

  gestureUpdate(offset: number, total: number): void {
    this.gestureManager.updateGesture(offset, total);
  }

  gestureEnd(): void {
    this.gestureManager.endGesture();
  }

  gestureRestore(): void {
    this.gestureManager.restoreGesture();
  }
}

class FullScreenVideoOverlayDialog {
  private customDialogComponentId: int = 0;

  defaultOnFullScreenVideoOverlayEnter(
    handler: FullScreenVideoOverlayHandlerInner,
    mediaInfo: string
  ): void {
    console.info('[FullscreenVideoOverlay] defaultOnFullScreenVideoOverlayEnter start');
    try {
      let videoEntity: ArkwebVideoInfo = new ArkwebVideoInfo();
      try {
        let parsed: ArkwebVideoInfo | null | undefined =
          JSON.parse<ArkwebVideoInfo>(mediaInfo, Class.from<ArkwebVideoInfo>());
        if (parsed != null && parsed != undefined) {
          videoEntity = parsed;
        }
      } catch (err) {
        console.error(`[FullscreenVideoOverlay] parse mediaInfo failed: ${err}`);
        return;
      }

      const state = new VideoPlayerState();
      const logic = new VideoPlayerLogic(state, handler);

      // Initialize state
      state.title = videoEntity.title;
      state.totalSecond = videoEntity.duration;
      state.currentSecond = videoEntity.curTime;
      state.currentPlaybackRate = videoEntity.playbackrate;
      state.isPlaying = videoEntity.isPlaying;
      state.isMuted = videoEntity.muted;
      state.width = videoEntity.width
      state.height = videoEntity.height
      state.isShowPlaybackSpeed = true;

      logic.showBars();

      let windowSize: display.Display | null = null;
      try {
        windowSize = display.getDefaultDisplaySync();
      } catch (err) {
        console.error(`[FullscreenVideoOverlay] getDefaultDisplaySync failed: ${err}`);
      }
      if (windowSize == null) {
        console.error('[FullscreenVideoOverlay] getDefaultDisplaySync returned null');
        return;
      }
      state.displayWidthVp = UIContext.resolveUIContext()?.px2vp(windowSize.width);
      state.isPortrait = windowSize.height > windowSize.width;
      const adaptPadding = 16;

      let promptAction = UIContext.resolveUIContext()?.getPromptAction()
      if (promptAction) {
        promptAction.openCustomDialog({
          builder: (): void => this.fullScreenVideoDialog(state, handler, logic, videoEntity, adaptPadding),
          autoCancel: false,
          alignment: DialogAlignment.Center,
          maskColor: Color.Black,
          backgroundColor: Color.Transparent,
          cornerRadius: 0,
          shadow: { radius: 0 },
          width: '100%',
          height: '100%',
          backgroundBlurStyle: undefined,
          transition: TransitionEffect.IDENTITY,
          onWillDismiss: (dismissDialogAction: DismissDialogAction): void => {
            console.info('[FullscreenVideoOverlay] dialog onWillDismiss reason: ' + dismissDialogAction.reason)
            if (dismissDialogAction.reason === DismissReason.PRESS_BACK ||
                dismissDialogAction.reason === DismissReason.TOUCH_OUTSIDE) {
              logic.exitFullScreen()
            }
          }
        })
        .then((dialogId) => {
          this.customDialogComponentId = dialogId;
          logic.dialogController = { close: (): void => { this.closeDialog() } };
        })
        .catch((error) => {
          console.error(`[FullscreenVideoOverlay] openCustomDialog failed: ${error.code}`);
        })
      }
    } catch (err) {
      console.error(`[FullscreenVideoOverlay] defaultOnFullScreenVideoOverlayEnter error: ${err}`);
    }
  }

  private closeDialog(): void {
    if (this.customDialogComponentId > 0) {
      let promptAction = UIContext.resolveUIContext()?.getPromptAction()
      if (promptAction) {
        promptAction.closeCustomDialog(this.customDialogComponentId);
      }
    }
  }

  @Builder
  fullScreenVideoDialog(
    state: VideoPlayerState,
    handler: FullScreenVideoOverlayHandlerInner,
    logic: VideoPlayerLogic,
    videoEntity: ArkwebVideoInfo,
    adaptPadding: number
  ): void {
    FullScreenVideoDialogContent({
      state: state,
      handler: handler,
      logic: logic,
      videoEntity: videoEntity,
      adaptPadding: adaptPadding
    })
  }
}

@Component
struct FullScreenVideoDialogContent {
  @State state: VideoPlayerState = new VideoPlayerState()
  handler: FullScreenVideoOverlayHandlerInner | null = null
  logic: VideoPlayerLogic | null = null
  videoEntity: ArkwebVideoInfo = new ArkwebVideoInfo()
  adaptPadding: number = 16
  private xComponentController: XComponentController = new XComponentController()

  build() {
    Stack() {
      RelativeContainer() {
      // XComponent with gesture handling
      Stack() {
        XComponent({ type: XComponentType.SURFACE, controller: this.xComponentController })
        .onLoad(() => {
          let surfaceId = this.xComponentController.getXComponentSurfaceId()
          console.log(`[FullscreenVideoOverlay] XComponent surfaceId:${surfaceId}`)
          const handler = this.handler
          if (handler) {
            handler.setVideoSurface(surfaceId)
          }
          this.state.width = this.videoEntity.width
          this.state.height = this.videoEntity.height
          const listener = new VideoPlayerListenerInner({
            onStatusChanged: (s: number): void => {
              console.info(`[FullscreenVideoOverlay] video status change: ${s}.`)
              if (s == PlaybackStatus.PLAYING) {
                this.state.isPlaying = true
              } else if (s == PlaybackStatus.PAUSED) {
                this.state.isPlaying = false
              } else if (s == PlaybackStatus.BUFFERING) {
                this.state.displayLoading = true
              } else if (s == PlaybackStatus.BUFFERED) {
                this.state.displayLoading = false
              } else {
                console.error('[FullscreenVideoOverlay] unsupported playback state.')
              }
            },
            onMutedChanged: (muted: boolean): void => {
              console.info(`[FullscreenVideoOverlay] video muted changed: ${muted}.`)
              this.state.isMuted = muted ? MutedStatus.MUTED : MutedStatus.UNMUTED
            },
            onPlaybackRateChanged: (playbackRate: number): void => {
              this.state.currentPlaybackRate = playbackRate
            },
            onDurationChanged: (duration: number): void => {
              if (!isFinite(duration) || isNaN(duration)) {
                this.state.totalSecond = 0
                return
              }
              this.state.totalSecond = duration
            },
            onTimeUpdate: (currentPlayTime: number): void => {
              if (this.state.dialogComponent !== DialogComponentType.SeekComponent) {
                this.state.currentSecond = currentPlayTime
              }
            },
            onBufferedEndTimeChanged: (bufferedEndTime: number): void => {
              this.state.bufferedEndTime = bufferedEndTime
            },
            onEnded: (): void => {
              this.state.isPlaying = false
              const logic = this.logic
              if (logic) {
                logic.showBars()
                setTimeout((): void => {
                  logic.exitFullScreen()
                }, 1000)
              }
            },
            onVideoSizeChanged: (w: number, h: number): void => {
              this.state.width = w
              this.state.height = h
            },
          })
          const handler2 = this.handler
          if (handler2) {
            handler2.addListener(listener)
          }
        })
        .onDestroy(() => {
          console.info('[FullscreenVideoOverlay] XComponent onDestroy')
        })
        .aspectRatio((this.state.width > 0 && this.state.height > 0) ? this.state.width / this.state.height : -1)
        .renderFit(RenderFit.RESIZE_FILL)
      }
      .width('100%')
      .height('100%')
      .backgroundColor(Color.Black)
      .gesture(
        GestureGroup(GestureMode.Exclusive,
          GestureGroup(GestureMode.Exclusive,
            TapGesture({ count: 2 })
              .onAction((): void => {
                const logic = this.logic
                if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
                  if (logic) {
                    logic.closePlaybackRateMenu()
                  }
                } else {
                  if (logic) {
                    logic.switchPlayingStatus()
                  }
                }
              }),
            TapGesture({ count: 1 })
              .onAction((): void => {
                const logic = this.logic
                if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
                  if (logic) {
                    logic.closePlaybackRateMenu()
                  }
                } else if (this.state.displayState === VideoPlayerComponentDisplayType.HIDDEN) {
                  if (logic) {
                    logic.showBars()
                  }
                } else {
                  if (logic) {
                    logic.hideBars()
                  }
                }
              })
          ),
          PanGesture({ direction: PanDirection.Horizontal })
            .onActionStart((): void => {
              const logic = this.logic
              if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU ||
                  this.state.totalSecond <= 0) {
                return
              }
              if (logic) {
                logic.gestureStart(VideoStateGestureManagerType.SEEK_MANAGER)
              }
            })
            .onActionUpdate((event: GestureEvent): void => {
              const logic = this.logic
              if (event && logic) {
                logic.gestureUpdate(event.offsetX, this.state.displayWidthVp)
              }
            })
            .onActionEnd((): void => {
              const logic = this.logic
              if (logic) {
                logic.gestureEnd()
              }
            })
            .onActionCancel((): void => {
              const logic = this.logic
              if (logic) {
                logic.gestureEnd()
              }
            })
        )
      )

      // Title bar
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        VideoPlayerTitle({
          logic: this.logic,
          adaptPadding: this.adaptPadding,
          state: this.state
        })
        .alignRules({ top: { anchor: '__container__', align: VerticalAlign.Top } } as AlignRuleOption)
        .accessibilityLevel('no')
      }

      // Center seek overlay
      CenterOverlay({ state: this.state })
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center },
        } as AlignRuleOption)

      // Playback rate menu
      if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
        PlaybackRateMenu({
          logic: this.logic,
          isPortrait: this.state.isPortrait,
          state: this.state
        })
        .width(200)
        .alignRules({ right: { anchor: '__container__', align: HorizontalAlign.End } } as AlignRuleOption)
        .accessibilityLevel('no')
      }

      // Bottom control bar
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        Column() {
          BottomControllerToolBar({
            logic: this.logic,
            adaptPadding: this.adaptPadding,
            state: this.state
          })
        }
        .height(80)
        .alignRules({ bottom: { anchor: '__container__', align: VerticalAlign.Bottom } } as AlignRuleOption)
        .accessibilityLevel('no')
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.Black)
    }
    .width('100%')
    .height('100%')
  }
}

function pressedStyles(instance: CommonMethod): void {
  instance.backgroundColor('#40FFFFFF')
    .borderRadius(20)
}

function normalStyles(instance: CommonMethod): void {
  instance.backgroundColor('#1AFFFFFF')
    .borderRadius(20)
}

@Component
struct VideoPlayerTitle {
  @PropRef state: VideoPlayerState
  logic: VideoPlayerLogic | null = null
  adaptPadding: number = 16

  build() {
    Column() {
      Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center }) {
        Row() {
          Row() {
            SymbolGlyph($r('sys.symbol.chevron_backward'))
              .fontSize('24vp')
              .fontWeight(FontWeight.Regular)
              .fontColor([$r('sys.color.ohos_id_color_text_primary_dark')])
          }
          .width('40vp')
          .height('40vp')
          .justifyContent(FlexAlign.Center)
          .alignItems(VerticalAlign.Center)
          .margin({ end: LengthMetrics.vp(8) })
          .responseRegion({ x: 0, y: 0, width: '48vp', height: '56vp' })
          .stateStyles({ pressed: pressedStyles, normal: normalStyles })
          .onClick((): void => {
            console.log('VideoPlayerTitle back icon onClick')
            const logic = this.logic
            if (logic) {
              logic.exitFullScreen()
            }
          })
          .id('btnBack')

          Text(this.state.title)
            .maxFontSize('20fp')
            .minFontSize(14)
            .heightAdaptivePolicy(TextHeightAdaptivePolicy.MIN_FONT_SIZE_FIRST)
            .maxLines(2)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
            .wordBreak(WordBreak.BREAK_WORD)
            .fontColor('#E6FFFFFF')
            .fontWeight(FontWeight.Bold)
            .textAlign(TextAlign.Start)
            .height(50)
            .flexShrink(1)
            .accessibilityText(this.state.title)
            .onClick((): void => {
              const logic = this.logic
              if (logic) {
                logic.resetBarsTimer()
              }
            })
        }

        // Keep this empty Row as a second Flex child: it gives the title Row a
        // definite width budget so minFontSize/maxFontSize + ellipsis engage and
        // the title shrinks instead of overflowing in portrait. Mirrors
        // webview_export.js. Do not remove.
        Row({ space: 16 } as RowOptions) {
        }
        .flexShrink(0)
        .margin({ start: LengthMetrics.vp(8) })
      }
      .width('100%')
      .height(56)
      .transition(this.scaleTransition())
    }
    .height(80)
    .width('100%')
    .padding({ top: 0, left: this.adaptPadding, right: this.adaptPadding })
    .linearGradient({ colors: [['#99000000', 0], ['#00000000', 1]] })
    .transition(TransitionEffect.OPACITY.animation({ duration: 415 }))
  }

  scaleTransition() {
    return TransitionEffect.scale({ x: 0.95, y: 0.95 })
      .animation({ curve: Curve.Friction, delay: 150, duration: 415 })
      .combine(TransitionEffect.opacity(0));
  }
}

@Component
struct CenterOverlay {
  @PropRef state: VideoPlayerState

  build() {
    Stack() {
      if (this.state.dialogComponent === DialogComponentType.SeekComponent) {
        SeekComponent({ state: this.state })
          .alignRules({
            center: { anchor: '__container__', align: VerticalAlign.Center },
            middle: { anchor: '__container__', align: HorizontalAlign.Center },
          } as AlignRuleOption)
      }
    }
  }
}

@Component
struct SeekComponent {
  @PropRef state: VideoPlayerState
  @State progressWidth: number = 0;

  build() {
    Column() {
      Text() {
        Span(parseSecTimeToLocalTime(this.state.currentSecond))
          .fontColor('rgba(255,255,255, 0.9)')
          .fontSize(18)
        Span(`/${parseSecTimeToLocalTime(this.state.totalSecond)}`)
          .baselineOffset(new LengthMetrics(2))
          .fontColor('rgba(255,255,255, 0.6)')
          .fontSize(12)
      }
      .margin({ bottom: 8 })
      .onAreaChange((_oldVal: Area, newVal: Area) => {
        this.progressWidth = newVal.width as number;
      })

      Progress({ value: this.state.currentSecond, total: this.state.totalSecond, type: ProgressType.Linear })
        .style({ enableSmoothEffect: false, strokeWidth: 2 } as ProgressStyleOptions)
        .width(this.progressWidth)
        .backgroundColor('rgba(255,255,255, 0.2)')
        .color('rgba(255,255,255, 0.9)')
        .direction(Direction.Ltr)
    }
    .backgroundColor('rgba(0, 0, 0, 0.6)')
    .borderRadius(8)
    .padding(16)
  }
}

@Component
struct PlaybackRateMenu {
  @PropRef state: VideoPlayerState
  logic: VideoPlayerLogic | null = null
  isPortrait: boolean = false
  @State speedValuePattern: string = '%sx'
  private subscriber: commonEventManager.CommonEventSubscriber | null = null
  private appStateCallback: ApplicationStateChangeCallback | null = null

  private setResource(): void {
    try {
      let manager = resourceManager.getSysResourceManager()
      this.speedValuePattern = manager.getStringByNameSync('media_video_playback_speed_value_pattern')
    } catch (e) {
      console.error('[FullscreenVideoOverlay] Failed to load speed_value_pattern: ' + e)
    }
  }

  aboutToAppear(): void {
    this.setResource()
    let that = this
    // Subscribe to locale change events
    try {
      let sub = commonEventManager.createSubscriberSync({
        events: [commonEventManager.Support.COMMON_EVENT_LOCALE_CHANGED]
      })
      this.subscriber = sub
      commonEventManager.subscribe(sub, (err: BusinessError | null, data?: commonEventManager.CommonEventData): void => {
        if (err) {
          console.error(`[PlaybackRateMenu] Locale change subscribe error: ${err.code}, ${err.message}`)
          return
        }
        console.info('[PlaybackRateMenu] Locale changed event received.')
        setTimeout((): void => {
          that.setResource()
        }, 200)
      })
    } catch (e) {
      console.error('[PlaybackRateMenu] Failed to create subscriber: ' + e)
    }
    // Subscribe to app foreground events (locale may have changed while backgrounded)
    let onForeground = (): void => {
      that.setResource()
    }
    let appStateCallback: ApplicationStateChangeCallback = {
      onApplicationForeground(): void {
        setTimeout(onForeground, 200)
      },
      onApplicationBackground(): void {}
    }
    this.appStateCallback = appStateCallback
    let applicationContext = UIContext.resolveUIContext()?.getHostContext()?.getApplicationContext()
    if (applicationContext) {
      applicationContext.onApplicationStateChange(appStateCallback)
    }
  }

  aboutToBeDeleted(): void {
    let sub = this.subscriber
    if (sub) {
      this.subscriber = null
      commonEventManager.unsubscribe(sub, (err: BusinessError | null): void => {
        if (err) {
          console.error(`[PlaybackRateMenu] Failed to unsubscribe: ${err.code}, ${err.message}`)
        }
      })
    }
    let callback = this.appStateCallback
    if (callback) {
      let applicationContext = UIContext.resolveUIContext()?.getHostContext()?.getApplicationContext()
      applicationContext?.offApplicationStateChange(callback)
      this.appStateCallback = null
    }
  }

  build() {
    SideBarContainer(SideBarContainerType.Overlay) {
      Column() {
        Stack() {
          // Background gradient mask (replaces ic_side_bar SVG)
          Column() {
          }
          .width(200)
          .height('100%')
          .justifyContent(this.isPortrait ? FlexAlign.Center : FlexAlign.Start)
          .linearGradient({
            direction: GradientDirection.Left,
            colors: [['rgba(0, 0, 0, 0.902)', 0], ['rgba(0, 0, 0, 0.4)', 0.832], ['rgba(0, 0, 0, 0)', 1]]
          })
          .visibility(Visibility.Visible)
          .transition(TransitionEffect.OPACITY.animation({
            delay: 83,
            duration: 1000,
            curve: Curve.Friction
          }))

          // Rate list
          Scroll() {
            Column() {
              ForEach(PLAY_RATE_LIST, (item: number, index: int) => {
                Column() {
                  // Selected item
                  if (this.state.currentPlaybackRate === item) {
                    Row() {
                      Text(formatRateNumber(item, this.speedValuePattern))
                        .fontColor('#5291FF')
                        .fontSize(14)
                        .lineHeight(16)
                    }
                    .height(48)
                    .width(200)
                    .justifyContent(FlexAlign.Center)
                    .linearGradient({
                      direction: GradientDirection.Right,
                      colors: [['rgba(255, 255, 255, 0)', 0], ['rgba(255, 255, 255, 0.2)', 1]]
                    })
                  } else {
                    // Unselected item
                    Row() {
                      Text(formatRateNumber(item, this.speedValuePattern))
                        .fontColor('#E6FFFFFF')
                        .fontSize(14)
                        .lineHeight(16)
                    }
                    .height(48)
                    .width(200)
                    .justifyContent(FlexAlign.Center)
                  }
                  // Divider between items
                  if (index !== PLAY_RATE_LIST.length - 1) {
                    Divider()
                      .height(1)
                      .color(Color.Transparent)
                      .linearGradient({
                        direction: GradientDirection.Right,
                        colors: [['rgba(255, 255, 255, 0)', 0], ['rgba(255, 255, 255, 0.1)', 1]]
                      })
                      .margin({ left: 16, right: 16 })
                      .borderRadius(16)
                  }
                }
                .id(`video_speed_${index}`)
                .transition(this.rateTransition(PLAY_RATE_LIST.length as int, index))
                .onClick((): void => {
                  if (this.state.currentPlaybackRate === item) {
                    return
                  }
                  const logic = this.logic
                  if (logic) {
                    logic.switchPlaybackRate(item)
                    logic.closePlaybackRateMenu()
                  }
                })
              }, (item: number, index: int) => `${item}`)
            }
            .backgroundColor(Color.Transparent)
          }
          .scrollable(ScrollDirection.Vertical)
          .scrollBar(BarState.Off)
          .edgeEffect(EdgeEffect.Spring)
        }
      }
      .justifyContent(FlexAlign.Center)
      .backgroundColor(Color.Transparent)
      .accessibilityLevel('no')
      .onClick(() => {})
    }
    .showControlButton(false)
    .sideBarWidth(200)
    .sideBarPosition(SideBarPosition.End)
    .divider({ strokeWidth: 0 })
  }

  // Staggered entrance/exit transition for rate items
  private rateTransition(length: int, index: int): TransitionEffect {
    return TransitionEffect.asymmetric(
      TransitionEffect.translate({ y: (length - index) * 10 + 10 })
        .animation({ curve: Curve.Friction, delay: 83 + index * 50 as int, duration: 500 })
        .combine(TransitionEffect.opacity(0.01)
          .animation({ curve: Curve.Friction, delay: 83 + index * 50 as int, duration: 500 })),
      TransitionEffect.translate({ y: (length - index) * 10 + 10 })
        .animation({ curve: Curve.Friction, delay: (length - index - 1) * 50 as int, duration: 500 })
        .combine(TransitionEffect.opacity(0.01)
          .animation({ curve: Curve.Friction, delay: (length - index - 1) * 50 as int, duration: 500 }))
    )
  }
}

@Component
struct BottomControllerToolBar {
  @PropRef state: VideoPlayerState
  logic: VideoPlayerLogic | null = null
  adaptPadding: number = 16
  @State speedValuePattern: string = '%sx'
  @State speedButtonText: string = '1.0x'
  private subscriber: commonEventManager.CommonEventSubscriber | null = null
  private appStateCallback: ApplicationStateChangeCallback | null = null

  private setResource(): void {
    try {
      let manager = resourceManager.getSysResourceManager()
      this.speedValuePattern = manager.getStringByNameSync('media_video_playback_speed_value_pattern')
      this.speedButtonText = manager.getStringByNameSync('media_video_playback_speed_button')
    } catch (e) {
      console.error('[FullscreenVideoOverlay] Failed to load speed resources: ' + e)
    }
  }

  aboutToAppear(): void {
    this.setResource()
    let that = this
    // Subscribe to locale change events
    try {
      let sub = commonEventManager.createSubscriberSync({
        events: [commonEventManager.Support.COMMON_EVENT_LOCALE_CHANGED]
      })
      this.subscriber = sub
      commonEventManager.subscribe(sub, (err: BusinessError | null, data?: commonEventManager.CommonEventData): void => {
        if (err) {
          console.error(`[FullscreenVideoOverlay] Locale change subscribe error: ${err.code}, ${err.message}`)
          return
        }
        console.info('[FullscreenVideoOverlay] Locale changed event received.')
        setTimeout((): void => {
          that.setResource()
        }, 200)
      })
    } catch (e) {
      console.error('[FullscreenVideoOverlay] Failed to create subscriber: ' + e)
    }
    // Subscribe to app foreground events (locale may have changed while backgrounded)
    let onForeground = (): void => {
      that.setResource()
    }
    let appStateCallback: ApplicationStateChangeCallback = {
      onApplicationForeground(): void {
        setTimeout(onForeground, 200)
      },
      onApplicationBackground(): void {}
    }
    this.appStateCallback = appStateCallback
    let applicationContext = UIContext.resolveUIContext()?.getHostContext()?.getApplicationContext()
    if (applicationContext) {
      applicationContext.onApplicationStateChange(appStateCallback)
    }
  }

  aboutToBeDeleted(): void {
    let sub = this.subscriber
    if (sub) {
      this.subscriber = null
      commonEventManager.unsubscribe(sub, (err: BusinessError | null): void => {
        if (err) {
          console.error(`[FullscreenVideoOverlay] Failed to unsubscribe: ${err.code}, ${err.message}`)
        }
      })
    }
    let callback = this.appStateCallback
    if (callback) {
      let applicationContext = UIContext.resolveUIContext()?.getHostContext()?.getApplicationContext()
      applicationContext?.offApplicationStateChange(callback)
      this.appStateCallback = null
    }
  }

  private scaleTransition() {
    return TransitionEffect.scale({ x: 0.95, y: 0.95 })
      .animation({ curve: Curve.Friction, delay: 75, duration: 415 })
      .combine(TransitionEffect.opacity(0));
  }

  build() {
    Column() {
      // Progress bar row
      if (this.state.totalSecond > 0) {
        Row() {
          Flex({ direction: FlexDirection.Row, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center }) {
            Text(parseSecTimeToLocalTime(this.state.currentSecond))
              .fontColor('#E6FFFFFF')
              .textAlign(TextAlign.Center)
              .fontSize($r('sys.float.Caption_M'))
              .fontSize(10)
              .lineHeight(12)
              .textSelectable(TextSelectableMode.UNSELECTABLE)
              .maxLines(1)
              .flexShrink(0)
              .textOverflow( { overflow: TextOverflow.None } )
              .flexShrink(0)

            Slider({ value: Math.floor(this.state.currentSecond), max: Math.floor(this.state.totalSecond) })
              .direction(Direction.Ltr)
              .trackColor('#33FFFFFF')
              .selectedColor('#FFFFFF')
              .blockColor('#E5E5E5')
              .blockSize({ width: 10, height: 10 })
              .trackThickness(2)
              .trackBorderRadius(1.5)
              .width('100%')
              .onChange((currentSecond: number, mode: SliderChangeMode): void => {
                const logic = this.logic
                if (logic) {
                  logic.playbackProgressChange(currentSecond, mode)
                }
              })

            Text(parseSecTimeToLocalTime(this.state.totalSecond))
              .fontColor('#E6FFFFFF')
              .textAlign(TextAlign.Center)
              .fontSize(10)
              .lineHeight(12)
              .textSelectable(TextSelectableMode.UNSELECTABLE)
              .maxLines(1)
              .flexShrink(0)
              .textOverflow({ overflow: TextOverflow.None })
              .accessibilityLevel('yes')
          }
          .direction(Direction.Ltr)
          .height(14)
          .width('100%')
          .transition(this.scaleTransition())
        }
        .height(24)
        .zIndex(1)
      }

      // Bottom toolbar
      Row() {
        // Play button
        Row() {
          Image(this.state.isPlaying ? $r('sys.media.ohos_ic_public_pause') : $r('sys.media.ohos_ic_public_play'))
            .width(24)
            .height(24)
            .fillColor('#FFF')
            .accessibilityText(this.state.isPlaying ?
              (isZhLocale() ? '暂停按钮' : 'Pause') :
              (isZhLocale() ? '播放按钮' : 'Play'))
        }
        .responseRegion({ x: -8, y: -8, width: '40vp', height: '40vp' })
        .onClick((): void => {
          console.log('[FullscreenVideoOverlay] toolbar call switchPlayingStatus.')
          const logic = this.logic
          if (logic) {
            logic.switchPlayingStatus()
          }
        })
        .transition(this.scaleTransition())
        .id('btnPlay')

        Row({ space: 24 } as RowOptions) {
          // Playback speed button
          if (this.state.isShowPlaybackSpeed && this.state.totalSecond > 0) {
            Row() {
              Text(getSpeedContent(this.state.currentPlaybackRate, this.speedValuePattern, this.speedButtonText))
                .fontSize(14)
                .fontColor('#FFF')
                .height(24)
                .accessibilityText(getSpeedContent(this.state.currentPlaybackRate, this.speedValuePattern, this.speedButtonText))
            }
            .responseRegion(this.getFontResponseRegion(getSpeedContent(this.state.currentPlaybackRate, this.speedValuePattern, this.speedButtonText), 14))
            .onClick((): void => {
              const logic = this.logic
              if (logic) {
                logic.openPlayRateMenu()
              }
            })
            .transition(this.scaleTransition())
            .id('btnPlaybackRateSwitch')
          }

          // Mute button
          Row() {
            Image(this.state.isMuted === MutedStatus.MUTED ?
              $r('sys.media.ohos_ic_public_sound_off') :
              $r('sys.media.ohos_ic_public_sound'))
              .width(24)
              .height(24)
              .fillColor('#FFF')
              .matchTextDirection(true)
              .accessibilityText(this.state.isMuted === MutedStatus.MUTED ?
                (isZhLocale() ? '解除静音' : 'Unmute') :
                (isZhLocale() ? '设置静音' : 'Mute'))
          }
          .transition(this.scaleTransition())
          .responseRegion({ x: -8, y: -8, width: '40vp', height: '40vp' })
          .onClick((): void => {
            if (this.state.isMuted === MutedStatus.DISABLE) {
              return
            }
            const logic = this.logic
            if (logic) {
              logic.setMute(this.state.isMuted === MutedStatus.MUTED ? MutedStatus.UNMUTED : MutedStatus.MUTED)
            }
          })
          .id('btnMute')
        }
      }
      .height(40)
      .justifyContent(FlexAlign.SpaceBetween)
      .width('100%')
      .margin({ bottom: 8 })
    }
    .linearGradient({ colors: [['#00000000', 0], ['#99000000', 1]] })
    .padding({ left: this.adaptPadding, right: this.adaptPadding })
    .justifyContent(FlexAlign.End)
    .height('100%')
    .width('100%')
    .transition(TransitionEffect.OPACITY.animation({ duration: 415 }))
    .accessibilityLevel('no')
    .onClick((): void => { const logic = this.logic; if (logic) { logic.resetBarsTimer() } })
  }

  // Obtains the hot zone range in multiple languages.
  // The hot zone height is fixed at 40 vp and the width is adaptive.
  getFontResponseRegion(content: string, size: number): Rectangle {
    const fontWidth: number = UIContext.resolveUIContext()?.px2vp(MeasureText.measureText({
        textContent: content,
        fontSize: size,
    }));
    return {
        x: -8, y: -8, width: fontWidth + 16, height: 40
    };
  }
}

export { FullScreenVideoOverlayDialog }
