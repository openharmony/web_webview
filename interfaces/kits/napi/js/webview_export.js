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

let cert = requireInternal('security.cert');
let webview = requireInternal('web.webview');
let fileIo = requireNapi('file.fs');
let fileUri = requireNapi('file.fileuri');
let picker = requireNapi('file.picker');
let photoAccessHelper = requireNapi('file.photoAccessHelper');
let cameraPicker = requireNapi('multimedia.cameraPicker');
let camera = requireNapi('multimedia.camera');
let accessControl = requireNapi('abilityAccessCtrl');
let avsession = requireNapi('multimedia.avsession');
let deviceinfo = requireInternal('deviceInfo');
let promptAction = requireNapi('promptAction');
let dataShare = requireNapi('data.dataShare');
let webNativeMessagingExtensionManager = requireNapi('web.webnativemessagingextensionmanager_napi');
let { LengthMetrics } = requireNapi('arkui.node');
let commonEventManager = requireNapi('commonEventManager');
let resourceManager = requireNapi('resourceManager');
let MeasureText = requireNapi('measure');
let display = requireNapi('display');
let i18n = requireNapi('i18n');
let util = requireNapi('util');
const curves = requireNativeModule('ohos.curves');

const PARAM_CHECK_ERROR = 401;

const ERROR_MSG_INVALID_PARAM = 'Invalid input parameter';

let errMsgMap = new Map();
errMsgMap.set(PARAM_CHECK_ERROR, ERROR_MSG_INVALID_PARAM);
let customDialogComponentId = 0;
let onShowFileSelectorEvent = undefined;

let defaultPublicPath = '/storage/Users/currentUser/';

let publicDirectoryMap = new Map([
    ['desktop', defaultPublicPath + 'desktop'],
    ['documents', defaultPublicPath + 'documents'],
    ['downloads', defaultPublicPath + 'download'],
    ['music', defaultPublicPath + 'music'],
    ['pictures', defaultPublicPath + 'images'],
    ['videos', defaultPublicPath + 'videos'],
]);

class BusinessError extends Error {
  constructor(code, errorMsg = 'undefined') {
    if (errorMsg === 'undefined') {
      let msg = errMsgMap.get(code);
      super(msg);
    } else {
      super(errorMsg);
    }
    this.code = code;
  }
}

export let AVCastPickerState;
(function(pickerState) {
    pickerState[pickerState.STATE_APPEARING = 0] = 'STATE_APPEARING';
    pickerState[pickerState.STATE_DISAPPEARING = 1] = 'STATE_DISAPPEARING';
})(AVCastPickerState || (AVCastPickerState = {}));

const PLAY_RATE_LIST = [0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 3.0];
const SINGLE_CLICK_TAG = 'singleClickTag';
const SINGLE_HORIZONTAL_TAG = 'singleHorizontal';
const INVALID_PARAM = 'invalid param';

// Video playback status enum.
const PlaybackStatus = {
  PAUSED: 0,
  PLAYING: 1,
  BUFFERING: 2,
  BUFFERED: 3,
  MUTED: 4,
  PLAYBACK_RATE_CHANGED: 5,
  DURATION_CHANGED: 6,
  UNKNOWN: 12
}

// Video control action enum.
const MediaControlAction = {
  PLAY: 0,
  PAUSE: 1,
  SEEK: 2,
  SET_MUTED: 3,
  SET_PLAYBACK_RATE: 4,
  EXIT: 5
}

// Media muted status enum.
const MutedStatus = {
  MUTED: 0,
  UNMUTED: 1,
  DISABLE: 2
}

// Dialog component type enum.
const DialogComponentType = {
  None: 0,
  SeekComponent: 1
}

// Component display type enum.
const VideoPlayerComponentDisplayType = {
  HIDDEN: 0, // Hide all components.
  VISIBLE: 1, // Only show title and bottom bar.
  PLAYBACK_RATE_MENU: 2, // Show the play rate menu.
}

// Gesture manager type Enum.
const VideoStateGestureManagerType = {
  SEEK_MANAGER: 0
}

// Format rate number with locale-aware pattern from resources.
function formatRateNumber(rate, valuePattern, context) {
    try {
        let locale = context?.resourceManager?.getConfiguration()?.locale?.[0]
            || i18n.System.getSystemLocale();
        let latnLocale = locale + '-u-nu-latn';
        let formatNum = new Intl.NumberFormat(latnLocale, {
            minimumFractionDigits: 1,
            maximumFractionDigits: 2,
        }).format(rate);
        return util.format(valuePattern, formatNum);
    } catch (e) {
        // Fallback: use the raw number string
        return rate.toString();
    }
}

function isZhLocale() {
  let lang = i18n.System.getSystemLanguage();
  return lang.split('-')[0].toLowerCase() === 'zh';
}

// Video playback status class.
@Observed
class VideoPlayerState {
  isPlaying = false;
  currentSecond = 0;
  totalSecond = 0;
  volume = 0;
  isMuted = MutedStatus.UNMUTED;
  currentPlaybackRate = 1;
  title = '';
  bufferedEndTime = 0;
  displayLoading = false;
  dialogComponent = DialogComponentType.None;
  isShowDownload = true;
  isShowPlaybackSpeed = true;
  width = 0;
  height = 0;
  displayState = VideoPlayerComponentDisplayType.HIDDEN;
}

// Parse the media info.
function parseArkwebMediaExtraInfo(info) {
  try {
      return JSON.parse(info);
  } catch (err) {
      console.error('[FullscreenVideoOverlay] parseArkwebMediaExtraInfo failed, error: ', err);
      return undefined;
  }
}

// Convert the media info to video entity.
function ofArkwebVideoEntity(entity) {
  const arkwebEntity = parseArkwebMediaExtraInfo(entity);

  return {
    title: arkwebEntity?.title,
    duration: arkwebEntity?.duration,
    curTime: arkwebEntity?.curTime,
    playbackRate: arkwebEntity?.playbackrate,
    width: arkwebEntity?.width,
    height: arkwebEntity?.height,
    isPlaying: arkwebEntity?.isPlaying,
    downloadBtn: arkwebEntity?.downloadBtn,
    playbackrateBtn: arkwebEntity?.playbackrateBtn,
    id: arkwebEntity?.id,
    muted: arkwebEntity?.muted
  };
}

// Convert seconds into time display.
const parseSecTimeToLocalTime = (secTime) => {
  let minute = Math.floor(secTime / 60);
  let secondLeft = Math.floor(secTime % 60);
  let hour = Math.floor(minute / 60);
  let minuteLeft = Math.floor(minute % 60);
  if (hour === 0) {
      return `${minuteLeft.toString().padStart(2, '0')}:${secondLeft.toString().padStart(2, '0')}`;
  }
  else {
      return `${hour.toString().padStart(2, '0')}:${minuteLeft.toString().padStart(2, '0')}:${secondLeft.toString()
          .padStart(2, '0')}`;
  }
}

function __Text__globalAdaptTitleStyls(minSize = '16fp', maxSize = '20fp') {
  Text.heightAdaptivePolicy(TextHeightAdaptivePolicy.MIN_FONT_SIZE_FIRST);
  Text.textOverflow({ overflow: TextOverflow.ELLIPSIS });
  Text.wordBreak(WordBreak.BREAK_WORD);
  Text.maxFontSize(maxSize);
  Text.minFontSize(minSize);
  Text.maxLines(2);
}

function getCertificatePromise(certChainData) {
  let x509CertArray = [];
  if (!(certChainData instanceof Array)) {
    console.log('failed, cert chain data type is not array');
    return Promise.all(x509CertArray);
  }

  for (let i = 0; i < certChainData.length; i++) {
    let encodeBlobData = {
      data: certChainData[i],
      encodingFormat: cert.EncodingFormat.FORMAT_DER
    };
    x509CertArray[i] = cert.createX509Cert(encodeBlobData);
  }

  return Promise.all(x509CertArray);
}

function takePhoto(callback) {
  let pickerProfileOptions = {
    'cameraPosition': camera.CameraPosition.CAMERA_POSITION_BACK,
  };
  let acceptTypes = callback.fileparam.getAcceptType();
  let mediaType = [];
  if (isContainImageMimeType(acceptTypes)) {
    mediaType.push(cameraPicker.PickerMediaType.PHOTO);
  }
  if (isContainVideoMimeType(acceptTypes)) {
    mediaType.push(cameraPicker.PickerMediaType.VIDEO);
  }
  let result = [];
  cameraPicker.pick(getContext(this), mediaType, pickerProfileOptions)
    ?.then((pickerResult) => {
      result.push(pickerResult.resultUri);
    }).catch((error) => {
      console.log('selectFile error:' + JSON.stringify(error));
      promptAction.showToast({ message: '无法打开拍照功能，请检查是否具备拍照功能' });
    }).finally(() => {
      onShowFileSelectorEvent?.fileresult.handleFileList(result);
      onShowFileSelectorEvent = undefined;
    });
}

function needShowDialog(params) {
  let result = false;
  try {
    let currentDevice = deviceinfo.deviceType.toLowerCase();
    if (currentDevice === '2in1') {
      return false;
    }
    if (params.isCapture()) {
      console.log('input element contain capture tag, not show dialog');
      return false;
    }
    let acceptTypes = params.getAcceptType();
    if (isContainImageMimeType(acceptTypes) || isContainVideoMimeType(acceptTypes)) {
      result = true;
    }
  } catch (error) {
    console.log('show dialog happend error:' + JSON.stringify(error));
  }
  return result;
}

function selectFile(callback) {
  let documentPicker = new picker.DocumentViewPicker();
  let result = [];
  if (callback.fileparam.getMode() !== FileSelectorMode.FileSaveMode) {
    documentPicker.select(createDocumentSelectionOptions(callback.fileparam))
      .then((documentSelectResult) => {
        result = documentSelectResult;
      }).catch((error) => {
        console.log('selectFile error: ' + JSON.stringify(error));
        promptAction.showToast({ message: '无法打开文件功能，请检查是否具备文件功能' });
      }).finally(() => {
        onShowFileSelectorEvent?.fileresult.handleFileList(result);
        onShowFileSelectorEvent = undefined;
      });
  } else {
    documentPicker.save(createDocumentSaveOptions(callback.fileparam))
      .then((documentSaveResult)=>{
        let tempUri = saveFile(documentSaveResult);
        result.push(tempUri);
      }).catch((error) => {
        console.log('saveFile error: ' + JSON.stringify(error));
        promptAction.showToast({ message: '无法打开文件功能，请检查是否具备文件功能' });
      }).finally(() => {
        onShowFileSelectorEvent?.fileresult.handleFileList(result);
        onShowFileSelectorEvent = undefined;
      });
  }
}

function saveFile(documentSaveResult) {
  let filePaths = documentSaveResult;
  let tempUri = '';
  if (filePaths.length > 0) {
    let tempPath = '';
    try {
      let fileName = filePaths[0].substr(filePaths[0].lastIndexOf('/'));
      tempPath = getContext(this).filesDir + fileName;
      tempUri = fileUri.getUriFromPath(tempPath); 
      let randomAccessFile = fileIo.createRandomAccessFileSync(tempPath, fileIo.OpenMode.CREATE); 
      randomAccessFile.close();
    } catch (err) {
      console.error(`saveFile failed when getPath with error message: ${err.message}, error code: ${err.code}`);
    }

    let watcher = fileIo.createWatcher(tempPath, 0x4, () => {
      let tempFile = null;
      let destinyFile = null;
      let arrayBuffer;
      Promise.all([fileIo.lstat(tempPath),
      fileIo.open(tempPath, fileIo.OpenMode.READ_ONLY),
      fileIo.open(filePaths[0], fileIo.OpenMode.READ_WRITE)])
        .then(([stat, tmpFile, destFile]) => {
          tempFile = tmpFile;
          destinyFile = destFile;
          arrayBuffer = new ArrayBuffer(stat.size);
          return fileIo.read(tempFile.fd, arrayBuffer);
        })
        .then(readLen => {
          if (readLen) {
            console.log(`saveFile read data to file succeed and size is ${readLen}`);
            return fileIo.write(destinyFile.fd, arrayBuffer);
          }
          throw new Error('read file is Empty');
        })
        .then(writeLen => {
          if (writeLen) {
            console.log(`saveFile write data to file succeed and size is ${writeLen}`);
            return;
          }
          throw new Error('write file length is 0');
        })
        .catch((err) => {
          console.error(`saveFile failed with error message: ${err.message}, error code: ${err.code}`);
        })
        .finally(() => {
          let finalSet = new Set();
          if (tempFile) {
            finalSet.add(fileIo.close(tempFile));
          }
          if (destinyFile) {
            finalSet.add(fileIo.close(destinyFile));
          }
          watcher.stop();
          return Promise.all(finalSet).finally(() => {
            fileIo.unlink(tempPath);
          });
        })
        .catch((err) => {
          console.error(`saveFile end failed with error message: ${err.message}, error code: ${err.code}`);
        })
    });
    watcher.start();
  }
  return tempUri;
}

