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

import {
  Flex,
  Text,
  Column,
  Row,
  Scroll,
  SymbolGlyph,
  FileSelectorParam,
  FileSelectorResult,
  OnShowFileSelectorEvent,
  DismissDialogAction,
  DismissReason,
  $r,
  FontWeight,
  Margin,
  EdgeWidths,
  FlexAlign,
  FlexDirection,
  ItemAlign,
  Builder
} from '@ohos.arkui.component'
import { BusinessError } from '@ohos.base'
import { LengthMetrics } from 'arkui.Graphics'
import cameraPicker from '@ohos.multimedia.cameraPicker'
import camera from '@ohos.multimedia.camera'
import deviceInfo from '@ohos.deviceInfo'
import { UIContext } from '@ohos.arkui.UIContext'

class SelectorDialog {
  private customDialogComponentId: int = 0

  defaultOnShowFileSelector(param: FileSelectorParam, result: FileSelectorResult) {
    console.info('defaultOnShowFileSelector implementation')

    const event: OnShowFileSelectorEvent = { fileSelector: param, result: result }
    const currentDevice = deviceInfo.deviceType.toLowerCase()
    if (needShowDialog(event)) {
      UIContext.getFocusedUIContext()?.getPromptAction()
        .openCustomDialog({
          builder: () => this.fileSelectorDialog(event),
          onWillDismiss: (dismissDialogAction: DismissDialogAction) => {
            console.info('reason: ' + JSON.stringify(dismissDialogAction.reason))
            console.log('dialog onWillDismiss')
            if (dismissDialogAction.reason === DismissReason.PRESS_BACK) {
              event.result.handleFileList([])
              dismissDialogAction.dismiss()
            }
            if (dismissDialogAction.reason === DismissReason.TOUCH_OUTSIDE) {
              event.result.handleFileList([])
              dismissDialogAction.dismiss()
            }
          }
        })
        .then((dialogId) => {
          this.customDialogComponentId = dialogId
        })
        .catch((error) => {
          event.result.handleFileList([])
          console.error(`openCustomDialog error code is ${error.code}, message is ${error.message}`)
        })
    } else if (currentDevice !== '2in1' && event.fileSelector.isCapture() &&
      (isContainImageMimeType(event.fileSelector.getAcceptType()) ||
      isContainVideoMimeType(event.fileSelector.getAcceptType()))) {
      console.log('takePhoto will be directly invoked due to the capture property')
      takePhoto(event)
    } else {
      console.log('selectFile will be invoked by web')
      selectFile(event)
    }
  }

