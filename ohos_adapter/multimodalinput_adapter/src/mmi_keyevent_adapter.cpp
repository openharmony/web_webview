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

#include "mmi_keyevent_adapter.h"

#include "key_event.h"

namespace OHOS::NWeb {
namespace MMIAdapter {
const int32_t KeyEvent::KEYCODE_0 = OHOS::MMI::KeyEvent::KEYCODE_0;
const int32_t KeyEvent::KEYCODE_1 = OHOS::MMI::KeyEvent::KEYCODE_1;
const int32_t KeyEvent::KEYCODE_2 = OHOS::MMI::KeyEvent::KEYCODE_2;
const int32_t KeyEvent::KEYCODE_3 = OHOS::MMI::KeyEvent::KEYCODE_3;
const int32_t KeyEvent::KEYCODE_4 = OHOS::MMI::KeyEvent::KEYCODE_4;
const int32_t KeyEvent::KEYCODE_5 = OHOS::MMI::KeyEvent::KEYCODE_5;
const int32_t KeyEvent::KEYCODE_6 = OHOS::MMI::KeyEvent::KEYCODE_6;
const int32_t KeyEvent::KEYCODE_7 = OHOS::MMI::KeyEvent::KEYCODE_7;
const int32_t KeyEvent::KEYCODE_8 = OHOS::MMI::KeyEvent::KEYCODE_8;
const int32_t KeyEvent::KEYCODE_9 = OHOS::MMI::KeyEvent::KEYCODE_9;
const int32_t KeyEvent::KEYCODE_DPAD_UP = OHOS::MMI::KeyEvent::KEYCODE_DPAD_UP;
const int32_t KeyEvent::KEYCODE_DPAD_DOWN = OHOS::MMI::KeyEvent::KEYCODE_DPAD_DOWN;
const int32_t KeyEvent::KEYCODE_DPAD_LEFT = OHOS::MMI::KeyEvent::KEYCODE_DPAD_LEFT;
const int32_t KeyEvent::KEYCODE_DPAD_RIGHT = OHOS::MMI::KeyEvent::KEYCODE_DPAD_RIGHT;
const int32_t KeyEvent::KEYCODE_A = OHOS::MMI::KeyEvent::KEYCODE_A;
const int32_t KeyEvent::KEYCODE_B = OHOS::MMI::KeyEvent::KEYCODE_B;
const int32_t KeyEvent::KEYCODE_C = OHOS::MMI::KeyEvent::KEYCODE_C;
const int32_t KeyEvent::KEYCODE_D = OHOS::MMI::KeyEvent::KEYCODE_D;
const int32_t KeyEvent::KEYCODE_E = OHOS::MMI::KeyEvent::KEYCODE_E;
const int32_t KeyEvent::KEYCODE_F = OHOS::MMI::KeyEvent::KEYCODE_F;
const int32_t KeyEvent::KEYCODE_G = OHOS::MMI::KeyEvent::KEYCODE_G;
const int32_t KeyEvent::KEYCODE_H = OHOS::MMI::KeyEvent::KEYCODE_H;
const int32_t KeyEvent::KEYCODE_I = OHOS::MMI::KeyEvent::KEYCODE_I;
const int32_t KeyEvent::KEYCODE_J = OHOS::MMI::KeyEvent::KEYCODE_J;
const int32_t KeyEvent::KEYCODE_K = OHOS::MMI::KeyEvent::KEYCODE_K;
const int32_t KeyEvent::KEYCODE_L = OHOS::MMI::KeyEvent::KEYCODE_L;
const int32_t KeyEvent::KEYCODE_M = OHOS::MMI::KeyEvent::KEYCODE_M;
const int32_t KeyEvent::KEYCODE_N = OHOS::MMI::KeyEvent::KEYCODE_N;
const int32_t KeyEvent::KEYCODE_O = OHOS::MMI::KeyEvent::KEYCODE_O;
const int32_t KeyEvent::KEYCODE_P = OHOS::MMI::KeyEvent::KEYCODE_P;
const int32_t KeyEvent::KEYCODE_Q = OHOS::MMI::KeyEvent::KEYCODE_Q;
const int32_t KeyEvent::KEYCODE_R = OHOS::MMI::KeyEvent::KEYCODE_R;
const int32_t KeyEvent::KEYCODE_S = OHOS::MMI::KeyEvent::KEYCODE_S;
const int32_t KeyEvent::KEYCODE_T = OHOS::MMI::KeyEvent::KEYCODE_T;
const int32_t KeyEvent::KEYCODE_U = OHOS::MMI::KeyEvent::KEYCODE_U;
const int32_t KeyEvent::KEYCODE_V = OHOS::MMI::KeyEvent::KEYCODE_V;
const int32_t KeyEvent::KEYCODE_W = OHOS::MMI::KeyEvent::KEYCODE_W;
const int32_t KeyEvent::KEYCODE_X = OHOS::MMI::KeyEvent::KEYCODE_X;
const int32_t KeyEvent::KEYCODE_Y = OHOS::MMI::KeyEvent::KEYCODE_Y;
const int32_t KeyEvent::KEYCODE_Z = OHOS::MMI::KeyEvent::KEYCODE_Z;
const int32_t KeyEvent::KEYCODE_COMMA = OHOS::MMI::KeyEvent::KEYCODE_COMMA;
const int32_t KeyEvent::KEYCODE_PERIOD = OHOS::MMI::KeyEvent::KEYCODE_PERIOD;
const int32_t KeyEvent::KEYCODE_ALT_LEFT = OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT;
const int32_t KeyEvent::KEYCODE_ALT_RIGHT = OHOS::MMI::KeyEvent::KEYCODE_ALT_RIGHT;
const int32_t KeyEvent::KEYCODE_SHIFT_LEFT = OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT;
const int32_t KeyEvent::KEYCODE_SHIFT_RIGHT = OHOS::MMI::KeyEvent::KEYCODE_SHIFT_RIGHT;
const int32_t KeyEvent::KEYCODE_TAB = OHOS::MMI::KeyEvent::KEYCODE_TAB;
const int32_t KeyEvent::KEYCODE_SPACE = OHOS::MMI::KeyEvent::KEYCODE_SPACE;
const int32_t KeyEvent::KEYCODE_ENTER = OHOS::MMI::KeyEvent::KEYCODE_ENTER;
const int32_t KeyEvent::KEYCODE_DEL = OHOS::MMI::KeyEvent::KEYCODE_DEL;
const int32_t KeyEvent::KEYCODE_GRAVE = OHOS::MMI::KeyEvent::KEYCODE_GRAVE;
const int32_t KeyEvent::KEYCODE_MINUS = OHOS::MMI::KeyEvent::KEYCODE_MINUS;
const int32_t KeyEvent::KEYCODE_EQUALS = OHOS::MMI::KeyEvent::KEYCODE_EQUALS;
const int32_t KeyEvent::KEYCODE_LEFT_BRACKET = OHOS::MMI::KeyEvent::KEYCODE_LEFT_BRACKET;
const int32_t KeyEvent::KEYCODE_RIGHT_BRACKET = OHOS::MMI::KeyEvent::KEYCODE_RIGHT_BRACKET;
const int32_t KeyEvent::KEYCODE_BACKSLASH = OHOS::MMI::KeyEvent::KEYCODE_BACKSLASH;
const int32_t KeyEvent::KEYCODE_SEMICOLON = OHOS::MMI::KeyEvent::KEYCODE_SEMICOLON;
const int32_t KeyEvent::KEYCODE_APOSTROPHE = OHOS::MMI::KeyEvent::KEYCODE_APOSTROPHE;
const int32_t KeyEvent::KEYCODE_SLASH = OHOS::MMI::KeyEvent::KEYCODE_SLASH;
const int32_t KeyEvent::KEYCODE_PAGE_UP = OHOS::MMI::KeyEvent::KEYCODE_PAGE_UP;
const int32_t KeyEvent::KEYCODE_PAGE_DOWN = OHOS::MMI::KeyEvent::KEYCODE_PAGE_DOWN;
const int32_t KeyEvent::KEYCODE_ESCAPE = OHOS::MMI::KeyEvent::KEYCODE_ESCAPE;
const int32_t KeyEvent::KEYCODE_FORWARD_DEL = OHOS::MMI::KeyEvent::KEYCODE_FORWARD_DEL;
const int32_t KeyEvent::KEYCODE_CTRL_LEFT = OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT;
const int32_t KeyEvent::KEYCODE_CTRL_RIGHT = OHOS::MMI::KeyEvent::KEYCODE_CTRL_RIGHT;
const int32_t KeyEvent::KEYCODE_CAPS_LOCK = OHOS::MMI::KeyEvent::KEYCODE_CAPS_LOCK;
const int32_t KeyEvent::KEYCODE_SCROLL_LOCK = OHOS::MMI::KeyEvent::KEYCODE_SCROLL_LOCK;
const int32_t KeyEvent::KEYCODE_META_LEFT = OHOS::MMI::KeyEvent::KEYCODE_META_LEFT;
const int32_t KeyEvent::KEYCODE_META_RIGHT = OHOS::MMI::KeyEvent::KEYCODE_META_RIGHT;
const int32_t KeyEvent::KEYCODE_SYSRQ = OHOS::MMI::KeyEvent::KEYCODE_SYSRQ;
const int32_t KeyEvent::KEYCODE_BREAK = OHOS::MMI::KeyEvent::KEYCODE_BREAK;
const int32_t KeyEvent::KEYCODE_MOVE_HOME = OHOS::MMI::KeyEvent::KEYCODE_MOVE_HOME;
const int32_t KeyEvent::KEYCODE_MOVE_END = OHOS::MMI::KeyEvent::KEYCODE_MOVE_END;
const int32_t KeyEvent::KEYCODE_INSERT = OHOS::MMI::KeyEvent::KEYCODE_INSERT;
const int32_t KeyEvent::KEYCODE_F1 = OHOS::MMI::KeyEvent::KEYCODE_F1;
const int32_t KeyEvent::KEYCODE_F2 = OHOS::MMI::KeyEvent::KEYCODE_F2;
const int32_t KeyEvent::KEYCODE_F3 = OHOS::MMI::KeyEvent::KEYCODE_F3;
const int32_t KeyEvent::KEYCODE_F4 = OHOS::MMI::KeyEvent::KEYCODE_F4;
const int32_t KeyEvent::KEYCODE_F5 = OHOS::MMI::KeyEvent::KEYCODE_F5;
const int32_t KeyEvent::KEYCODE_F6 = OHOS::MMI::KeyEvent::KEYCODE_F6;
const int32_t KeyEvent::KEYCODE_F7 = OHOS::MMI::KeyEvent::KEYCODE_F7;
const int32_t KeyEvent::KEYCODE_F8 = OHOS::MMI::KeyEvent::KEYCODE_F8;
const int32_t KeyEvent::KEYCODE_F9 = OHOS::MMI::KeyEvent::KEYCODE_F9;
const int32_t KeyEvent::KEYCODE_F10 = OHOS::MMI::KeyEvent::KEYCODE_F10;
const int32_t KeyEvent::KEYCODE_F11 = OHOS::MMI::KeyEvent::KEYCODE_F11;
const int32_t KeyEvent::KEYCODE_F12 = OHOS::MMI::KeyEvent::KEYCODE_F12;
const int32_t KeyEvent::KEYCODE_NUM_LOCK = OHOS::MMI::KeyEvent::KEYCODE_NUM_LOCK;
const int32_t KeyEvent::KEYCODE_NUMPAD_0 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_0;
const int32_t KeyEvent::KEYCODE_NUMPAD_1 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_1;
const int32_t KeyEvent::KEYCODE_NUMPAD_2 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_2;
const int32_t KeyEvent::KEYCODE_NUMPAD_3 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_3;
const int32_t KeyEvent::KEYCODE_NUMPAD_4 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_4;
const int32_t KeyEvent::KEYCODE_NUMPAD_5 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_5;
const int32_t KeyEvent::KEYCODE_NUMPAD_6 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_6;
const int32_t KeyEvent::KEYCODE_NUMPAD_7 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_7;
const int32_t KeyEvent::KEYCODE_NUMPAD_8 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_8;
const int32_t KeyEvent::KEYCODE_NUMPAD_9 = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_9;
const int32_t KeyEvent::KEYCODE_NUMPAD_DIVIDE = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE;
const int32_t KeyEvent::KEYCODE_NUMPAD_MULTIPLY = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY;
const int32_t KeyEvent::KEYCODE_NUMPAD_SUBTRACT = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT;
const int32_t KeyEvent::KEYCODE_NUMPAD_ADD = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_ADD;
const int32_t KeyEvent::KEYCODE_NUMPAD_DOT = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_DOT;
const int32_t KeyEvent::KEYCODE_NUMPAD_ENTER = OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_ENTER;
const int32_t KeyEvent::KEYCODE_COPY = OHOS::MMI::KeyEvent::KEYCODE_COPY;
const int32_t KeyEvent::KEYCODE_PASTE = OHOS::MMI::KeyEvent::KEYCODE_PASTE;
const int32_t KeyEvent::KEYCODE_CUT = OHOS::MMI::KeyEvent::KEYCODE_CUT;
} // namespace MMIAdapter
} // namespace OHOS::NWeb
