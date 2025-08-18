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
let deviceinfo = requireInternal('deviceInfo');
let promptAction = requireNapi('promptAction');
const PARAM_CHECK_ERROR = 401;

const ERROR_MSG_INVALID_PARAM = 'Invalid input parameter';

let errMsgMap = new Map();
errMsgMap.set(PARAM_CHECK_ERROR, ERROR_MSG_INVALID_PARAM);
let customDialogComponentId = 0;

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

function takePhoto(param, selectResult) {
  try {
    let pickerProfileOptions = {
      'cameraPosition': camera.CameraPosition.CAMERA_POSITION_BACK,
    };
    let acceptTypes = param.getAcceptType();
    let mediaType = [];
    if (isContainImageMimeType(acceptTypes)) {
      mediaType.push(cameraPicker.PickerMediaType.PHOTO);
    }
    if (isContainVideoMimeType(acceptTypes)) {
      mediaType.push(cameraPicker.PickerMediaType.VIDEO);
    }
    cameraPicker.pick(getContext(this), mediaType, pickerProfileOptions)
      .then((pickerResult) => {
        selectResult.handleFileList([pickerResult.resultUri]);
      }).catch((error) => {
        console.log('selectFile error:' + JSON.stringify(error));
        throw error;
      });

  } catch (error) {
    console.log('the pick call failed, error code' + JSON.stringify(error));
    selectResult.handleFileList([]);
    promptAction.showToast({ message: '无法打开拍照功能，请检查是否具备拍照功能' });
  }
}