function createDocumentSelectionOptions(param) {
  let documentSelectOptions = new picker.DocumentSelectOptions();
  let currentDevice = deviceinfo.deviceType.toLowerCase();
  try {
    let defaultSelectNumber = 500;
    let defaultSelectMode = picker.DocumentSelectMode.MIXED;
    documentSelectOptions.maxSelectNumber = defaultSelectNumber;
    documentSelectOptions.selectMode = defaultSelectMode;
    documentSelectOptions.defaultFilePathUri = getDefaultPath(param);
    let mode = param.getMode();
    switch (mode) {
      case FileSelectorMode.FileOpenMode:
        documentSelectOptions.maxSelectNumber = 1;
        documentSelectOptions.selectMode = picker.DocumentSelectMode.FILE;
        break;
      case FileSelectorMode.FileOpenMultipleMode:
        documentSelectOptions.selectMode = picker.DocumentSelectMode.FILE;
        break;
      case FileSelectorMode.FileOpenFolderMode:
        documentSelectOptions.selectMode = picker.DocumentSelectMode.FOLDER;
        break;
      default:
        break;
    }
    documentSelectOptions.fileSuffixFilters = [];
    let suffix = param.getAcceptType().join(',');
    let accepts = param.getAcceptableFileTypes();
    let descriptions = param.getDescriptions();
    if (accepts && accepts.length > 0) {
      suffixFromAccepts(documentSelectOptions.fileSuffixFilters, descriptions, accepts);
    } else if (suffix) {
      documentSelectOptions.fileSuffixFilters.push(suffix);
    }
    if (currentDevice !== 'phone' && !param.isAcceptAllOptionExcluded()) {
      documentSelectOptions.fileSuffixFilters.push('.*');
    }
  } catch (error) {
    console.log('selectFile error: ' + + JSON.stringify(error));
  }
  return documentSelectOptions;
}

function createDocumentSaveOptions(param) {
  let documentSaveOptions = new picker.DocumentSaveOptions();
  let currentDevice = deviceinfo.deviceType.toLowerCase();
  try {
    documentSaveOptions.pickerMode = picker.DocumentPickerMode.DEFAULT;
    documentSaveOptions.fileSuffixChoices = [];
    documentSaveOptions.newFileNames = [ param.getSuggestedName() ];
    documentSaveOptions.defaultFilePathUri = getDefaultPath(param);
    let suffix = param.getAcceptType().join(',');
    let accepts = param.getAcceptableFileTypes();
    let descriptions = param.getDescriptions();
    if (accepts && accepts.length > 0) {
      suffixFromAccepts(documentSaveOptions.fileSuffixChoices, descriptions, accepts);
    } else if (suffix) {
      documentSaveOptions.fileSuffixChoices.push(suffix);
    }
    if (currentDevice !== 'phone' && !param.isAcceptAllOptionExcluded()) {
      documentSaveOptions.fileSuffixChoices.push('.*');
    }
  } catch (error) {
    console.log('saveFile error: ' + + JSON.stringify(error));
  }
  return documentSaveOptions;
}

function getDefaultPath(param) {
  let path = param.getDefaultPath();
  if (publicDirectoryMap.get(path) !== undefined) {
    path = publicDirectoryMap.get(path);
  }
  return fileUri.getUriFromPath(path);
}

function suffixFromAccepts(suffix, descriptions, accepts) {
  let n = accepts.length;
  for (let i = 0; i < n; i++) {
    let m = accepts[i].length;
    let extList = [];
    for (let j = 0; j < m; j++) {
      extList.push(accepts[i][j].acceptableType.join(','));
    }
    let ext = extList.join(',');
    let desc = descriptions[i] + '(' + ext + ')' + '|';
    suffix.push(desc + ext);
  }
}

function isContainImageMimeType(acceptTypes) {
  if (!(acceptTypes instanceof Array)) {
    return false;
  }
  if (acceptTypes.length < 1) {
    return true;
  }

  let imageTypes = ['tif', 'xbm', 'tiff', 'pjp', 'jfif', 'bmp', 'avif', 'apng', 'ico',
    'webp', 'svg', 'gif', 'svgz', 'jpg', 'jpeg', 'png', 'pjpeg'];
  for (let i = 0; i < acceptTypes.length; i++) {
    for (let j = 0; j < imageTypes.length; j++) {
      if (acceptTypes[i].includes(imageTypes[j])) {
        return true;
      }
    }
  }
  return false;
}

function isContainVideoMimeType(acceptTypes) {
  if (!(acceptTypes instanceof Array)) {
    return false;
  }
  if (acceptTypes.length < 1) {
    return true;
  }

  let videoTypes = ['ogm', 'ogv', 'mpg', 'mp4', 'mpeg', 'm4v', 'webm'];
  for (let i = 0; i < acceptTypes.length; i++) {
    for (let j = 0; j < videoTypes.length; j++) {
      if (acceptTypes[i].includes(videoTypes[j])) {
        return true;
      }
    }
  }
  return false;
}

