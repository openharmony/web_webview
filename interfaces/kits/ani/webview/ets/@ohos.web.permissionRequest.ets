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

import abilityAccessCtrl, { PermissionRequestResult } from '@ohos.abilityAccessCtrl'
import { PermissionRequest, OnPermissionRequestEvent } from '@ohos.arkui.component'
import { UIContext } from '@ohos.arkui.UIContext'

class PermissionClipboard {
  static defaultPermissionClipboard(request: PermissionRequest) {
    const event: OnPermissionRequestEvent = { request: request }

    const accessManager = abilityAccessCtrl.createAtManager()
    const abilityContext = UIContext.resolveUIContext()?.getHostContext()!
    accessManager.requestPermissionsFromUser(abilityContext, ['ohos.permission.READ_PASTEBOARD'])
      .then((data: PermissionRequestResult) => {
        if (data.authResults[0] === 0) {
          console.log('requestPermissionsFromUserWeb is allowed')
          event.request.grant(event.request.getAccessibleResource())
        } else {
          console.log('requestPermissionsFromUserWeb is refused')
          event.request.deny()
        }
      })
      .catch((error) => {
        console.error(`requestPermissionsFromUserWeb error code is ${error.code}, message is ${error.message}`)
        event.request.deny()
      })
  }
}