function needShowDialog(params) {
  let result = false;
  try {
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

function selectFile(param, result) {
  try {
    let documentPicker = new picker.DocumentViewPicker();
    if (param.getMode() !== FileSelectorMode.FileSaveMode) {
      documentPicker.select(createDocumentSelectionOptions(param))
        .then((documentSelectResult) => {
          let filePath = documentSelectResult;
          result.handleFileList(filePath);
        }).catch((error) => {
          console.log('selectFile error: ' + JSON.stringify(error));
          throw error;
        });
    } else {
      documentPicker.save(createDocumentSaveOptions(param))
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
                console.log('Web save file succeeded in copying.');
                fileIo.unlink(tempPath);
              }).catch((err) => {
                console.error(`Web save file failed to copy: ${JSON.stringify(err)}.`);
              }).finally(() => {
                watcher.stop();
              });
            });
            watcher.start();
          }
          result.handleFileList([tempUri]);
        }).catch((error) => {
        console.log('saveFile error: ' + JSON.stringify(error));
        throw error;
      });
    }
  } catch (error) {
    console.log('picker error: ' + JSON.stringify(error));
    result.handleFileList([]);
    promptAction.showToast({ message: '无法打开文件功能，请检查是否具备文件功能' });
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
    if (suffix) {
      documentSelectOptions.fileSuffixFilters.push(suffix);
    }
    if (currentDevice !== 'phone') {
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
    let suffix = param.getAcceptType().join(',');
    if (suffix) {
      documentSaveOptions.fileSuffixChoices.push(suffix);
    }
    if (currentDevice !== 'phone') {
      documentSaveOptions.fileSuffixChoices.push('.*');
    }
  } catch (error) {
    console.log('saveFile error: ' + + JSON.stringify(error));
  }
  return documentSaveOptions;
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

function fileSelectorListItem(callback, sysResource, text, func) {
  const itemCreation = (elmtId, isInitialRender) => {
    ViewStackProcessor.StartGetAccessRecordingFor(elmtId);
    itemCreation2(elmtId, isInitialRender);
    if (!isInitialRender) {
      ListItem.pop();
    }
    ViewStackProcessor.StopGetAccessRecording();
  };
  const itemCreation2 = (elmtId, isInitialRender) => {
    ListItem.create(deepRenderFunction, true);
    ListItem.onClick(() => {
      promptAction.closeCustomDialog(customDialogComponentId);
      func(callback.fileparam, callback.fileresult);
    });
    ListItem.height(48);
    ListItem.padding({
      left: 24,
      right: 24
    });
  };
  const deepRenderFunction = (elmtId, isInitialRender) => {
    itemCreation(elmtId, isInitialRender);
    Row.create();
    SymbolGlyph.create({ 'id': -1, 'type': -1, params: [sysResource], 'bundleName': 'com.example.selectdialog', 'moduleName': 'entry' });
    SymbolGlyph.width(24);
    SymbolGlyph.height(24);
    SymbolGlyph.fontSize(24);
    SymbolGlyph.margin({
      right: 16
    });
    Row.create();
    Row.width(deviceinfo.deviceType.toLowerCase() === '2in1' ? 312 : 240);
    Row.border({ width: { bottom: 0.5 }, color: '#33000000' });
    Text.create(text);
    Text.fontSize(16);
    Text.fontWeight(FontWeight.Medium);
    Text.lineHeight(19);
    Text.margin({
      top: 13,
      bottom: 13
    });
    Text.pop();
    Row.pop();
    Row.pop();
    ListItem.pop();
  };
  itemCreation(ViewStackProcessor.AllocateNewElmetIdForNextComponent(), true);
  ListItem.pop();
}

function fileSelectorDialog(callback) {
  Row.create();
  Row.height(56);
  Text.create('选择上传');
  Text.fontSize(20);
  Text.fontWeight(FontWeight.Bold);
  Text.lineHeight(23);
  Text.margin({
    top: 15,
    bottom: 15,
    left: 24,
    right: 24,
  });
  Text.pop();
  Row.pop();
  List.create();
  List.width('100%');
  fileSelectorListItem(callback, 'sys.symbol.picture', '照片', selectPicture);
  fileSelectorListItem(callback, 'sys.symbol.camera', '拍照', takePhoto);
  fileSelectorListItem(callback, 'sys.symbol.doc_text', '文件', selectFile);
  List.pop();
}

function fileSelectorDialogForPC(callback) {
  Column.create();
  Column.height(272);
  Column.width(400);
  fileSelectorDialog(callback);
  Row.create();
  Row.onClick(() => {
    try {
      console.log('Get Alert Dialog handled');
      callback.fileresult.handleFileList([]);
      promptAction.closeCustomDialog(customDialogComponentId);
    }
    catch (error) {
      let message = error.message;
      let code = error.code;
      console.error(`closeCustomDialog error code is ${code}, message is ${message}`);
    }
  });
  Row.width(368);
  Row.height(40);
  Row.margin(16);
  Row.borderRadius(5);
  Row.backgroundColor('#ededed');
  Row.justifyContent(FlexAlign.Center);
  Text.create('取消');
  Text.fontSize(16);
  Text.fontColor('#FF0A59F7');
  Text.fontWeight(FontWeight.Medium);
  Text.margin({
    top: 10,
    bottom: 10,
    left: 16,
    right: 16
  });
  Text.pop();
  Row.pop();
  Column.pop();
}

function fileSelectorDialogForPhone(callback) {
  Column.create();
  Column.height(264);
  Column.width(328);
  fileSelectorDialog(callback);
  Row.create();
  Row.onClick(() => {
    try {
      console.log('Get Alert Dialog handled');
      callback.fileresult.handleFileList([]);
      promptAction.closeCustomDialog(customDialogComponentId);
    }
    catch (error) {
      let message = error.message;
      let code = error.code;
      console.error(`closeCustomDialog error code is ${code}, message is ${message}`);
    }
  });
  Row.width(296);
  Row.height(40);
  Row.margin({
    top: 8,
    bottom: 16,
    left: 16,
    right: 16
  });
  Row.borderRadius(5);
  Row.justifyContent(FlexAlign.Center);
  Text.create('取消');
  Text.fontSize(16);
  Text.fontColor('#FF0A59F7');
  Text.fontWeight(FontWeight.Medium);
  Text.margin({
    top: 10,
    bottom: 10,
    left: 104,
    right: 104
  });
  Text.pop();
  Row.pop();
  Column.pop();
}

function selectPicture(param, selectResult) {
  try {
    let photoResultArray = [];
    let photoSelectOptions = new photoAccessHelper.PhotoSelectOptions();
    if (param.getMode() === FileSelectorMode.FileOpenMode) {
      console.log('allow select single photo or video');
      photoSelectOptions.maxSelectNumber = 1;
    }
    let acceptTypes = param.getAcceptType();
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
      selectResult.handleFileList(photoResultArray);
    });
  } catch (error) {
    console.log('selectPicture error' + JSON.stringify(error));
    selectResult.handleFileList([]);
    promptAction.showToast({ message: '无法打开图片功能，请检查是否具备图片功能' });
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
    let currentDevice = deviceinfo.deviceType.toLowerCase();
    if (needShowDialog(callback.fileparam)) {
      promptAction.openCustomDialog({
        builder: () => {
          if (currentDevice === '2in1') {
            fileSelectorDialogForPC(callback);
          } else {
            fileSelectorDialogForPhone(callback);
          }
        },
        onWillDismiss: (dismissDialogAction) => {
          console.info('reason' + JSON.stringify(dismissDialogAction.reason));
          console.log('dialog onWillDismiss');
          if (dismissDialogAction.reason === DismissReason.PRESS_BACK) {
            callback.fileresult.handleFileList([]);
            dismissDialogAction.dismiss();
          }
          if (dismissDialogAction.reason === DismissReason.TOUCH_OUTSIDE) {
            callback.fileresult.handleFileList([]);
            dismissDialogAction.dismiss();
          }
        }
      }).then((dialogId) => {
        customDialogComponentId = dialogId;
      })
        .catch((error) => {
          callback.fileresult.handleFileList([]);
          console.error(`openCustomDialog error code is ${error.code}, message is ${error.message}`);
        });
    } else if (callback.fileparam.isCapture() &&
      (isContainImageMimeType(callback.fileparam.getAcceptType()) || isContainVideoMimeType(callback.fileparam.getAcceptType()))) {
      console.log('take photo will be directly invoked due to the capture property');
      takePhoto(callback.fileparam, callback.fileresult);
    } else {
      console.log('selectFile will be invoked by web');
      selectFile(callback.fileparam, callback.fileresult);
    }
  }
});

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

export default webview;