// The dialog for creating XComponent component.
class XComponentDialog extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__state = new ObservedPropertyObjectPU(new VideoPlayerState(), this, "state");
      this.addProvidedVar("state", this.__state, false);
      this.__adaptPadding = new ObservedPropertySimplePU(16, this, "adaptPadding");
      this.__windowSizeHeight = new ObservedPropertySimplePU(0, this, "windowSizeHeight");
      this.__windowSizeWidth = new ObservedPropertySimplePU(0, this, "windowSizeWidth");
      this.__isPortrait = new ObservedPropertySimplePU(false, this, "isPortrait");
      this.controller = undefined;
      this.xComponentController = new XComponentController();
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
      if (params.state !== undefined) {
        this.state = params.state;
      }
      if (params.adaptPadding !== undefined) {
        this.adaptPadding = params.adaptPadding;
      }
      if (params.windowSizeHeight !== undefined) {
        this.windowSizeHeight = params.windowSizeHeight;
      }
      if (params.windowSizeWidth !== undefined) {
        this.windowSizeWidth = params.windowSizeWidth;
      }
      if (params.controller !== undefined) {
          this.controller = params.controller;
      }
      if (params.xComponentController !== undefined) {
          this.xComponentController = params.xComponentController;
      }
      if (params.callback !== undefined) {
        this.handler = params.callback.handler;
        this.logic = new VideoPlayerLogic(this.state, this.handler);
        this.videoEntity = ofArkwebVideoEntity(params.callback.mediaInfo);
        this.state.width = this.videoEntity.width ?? 0;
        this.state.height = this.videoEntity.height ?? 0;
      }
  }
  updateStateVars(params) {
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
    this.__state.purgeDependencyOnElmtId(rmElmtId);
    this.__adaptPadding.purgeDependencyOnElmtId(rmElmtId);
    this.__windowSizeWidth.purgeDependencyOnElmtId(rmElmtId);
    this.__windowSizeHeight.purgeDependencyOnElmtId(rmElmtId);
    this.__isPortrait.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
    this.__state.aboutToBeDeleted();
    this.__adaptPadding.aboutToBeDeleted();
    this.__windowSizeWidth.aboutToBeDeleted();
    this.__windowSizeHeight.aboutToBeDeleted();
    this.__isPortrait.aboutToBeDeleted();
    SubscriberManager.Get().delete(this.id__());
    this.aboutToBeDeletedInternal();
  }

  get state() {
      return this.__state.get();
  }
  set state(newValue) {
      this.__state.set(newValue);
  }

  get adaptPadding() {
      return this.__adaptPadding.get();
  }
  set adaptPadding(newValue) {
      this.__adaptPadding.set(newValue);
  }
  get windowSizeWidth() {
    return this.__windowSizeWidth.get();
  }
  set windowSizeWidth(newValue) {
    this.__windowSizeWidth.set(newValue);
  }

  get windowSizeHeight() {
    return this.__windowSizeHeight.get();
  }
  set windowSizeHeight(newValue) {
    this.__windowSizeHeight.set(newValue);
  }

  get isPortrait() {
    return this.__isPortrait.get();
  }
  set isPortrait(newValue) {
    this.__isPortrait.set(newValue);
  }
  setController(ctr) {
    this.controller = ctr;
    this.logic.dialogController = ctr;
  }

  aboutToAppear() {
    this.readDisplaySize();
    this.initState();
    this.logic.showBars();
  }

  readDisplaySize() {
    try {
      let displayData = display.getDefaultDisplaySync();
      this.windowSizeHeight = px2vp(displayData.height);
      this.windowSizeWidth = px2vp(displayData.width);
      this.isPortrait = this.windowSizeHeight > this.windowSizeWidth;
    } catch (err) {
      console.error(`[FullscreenVideoOverlay] getDefaultDisplaySync failed: ${err}`);
    }
  }

  initialRender() {
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      RelativeContainer.create();
      RelativeContainer.onClick(() => { });
      RelativeContainer.width('100%');
      RelativeContainer.height('100%');
      RelativeContainer.backgroundColor(Color.Black);
    }, RelativeContainer);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Stack.create();
      Stack.width('100%');
      Stack.height('100%');
      Stack.id(`video_player`);
      Stack.backgroundColor(Color.Black);
      Gesture.create(GesturePriority.Low);
      GestureGroup.create(GestureMode.Exclusive);
      TapGesture.create({ count: 2 });
      TapGesture.onAction(() => {
          if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
              this.logic.closePlaybackRateMenu();
          } else {
              this.logic.switchPlayingStatus();
          }
      });
      TapGesture.pop();
      TapGesture.create({ count: 1 });
      TapGesture.tag(SINGLE_CLICK_TAG);
      TapGesture.onAction(() => {
          if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
              this.logic.closePlaybackRateMenu();
          } else if (this.state.displayState === VideoPlayerComponentDisplayType.HIDDEN) {
              this.logic.showBars();
          } else {
              this.logic.hideBars();
          }
      });
      TapGesture.pop();
      GestureGroup.pop();
      Gesture.pop();
      Gesture.create(GesturePriority.Low);
      PanGesture.create({ direction: PanDirection.Horizontal });
      PanGesture.tag(SINGLE_HORIZONTAL_TAG);
      PanGesture.onActionStart(() => {
          this.logic.gestureStart(VideoStateGestureManagerType.SEEK_MANAGER);
      });
      PanGesture.onActionUpdate((event) => {
          if (event) {
              this.logic.gestureUpdate(event.offsetX, this.windowSizeWidth);
          }
      });
      PanGesture.onActionEnd(() => {
          this.logic.gestureEnd();
      });
      PanGesture.onActionCancel(() => {
          this.logic.gestureEnd();
      });
      PanGesture.pop();
      Gesture.pop();
      Stack.onGestureJudgeBegin((gestureInfo) => {
          if (gestureInfo.type === GestureControl.GestureType.PAN_GESTURE) {
              const judgeResult = this.canShowPlayerSeek();
              return judgeResult ? GestureJudgeResult.CONTINUE : GestureJudgeResult.REJECT;
          }
          return GestureJudgeResult.CONTINUE;
      });
    }, Stack);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      XComponent.create({
        id: 'Web_XComponent',
        type: XComponentType.SURFACE,
        controller: this.xComponentController,
      }, "FullScreen_XComponent");
      XComponent.onLoad(() => {
        let surfaceId = this.xComponentController.getXComponentSurfaceId();
        console.log(`[FullscreenVideoOverlay] XComponent surfaceId:${surfaceId}, width:${this.state.width}, height:${this.state.height}.`);
        this.handler.setVideoSurface(surfaceId);
        const listener = new VideoPlayerListener({
          onStatusChanged: (s) => this.onStatusChanged(s),
          onMutedChanged: (m) => this.onMutedChanged(m),
          onPlaybackRateChanged: (r) => this.onPlaybackRateChanged(r),
          onDurationChanged: (d) => this.onDurationChanged(d),
          onTimeUpdate: (t) => this.onTimeUpdate(t),
          onBufferedEndTimeChanged: (bt) => this.onBufferedEndTimeChanged(bt),
          onEnded: () => this.onEnded(),
          onVideoSizeChanged: (w, h) => this.onVideoSizeChanged(w, h)
        });
        this.handler.addListener(listener);
      });
      XComponent.onDestroy(() => {
        console.info(`[FullscreenVideoOverlay] XComponent onDestroy surfaceId:${this.xComponentController.getXComponentSurfaceId()}.`);
      });
      XComponent.aspectRatio((this.state.width && this.state.height) === 0 ? -1 : this.state.width / this.state.height);
      XComponent.renderFit(RenderFit.RESIZE_FILL);
    }, XComponent);
    Stack.pop();
    // Title and back button.
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      If.create();
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        this.ifElseBranchUpdateFunction(0, () => {
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            __Common__.create();
            __Common__.alignRules({
              top: { anchor: '__container__', align: VerticalAlign.Top },
            });
            __Common__.accessibilityLevel('no');
          }, __Common__);
          {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
              if (isInitialRender) {
                let componentCall = new VideoPlayerTitle(this, {
                  logic: this.logic,
                  adaptPadding: this.adaptPadding,
                });
                ViewPU.create(componentCall);
                let paramsLambda = () => {
                  return {
                    logic: this.logic,
                    adaptPadding: this.adaptPadding,
                  };
                };
                componentCall.paramsGenerator_ = paramsLambda;
              }
              else {
                this.updateStateVarsOfChildByElmtId(elmtId, {
                  adaptPadding: this.adaptPadding
                });
              }
            }, { name: "VideoPlayerTitle" });
          }
          __Common__.pop();
        });
      }
      else {
        this.ifElseBranchUpdateFunction(1, () => {
        });
      }
    }, If);
    If.pop();
    // Progress dialog.
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      __Common__.create();
      __Common__.alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center },
      });
    }, __Common__);
    {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          if (isInitialRender) {
              let componentCall = new
              ScreenCenterDialog(this, {});
              ViewPU.create(componentCall);
              let paramsLambda = () => {
                  return {};
              };
              componentCall.paramsGenerator_ = paramsLambda;
          }
          else {
              this.updateStateVarsOfChildByElmtId(elmtId, {});
          }
      }, { name: "ScreenCenterDialog" });
    }
    __Common__.pop();
    // Playback rate dialog.
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      If.create();
      if (this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU) {
        this.ifElseBranchUpdateFunction(0, () => {
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.width(200);
            if (this.isPortrait) {
              Row.height('100%');
            }
            Row.alignRules({
              end: { anchor: '__container__', align: HorizontalAlign.End },
            });
            Row.accessibilityLevel('no');
          }, Row);
          {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
              if (isInitialRender) {
                let componentCall = new PlaybackRateSideBar(this, {
                  playState: this.__state,
                  isVisible: this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU,
                  logic: this.logic,
                  isPortrait: this.isPortrait,
                });
                ViewPU.create(componentCall);
                let paramsLambda = () => {
                  return {
                    playState: this.state,
                    isVisible: this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU,
                    logic: this.logic,
                    isPortrait: this.isPortrait,
                  };
                };
                componentCall.paramsGenerator_ = paramsLambda;
              }
              else {
                this.updateStateVarsOfChildByElmtId(elmtId, {
                  isVisible: this.state.displayState === VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU,
                });
              }
            }, { name: "PlaybackRateSideBar" });
          }
          Row.pop();
        });
      }
      else {
        this.ifElseBranchUpdateFunction(1, () => {
        });
      }
    }, If);
    If.pop();
    // Bottom control bar.
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      If.create();
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        this.ifElseBranchUpdateFunction(0, () => {
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Column.create();
            Column.height(80);
            Column.alignRules({
              bottom: { anchor: '__container__', align: VerticalAlign.Bottom }
            });
            Column.accessibilityLevel('no');
          }, Column);
          {
            this.observeComponentCreation2((elmtId, isInitialRender) => {
              if (isInitialRender) {
                let componentCall = new BottomControllerToolBar(this, {
                  isVisible: this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE,
                  logic: this.logic,
                  adaptPadding: this.adaptPadding
                });
                ViewPU.create(componentCall);
                let paramsLambda = () => {
                  return {
                    isVisible: this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE,
                    logic: this.logic,
                    adaptPadding: this.adaptPadding
                  };
                };
                componentCall.paramsGenerator_ = paramsLambda;
              }
              else {
                this.updateStateVarsOfChildByElmtId(elmtId, {
                  isVisible: this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE,
                  adaptPadding: this.adaptPadding
                });
              }
            }, { name: "BottomControllerToolBar" });
          }
          Column.pop();
        });
      }
      else {
        this.ifElseBranchUpdateFunction(1, () => {
        });
      }
    }, If);
    If.pop();
    RelativeContainer.pop();
  }

  rerender() {
    this.updateDirtyElements();
  }

  initState() {
    this.state.title = this.videoEntity.title ?? '';
    this.state.totalSecond = this.videoEntity.duration ?? 0;
    this.state.currentSecond = this.videoEntity.curTime ?? 0;
    this.state.currentPlaybackRate = this.videoEntity.playbackRate ?? 1;
    this.state.isPlaying = this.videoEntity.isPlaying ?? false;
    this.state.isShowPlaybackSpeed = true;
    this.state.isMuted = this.videoEntity.muted ?? MutedStatus.UNMUTED;
  }

  canShowPlayerSeek() {
    return this.state.displayState !== VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU &&
        this.state.totalSecond > 0;
  }

  onStatusChanged (status) {
    console.info(`[FullscreenVideoOverlay] video stauts change: ${status}.`);
    switch (status) {
        case PlaybackStatus.PLAYING:
            this.state.isPlaying = true ;
            break;
        case PlaybackStatus.PAUSED:
            this.state.isPlaying = false;
            break;
        case PlaybackStatus.BUFFERING:
            this.state.displayLoading = true;
            break;
        case PlaybackStatus.BUFFERED:
            this.state.displayLoading = false;
            break;
        default:
            console.error('[FullscreenVideoOverlay] unsupported playback state.');
     }
  }

  onMutedChanged = (muted) => {
    console.info(`[FullscreenVideoOverlay] video muted changed: ${muted}.`)
    this.state.isMuted = muted ? MutedStatus.MUTED : MutedStatus.UNMUTED;
  }
  onPlaybackRateChanged = (playbackRate) => {
    this.state.currentPlaybackRate = playbackRate;
  }
  onDurationChanged = (duration) => {
    if (!isFinite(duration) || isNaN(duration)) {
      this.state.totalSecond = 0;
      return;
    }
    this.state.totalSecond = duration;
  }
  onTimeUpdate = (currentPlayTime) => {
    if (this.state.dialogComponent !== DialogComponentType.SeekComponent) {
      this.state.currentSecond = currentPlayTime;
    }
  }
  onBufferedEndTimeChanged = (bufferedEndTime) => {
    this.state.bufferedEndTime = bufferedEndTime;
  }
  onEnded = () => {
    this.state.isPlaying = false;
    this.logic.showBars();
    setTimeout(() => {
        this.logic.exitFullScreen();
    }, 1000);
  }
  onVolumeChanged = (volume) => {}
  onSeeking = () => {}
  onSeekFinished = () => {}
  onError = (error, errorMsg) => {}
  onVideoSizeChanged = (width, height) => {
    this.state.width = width;
    this.state.height = height;
  }
}

// The component is about video subtitle.
class VideoPlayerTitle extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__videoPlayerState = this.initializeConsume('state', "videoPlayerState");
      this.__adaptPadding = new SynchedPropertySimpleOneWayPU(params.adaptPadding, this, "adaptPadding");
      this.__foldStatusExpanded = this.createStorageProp('foldStatusExpanded', 0, "foldStatusExpanded");
      this.__paddingTop = new ObservedPropertySimplePU(0, this, "paddingTop");
      this.__orientationLandscape = this.createStorageProp('orientationLandscape', false, "orientationLandscape");
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
      if (params.adaptPadding === undefined) {
          this.__adaptPadding.set(16);
      }
      if (params.paddingTop !== undefined) {
          this.paddingTop = params.paddingTop;
      }
      if (params.logic !== undefined) {
        this.logic = params.logic;
      }
  }
  updateStateVars() {
      this.__adaptPadding.reset(params.adaptPadding);
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__videoPlayerState.purgeDependencyOnElmtId(rmElmtId);
      this.__adaptPadding.purgeDependencyOnElmtId(rmElmtId);
      this.__foldStatusExpanded.purgeDependencyOnElmtId(rmElmtId);
      this.__paddingTop.purgeDependencyOnElmtId(rmElmtId);
      this.__orientationLandscape.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      this.__videoPlayerState.aboutToBeDeleted();
      this.__adaptPadding.aboutToBeDeleted();
      this.__foldStatusExpanded.aboutToBeDeleted();
      this.__paddingTop.aboutToBeDeleted();
      this.__orientationLandscape.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get videoPlayerState() {
      return this.__videoPlayerState.get();
  }
  set videoPlayerState(newValue) {
      this.__videoPlayerState.set(newValue);
  }

  get adaptPadding() {
      return this.__adaptPadding.get();
  }
  set adaptPadding(newValue) {
      this.__adaptPadding.set(newValue);
  }

  get foldStatusExpanded() {
      return this.__foldStatusExpanded.get();
  }
  set foldStatusExpanded(newValue) {
      this.__foldStatusExpanded.set(newValue);
  }

  get paddingTop() {
      return this.__paddingTop.get();
  }
  set paddingTop(newValue) {
      this.__paddingTop.set(newValue);
  }

  get orientationLandscape() {
      return this.__orientationLandscape.get();
  }
  set orientationLandscape(newValue) {
      this.__orientationLandscape.set(newValue);
  }
  initialRender() {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Column.create();
          Column.height(80);
          Column.width('100%');
          Column.padding({
              top: this.paddingTop,
              left: this.adaptPadding,
              right: this.adaptPadding,
          });
          Column.linearGradient({
              colors: [['#99000000', 0], ['#00000000', 1]]
          });
          Column.transition(TransitionEffect.OPACITY.animation({ duration: 415 }));
      }, Column);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Flex.create({ direction: FlexDirection.Row, justifyContent: FlexAlign.SpaceBetween, alignItems: ItemAlign.Center });
          Flex.width('100%');
          Flex.height(56);
          Flex.transition(this.scaleTransition());
      }, Flex);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Row.create();
      }, Row);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Row.create();
          Row.width('40vp');
          Row.height('40vp');
          Row.justifyContent(FlexAlign.Center);
          Row.alignItems(VerticalAlign.Center);
          Row.margin({ end: LengthMetrics.vp(8) });
          Row.responseRegion({
              x: 0,
              y: 0,
              width: '48vp',
              height: '56vp'
          });
          ViewStackProcessor.visualState("pressed");
          Row.backgroundColor('#40FFFFFF');
          Row.borderRadius({ "id": 125829702, "type": 10002, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
          ViewStackProcessor.visualState("normal");
          Row.backgroundColor('#1AFFFFFF');
          Row.borderRadius({ "id": 125829702, "type": 10002, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
          ViewStackProcessor.visualState();
          Row.onClick(() => {
              console.log('[FullscreenVideoOverlay] VideoPlayerTitle back icon onClick.');
              this.logic.exitFullScreen();
          });
          Row.id('btnBack');
      }, Row);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          SymbolGlyph.create({ "id": 125833534, "type": 40000, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
          SymbolGlyph.fontSize('24vp');
          SymbolGlyph.fontWeight(FontWeight.Regular);
          SymbolGlyph.fontColor([{ "id": 125829213, "type": 10001, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" }]);
          SymbolGlyph.accessibilityText(isZhLocale() ? '退出全屏' : 'Exit full screen');
        }, SymbolGlyph);
      Row.pop();
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Text.create(this.videoPlayerState.title);
          __Text__globalAdaptTitleStyls(14);
          Text.fontColor('#E6FFFFFF');
          Text.fontWeight(FontWeight.Bold);
          Text.textAlign(TextAlign.Start);
          Text.height(50);
          Text.flexShrink(1);
          Text.accessibilityText(this.videoPlayerState.title);
          Text.onClick(() => {
              this.logic.resetBarsTimer();
          });
      }, Text);
      Text.pop();
      Row.pop();
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Row.create({ space: 16 });
          Row.flexShrink(0);
          Row.margin({ start: LengthMetrics.vp(8) });
      }, Row);
      Row.pop();
      Flex.pop();
      Column.pop();
  }

  scaleTransition() {
    return TransitionEffect.scale({ x: 0.95, y: 0.95 })
        .animation({ curve: Curve.Friction, delay: 150, duration: 415 })
        .combine(TransitionEffect.opacity(0));
  }

  rerender() {
      this.updateDirtyElements();
  }
}

