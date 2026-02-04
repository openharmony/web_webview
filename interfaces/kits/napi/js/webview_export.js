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
let Environment = requireNapi('file.environment');

const PARAM_CHECK_ERROR = 401;

const ERROR_MSG_INVALID_PARAM = 'Invalid input parameter';

let errMsgMap = new Map();
errMsgMap.set(PARAM_CHECK_ERROR, ERROR_MSG_INVALID_PARAM);
let customDialogComponentId = 0;
let onShowFileSelectorEvent = undefined;

let defaultBasicPath = 'file://docs';
let defaultPublicPath = initDefaultPublicPath();

let publicDirectoryMap = new Map([
    ['desktop', defaultPublicPath + 'desktop'],
    ['documents', defaultPublicPath + 'documents'],
    ['downloads', defaultPublicPath + 'download'],
    ['music', defaultPublicPath + 'music'],
    ['pictures', defaultPublicPath + 'images'],
    ['videos', defaultPublicPath + 'videos'],
]);

function initDefaultPublicPath() {
  if (deviceinfo.deviceType.toLowerCase() !== '2in1') {
    return '';
  }
  let path = Environment.getUserDownloadDir();
  const pathEndIndex = path.lastIndexOf('/');
  if (pathEndIndex !== -1) {
    return path.substring(0,pathEndIndex + 1);
  }
  return '';
}

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
    .then((pickerResult) => {
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
      .then((documentSaveResult) => {
        let filePaths = documentSaveResult;
        let tempUri = '';
        if (filePaths.length > 0) {
          let fileName = filePaths[0].substr(filePaths[0].lastIndexOf('/'));
          let tempPath = getContext(this).filesDir + fileName;
          tempUri = fileUri.getUriFromPath(tempPath);
          let randomAccessFile = fileIo.createRandomAccessFileSync(tempPath, fileIo.OpenMode.CREATE);
          randomAccessFile.close();

          let watcher = fileIo.createWatcher(tempPath, 0x4, () => {
            fileIo.copy(tempUri, filePaths[0]).then(() => {
              console.log('Web save file succeeded in copying. ');
              fileIo.unlink(tempPath);
            }).catch((err) => {
              console.error(`Web save file failed to copy: ${JSON.stringify(err)}`);
            }).finally(() => {
              watcher.stop();
            });
          });
          watcher.start();
        }
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

function createDocumentSelectionOptions(param) {
  let documentSelectOptions = new picker.DocumentSelectOptions();
  let currentDevice = deviceinfo.deviceType.toLowerCase();
  try {
    let defaultSelectNumber = 500;
    let defaultSelectMode = picker.DocumentSelectMode.MIXED;
    documentSelectOptions.maxSelectNumber = defaultSelectNumber;
    documentSelectOptions.selectMode = defaultSelectMode;
    if (currentDevice === '2in1') {
      documentSelectOptions.defaultFilePathUri = getDefaultPath(param);
    }
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
    if (currentDevice === '2in1') {
      documentSaveOptions.defaultFilePathUri = getDefaultPath(param);
    }
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
  return defaultBasicPath + path;
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
    console.log(`canIUse ${callback.syscap} supported is ${result}`);
    return result;
  }
});

export default webview;
