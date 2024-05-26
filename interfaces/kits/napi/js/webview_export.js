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
let picker = requireNapi('file.picker');
let cameraPicker = requireNapi('multimedia.cameraPicker');
let camera = requireNapi('multimedia.camera');
let accessControl = requireNapi('abilityAccessCtrl');
let deviceinfo = requireInternal('deviceInfo');
const PARAM_CHECK_ERROR = 401;

const ERROR_MSG_INVALID_PARAM = 'Invalid input parameter';

let errMsgMap = new Map();
errMsgMap.set(PARAM_CHECK_ERROR, ERROR_MSG_INVALID_PARAM);

class BusinessError extends Error {
  constructor(code, errorMsg = "undefined") {
    if(errorMsg === "undefined") {
      let msg = errMsgMap.get(code);
      super(msg);      
    }else{
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
    if (isContainImageMimeType(acceptTypes) && !isContainVideoMimeType(acceptTypes)) {
      mediaType.push(cameraPicker.PickerMediaType.PHOTO);
    } else if (!isContainImageMimeType(acceptTypes) && isContainVideoMimeType(acceptTypes)) {
      mediaType.push(cameraPicker.PickerMediaType.VIDEO);
    } else {
      mediaType.push(cameraPicker.PickerMediaType.PHOTO);
      mediaType.push(cameraPicker.PickerMediaType.VIDEO);
    }
    cameraPicker.pick(getContext(this), mediaType, pickerProfileOptions)
    .then((pickerResult) => {
      if (pickerResult.resultCode === 0) {
        selectResult.handleFileList([pickerResult.resultUri]);
      }
    }).catch((error) => {
      console.log('selectFile error:' + JSON.stringify(error));
    });

  } catch (error) {
    console.log('the pick call failed, error code' + JSON.stringify(error));
  }
}

function needShowDialog(params) {
  let result = false;
  try {
    let currentDevice = deviceinfo.deviceType.toLowerCase();
    if (currentDevice !== 'phone') {
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
    let documentSelectOptions = createDocumentSelectionOptions(param);
    let documentPicker = new picker.DocumentViewPicker();
    documentPicker.select(documentSelectOptions)
      .then((documentSelectResult) => {
        if (documentSelectResult && documentSelectResult.length > 0) {
          let filePath = documentSelectResult;
          result.handleFileList(filePath);
        }
      }).catch((error) => {
        console.log('selectFile error: ' + JSON.stringify(error));
      });
  } catch (error) {
    console.log('picker error: ' + JSON.stringify(error));
  }
}

function createDocumentSelectionOptions(param) {
  let documentSelectOptions = new picker.DocumentSelectOptions();
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
      case FileSelectorMode.FileSaveMode:
        break;
      default:
        break;
    }
    documentSelectOptions.fileSuffixFilters = [];
    let suffix = param.getAcceptType().join(',');
    if (suffix) {
      documentSelectOptions.fileSuffixFilters.push(suffix);
    }
 } catch (error) {
    console.log('selectFile error: ' + + JSON.stringify(error));
    return documentSelectOptions;
 }
  return documentSelectOptions;
}

function isContainImageMimeType(acceptTypes) {
  if (!(acceptTypes instanceof Array) || acceptTypes.length < 1) {
    return false;
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
  if (!(acceptTypes instanceof Array) || acceptTypes.length < 1) {
    return false;
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

function selectPicture(param, selectResult) {
  try {
    let photoResultArray = [];
    let photoSelectOptions = new picker.PhotoSelectOptions();
    let acceptTypes = param.getAcceptType();
    photoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_VIDEO_TYPE;
    if (isContainImageMimeType(acceptTypes) && !isContainVideoMimeType(acceptTypes)) {
      photoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE;
    }
    if (!isContainImageMimeType(acceptTypes) && isContainVideoMimeType(acceptTypes)) {
      photoSelectOptions.MIMEType = picker.PhotoViewMIMETypes.VIDEO_TYPE;
    }

    let photoPicker = new picker.PhotoViewPicker();
    photoPicker.select(photoSelectOptions).then((photoSelectResult) => {
      if (photoSelectResult.photoUris.length <= 0) {
        return;
      }
      for (let i = 0; i < photoSelectResult.photoUris.length; i++) {
        photoResultArray.push(photoSelectResult.photoUris[i]);
      }
      selectResult.handleFileList(photoResultArray);
    });
  } catch (error) {
    console.log('selectPicture error' + JSON.stringify(error));
  }
}

Object.defineProperty(webview.WebviewController.prototype, 'getCertificate', {
  value: function (callback) {
    if (arguments.length !== 0 && arguments.length !== 1) {
      throw new BusinessError(PARAM_CHECK_ERROR, 
        "BusinessError 401: Parameter error. The number of params must be zero or one.");
    }

    let certChainData = this.innerGetCertificate();
    if (callback === undefined) {
      console.log('get certificate promise');
      return getCertificatePromise(certChainData);
    } else {
      console.log('get certificate async callback');
      if (typeof callback !== 'function') {
        throw new BusinessError(PARAM_CHECK_ERROR, 
          "BusinessError 401: Parameter error. The type of 'callback' must be function." );
      }
      getCertificatePromise(certChainData).then(x509CertArray => {
        callback(undefined, x509CertArray);
      }).catch(error => {
        callback(error, undefined);
      });
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'fileSelectorShowFromUserWeb', {
  value:  function (callback) {
    if (needShowDialog(callback.fileparam)) {
      ActionSheet.show({
        title: '选择上传',
        autoCancel: true,
        confirm: {
          defaultFocus: true,
          value: '取消',
          style: DialogButtonStyle.DEFAULT,
          action: () => {
            console.log('Get Alert Dialog handled');
          }
        },
        cancel: () => {
          console.log('actionSheet canceled');
        },
        alignment: DialogAlignment.Bottom,
        offset: { dx: 0, dy: -10 },
        sheets: [
          {
            icon: $r('sys.media.ohos_ic_public_albums'),
            title: '图片',
            action: () => {
              selectPicture(callback.fileparam, callback.fileresult);
            }
          },
          {
            icon: $r('sys.media.ohos_ic_public_camera'),
            title: '拍照',
            action: () => {
              takePhoto(callback.fileparam, callback.fileresult);
             }
          },
          {
            icon: $r('sys.media.ohos_ic_public_email'),
            title: '文件',
            action: () => {
              selectFile(callback.fileparam, callback.fileresult);
            }
          }
        ]
      });
    } else {
      console.log('selectFile will be invoked by web');
      selectFile(callback.fileparam, callback.fileresult);
    }
  }
});

Object.defineProperty(webview.WebviewController.prototype, 'requestPermissionsFromUserWeb', {
  value:  function (callback) {
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
  value:  function (callback) {
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
      callback.result.continueLoad();
    }
  }
});

export default webview;