// The center dialog used to show progress.
class ScreenCenterDialog extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__state = this.initializeConsume('state', "state");
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
  }
  updateStateVars(params) {
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__state.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      this.__state.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get state() {
      return this.__state.get();
  }
  set state(newValue) {
      this.__state.set(newValue);
  }
  initialRender() {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Stack.create();
      }, Stack);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          If.create();
          if (this.state.dialogComponent === DialogComponentType.SeekComponent) {
              this.ifElseBranchUpdateFunction(0, () => {
                  this.observeComponentCreation2((elmtId, isInitialRender) => {
                      __Common__.create();
                      __Common__.alignRules({
                          center: { anchor: '__container__', align: VerticalAlign.Center },
                          middle: { anchor: '__container__', align: HorizontalAlign.Center },
                      });
                  }, __Common__);
                  {
                      this.observeComponentCreation2((elmtId, isInitialRender) => {
                          if (isInitialRender) {
                              let componentCall = new SeekComponent(this, {});
                              ViewPU.create(componentCall);
                              let paramsLambda = () => {
                                  return {};
                              };
                              componentCall.paramsGenerator_ = paramsLambda;
                          }
                          else {
                              this.updateStateVarsOfChildByElmtId(elmtId, {});
                          }
                      }, { name: "SeekComponent" });
                  }
                  __Common__.pop();
              });
          }
          else {
              this.ifElseBranchUpdateFunction(1, () => {
              });
          }
      }, If);
      If.pop();
      Stack.pop();
  }
  rerender() {
      this.updateDirtyElements();
  }
}

// The seek component.
class SeekComponent extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__videoPlayerState = this.initializeConsume('state', "videoPlayerState");
      this.__progressWidth = new ObservedPropertyObjectPU(0, this, "progressWidth");
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
      if (params.progressWidth !== undefined) {
          this.progressWidth = params.progressWidth;
      }
  }
  updateStateVars(params) {
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__videoPlayerState.purgeDependencyOnElmtId(rmElmtId);
      this.__progressWidth.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      this.__videoPlayerState.aboutToBeDeleted();
      this.__progressWidth.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get videoPlayerState() {
      return this.__videoPlayerState.get();
  }
  set videoPlayerState(newValue) {
      this.__videoPlayerState.set(newValue);
  }

  get progressWidth() {
      return this.__progressWidth.get();
  }
  set progressWidth(newValue) {
      this.__progressWidth.set(newValue);
  }
  initialRender() {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Column.create();
          Column.backgroundColor('rgba(0, 0, 0, 0.6)');
          Column.borderRadius(8);
          Column.padding(16);
      }, Column);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Text.create();
          Text.onAreaChange((oldVal, newVal) => {
              this.progressWidth = newVal.width;
          });
          Text.margin({ bottom: 8 });
      }, Text);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Span.create(parseSecTimeToLocalTime(this.videoPlayerState.currentSecond));
          Span.fontColor('rgba(255,255,255, 0.9)');
          Span.fontSize(18);
      }, Span);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Span.create(`/${parseSecTimeToLocalTime(this.videoPlayerState.totalSecond)}`);
          Span.baselineOffset(new LengthMetrics(2));
          Span.fontColor('rgba(255,255,255, 0.6)');
          Span.fontSize(12);
      }, Span);
      Text.pop();
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          Progress.create({
              value: this.videoPlayerState.currentSecond,
              total: this.videoPlayerState.totalSecond,
              type: ProgressType.Linear,
          });
          Progress.style({
              enableSmoothEffect: false,
              strokeWidth: 2,
          });
          Progress.width(ObservedObject.GetRawObject(this.progressWidth));
          Progress.backgroundColor('rgba(255,255,255, 0.2)');
          Progress.color('rgba(255,255,255, 0.9)');
          Progress.direction(Direction.Ltr);
      }, Progress);
      Column.pop();
  }
  rerender() {
      this.updateDirtyElements();
  }
}

// The side bar for playback rate control.
class PlaybackRateSideBar extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__playState = new SynchedPropertySimpleTwoWayPU(params.playState, this, "playState");
      this.__isVisible = new SynchedPropertySimpleOneWayPU(params.isVisible, this, "isVisible");
      this.__speed_value_pattern = new ObservedPropertyObjectPU(
        { 'id': -1, 'type': -1, params: ['sys.string.media_video_playback_speed_value_pattern'],
          'bundleName': 'com.example.fullscreenvideooverlay', 'moduleName': 'entry' },
        this, 'speed_value_pattern');
      this._context = getContext(this);
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
      if (params.isVisible === undefined) {
          this.__isVisible.set(false);
      }
      if (params.logic !== undefined) {
        this.logic = params.logic;
      }
      if (params.isPortrait !== undefined) {
        this.isPortrait = params.isPortrait;
      }
      if (params._context !== undefined) {
        this._context = params._context;
      }
  }
  updateStateVars(params) {
      this.__isVisible.reset(params.isVisible);
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__playState.purgeDependencyOnElmtId(rmElmtId);
      this.__isVisible.purgeDependencyOnElmtId(rmElmtId);
      this.__speed_value_pattern.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      if (this.subscriber) {
        commonEventManager.unsubscribe(this.subscriber, (err) => {
          if (err) {
            console.error(`[PlaybackRateSideBar] Failed to unsubscribe locale resource. Code: ${err.code}, message: ${err.message}`);
            return;
          }
          this.subscriber = undefined;
        });
      }
      if (this._appStateCallback) {
        this._context.getApplicationContext().off('applicationStateChange', this._appStateCallback);
        this._appStateCallback = undefined;
      }
      this.__playState.aboutToBeDeleted();
      this.__isVisible.aboutToBeDeleted();
      this.__speed_value_pattern.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get playState() {
      return this.__playState.get();
  }
  set playState(newValue) {
      this.__playState.set(newValue);
  }

  get isVisible() {
      return this.__isVisible.get();
  }
  set isVisible(newValue) {
      this.__isVisible.set(newValue);
  }

  get speed_value_pattern() {
    return this.__speed_value_pattern.get();
  }
  set speed_value_pattern(newValue) {
    this.__speed_value_pattern.set(newValue);
  }

  initialRender() {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          SideBarContainer.create(SideBarContainerType.Overlay);
          SideBarContainer.showControlButton(false);
          SideBarContainer.width(200);
          SideBarContainer.sideBarPosition(SideBarPosition.End);
          SideBarContainer.divider({
            strokeWidth: 0
          });
      }, SideBarContainer);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        Column.create();
        Column.justifyContent(FlexAlign.Center);
        Column.backgroundColor({ "id": 125829131, "type": 10001, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
        Column.accessibilityLevel('no');
        Column.onClick(() => { });
      }, Column);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        Stack.create();
      }, Stack);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        Column.create();
        Column.width(200);
        Column.height('100%');
        if (this.isPortrait) {
          Column.justifyContent(FlexAlign.Center);
        }
        Column.linearGradient({
            direction: GradientDirection.Left,
            colors: [['rgba(0, 0, 0, 0.902)', 0], ['rgba(0, 0, 0, 0.4)', 0.832], ['rgba(0, 0, 0, 0)', 1]]
        });
        Column.visibility(this.isVisible ? Visibility.Visible : Visibility.Hidden);
        Column.transition(TransitionEffect.OPACITY.animation({
            delay: 83,
            duration: 1000,
            curve: Curve.Friction
        }));
      }, Column);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        Scroll.create();
        Scroll.scrollable(ScrollDirection.Vertical);
        Scroll.scrollBar(BarState.Off);
        Scroll.edgeEffect(EdgeEffect.Spring);
      }, Scroll);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        Column.create();
        Column.backgroundColor({ "id": 125829131, "type": 10001, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
      }, Column);
      this.observeComponentCreation2((elmtId, isInitialRender) => {
        ForEach.create();
        const forEachItemGenFunction = (_item, index) => {
            const item = _item;
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                Column.create();
                Column.id(`video_speed_${index}`);
                Column.visibility(this.isVisible ? Visibility.Visible : Visibility.Hidden);
                Column.transition(this.rateTransition(PLAY_RATE_LIST.length, index));
                Column.onClick(() => {
                    if (this.playState.currentPlaybackRate === item) {
                        return;
                    }
                    this.logic.switchPlaybackRate(item);
                    Context.animateTo({}, () => {
                        this.logic.closePlaybackRateMenu();
                    });
                });
            }, Column);
            {
                this.observeComponentCreation2((elmtId, isInitialRender) => {
                    if (isInitialRender) {
                        let componentCall = new SideBarItem(this, {
                          isSelect: this.playState.currentPlaybackRate === item,
                          content: formatRateNumber(item, this.speed_value_pattern, this._context),
                        });
                        ViewPU.create(componentCall);
                        let paramsLambda = () => {
                          return {
                            isSelect: this.playState.currentPlaybackRate === item,
                            content: formatRateNumber(item, this.speed_value_pattern, this._context),
                          };
                        };
                        componentCall.paramsGenerator_ = paramsLambda;
                    }
                    else {
                        this.updateStateVarsOfChildByElmtId(elmtId, {
                          isSelect: this.playState.currentPlaybackRate === item,
                          content: formatRateNumber(item, this.speed_value_pattern, this._context),
                        });
                    }
                }, { name: "SideBarItem" });
            }
            this.observeComponentCreation2((elmtId, isInitialRender) => {
                If.create();
                if (index !== PLAY_RATE_LIST.length - 1) {
                    this.ifElseBranchUpdateFunction(0, () => {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            Divider.create();
                            Divider.height(1);
                            Divider.color(Color.Transparent);
                            Divider.linearGradient({
                                direction: GradientDirection.Right,
                                colors: [['rgba(255, 255, 255, 0.2)', 0], ['rgba(255, 255, 255, 0)', 1]]
                            });
                            Divider.margin({ left: 16, right: 16 });
                            Divider.borderRadius(16);
                        }, Divider);
                    });
                }
                else {
                    this.ifElseBranchUpdateFunction(1, () => {
                    });
                }
            }, If);
            If.pop();
            Column.pop();
        };
        this.forEachUpdateFunction(elmtId, PLAY_RATE_LIST, forEachItemGenFunction, (item) => `${item}_${this.speed_value_pattern}`, true, false);
      }, ForEach);
      ForEach.pop();
      Column.pop();
      Scroll.pop();
      Stack.pop();
      Column.pop();
      SideBarContainer.pop();
  }
  aboutToAppear() {
    this.setResource();
    let that = this;
    commonEventManager.createSubscriber({
      events: [commonEventManager.Support.COMMON_EVENT_LOCALE_CHANGED]
    }).then((commonEventSubscriber) => {
      this.subscriber = commonEventSubscriber;
      commonEventManager.subscribe(commonEventSubscriber, (err, data) => {
        if (err) {
          console.error(`[PlaybackRateSideBar] Locale change subscribe error: ${err.code}, ${err.message}`);
          return;
        }
        console.info('[PlaybackRateSideBar] Locale changed event received.');
        setTimeout(() => {
          that.setResource();
        }, 200);
      });
    }).catch((err) => {
      console.error(`[PlaybackRateSideBar] CreateSubscriber failed: ${err.code}, ${err.message}`);
    });
    this._appStateCallback = {
      onApplicationForeground() {
        setTimeout(() => {
          that.setResource();
        }, 200);
      },
      onApplicationBackground() {
      }
    };
    this._context.getApplicationContext().on('applicationStateChange', this._appStateCallback);
  }

  setResource() {
    try {
      let manager = resourceManager.getSysResourceManager();
      this.speed_value_pattern = manager.getStringByNameSync('media_video_playback_speed_value_pattern');
    } catch (e) {
      console.error('[FullscreenVideoOverlay] Failed to load speed_value_pattern: ' + e);
    }
  }
  rateTransition(length, index) {
    return TransitionEffect.asymmetric(TransitionEffect.translate({ y: (length - index) * 10 + 10 }).animation({
        curve: curves.cubicBezierCurve(0.2, 0, 0.2, 1), delay: 83 * index + index * 50, duration: 500
    })
    .combine(TransitionEffect.opacity(0.01).animation({ curve: curves.cubicBezierCurve(0.2, 0, 0.2, 1), delay: 83 * index + index * 50, duration: 500 }))),
    TransitionEffect.translate({ y: (length - index) * 10 + 10}).animation({ curve: curves.cubicBezierCurve(0.2, 0, 0.2, 1), delay: (length - index -1) * 50, duration: 500 })
    .combine(TransitionEffect.opacity(0.01).animation({ curve: curves.cubicBezierCurve(0.2, 0, 0.2, 1), delay: (length - index -1) * 50, duration: 500 }));
  }
  rerender() {
      this.updateDirtyElements();
  }
}

