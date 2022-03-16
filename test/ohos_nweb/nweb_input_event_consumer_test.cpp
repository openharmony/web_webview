/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_input_event_consumer_test.h"
#include "nweb_test_log.h"

namespace OHOS::NWeb {
NWebInputEventConsumerTest::NWebInputEventConsumerTest(std::shared_ptr<NWeb> nweb)
    : nwebweak_(nweb) {}

void NWebInputEventConsumerTest::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    DispatchPointerEvent(pointerEvent);
    pointerEvent->MarkProcessed();
}

void NWebInputEventConsumerTest::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    DispatchKeyEvent(keyEvent);
    keyEvent->MarkProcessed();
}

void NWebInputEventConsumerTest::DispatchPointerEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (nwebweak_.expired()) {
        TESTLOG_W("nweb instance has expired");
        return;
    }
    auto nweb = nwebweak_.lock();
    if (pointerEvent == nullptr) {
        TESTLOG_E("pointerEvent is invaild.");
        return;
    }
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        TESTLOG_E("get pointer item failed.");
        return;
    }
    int32_t action = pointerEvent->GetPointerAction();
    switch (action) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN: {
            TESTLOG_I("nweb receive pointer touch down event");
            nweb->OnTouchPress(pointerID, item.GetLocalX(), item.GetLocalY());
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_UP: {
            TESTLOG_I("nweb receive pointer touch up event");
            nweb->OnTouchRelease(pointerID);
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_MOVE: {
            TESTLOG_I("nweb receive pointer touch move event");
            nweb->OnTouchMove(pointerID, item.GetLocalX(), item.GetLocalY());
            break;
        }
        case MMI::PointerEvent::POINTER_ACTION_CANCEL: {
            TESTLOG_I("nweb receive pointer touch cancel event");
            nweb->OnTouchCancel();
            break;
        }
        default: {
            TESTLOG_W("unsupport PointerEvent action[%{public}d] received", action);
            break;
        }
    }
}

void NWebInputEventConsumerTest::DispatchKeyEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    if (nwebweak_.expired()) {
        TESTLOG_W("nweb instance has expired");
        return;
    }
    auto nweb = nwebweak_.lock();
    if (keyEvent == nullptr) {
        TESTLOG_E("keyEvent is invaild.");
        return;
    }
    int32_t keyCode = keyEvent->GetKeyCode();
    if (keyCode == MMI::KeyEvent::KEYCODE_BACK) {
        TESTLOG_I("nweb receive key back event");
        nweb->OnNavigateBack();
    } else {
        TESTLOG_W("unsupport KeyEvent code[%{public}d] received", keyCode);
    }
}
} // namespace OHOS::NWeb