  @Builder
  private fileSelectorDialog(event: OnShowFileSelectorEvent) {
    Flex({ justifyContent: FlexAlign.Center, direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
      Row() {
        Text($r('sys.string.choose_to_upload'))
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }
      .constraintSize({ minHeight: 56 })
      .width('calc(100% - 48vp)')
      .justifyContent(FlexAlign.Center)

      Scroll() {
        Column() {
          this.fileSelectorListItem('sys.symbol.picture', 'sys.string.gallery', selectPicture, event)
          const acceptTypes = event.fileSelector.getAcceptType()
          let cameraOption = 'sys.string.taking_photos_or_videos'
          if (isContainImageMimeType(acceptTypes) && !isContainVideoMimeType(acceptTypes)) {
            cameraOption = 'sys.string.taking_photos'
          }
          if (!isContainImageMimeType(acceptTypes) && isContainVideoMimeType(acceptTypes)) {
            cameraOption = 'sys.string.video_recording'
          }
          this.fileSelectorListItem('sys.symbol.camera', cameraOption, takePhoto, event)
          this.fileSelectorListItem('sys.symbol.doc_text', 'sys.string.document', selectFile, event)
        }
      }

      Row() {
        Text($r('sys.string.general_cancel'))
          .fontColor('#FF0A59F7')
          .fontWeight(FontWeight.Medium)
      }
      .onClick((e) => {
        try {
          console.log('Get Alert Dialog handled')
          event.result.handleFileList([])
          UIContext.getFocusedUIContext()?.getPromptAction().closeCustomDialog(this.customDialogComponentId)
        } catch (error: BusinessError) {
          console.error(`closeCustomDialog error code is ${error.code}, message is ${error.message}`)
        }
      })
      .constraintSize({ minHeight: 40 })
      .margin({
        top: 8,
        bottom: 16
      } as Margin)
      .width('calc(100% - 32vp)')
      .justifyContent(FlexAlign.Center)
    }
    .height('auto')
  }

  @Builder
  private fileSelectorListItem(sysResource: string, text: string, func: (e: OnShowFileSelectorEvent) => void,
    event: OnShowFileSelectorEvent) {
    Row() {
      SymbolGlyph($r(sysResource))
        .width(24)
        .height(24)
        .fontSize(24)
        .fontWeight(FontWeight.Medium)
        .margin({
          end: LengthMetrics.vp(16)
        })
        .fontColor([$r('sys.color.font_primary')])
      Row() {
        Text($r(text))
          .fontWeight(FontWeight.Medium)
      }
      .constraintSize({ minHeight: 56 })
      .width('calc(100% - 40vp)')
      .border({ width: { bottom: 0.5 } as EdgeWidths, color: '#33000000' })
    }
    .onClick((e) => {
      UIContext.getFocusedUIContext()?.getPromptAction().closeCustomDialog(this.customDialogComponentId)
      func(event)
    })
    .width('calc(100% - 48vp)')
  }
}

function needShowDialog(event: OnShowFileSelectorEvent) {
  let result = false
  try {
    const currentDevice = deviceInfo.deviceType.toLowerCase()
    if (currentDevice === '2in1') {
      return false
    }
    if (event.fileSelector.isCapture()) {
      console.log('input element contain capture tag, not show dialog')
      return false
    }
    const acceptTypes = event.fileSelector.getAcceptType()
    if (isContainImageMimeType(acceptTypes) || isContainVideoMimeType(acceptTypes)) {
      result = true
    }
  } catch (error) {
    console.log('show dialog happened error: ' + JSON.stringify(error))
  }
  return result
}

function isContainImageMimeType(acceptTypes: Array<string> | undefined) {
  if (!(acceptTypes instanceof Array)) {
    return false
  }
  if (acceptTypes.length < 1) {
    return true
  }

  const imageTypes = ['tif', 'xbm', 'tiff', 'pjp', 'jfif', 'bmp', 'avif', 'apng', 'ico',
    'webp', 'svg', 'gif', 'svgz', 'jpg', 'jpeg', 'png', 'pjpeg']
  for (const acceptType of acceptTypes) {
    for (const imageType of imageTypes) {
      if ((acceptType as string).includes(imageType)) {
        return true
      }
    }
  }
  return false
}

function isContainVideoMimeType(acceptTypes: Array<string> | undefined) {
  if (!(acceptTypes instanceof Array)) {
    return false
  }
  if (acceptTypes.length < 1) {
    return true
  }

  const videoTypes = ['ogm', 'ogv', 'mpg', 'mp4', 'mpeg', 'm4v', 'webm']
  for (const acceptType of acceptTypes) {
    for (const videoType of videoTypes) {
      if ((acceptType as string).includes(videoType)) {
        return true
      }
    }
  }
  return false
}

function selectPicture(event: OnShowFileSelectorEvent) {
  event.result.handleFileList([])
  UIContext.getFocusedUIContext()?.getPromptAction().showToast({ message: '无法打开图片功能，请检查是否具备图片功能' })
}

function takePhoto(event: OnShowFileSelectorEvent) {
  const pickerProfileOptions: cameraPicker.PickerProfile = {
    'cameraPosition': camera.CameraPosition.CAMERA_POSITION_BACK,
  }
  const acceptTypes = event.fileSelector.getAcceptType()
  const mediaType: cameraPicker.PickerMediaType[] = []
  if (isContainImageMimeType(acceptTypes)) {
    mediaType.push(cameraPicker.PickerMediaType.PHOTO)
  }
  if (isContainVideoMimeType(acceptTypes)) {
    mediaType.push(cameraPicker.PickerMediaType.VIDEO)
  }
  const result: string[] = []
  cameraPicker.pick(UIContext.getFocusedUIContext()?.getHostContext()!, mediaType, pickerProfileOptions)
    .then((pickerResult) => {
      result.push(pickerResult.resultUri)
    })
    .catch((error) => {
      console.log('selectFile error: ' + JSON.stringify(error))
      UIContext.getFocusedUIContext()?.getPromptAction().showToast({
        message: '无法打开拍照功能，请检查是否具备拍照功能'
      })
    })
    .finally(() => {
      event.result.handleFileList(result)
    })
}

function selectFile(event: OnShowFileSelectorEvent) {
  event.result.handleFileList([])
  UIContext.getFocusedUIContext()?.getPromptAction().showToast({ message: '无法打开文件功能，请检查是否具备文件功能' })
}