// The item in side bar.
class SideBarItem extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__isSelect = new SynchedPropertySimpleOneWayPU(params.isSelect, this, "isSelect");
      this.__content = new SynchedPropertySimpleOneWayPU('', this, "content");
      this.clickFunction = () => {};
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
      if (params.isSelect === undefined) {
          this.__isSelect.set(false);
      }
      if (params.content !== undefined) {
          this.content = params.content;
      }
      if (params.clickFunction !== undefined) {
        this.clickFunction = params.clickFunction;
      }
  }
  updateStateVars(params) {
      this.__isSelect.reset(params.isSelect);
      this.__content.reset(params.content);
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__isSelect.purgeDependencyOnElmtId(rmElmtId);
      this.__content.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      this.__isSelect.aboutToBeDeleted();
      this.__content.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get isSelect() {
      return this.__isSelect.get();
  }
  set isSelect(newValue) {
      this.__isSelect.set(newValue);
  }

  get content() {
      return this.__content.get();
  }
  set content(newValue) {
      this.__content.set(newValue);
  }

  initialRender() {
      this.observeComponentCreation2((elmtId, isInitialRender) => {
          If.create();
          if (this.isSelect) {
              this.ifElseBranchUpdateFunction(0, () => {
                  this.observeComponentCreation2((elmtId, isInitialRender) => {
                      Row.create();
                      Row.width(200);
                      Row.height(48);
                      Row.justifyContent(FlexAlign.Center);
                      Row.linearGradient({
                          direction: GradientDirection.Right,
                          colors: [['rgba(255, 255, 255, 0.1)', 0], ['rgba(255, 255, 255, 0)', 1]]
                      });
                  }, Row);
                  this.observeComponentCreation2((elmtId, isInitialRender) => {
                      Text.create(this.content);
                      Text.fontColor('#5291FF');
                      Text.fontSize(14);
                      Text.lineHeight(16);
                  }, Text);
                  Text.pop();
                  Row.pop();
              });
          } else {
              this.ifElseBranchUpdateFunction(1, () => {
                  this.observeComponentCreation2((elmtId, isInitialRender) => {
                      Row.create();
                      Row.width(200);
                      Row.height(48);
                      Row.justifyContent(FlexAlign.Center);
                  }, Row);
                  this.observeComponentCreation2((elmtId, isInitialRender) => {
                      Text.create(this.content);
                      Text.fontColor('#E6FFFFFF');
                      Text.fontSize(14);
                      Text.lineHeight(16);
                  }, Text);
                  Text.pop();
                  Row.pop();
              });
          }
      }, If);
      If.pop();
  }
  rerender() {
      this.updateDirtyElements();
  }
}

// The component is about video control.
class BottomControllerToolBar extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
      super(parent, __localStorage, elmtId, extraInfo);
      if (typeof paramsLambda === "function") {
          this.paramsGenerator_ = paramsLambda;
      }
      this.__playState = this.initializeConsume('state', "playState");
      this.__isVisible = new SynchedPropertySimpleOneWayPU(params.isVisible, this, "isVisible");
      this.__adaptPadding = new SynchedPropertySimpleOneWayPU(params.adaptPadding, this, "adaptPadding");
      this.__foldStatusExpanded = this.createStorageProp('foldStatusExpanded', 0, "foldStatusExpanded");
      this.__media_video_playback_speed_button = new ObservedPropertyObjectPU(
        { 'id': -1, 'type': -1, params: ['sys.string.media_video_playback_speed_button'],
          'bundleName': 'com.example.fullscreenvideooverlay', 'moduleName': 'entry' },
        this, 'media_video_playback_speed_button');
      this.__speed_value_pattern = new ObservedPropertyObjectPU(
        { 'id': -1, 'type': -1, params: ['sys.string.media_video_playback_speed_value_pattern'],
          'bundleName': 'com.example.fullscreenvideooverlay', 'moduleName': 'entry' },
        this, 'speed_value_pattern');
      this._context = getContext(this);
      this.setInitiallyProvidedValue(params);
      this.finalizeConstruction();
  }
  setInitiallyProvidedValue(params) {
    if (params.isVisible === undefined) {
        this.__isVisible.set(false);
    }
    if (params.adaptPadding === undefined) {
        this.__adaptPadding.set(16);
    }
    if (params._context !== undefined) {
        this._context = params._context;
    }
    if (params.logic !== undefined) {
      this.logic = params.logic;
    }
  }
  updateStateVars(params) {
      this.__isVisible.reset(params.isVisible);
      this.__adaptPadding.reset(params.adaptPadding);
  }
  aboutToAppear() {
    this.setResource();
    commonEventManager.createSubscriber({
      events: [commonEventManager.Support.COMMON_EVENT_LOCALE_CHANGED]
    }).then((commonEventSubscriber) => {
      this.subscriber = commonEventSubscriber;
      commonEventManager.subscribe(commonEventSubscriber, (err, data) => {
        if (err) {
          console.error(`[FullscreenVideoOverlay] Locale change subscribe error: ${err.code}, ${err.message}`);
          return;
        }
        console.info('[FullscreenVideoOverlay] Locale changed event received.');
        setTimeout(() => {
          that.setResource();
        }, 200);
      });
    }).catch((err) => {
      console.error(`[FullscreenVideoOverlay] CreateSubscriber failed: ${err.code}, ${err.message}`);
    });
    let that = this;
    this._appStateCallback = {
      onApplicationForeground() {
        setTimeout(() => {
          that.setResource();
        }, 200);
      },
      onApplicationBackground() {
      }
    };
    this._context.getApplicationContext().on('applicationStateChange', this._appStateCallback);
  }
  purgeVariableDependenciesOnElmtId(rmElmtId) {
      this.__playState.purgeDependencyOnElmtId(rmElmtId);
      this.__isVisible.purgeDependencyOnElmtId(rmElmtId);
      this.__adaptPadding.purgeDependencyOnElmtId(rmElmtId);
      this.__foldStatusExpanded.purgeDependencyOnElmtId(rmElmtId);
      this.__media_video_playback_speed_button.purgeDependencyOnElmtId(rmElmtId);
      this.__speed_value_pattern.purgeDependencyOnElmtId(rmElmtId);
  }
  aboutToBeDeleted() {
      if (this.subscriber) {
        commonEventManager.unsubscribe(this.subscriber, (err) => {
          if (err) {
            console.error(`Failed to unsubscribe speed resource. Code: ${err.code}, message: ${err.message}`);
            return;
          }
          this.subscriber = undefined;
        });
      }
      if (this._appStateCallback) {
        this._context.getApplicationContext().off('applicationStateChange', this._appStateCallback);
        this._appStateCallback = undefined;
      }
      this.__playState.aboutToBeDeleted();
      this.__isVisible.aboutToBeDeleted();
      this.__adaptPadding.aboutToBeDeleted();
      this.__foldStatusExpanded.aboutToBeDeleted();
      this.__media_video_playback_speed_button.aboutToBeDeleted();
      this.__speed_value_pattern.aboutToBeDeleted();
      SubscriberManager.Get().delete(this.id__());
      this.aboutToBeDeletedInternal();
  }

  get playState() {
      return this.__playState.get();
  }
  set playState(newValue) {
      this.__playState.set(newValue);
  }

  get isVisible() {
      return this.__isVisible.get();
  }
  set isVisible(newValue) {
      this.__isVisible.set(newValue);
  }

  get adaptPadding() {
      return this.__adaptPadding.get();
  }
  set adaptPadding(newValue) {
      this.__adaptPadding.set(newValue);
  }

  get foldStatusExpanded() {
      return this.__foldStatusExpanded.get();
  }
  set foldStatusExpanded(newValue) {
      this.__foldStatusExpanded.set(newValue);
  }

  get media_video_playback_speed_button() {
    return this.__media_video_playback_speed_button.get();
  }
  set media_video_playback_speed_button(newValue) {
    this.__media_video_playback_speed_button.set(newValue);
  }

  get speed_value_pattern() {
    return this.__speed_value_pattern.get();
  }
  set speed_value_pattern(newValue) {
    this.__speed_value_pattern.set(newValue);
  }

  initialRender() {
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Column.create();
      Column.linearGradient({
        colors: [['#00000000', 0], ['#99000000', 1]]
      });
      Column.padding({
        left: this.adaptPadding,
        right: this.adaptPadding,
      });
      Column.justifyContent(FlexAlign.End);
      Column.height('100%');
      Column.width('100%');
      Column.transition(TransitionEffect.OPACITY.animation({ duration: 415 }));
      Column.accessibilityLevel('no');
      Column.onClick(() => { this.logic.resetBarsTimer(); });
    }, Column);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      If.create();
      // Progress bar: Hide both the progress bar and time when the time is 0 or less.
      if (this.isVisible && this.playState.totalSecond) {
          this.ifElseBranchUpdateFunction(0, () => {
              this.observeComponentCreation2((elmtId, isInitialRender) => {
                  Row.create();
                  Row.height(24);
                  Row.zIndex(1);
              }, Row);
              this.observeComponentCreation2((elmtId, isInitialRender) => {
                  Flex.create({ direction: FlexDirection.Row, justifyContent: FlexAlign.Center, alignItems: ItemAlign.Center });
                  Flex.direction(Direction.Ltr);
                  Flex.height(14);
                  Flex.width('100%');
                  Flex.transition(this.scaleTransition());
              }, Flex);
              this.observeComponentCreation2((elmtId, isInitialRender) => {
                  Text.create(parseSecTimeToLocalTime(this.playState.currentSecond));
                  Text.fontColor('#E6FFFFFF');
                  Text.textAlign(TextAlign.Center);
                  Text.fontSize({ "id": 125830974, "type": 10002, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
                  Text.fontSize(10);
                  Text.lineHeight(12);
                  Text.textSelectable(TextSelectableMode.UNSELECTABLE);
                  Text.maxLines(1);
                  Text.flexShrink(0);
                  Text.textOverflow({ overflow: TextOverflow.None });
                  Text.flexShrink(0);
              }, Text);
              Text.pop();
              this.observeComponentCreation2((elmtId, isInitialRender) => {
                  Slider.create({
                      value: Math.floor(this.playState.currentSecond),
                      max: Math.floor(this.playState.totalSecond)
                  });
                  Slider.direction(Direction.Ltr);
                  Slider.trackColor('#33FFFFFF');
                  Slider.selectedColor('#FFFFFF');
                  Slider.blockColor('#E5E5E5');
                  Slider.blockSize({ width: 10, height: 10 });
                  Slider.trackThickness(2);
                  Slider.trackBorderRadius(1.5);
                  Slider.width('100%');
                  Slider.onChange((currentSecond, mode) => {
                      this.logic.playbackProgressChange(currentSecond, mode);
                  });
              }, Slider);
              this.observeComponentCreation2((elmtId, isInitialRender) => {
                  Text.create(parseSecTimeToLocalTime(this.playState.totalSecond));
                  Text.fontColor('#E6FFFFFF');
                  Text.textAlign(TextAlign.Center);
                  Text.fontSize({ "id": 125830974, "type": 10002, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
                  Text.fontSize(10);
                  Text.lineHeight(12);
                  Text.textSelectable(TextSelectableMode.UNSELECTABLE);
                  Text.maxLines(1);
                  Text.flexShrink(0);
                  Text.textOverflow({ overflow: TextOverflow.None });
                  Text.accessibilityLevel('yes');
              }, Text);
              Text.pop();
              Flex.pop();
              Row.pop();
          });
      }
      else {
          this.ifElseBranchUpdateFunction(1, () => {
          });
      }
    }, If);
    If.pop();
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      If.create();
      // Bottom toolbar.
      if (this.isVisible) {
        this.ifElseBranchUpdateFunction(0, () => {
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.height(40);
            Row.justifyContent(FlexAlign.SpaceBetween);
            Row.width('100%');
            Row.margin({
              bottom: 8
            });
          }, Row);
          // Play Button
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create();
            Row.responseRegion({ x: -8, y: -8, width: '40vp', height: '40vp' });
            Row.transition(this.scaleTransition());
            Row.onClick(() => {
              console.log(`[FullscreenVideoOverlay] toolbar call switchPlayingStatus.`);
              this.logic.switchPlayingStatus();
            });
            Row.id('btnPlay');
          }, Row);
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Image.create(this.playState.isPlaying ?
              { "id": 125830396, "type": 20000, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" }
              : { "id": 125830399, "type": 20000, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" });
            Image.width(24);
            Image.height(24);
            Image.fillColor('#FFF');
            Image.accessibilityText(this.playState.isPlaying ?
              (isZhLocale() ? '暂停按钮' : 'Pause') :
              (isZhLocale() ? '播放按钮' : 'Play'));
          }, Image);
          Row.pop();
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            Row.create({ space: 24 });
          }, Row);
          this.observeComponentCreation2((elmtId, isInitialRender) => {
            If.create();
            // Playback Speed Button
            if (this.playState.isShowPlaybackSpeed && this.playState.totalSecond > 0) {
                this.ifElseBranchUpdateFunction(0, () => {
                    if (!If.canRetake('btnPlaybackRateSwitch')) {
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            Row.create();
                            Row.responseRegion(this.getFontResponseRegion(this.getSpeedContent(), 14));
                            Row.onClick(() => {
                                Context.animateTo({}, () => {
                                    this.logic.openPlayRateMenu();
                                });
                            });
                            Row.transition(this.scaleTransition());
                            Row.id('btnPlaybackRateSwitch');
                        }, Row);
                        this.observeComponentCreation2((elmtId, isInitialRender) => {
                            Text.create(this.getSpeedContent());
                            Text.fontSize(14);
                            Text.fontColor('#FFF');
                            Text.height(24);
                            Text.accessibilityText(this.getSpeedContent());
                        }, Text);
                        Text.pop();
                        Row.pop();
                    }
                });
            }
            else {
                this.ifElseBranchUpdateFunction(1, () => {
                });
            }
          }, If);
          If.pop();
          // Muted Button
          this.observeComponentCreation2((elmtId, isInitialRender) => {
              Row.create();
              Row.transition(this.scaleTransition());
              Row.responseRegion({ x: -8, y: -8, width: '40vp', height: '40vp' });
              Row.onClick(() => {
                  if (this.playState.isMuted === MutedStatus.DISABLE) {
                      return;
                  }
                  this.logic.setMute(this.playState.isMuted === MutedStatus.MUTED ? MutedStatus.UNMUTED : MutedStatus.MUTED);
              });
              Row.id('btnMute');
          }, Row);
          this.observeComponentCreation2((elmtId, isInitialRender) => {
              Image.create(this.getMuteImage());
              Image.width(24);
              Image.height(24);
              Image.fillColor('#FFF');
              Image.matchTextDirection(true);
              Image.accessibilityText(this.playState.isMuted === MutedStatus.MUTED ?
                (isZhLocale() ? '解除静音' : 'Unmute') :
                (isZhLocale() ? '设置静音' : 'Mute'));
          }, Image);
          Row.pop();
          Row.pop();
          Row.pop();
          });
        }
        else {
          this.ifElseBranchUpdateFunction(1, () => {
          });
        }
      }, If);
    If.pop();
    Column.pop();
  }

  // Obtain the actual content displayed at the speedup ratio.
  getSpeedContent() {
    if (this.playState.currentPlaybackRate === 1.0) {
      return this.media_video_playback_speed_button;
    }
    return formatRateNumber(this.playState.currentPlaybackRate, this.speed_value_pattern, this._context);
  }

  // Obtains the hot zone range in multiple languages.
  // The hot zone height is fixed at 40 vp and the width is adaptive.
  getFontResponseRegion(content, size) {
      const fontWidth = px2vp(MeasureText.measureText({
          textContent: content,
          fontSize: size,
      }));
      return {
          x: -8, y: -8, width: fontWidth + 16, height: 40
      };
  }

  // Decide to show which style of the speaker button.
  getMuteImage() {
    return this.playState.isMuted === MutedStatus.MUTED ?
      { "id": 125830401, "type": 20000, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" }
      : { "id": 125830402, "type": 20000, params: [], "bundleName": "com.example.fullscreenvideooverlay", "moduleName": "entry" };
  }

  setResource() {
    try {
      let manager = resourceManager.getSysResourceManager();
      this.media_video_playback_speed_button = manager.getStringByNameSync('media_video_playback_speed_button');
      this.speed_value_pattern = manager.getStringByNameSync('media_video_playback_speed_value_pattern');
    } catch (e) {
      console.error('[FullscreenVideoOverlay] Failed to load speed resources: ' + e);
    }
  }

  // Progress bar, title size scaling animation along the center point.
  scaleTransition() {
      return TransitionEffect.scale({ x: 0.95, y: 0.95 })
          .animation({ curve: Curve.Friction, delay: 75, duration: 415 })
          .combine(TransitionEffect.opacity(0));
  }

  rerender() {
      this.updateDirtyElements();
  }
}

class VideoPlayerLogic {
  seekDisplayTimer = null;
  allBarsDisplayTimer = null;
  dialogController = null;
  constructor(state, handler) {
    this.state = state;
    this.handler = handler;
    this.gestureManager = new VideoGestureManager(this.state, this.handler);
  }

  switchPlayingStatus() {
    console.info(`[FullscreenVideoOverlay] switchPlayingStatus isPlaying:${this.state.isPlaying}.`);
    if (!this.handler) {
      console.warn(`[FullscreenVideoOverlay] switchPlayingStatus isPlaying:${this.state.isPlaying} failed.`);
    }

    if (this.state.isPlaying) {
      this.handler.requestMediaControl(MediaControlAction.PAUSE, INVALID_PARAM);
      this.state.isPlaying = false;
      clearTimeout(this.allBarsDisplayTimer);
    } else {
      this.handler.requestMediaControl(MediaControlAction.PLAY, INVALID_PARAM);
      this.state.isPlaying = true;
      if (this.state.displayState === VideoPlayerComponentDisplayType.VISIBLE) {
        this.resetBarsTimer();
      }
    }
  }

  setMute(mutedStatus) {;
    if (mutedStatus === MutedStatus.DISABLE) {
      return;
    }

    if (this.handler) {
      let param = mutedStatus === MutedStatus.MUTED ? 1 : 0;
      this.handler.requestMediaControl(MediaControlAction.SET_MUTED, String(param));
      this.state.isMuted = mutedStatus;
    } else {
      console.info(`[FullscreenVideoOverlay] request to set muted:${param} failed.`);
    }
  }

  // Exit fullscreen.
  exitFullScreen() {
    if (this.handler) {
      this.handler.requestMediaControl(MediaControlAction.EXIT, INVALID_PARAM);
      this.dialogController?.close();
      console.info(`[FullscreenVideoOverlay] exitfullscreen`);
    } else {
      console.warn(`[FullscreenVideoOverlay] request to exitfullscreen failed.`)
    }
  }

  // Implement the seeking function.
  playbackProgressChange(currentSecond, mode) {
    this.state.currentSecond = currentSecond;
    if (mode === SliderChangeMode.Begin) {
      this.showSeekComponent();
    } else if (mode === SliderChangeMode.End || mode === SliderChangeMode.Click) {
      this.hideSeekComponent();
      if (this.handler)
        this.handler.requestMediaControl(MediaControlAction.SEEK, String(currentSecond));
      this.resetBarsTimer();
    }
  }

  // Prevent the dialog from being closed again when the button is pressed
  // within 1 second after the hand up.
  showSeekComponent() {
    clearTimeout(this.seekDisplayTimer);
    this.state.dialogComponent = DialogComponentType.SeekComponent;
  }

  hideSeekComponent() {
    this.seekDisplayTimer = setTimeout(() => {
        this.state.dialogComponent = DialogComponentType.None;
    }, 1000);
  }

  // Display the bottom bar, top bar and so on.
  showBars() {
    clearTimeout(this.allBarsDisplayTimer);
    this.state.displayState = VideoPlayerComponentDisplayType.VISIBLE;
    if (this.state.isPlaying) {
        this.allBarsDisplayTimer = setTimeout(() => {
            this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
        }, 5000);
    }
  }

  // Hide the bottom bar, top bar and so on.
  hideBars() {
    clearTimeout(this.allBarsDisplayTimer);
    this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
  }

  // Reset the auto-hide timer for bars, only effective when playing.
  resetBarsTimer() {
    if (this.state.isPlaying) {
        clearTimeout(this.allBarsDisplayTimer);
        this.allBarsDisplayTimer = setTimeout(() => {
            this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
        }, 5000);
    }
  }

  // Open the play rate menu.
  openPlayRateMenu() {
    clearTimeout(this.allBarsDisplayTimer);
    this.state.displayState = VideoPlayerComponentDisplayType.PLAYBACK_RATE_MENU;
  }

  // Close the play rate menu and hide all bars.
  closePlaybackRateMenu() {
    clearTimeout(this.allBarsDisplayTimer);
    this.state.displayState = VideoPlayerComponentDisplayType.HIDDEN;
  }

  // Switch the playback rate.
  switchPlaybackRate(rate) {
    if (this.state.currentPlaybackRate === rate) {
        return;
    }
    if (this.handler) {
        this.handler.requestMediaControl(MediaControlAction.SET_PLAYBACK_RATE, String(rate));
        this.state.currentPlaybackRate = rate;
    }
  }

  // Gesture control.
  gestureStart(mode) {
    this.gestureManager.startGesture(mode);
  }
  gestureUpdate(offset, total) {
    this.gestureManager.updateGesture(offset, total);
  }
  gestureEnd() {
    this.gestureManager.endGesture();
  }
  gestureRestore() {
    this.gestureManager.restoreGesture();
  }
}

class VideoPlayerListener {
  constructor(callbacks = {}) {
    this.callbacks = callbacks;
  }

  invokeCallback(callbackName, ...args) {
    const cb = this.callbacks[callbackName];

    if (typeof cb === 'function') {
      cb(...args);
      return true;
    } else {
      console.warn(`[FullscreenVideoOverlay] ${callbackName} is null, skipping execution.`);
    }
  }

  onStatusChanged = (status) => {
    if (!this.invokeCallback('onStatusChanged', status)) {
      console.warn(`[FullscreenVideoOverlay] onStatusChanged fallback, status: ${status}.`);
    }
  }
  onVolumeChanged = (volume) => {
    if (!this.invokeCallback('onVolumeChanged', volume)) {
      console.warn(`[FullscreenVideoOverlay] onVolumeChanged fallback, volume: ${volume}.`);
    }
  }
  onMutedChanged = (muted) => {
    if (!this.invokeCallback('onMutedChanged', muted)) {
      console.warn(`[FullscreenVideoOverlay] onMutedChanged fallback, muted: ${muted}.`);
    }
  }
  onPlaybackRateChanged = (playbackRate) => {
    if (!this.invokeCallback('onPlaybackRateChanged', playbackRate)) {
      console.warn(`[FullscreenVideoOverlay] onPlaybackRateChanged fallback, playbackRate: ${playbackRate}.`);
    }
  }
  onDurationChanged = (duration) => {
    if (!this.invokeCallback('onDurationChanged', duration)) {
      console.warn(`[FullscreenVideoOverlay] onDurationChanged fallback, duration: ${duration}.`);
    }
  }
  onTimeUpdate = (currentPlayTime) => {
    if (!this.invokeCallback('onTimeUpdate', currentPlayTime)) {
      console.warn(`[FullscreenVideoOverlay] onTimeUpdate fallback, currentPlayTime: ${currentPlayTime}.`);
    }
  }
  onBufferedEndTimeChanged = (bufferedEndTime) => {
    if (!this.invokeCallback('onBufferedEndTimeChanged', bufferedEndTime)) {
      console.warn(`[FullscreenVideoOverlay] onBufferedEndTimeChanged fallback, bufferedEndTime: ${bufferedEndTime}.`);
    }
  }
  onEnded = () => {
    if (!this.invokeCallback('onEnded')) {
      console.warn('[FullscreenVideoOverlay] onEnded fallback.');
    }
  }
  onSeeking = () => {
    if (!this.invokeCallback('onSeeking')) {
      console.warn('[FullscreenVideoOverlay] onSeeking fallback.');
    }
  }
  onSeekFinished = () => {
    if (!this.invokeCallback('onSeekFinished')) {
      console.warn('[FullscreenVideoOverlay] onSeekFinished fallback.');
    }
  }
  onError = (error, errorMsg) => {
    if (!this.invokeCallback('onError', error, errorMsg)) {
      console.warn(`[FullscreenVideoOverlay] onError fallback, error: ${error}, errorMsg: ${errorMsg}.`);
    }
  }
  onVideoSizeChanged = (width, height) => {
    if (!this.invokeCallback('onVideoSizeChanged', width, height)) {
      console.warn(`[FullscreenVideoOverlay] onVideoSizeChanged fallback, width: ${width}, height: ${height}.`);
    }
  }
}

class VideoGestureManager {
  _managerMap = new Map();
  _currentManager = null;
  _displayTimer = null;
  constructor(state, player) {
      this._state = state;
      this._player = player;
      this.initManager(state, player);
  }
  initManager(state, player) {
      this._managerMap.set(VideoStateGestureManagerType.SEEK_MANAGER, new SeekManager(state, player));
  }
  startGesture(type) {
      this._currentManager = this._managerMap.get(type);
      this._currentManager?.gestureStart();
      clearTimeout(this._displayTimer);
  }
  updateGesture(offset, total) {
      this._currentManager?.gestureUpdate(offset, total);
  }
  endGesture() {
      // The timer is controlled by the outer.
      this._currentManager?.gestureEnd();
      this._displayTimer = setTimeout(() => {
          this._state.dialogComponent = DialogComponentType.None;
      }, 1000);
      this._currentManager = undefined;
  }
  restoreGesture() {
      this._managerMap.forEach(manager => {
          manager.gestureRestore();
      });
  }
  disposeManager() {
      this._managerMap.clear();
      this._currentManager = undefined;
  }
}

 class SeekManager {
    _gestureStartValue = null;
    constructor(state, player) {
        this._state = state;
        this._player = player;
    }
    gestureStart() {
        this._gestureStartValue = this._state.currentSecond;
        this._state.dialogComponent = DialogComponentType.SeekComponent;
    }
    /**
     * Maps the relationship between gesture swipe distance and video progress:
     * - Total duration > 2 minutes: Seek duration = (Swipe distance / Container width) * Total duration
     * - Total duration <= 2 minutes: Seek duration = (Swipe distance / Container width) * 2 minutes
     * @param offsetX The horizontal swipe offset/distance.
     * @param width The width of the playback container.
     */
    gestureUpdate(offsetX, width) {
      if (width <= 0) {
        return;
      }
      const minReference = 120;
      const DAMPING = this._state.totalSecond <= minReference ? 0.2 : 0.4;
      const referenceDuration = Math.max(this._state.totalSecond, minReference);
      const delta = Math.floor((offsetX * DAMPING / width) * referenceDuration);
      let newVal = this._gestureStartValue + delta;
      newVal = Math.max(0, Math.min(this._state.totalSecond, newVal));
      this._state.currentSecond = newVal;
    }
    gestureEnd() {
      console.info(`[FullscreenVideoOverlay] seek gesture end:${this._state.currentSecond}.`)
      if (this._player)
        this._player.requestMediaControl(MediaControlAction.SEEK, String(this._state.currentSecond));
    }
    gestureRestore() {
    }
}

class SelectorDialog extends ViewPU {
  constructor(parent, params, __localStorage, elmtId = -1, paramsLambda = undefined, extraInfo) {
    super(parent, __localStorage, elmtId, extraInfo);
    if (typeof paramsLambda === 'function') {
      this.paramsGenerator_ = paramsLambda;
    }
    this.__choose_to_upload = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.choose_to_upload'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'choose_to_upload');
    this.__general_cancel = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.general_cancel'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'general_cancel');
    this.__taking_photos_or_videos = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.taking_photos_or_videos'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'taking_photos_or_videos');
    this.__taking_photos = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.taking_photos'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'taking_photos');
    this.__video_recording = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.video_recording'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'video_recording');
    this.__gallery = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.gallery'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'gallery');
    this.__document = new ObservedPropertyObjectPU({ 'id': -1, 'type': -1, params: ['sys.string.document'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }, this, 'document');
    this.setInitiallyProvidedValue(params);
  }

  setInitiallyProvidedValue(params) {
    if (params.callback !== undefined) {
      this.callback = params.callback;
    }
  }

  purgeVariableDependenciesOnElmtId(rmElmtId) {
    this.__choose_to_upload.purgeDependencyOnElmtId(rmElmtId);
    this.__general_cancel.purgeDependencyOnElmtId(rmElmtId);
    this.__taking_photos_or_videos.purgeDependencyOnElmtId(rmElmtId);
    this.__taking_photos.purgeDependencyOnElmtId(rmElmtId);
    this.__video_recording.purgeDependencyOnElmtId(rmElmtId);
    this.__gallery.purgeDependencyOnElmtId(rmElmtId);
    this.__document.purgeDependencyOnElmtId(rmElmtId);
  }

  aboutToBeDeleted() {
    commonEventManager.unsubscribe(this.subscriber, (err) => {
      if (err) {
        console.error(`Failed to unsubscribe. Code is ${err.code}, message is ${err.message}`);
        return;
      }
      // When a subscriber is no longer in use, it should be set to undefined to prevent memory leaks.
      this.subscriber = undefined;
      console.info('Succeeded in unsubscribing.');
    });
    this.__choose_to_upload.aboutToBeDeleted();
    this.__general_cancel.aboutToBeDeleted();
    this.__taking_photos_or_videos.aboutToBeDeleted();
    this.__taking_photos.aboutToBeDeleted();
    this.__video_recording.aboutToBeDeleted();
    this.__gallery.aboutToBeDeleted();
    this.__document.aboutToBeDeleted();
    SubscriberManager.Get().delete(this.id__());
    this.aboutToBeDeletedInternal();
  }

  get choose_to_upload() {
    return this.__choose_to_upload.get();
  }

  set choose_to_upload(newValue) {
    this.__choose_to_upload.set(newValue);
  }

  get general_cancel() {
    return this.__general_cancel.get();
  }

  set general_cancel(newValue) {
    this.__general_cancel.set(newValue);
  }

  get taking_photos_or_videos() {
    return this.__taking_photos_or_videos.get();
  }

  set taking_photos_or_videos(newValue) {
    this.__taking_photos_or_videos.set(newValue);
  }

  get taking_photos() {
    return this.__taking_photos.get();
  }

  set taking_photos(newValue) {
    this.__taking_photos.set(newValue);
  }

  get video_recording() {
    return this.__video_recording.get();
  }

  set video_recording(newValue) {
    this.__video_recording.set(newValue);
  }

  get gallery() {
    return this.__gallery.get();
  }

  set gallery(newValue) {
    this.__gallery.set(newValue);
  }

  get document() {
    return this.__document.get();
  }

  set document(newValue) {
    this.__document.set(newValue);
  }

  fileSelectorListItem(callback, sysResource, text, func) {
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Row.create();
      Row.onClick(() => {
        promptAction.closeCustomDialog(customDialogComponentId);
        func(callback);
      });
      Row.width('calc(100% - 48vp)');
    }, Row);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      SymbolGlyph.create({ 'id': -1, 'type': -1, params: [sysResource], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' });
      SymbolGlyph.fontSize('24vp');
      SymbolGlyph.fontWeight(FontWeight.Medium);
      SymbolGlyph.margin({
        end: LengthMetrics.vp(16)
      });
      SymbolGlyph.fontColor([{ 'id': -1, 'type': -1, params: ['sys.color.font_primary'], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' }]);
    }, SymbolGlyph);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Row.create();
      Row.constraintSize({ minHeight: 56 });
      Row.width('calc(100% - 40vp)');
      Row.border({ width: { bottom: 0.5 }, color: '#33000000' });
    }, Row);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      switch (text) {
        case 'sys.string.gallery':
          Text.create(this.gallery);
          break;
        case 'sys.string.taking_photos_or_videos':
          Text.create(this.taking_photos_or_videos);
          break;
        case 'sys.string.taking_photos':
          Text.create(this.taking_photos);
          break;
        case 'sys.string.video_recording':
          Text.create(this.video_recording);
          break;
        case 'sys.string.document':
          Text.create(this.document);
      }
      Text.fontSize('16vp');
      Text.fontWeight(FontWeight.Medium);
    }, Text);
    Text.pop();
    Row.pop();
    Row.pop();
  }

  fileSelectorDialog(callback) {
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Scroll.create();
    }, Scroll);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Column.create();
    }, Column);
    this.fileSelectorListItem.bind(this)(callback, 'sys.symbol.picture', 'sys.string.gallery', selectPicture);
    let acceptTypes = callback.fileparam.getAcceptType();
    let cameraOption = 'sys.string.taking_photos_or_videos';
    if (isContainImageMimeType(acceptTypes) && !isContainVideoMimeType(acceptTypes)) {
      cameraOption = 'sys.string.taking_photos';
    }
    if (!isContainImageMimeType(acceptTypes) && isContainVideoMimeType(acceptTypes)) {
      cameraOption = 'sys.string.video_recording';
    }
    this.fileSelectorListItem.bind(this)(callback, 'sys.symbol.camera', cameraOption, takePhoto);
    this.fileSelectorListItem.bind(this)(callback, 'sys.symbol.doc_text', 'sys.string.document', selectFile);
    Column.pop();
    Scroll.pop();
  }

  fileSelectorDialogForPhone(callback) {
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Flex.create({ justifyContent: FlexAlign.Center, direction: FlexDirection.Column, alignItems: ItemAlign.Center });
      Flex.height('auto');
    }, Flex);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Row.create();
      Row.constraintSize({ minHeight: 56 });
      Row.width('calc(100% - 48vp)');
      Row.justifyContent(FlexAlign.Center);
    }, Row);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Text.create(this.choose_to_upload);
      Text.fontSize('20vp');
      Text.fontWeight(FontWeight.Bold);
      Text.textAlign(TextAlign.Center);
    }, Text);
    Text.pop();
    Row.pop();
    this.fileSelectorDialog.bind(this)(callback);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Row.create();
      Row.onClick(() => {
        try {
          console.log('Get Alert Dialog handled');
          onShowFileSelectorEvent?.fileresult.handleFileList([]);
          onShowFileSelectorEvent = undefined;
          promptAction.closeCustomDialog(customDialogComponentId);
        }
        catch (error) {
          let message = error.message;
          let code = error.code;
          console.error(`closeCustomDialog error code is ${code}, message is ${message}`);
        }
      });
      Row.constraintSize({ minHeight: 40 });
      Row.margin({
        top: 8,
        bottom: 16
      });
      Row.width('calc(100% - 32vp)');
      Row.justifyContent(FlexAlign.Center);
      Row.flexShrink(0);
    }, Row);
    this.observeComponentCreation2((elmtId, isInitialRender) => {
      Text.create(this.general_cancel);
      Text.fontColor('#FF0A59F7');
      Text.fontSize('16vp');
      Text.fontWeight(FontWeight.Medium);
      Text.textAlign(TextAlign.Center);
    }, Text);
    Text.pop();
    Row.pop();
    Flex.pop();
  }

  setResource() {
    let manager = resourceManager.getSysResourceManager();
    setTimeout(() => {
      manager.getStringByName('choose_to_upload')
        .then((value) => {
          this.choose_to_upload = value;
        });
      manager.getStringByName('general_cancel')
        .then((value) => {
          this.general_cancel = value;
        });
      manager.getStringByName('taking_photos_or_videos')
        .then((value) => {
          this.taking_photos_or_videos = value;
        });
      manager.getStringByName('taking_photos')
        .then((value) => {
          this.taking_photos = value;
        });
      manager.getStringByName('video_recording')
        .then((value) => {
          this.video_recording = value;
        });
      manager.getStringByName('gallery')
        .then((value) => {
          this.gallery = value;
        });
      manager.getStringByName('document')
        .then((value) => {
          this.document = value;
        });
    }, 200);
  }

  initialRender() {
    this.fileSelectorDialogForPhone.bind(this)(this.callback);
    commonEventManager.createSubscriber({ events: [commonEventManager.Support.COMMON_EVENT_LOCALE_CHANGED] })
      .then((commonEventSubscriber) => {
        this.subscriber = commonEventSubscriber;
        commonEventManager.subscribe(commonEventSubscriber, (err, data) => {
          if (err) {
            console.error(`Failed to subscribe. Code is ${err.code}, message is ${err.message}`);
            return;
          }
          this.setResource();
        });
      }).catch((err) => {
        console.error(`CreateSubscriber failed. Code is ${err.code}, message is ${err.message}`);
      });
    let that = this;
    this.getUIContext().getHostContext()?.getApplicationContext().on('applicationStateChange', {
      onApplicationForeground() {
        console.info('SelectorDialog onApplicationForeground');
        that.setResource();
      },
      onApplicationBackground() {
        console.info('SelectorDialog onApplicationBackground');
      }
    });
  }

  rerender() {
    this.updateDirtyElements();
  }

  static getEntryName() {
    return 'SelectorDialog';
  }
}

function selectPicture(callback) {
  let photoResultArray = [];
  let photoSelectOptions = new photoAccessHelper.PhotoSelectOptions();
  if (callback.fileparam.getMode() === FileSelectorMode.FileOpenMode) {
    console.log('allow select single photo or video');
    photoSelectOptions.maxSelectNumber = 1;
  }
  let acceptTypes = callback.fileparam.getAcceptType();
  photoSelectOptions.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_VIDEO_TYPE;
  if (isContainImageMimeType(acceptTypes) && !isContainVideoMimeType(acceptTypes)) {
    photoSelectOptions.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE;
  }
  if (!isContainImageMimeType(acceptTypes) && isContainVideoMimeType(acceptTypes)) {
    photoSelectOptions.MIMEType = photoAccessHelper.PhotoViewMIMETypes.VIDEO_TYPE;
  }

  let photoPicker = new photoAccessHelper.PhotoViewPicker();
  photoPicker.select(photoSelectOptions).then((photoSelectResult) => {
    for (let i = 0; i < photoSelectResult.photoUris.length; i++) {
      photoResultArray.push(photoSelectResult.photoUris[i]);
    }
  }).catch((error) => {
    console.log('selectPicture error' + JSON.stringify(error));
    promptAction.showToast({ message: '无法打开图片功能，请检查是否具备图片功能' });
  }).finally(() => {
    onShowFileSelectorEvent?.fileresult.handleFileList(photoResultArray);
    onShowFileSelectorEvent = undefined;
  });
}

async function getManifestData(bundleName, connectExtensionOrigin, notifyCallback, callback) {
  try {
    const dsProxyHelper = await dataShare.createDataProxyHandle();
    const urisToGet = [`datashareproxy://${bundleName}/browserNativeMessagingHosts`];
    const config = {
      type: dataShare.DataProxyType.SHARED_CONFIG,
    };
    const results = await dsProxyHelper.get(urisToGet, config);
    let foundValid = false;
    for (let i = 0; i < results.length; i++) {
      try {
        const result = results[i];
        const json = result.value;
        let info = JSON.parse(json);
        const infoPath = info.path;
        if (typeof infoPath === 'string') {
          info.path = JSON.parse(infoPath);
          info.abilityName = info.path.abilityName;
        }
        if (info.name && info.description && info.allowed_origins && info.abilityName) {
          console.info('Native message json info is ok');
          if (!Array.isArray(info.allowed_origins)) {
            info.allowed_origins = [info.allowed_origins];
          }
          if (!info.allowed_origins.includes(connectExtensionOrigin)) {
            console.error('Origin not allowed, continue searching');
            continue;
          }
          foundValid = true;
          callback(info);
          break;
        }
      } catch (error) {
        console.error('NativeMessage JSON parse error:', error);
      }
    }
    if (!foundValid) {
      console.error('NativeMessage JSON no valid manifest found');
      callback(undefined);
    }
  } catch (error) {
      callback(undefined);
      console.error('Error getting config:', error);
  }
}


Object.defineProperty(webview.WebviewController.prototype, 'getCertificate', {
  value: function (callback) {
    if (arguments.length !== 0 && arguments.length !== 1) {
      throw new BusinessError(PARAM_CHECK_ERROR,
        'BusinessError 401: Parameter error. The number of params must be zero or one.');
    }

    let certChainData = this.innerGetCertificate();
    if (callback === undefined) {
      console.log('get certificate promise');
      return getCertificatePromise(certChainData);
    } else {
      console.log('get certificate async callback');
      if (typeof callback !== 'function') {
        throw new BusinessError(PARAM_CHECK_ERROR,
          'BusinessError 401: Parameter error. The type of "callback" must be function.');
      }
      return getCertificatePromise(certChainData).then(x509CertArray => {
        callback(undefined, x509CertArray);
      }).catch(error => {
        callback(error, undefined);
      });
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'fileSelectorShowFromUserWeb', {
  value: function (callback) {
    if (onShowFileSelectorEvent) {
      onShowFileSelectorEvent = callback;
      return;
    }
    onShowFileSelectorEvent = callback;
    let currentDevice = deviceinfo.deviceType.toLowerCase();
    if (needShowDialog(callback.fileparam)) {
      promptAction.openCustomDialog({
        builder: () => {
          ViewPU.create(new SelectorDialog(undefined, { callback: callback }));
        },
        onWillDismiss: (dismissDialogAction) => {
          console.info('reason' + JSON.stringify(dismissDialogAction.reason));
          console.log('dialog onWillDismiss');
          if (dismissDialogAction.reason === DismissReason.PRESS_BACK) {
            onShowFileSelectorEvent?.fileresult.handleFileList([]);
            dismissDialogAction.dismiss();
          }
          if (dismissDialogAction.reason === DismissReason.TOUCH_OUTSIDE) {
            onShowFileSelectorEvent?.fileresult.handleFileList([]);
            dismissDialogAction.dismiss();
          }
          onShowFileSelectorEvent = undefined;
        }
      }).then((dialogId) => {
        customDialogComponentId = dialogId;
      })
        .catch((error) => {
          onShowFileSelectorEvent?.fileresult.handleFileList([]);
          onShowFileSelectorEvent = undefined;
          console.error(`openCustomDialog error code is ${error.code}, message is ${error.message}`);
        });
    } else if (currentDevice !== '2in1' && callback.fileparam.isCapture() &&
        (isContainImageMimeType(callback.fileparam.getAcceptType()) || isContainVideoMimeType(callback.fileparam.getAcceptType()))) {
      console.log('take photo will be directly invoked due to the capture property');
      takePhoto(callback);
    } else {
      console.log('selectFile will be invoked by web');
      selectFile(callback);
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'OnMediaCastEnter', {
  value: function () {
    console.log('webview_export OnMediaCastEnter');
    let avCastPicker = new avsession.AVCastPickerHelper(getContext(this));
    try {
      avCastPicker.select();
      console.log('webview_export OnMediaCastEnter select, success');
      onPickerStateChange(avCastPicker);
    } catch (error) {
      console.log('webview_export OnMediaCastEnter select, fail: ', error);
    }
  }
});

async function onPickerStateChange(avCastPicker) {
  // Listening to the picker status
  console.log('webview_export onPickerStateChange, enter');
  try {
    avCastPicker.on('pickerStateChange', (state) => {
      console.info(`webview_export pickerStateChange: picker state change : ${state}`);
      if(state === AVCastPickerState.STATE_APPEARING) {
        console.log('webview_export pickerStateChange, The picker showing.');
      } else if (state === AVCastPickerState.STATE_DISAPPEARING) {
        console.log('webview_export pickerStateChange, The picker hiding');
        avCastPicker = undefined;
      }
    });
  } catch (error) {
    console.log('webview_export onPickerStateChange, fail: ', error);
  }
}

Object.defineProperty(webview.WebviewController.prototype, 'requestPermissionsFromUserWeb', {
  value: function (callback) {
    let accessManger = accessControl.createAtManager();
    let abilityContext = getContext(this);
    accessManger.requestPermissionsFromUser(abilityContext, ['ohos.permission.READ_PASTEBOARD'])
      .then((PermissionRequestResult) => {
        if (PermissionRequestResult.authResults[0] === 0) {
          console.log('requestPermissionsFromUserWeb is allowed');
          callback.request.grant(callback.request.getAccessibleResource());
        }
        else {
          console.log('requestPermissionsFromUserWeb is refused');
          callback.request.deny();
        }
      })
      .catch((error) => {
        callback.request.deny();
      });
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'openAppLink', {
  value: function (callback) {
    let abilityContext = getContext(this);
    try {
      let option = {
        appLinkingOnly: true
      };
      console.log('begin openAppLink');
      abilityContext.openLink(callback.url, option, null)
        .then(() => {
          console.log('applink success openLink');
          callback.result.cancelLoad();
        })
        .catch((error) => {
          console.log(`applink openLink ErrorCode: ${error.code},  Message: ${error.message}`);
          callback.result.continueLoad();
        });
    } catch (err) {
      console.log(`applink openLink ErrorCode: ${err.code},  Message: ${err.message}`);
      setTimeout(() => {
        callback.result.continueLoad();
      }, 1);
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'innerWebNativeMessageManager', {
  value: function (callback) {
    console.info('innerWebNativeMessageManager called');
    try {
      let bundleName = callback.bundleName;
      let readPipe = callback.readPipe;
      let writePipe = callback.writePipe;
      let connectExtensionOrigin = callback.extensionOrigin;
      getManifestData(bundleName, connectExtensionOrigin, callback.result, (result) => {
        try {
        if (!result) {
          console.error(`NativeMessage find DateShare is no ${bundleName} config`);
          callback.result.onFailed(4001);
          return;
        }
        let wantInfo = {
          bundleName: callback.bundleName,
          abilityName: result.abilityName,
          parameters: {
            'ohos.arkweb.messageReadPipe': { 'type': 'FD', 'value': readPipe },
            'ohos.arkweb.messageWritePipe': { 'type': 'FD', 'value': writePipe },
            'ohos.arkweb.extensionOrigin': connectExtensionOrigin
          },
        };
        console.debug(`innerWebNativeMessageManager want  ${JSON.stringify(wantInfo)}`);
        let options = {
          onConnect(connection) {
            callback.result.onConnect(connection.connectionId);
          },
          onDisconnect(connection) {
            callback.result.onDisconnect(connection.connectionId);
          },
          onFailed(code) {
            callback.result.onFailed(code);
          }
        };
        let abilityContext = getContext(this);
        let connectId = webNativeMessagingExtensionManager.connectNative(abilityContext, wantInfo, options);
        console.log(`innerWebNativeMessageManager  connectionId : ${connectId}` );
      } catch (error) {
        console.log(`inner callback error Message: ${JSON.stringify(error)}`);
      }
      });
    } catch (err) {
      console.log(`innerWebNativeMessageManager Message: ${JSON.stringify(err)}`);
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'innerNativeMessageDisconnect', {
  value: function (callback) {
    try {
      if (!callback) {
        console.error('Messaging disconnect failed: callback is undefined or null');
        return;
      }
      let connectId = callback.connectId;
      if (connectId === undefined || connectId === null) {
        console.error('Messaging disconnect failed: connectId is undefined or null');
        return;
      }
      console.log(`Messaging disconnect connectId= ${connectId}`);
      webNativeMessagingExtensionManager.disconnectNative(connectId);
    } catch (error) {
      console.error(`Messaging disconnect error: ${JSON.stringify(error)}`);
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'innerCanIUse', {
  value: function (callback) {
    let result = canIUse(callback.syscap);
    return result;
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'onFullScreenVideoOverlayEnter', {
  value: function (callback) {
    let dialogState = { id: 0 };
    let closeDialog = () => {
      if (dialogState.id > 0) {
        promptAction.closeCustomDialog(dialogState.id);
      }
    };
    promptAction.openCustomDialog({
      builder: () => {
        let jsDialog = new XComponentDialog(undefined, { callback: callback });
        jsDialog.setController({ close: closeDialog });
        ViewPU.create(jsDialog);
        let paramsLambda = () => { return {}; };
        jsDialog.paramsGenerator_ = paramsLambda;
      },
      autoCancel: false,
      alignment: DialogAlignment.Center,
      maskColor: Color.Black,
      backgroundColor: Color.Transparent,
      cornerRadius: 0,
      shadow: { radius: 0 },
      width: '100%',
      height: '100%',
      transition: TransitionEffect.IDENTITY,
      onWillDismiss: (dismissDialogAction) => {
        if (dismissDialogAction.reason === DismissReason.PRESS_BACK ||
            dismissDialogAction.reason === DismissReason.TOUCH_OUTSIDE) {
          callback.handler.requestMediaControl(MediaControlAction.EXIT, INVALID_PARAM);
          closeDialog();
        }
      }
    }).then((dialogId) => {
      dialogState.id = dialogId;
    }).catch((error) => {
      console.error(`[FullscreenVideoOverlay] openCustomDialog failed: ${error.code}`);
    });
  }
});

export default webview;
