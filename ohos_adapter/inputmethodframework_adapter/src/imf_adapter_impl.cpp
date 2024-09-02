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

#include "imf_adapter_impl.h"

#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "json/json.h"

namespace OHOS::NWeb {
constexpr char INPUT_METHOD[] = "INPUT_METHOD";
constexpr char ATTACH_CODE[] = "ATTACH_CODE";
constexpr char IS_SHOW_KEY_BOARD[] = "IS_SHOW_KEY_BOARD";
constexpr int32_t IMF_LISTENER_NULL_POINT = 1;
constexpr int32_t IMF_TEXT_CONFIG_NULL_POINT = 2;
const std::string AUTO_FILL_CANCEL_PRIVATE_COMMAND = "autofill.cancel";

InputMethod_TextConfig *IMFTextEditorProxyImpl::textConfig_ = nullptr;
std::shared_ptr<IMFTextListenerAdapterImpl> IMFTextEditorProxyImpl::textListener_ = nullptr;
InputMethod_TextInputType AdapterTextInputTypeToTextInputType(int32_t inputType)
{
    const static std::unordered_map<IMFAdapterTextInputType, InputMethod_TextInputType> INPUT_TYPE_MAP = {
        /* The text input type is NONE. */
        {IMFAdapterTextInputType::NONE,                 IME_TEXT_INPUT_TYPE_NONE},
        /* The text input type is TEXT. */
        {IMFAdapterTextInputType::TEXT,                 IME_TEXT_INPUT_TYPE_TEXT},
        /* The text input type is MULTILINE. */
        {IMFAdapterTextInputType::MULTILINE,            IME_TEXT_INPUT_TYPE_MULTILINE},
        /* The text input type is NUMBER. */
        {IMFAdapterTextInputType::NUMBER,               IME_TEXT_INPUT_TYPE_NUMBER},
        /* The text input type is PHONE. */
        {IMFAdapterTextInputType::PHONE,                IME_TEXT_INPUT_TYPE_PHONE},
        /* The text input type is DATETIME. */
        {IMFAdapterTextInputType::DATETIME,             IME_TEXT_INPUT_TYPE_DATETIME},
        /* The text input type is EMAIL ADDRESS. */
        {IMFAdapterTextInputType::EMAIL_ADDRESS,        IME_TEXT_INPUT_TYPE_EMAIL_ADDRESS},
        /* The text input type is URL. */
        {IMFAdapterTextInputType::URL,                  IME_TEXT_INPUT_TYPE_URL},
        /* The text input type is VISIBLE PASSWORD. */
        {IMFAdapterTextInputType::VISIBLE_PASSWORD,     IME_TEXT_INPUT_TYPE_VISIBLE_PASSWORD},
        /* The text input type is NUMBER PASSWORD. */
        {IMFAdapterTextInputType::NUMBER_PASSWORD,      IME_TEXT_INPUT_TYPE_NUMBER_PASSWORD},
        /* The text input type is SCREEN LOCK PASSWORD. */
        {IMFAdapterTextInputType::SCREEN_LOCK_PASSWORD, IME_TEXT_INPUT_TYPE_SCREEN_LOCK_PASSWORD},
        /* The text input type is USER NAME. */
        {IMFAdapterTextInputType::USER_NAME,            IME_TEXT_INPUT_TYPE_USER_NAME},
        /* The text input type is NEW PASSWORD. */
        {IMFAdapterTextInputType::NEW_PASSWORD,         IME_TEXT_INPUT_TYPE_NEW_PASSWORD},
        /* The text input type is NUMBER DECIMAL. */
        {IMFAdapterTextInputType::NUMBER_DECIMAL,       IME_TEXT_INPUT_TYPE_NUMBER_DECIMAL},
    };
    auto checkIter = INPUT_TYPE_MAP.find((IMFAdapterTextInputType)inputType);
    if (checkIter != INPUT_TYPE_MAP.end()) {
        return checkIter->second;
    }
    return IME_TEXT_INPUT_TYPE_NONE;
}

InputMethod_EnterKeyType AdapterEnterKeyTypeToOhEnterKeyType(int32_t enterKeyType)
{
    const static std::unordered_map<IMFAdapterEnterKeyType, InputMethod_EnterKeyType> ENTER_KEY_TYPE_MAP = {
        /* The enter key type is UNSPECIFIED. */
        {IMFAdapterEnterKeyType::UNSPECIFIED,   IME_ENTER_KEY_UNSPECIFIED},
        /* The enter key type is NONE. */
        {IMFAdapterEnterKeyType::NONE,          IME_ENTER_KEY_NONE},
        /* The enter key type is GO. */
        {IMFAdapterEnterKeyType::GO,            IME_ENTER_KEY_GO},
        /* The enter key type is SEARCH. */
        {IMFAdapterEnterKeyType::SEARCH,        IME_ENTER_KEY_SEARCH},
        /* The enter key type is SEND. */
        {IMFAdapterEnterKeyType::SEND,          IME_ENTER_KEY_SEND},
        /* The enter key type is NEXT. */
        {IMFAdapterEnterKeyType::NEXT,          IME_ENTER_KEY_NEXT},
        /* The enter key type is DONE. */
        {IMFAdapterEnterKeyType::DONE,          IME_ENTER_KEY_DONE},
        /* The enter key type is PREVIOUS. */
        {IMFAdapterEnterKeyType::PREVIOUS,      IME_ENTER_KEY_PREVIOUS},
        /* The enter key type is NEWLINE. */
        {IMFAdapterEnterKeyType::NEW_LINE,      IME_ENTER_KEY_NEWLINE},
    };
    auto checkIter = ENTER_KEY_TYPE_MAP.find((IMFAdapterEnterKeyType)enterKeyType);
    if (checkIter != ENTER_KEY_TYPE_MAP.end()) {
        return checkIter->second;
    }
    return IME_ENTER_KEY_UNSPECIFIED;
}

IMFAdapterKeyboardStatus ohKeyboardStatusToAdapterKeyboardStatus(InputMethod_KeyboardStatus keyboardStatus)
{
    const static std::unordered_map<InputMethod_KeyboardStatus, IMFAdapterKeyboardStatus> KEY_BOARD_STATUS_MAP = {
        {IME_KEYBOARD_STATUS_NONE,     IMFAdapterKeyboardStatus::NONE},
        {IME_KEYBOARD_STATUS_HIDE,     IMFAdapterKeyboardStatus::HIDE},
        {IME_KEYBOARD_STATUS_SHOW,     IMFAdapterKeyboardStatus::SHOW},
    };
    auto checkIter = KEY_BOARD_STATUS_MAP.find(keyboardStatus);
    if (checkIter != KEY_BOARD_STATUS_MAP.end()) {
        return checkIter->second;
    }
    return IMFAdapterKeyboardStatus::NONE;
}

const static std::unordered_map<InputMethod_Direction, IMFAdapterDirection> DIRECTION_MAP = {
    {IME_DIRECTION_NONE,     IMFAdapterDirection::NONE},
    {IME_DIRECTION_UP,       IMFAdapterDirection::UP},
    {IME_DIRECTION_DOWN,     IMFAdapterDirection::DOWN},
    {IME_DIRECTION_LEFT,     IMFAdapterDirection::LEFT},
    {IME_DIRECTION_RIGHT,    IMFAdapterDirection::RIGHT},
};

const static std::unordered_map<InputMethod_ExtendAction, IMFAdapterExtendAction> ACTION_MAP = {
    /* Select all text. */
    {IME_EXTEND_ACTION_SELECT_ALL,  IMFAdapterExtendAction::SELECT_ALL},
    /* Cut selected text. */
    {IME_EXTEND_ACTION_CUT,         IMFAdapterExtendAction::CUT},
    /* Copy selected text. */
    {IME_EXTEND_ACTION_COPY,        IMFAdapterExtendAction::COPY},
    /* Paste from paste board. */
    {IME_EXTEND_ACTION_PASTE,       IMFAdapterExtendAction::PASTE},
};

InputMethod_ErrorCode GetCursorInfo(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    double left, top, width, height;
    InputMethod_CursorInfo *destInfo, *srcInfo;

    InputMethod_ErrorCode ret = OH_TextConfig_GetCursorInfo(src, &srcInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get cursor info failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_CursorInfo_GetRect(srcInfo, &left, &top, &width, &height);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get rect failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_GetCursorInfo(dest, &destInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get cursor info failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_CursorInfo_SetRect(destInfo, left, top, width, height);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set rect failed ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetTextAvoidInfo(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    InputMethod_TextAvoidInfo *destAvoidInfo, *srcAvoidInfo;
    double positionY, height;

    InputMethod_ErrorCode ret = OH_TextConfig_GetTextAvoidInfo(src, &srcAvoidInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get avoid info failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextAvoidInfo_GetPositionY(srcAvoidInfo, &positionY);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get positionY failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextAvoidInfo_GetHeight(srcAvoidInfo, &height);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get height failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_GetTextAvoidInfo(dest, &destAvoidInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get avoid info failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextAvoidInfo_SetPositionY(destAvoidInfo, positionY);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set positionY failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextAvoidInfo_SetHeight(destAvoidInfo, height);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set height failed ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetInputType(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    InputMethod_TextInputType inputType;

    InputMethod_ErrorCode ret = OH_TextConfig_GetInputType(src, &inputType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get input type ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_SetInputType(dest, inputType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set input type ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetEnterKeyType(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    InputMethod_EnterKeyType enterKeyType;

    InputMethod_ErrorCode ret = OH_TextConfig_GetEnterKeyType(src, &enterKeyType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get enter key type ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_SetEnterKeyType(dest, enterKeyType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set enter key type ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetSelection(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    int32_t start, end;

    InputMethod_ErrorCode ret = OH_TextConfig_GetSelection(src, &start, &end);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get selection failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_SetSelection(dest, start, end);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set selection failed ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetWindowId(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    int32_t windowId;

    InputMethod_ErrorCode ret = OH_TextConfig_GetWindowId(src, &windowId);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get windowId failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_SetWindowId(dest, windowId);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set windowId ret failed %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode GetPreviewTextSupported(InputMethod_TextConfig *dest, InputMethod_TextConfig *src)
{
    bool supported;

    InputMethod_ErrorCode ret = OH_TextConfig_IsPreviewTextSupported(src, &supported);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Get preview supported failed ret %{public}d", ret);
        return ret;
    }
    ret = OH_TextConfig_SetPreviewTextSupport(dest, supported);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod adapter Set preview supported failed ret %{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

void IMFTextEditorProxyImpl::GetTextConfigFunc(InputMethod_TextEditorProxy *proxy, InputMethod_TextConfig *config)
{
    if (textConfig_) {
        GetCursorInfo(config, textConfig_);
        GetTextAvoidInfo(config, textConfig_);
        GetInputType(config, textConfig_);
        GetEnterKeyType(config, textConfig_);
        GetSelection(config, textConfig_);
        GetWindowId(config, textConfig_);
        GetPreviewTextSupported(config, textConfig_);
    }
}

void IMFTextEditorProxyImpl::InsertTextFunc(InputMethod_TextEditorProxy *proxy, const char16_t *text, size_t length)
{
    std::u16string textString = std::u16string(text);

    if (textListener_) {
        textListener_->InsertText(textString);
    }
}

void IMFTextEditorProxyImpl::DeleteForwardFunc(InputMethod_TextEditorProxy *proxy, int32_t length)
{
    if (textListener_) {
        textListener_->DeleteForward(length);
    }
}

void IMFTextEditorProxyImpl::DeleteBackwardFunc(InputMethod_TextEditorProxy *proxy, int32_t length)
{
    if (textListener_) {
        textListener_->DeleteBackward(length);
    }
}

void IMFTextEditorProxyImpl::SendKeyboardStatusFunc(
    InputMethod_TextEditorProxy *proxy, InputMethod_KeyboardStatus ohStatus)
{
    IMFAdapterKeyboardStatus status = ohKeyboardStatusToAdapterKeyboardStatus(ohStatus);
    if (textListener_) {
        textListener_->SendKeyboardStatus(status);
    }
}

void IMFTextEditorProxyImpl::SendEnterKeyFunc(InputMethod_TextEditorProxy *proxy, InputMethod_EnterKeyType type)
{
    if (textListener_) {
        std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> adapterFunction =
            std::make_shared<IMFAdapterFunctionKeyAdapterImpl>();
        switch (type) {
            case IME_ENTER_KEY_UNSPECIFIED:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::UNSPECIFIED);
                break;
            case IME_ENTER_KEY_NONE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NONE);
                break;
            case IME_ENTER_KEY_GO:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::GO);
                break;
            case IME_ENTER_KEY_SEARCH:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::SEARCH);
                break;
            case IME_ENTER_KEY_SEND:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::SEND);
                break;
            case IME_ENTER_KEY_NEXT:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NEXT);
                break;
            case IME_ENTER_KEY_DONE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::DONE);
                break;
            case IME_ENTER_KEY_PREVIOUS:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::PREVIOUS);
                break;
            case IME_ENTER_KEY_NEWLINE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NEW_LINE);
                break;
            default:
                WVLOG_E("unknown functionKey");
                break;
        }
        textListener_->SendFunctionKey(adapterFunction);
    }
}

void IMFTextEditorProxyImpl::MoveCursorFunc(InputMethod_TextEditorProxy *proxy, InputMethod_Direction ohDirection)
{
    IMFAdapterDirection direction = IMFAdapterDirection::NONE;

    if (textListener_) {
        auto checkIter = DIRECTION_MAP.find(ohDirection);
        if (checkIter != DIRECTION_MAP.end()) {
            direction = checkIter->second;
        }
        textListener_->MoveCursor(direction);
    }
}

void IMFTextEditorProxyImpl::HandleSetSelectionFunc(InputMethod_TextEditorProxy *proxy, int32_t start, int32_t end)
{
    if (textListener_) {
        textListener_->HandleSetSelection(start, end);
    }
}

void IMFTextEditorProxyImpl::HandleExtendActionFunc(
    InputMethod_TextEditorProxy *proxy, InputMethod_ExtendAction ohAction)
{
    int32_t action = 0;
    if (textListener_) {
        auto checkIter = ACTION_MAP.find(ohAction);
        if (checkIter != ACTION_MAP.end()) {
            action = (int32_t)checkIter->second;
        }
        textListener_->HandleExtendAction(action);
    }
}

void IMFTextEditorProxyImpl::GetLeftTextOfCursorFunc(
    InputMethod_TextEditorProxy *proxy, int32_t number, char16_t text[], size_t *length)
{
    std::u16string left;

    if (textListener_) {
        left = textListener_->GetLeftTextOfCursor(number);
        *length = left.copy(text, *length);
    }
}

void IMFTextEditorProxyImpl::GetRightTextOfCursorFunc(
    InputMethod_TextEditorProxy *proxy, int32_t number, char16_t text[], size_t *length)
{
    std::u16string right;

    if (textListener_) {
        right = textListener_->GetRightTextOfCursor(number);
        *length = right.copy(text, *length);
    }
}

int32_t IMFTextEditorProxyImpl::GetTextIndexAtCursorFunc(InputMethod_TextEditorProxy *proxy)
{
    if (textListener_) {
        return textListener_->GetTextIndexAtCursor();
    }
    return -1;
}

int32_t IMFTextEditorProxyImpl::ReceivePrivateCommandFunc(
    InputMethod_TextEditorProxy *proxy, InputMethod_PrivateCommand *privateCommand[], size_t size)
{
    if (textListener_) {
        return textListener_->ReceivePrivateCommand(privateCommand, size);
    }
    return 0;
}

int32_t IMFTextEditorProxyImpl::SetPreviewTextFunc(
    InputMethod_TextEditorProxy* proxy, const char16_t* text, size_t length, int32_t start, int32_t end)
{
    if (textListener_) {
        std::u16string textString(text, length);
        return textListener_->SetPreviewText(textString, start, end);
    }
    return 0;
}

void IMFTextEditorProxyImpl::FinishTextPreviewFunc(InputMethod_TextEditorProxy* proxy)
{
    if (textListener_) {
        return textListener_->FinishTextPreview();
    }
}

InputMethod_ErrorCode IMFTextEditorProxyImpl::TextEditorProxy_SetTextFunc(InputMethod_TextEditorProxy *textEditorProxy)
{
    InputMethod_ErrorCode ret = OH_TextEditorProxy_SetGetTextConfigFunc(textEditorProxy, GetTextConfigFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetGetLeftTextOfCursorFunc(textEditorProxy, GetLeftTextOfCursorFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetGetRightTextOfCursorFunc(textEditorProxy, GetRightTextOfCursorFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetGetTextIndexAtCursorFunc(textEditorProxy, GetTextIndexAtCursorFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetSetPreviewTextFunc(textEditorProxy, SetPreviewTextFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetFinishTextPreviewFunc(textEditorProxy, FinishTextPreviewFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetInsertTextFunc(textEditorProxy, InsertTextFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set insert text func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
    }
    return ret;
}


InputMethod_ErrorCode IMFTextEditorProxyImpl::TextEditorProxy_SetFunc(InputMethod_TextEditorProxy *textEditorProxy)
{
    InputMethod_ErrorCode ret = OH_TextEditorProxy_SetDeleteForwardFunc(textEditorProxy, DeleteForwardFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetDeleteBackwardFunc(textEditorProxy, DeleteBackwardFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetSendKeyboardStatusFunc(textEditorProxy, SendKeyboardStatusFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetSendEnterKeyFunc(textEditorProxy, SendEnterKeyFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetMoveCursorFunc(textEditorProxy, MoveCursorFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetHandleSetSelectionFunc(textEditorProxy, HandleSetSelectionFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetHandleExtendActionFunc(textEditorProxy, HandleExtendActionFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
        return ret;
    }
    ret = OH_TextEditorProxy_SetReceivePrivateCommandFunc(textEditorProxy, ReceivePrivateCommandFunc);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set text editor proxy func failed ret=%{public}d", ret);
        OH_TextEditorProxy_Destroy(textEditorProxy);
    }
    return ret;
}

InputMethod_TextEditorProxy* IMFTextEditorProxyImpl::TextEditorProxyCreate(
    std::shared_ptr<IMFTextListenerAdapter> listener, const std::shared_ptr<IMFTextConfigAdapter> config)
{
    InputMethod_ErrorCode ret;
    InputMethod_TextEditorProxy *textEditorProxy = OH_TextEditorProxy_Create();
    textListener_ = std::make_shared<IMFTextListenerAdapterImpl>(listener);

    if (config) {
        ret = ConstructTextConfig(config);
        if (ret != IME_ERR_OK) {
            return nullptr;
        }
        ret = TextEditorProxy_SetTextFunc(textEditorProxy);
        if (ret != IME_ERR_OK) {
            return nullptr;
        }
        ret = TextEditorProxy_SetFunc(textEditorProxy);
        if (ret != IME_ERR_OK) {
            return nullptr;
        }
    }
    return textEditorProxy;
}

void IMFTextEditorProxyImpl::TextEditorProxyDestroy(InputMethod_TextEditorProxy *textEditorProxy)
{
    OH_TextEditorProxy_Destroy(textEditorProxy);
    DestroyTextConfig();
    textListener_ = nullptr;
}

InputMethod_ErrorCode ConstructCursorInfo(InputMethod_TextConfig *textConfig,
    const std::shared_ptr<IMFTextConfigAdapter> config)
{
    InputMethod_CursorInfo *cursorInfo = nullptr;
    InputMethod_ErrorCode ret = OH_TextConfig_GetCursorInfo(textConfig, &cursorInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Get cursor info failed ret=%{public}d", ret);
        return ret;
    }
    ret = OH_CursorInfo_SetRect(cursorInfo, config->GetCursorInfo()->GetLeft(), config->GetCursorInfo()->GetTop(),
        config->GetCursorInfo()->GetWidth(), config->GetCursorInfo()->GetHeight());
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set cursor info failed ret=%{public}d", ret);
        return ret;
    }
    return IME_ERR_OK;
}

InputMethod_ErrorCode IMFTextEditorProxyImpl::ConstructTextConfig(const std::shared_ptr<IMFTextConfigAdapter> config)
{
    if (textConfig_) {
        DestroyTextConfig();
    }
    textConfig_ = OH_TextConfig_Create();
    if (!textConfig_) {
        return IME_ERR_NULL_POINTER;
    }
    InputMethod_TextInputType inputType =
        AdapterTextInputTypeToTextInputType(config->GetInputAttribute()->GetInputPattern());
    InputMethod_EnterKeyType enterKeyType =
        AdapterEnterKeyTypeToOhEnterKeyType(config->GetInputAttribute()->GetEnterKeyType());
    InputMethod_ErrorCode ret = OH_TextConfig_SetInputType(textConfig_, inputType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set input type failed ret=%{public}d", ret);
        DestroyTextConfig();
        return ret;
    }   
    ret = OH_TextConfig_SetEnterKeyType(textConfig_, enterKeyType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set enter key type failed ret=%{public}d", ret);
        DestroyTextConfig();
        OH_TextConfig_Destroy(textConfig_);
        return ret;
    }
    bool support = true;
    ret = OH_TextConfig_SetPreviewTextSupport(textConfig_, support);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Set preview support failed ret=%{public}d", ret);
        DestroyTextConfig();
        return ret;
    }
    ret = OH_TextConfig_SetWindowId(textConfig_, config->GetWindowId());
    if (ret != IME_ERR_OK) {
        DestroyTextConfig();
        WVLOG_E("Set windowId failed ret=%{public}d", ret);
        return ret;
    }
    ret = ConstructCursorInfo(textConfig_, config);
    if (ret != IME_ERR_OK) {
        DestroyTextConfig();
        return ret;
    }
    return IME_ERR_OK;
}

void IMFTextEditorProxyImpl::DestroyTextConfig(void)
{
    OH_TextConfig_Destroy(textConfig_);
    textConfig_ = nullptr;
}

IMFTextListenerAdapterImpl::IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter>& listener)
    : listener_(listener) {}

IMFTextListenerAdapterImpl::~IMFTextListenerAdapterImpl()
{
    listener_ = nullptr;
}

void IMFTextListenerAdapterImpl::InsertText(const std::u16string& text)
{
    if (listener_) {
        listener_->InsertText(text);
    }
}

void IMFTextListenerAdapterImpl::DeleteForward(int32_t length)
{
    if (listener_) {
        listener_->DeleteForward(length);
    }
}

void IMFTextListenerAdapterImpl::DeleteBackward(int32_t length)
{
    if (listener_) {
        listener_->DeleteBackward(length);
    }
}

void IMFTextListenerAdapterImpl::SendKeyEventFromInputMethod(void)
{
    if (listener_) {
        listener_->SendKeyEventFromInputMethod();
    }
}

void IMFTextListenerAdapterImpl::SendKeyboardStatus(const IMFAdapterKeyboardStatus& status)
{
    if (listener_) {
        listener_->SendKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::SendFunctionKey(std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> functionKey)
{
    if (listener_) {
        listener_->SendFunctionKey(functionKey);
    }
}

void IMFTextListenerAdapterImpl::SetKeyboardStatus(bool status)
{
    if (listener_) {
        listener_->SetKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::MoveCursor(const IMFAdapterDirection direction)
{
    if (listener_) {
        listener_->MoveCursor(direction);
    }
}

void IMFTextListenerAdapterImpl::HandleSetSelection(int32_t start, int32_t end)
{
    if (listener_) {
        listener_->HandleSetSelection(start, end);
    }
}

void IMFTextListenerAdapterImpl::HandleExtendAction(int32_t action)
{
    if (listener_) {
        listener_->HandleExtendAction(action);
    }
}

void IMFTextListenerAdapterImpl::HandleSelect(int32_t keyCode, int32_t cursorMoveSkip)
{
    if (listener_) {
        listener_->HandleSelect(keyCode, cursorMoveSkip);
    }
}

int32_t IMFTextListenerAdapterImpl::GetTextIndexAtCursor()
{
    if (listener_) {
        return listener_->GetTextIndexAtCursor();
    }
    return -1;
}

std::u16string IMFTextListenerAdapterImpl::GetLeftTextOfCursor(int32_t number)
{
    if (listener_) {
        return listener_->GetLeftTextOfCursor(number);
    }
    return u"";
}

std::u16string IMFTextListenerAdapterImpl::GetRightTextOfCursor(int32_t number)
{
    if (listener_) {
        return listener_->GetRightTextOfCursor(number);
    }
    return u"";
}

int32_t IMFTextListenerAdapterImpl::SetPreviewText(const std::u16string& text, int32_t start, int32_t end)
{
    if (listener_) {
        return listener_->SetPreviewText(text, start, end);
    }
    return -1;
}

void IMFTextListenerAdapterImpl::FinishTextPreview()
{
    if (listener_) {
        listener_->FinishTextPreview();
    }
}

std::shared_ptr<std::string> PrivateCommandGetStrValue(InputMethod_PrivateCommand *privateCommand)
{
    const char *value = nullptr;
    size_t length = 0;

    InputMethod_ErrorCode ret = OH_PrivateCommand_GetStrValue(privateCommand, &value, &length);
    if (ret != IME_ERR_OK) {
        return nullptr;
    }
    return std::make_shared<std::string>(value);
}

int32_t IMFTextListenerAdapterImpl::ReceivePrivateCommand(
    InputMethod_PrivateCommand *privateCommand[], size_t num)
{
    WVLOG_I("ReceivePrivateCommand");
    for (size_t i = 0; i < num; i++) {
        const char *key = nullptr;
        size_t length = 0;
        InputMethod_ErrorCode ret = OH_PrivateCommand_GetKey(privateCommand[i], &key, &length);
        if (ret != IME_ERR_OK) {
            return 0;
        }
        std::string keyString(key, length);
        if (keyString == PREVIEW_TEXT_STYLE_KEY) {
            std::shared_ptr<std::string> previewStyle = PrivateCommandGetStrValue(privateCommand[i]);

            bool isNeedUnderline = false;
            if (*previewStyle == PREVIEW_TEXT_STYLE_UNDERLINE) {
                isNeedUnderline = true;
            }
            if (listener_) {
                listener_->SetNeedUnderLine(isNeedUnderline);
            }
        } else if (keyString == AUTO_FILL_PARAMS_USERNAME) {
            std::shared_ptr<std::string> content = PrivateCommandGetStrValue(privateCommand[i]);
            if (listener_) {
                listener_->AutoFillWithIMFEvent(true, false, false, *content);
            }
        } else if (keyString == AUTO_FILL_PARAMS_OTHERACCOUNT) {
            std::string content("");
            if (listener_) {
                listener_->AutoFillWithIMFEvent(true, false, false, content);
            }
        }
    }
    return 0;
}

bool IMFAdapterImpl::Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard)
{
    if (!listener) {
        WVLOG_E("the listener is nullptr");
        return false;
    }
    if (!IMFTextEditorProxyImpl::textListener_) {
        textEditorProxy_ = IMFTextEditorProxyImpl::TextEditorProxyCreate(listener, nullptr);
        if (!textEditorProxy_) {
            WVLOG_E("new textListener failed");
            return false;
        }
    }
    InputMethod_AttachOptions *options = OH_AttachOptions_Create(isShowKeyboard);
    InputMethod_ErrorCode ret = OH_InputMethodController_Attach(textEditorProxy_, options, &inputMethodProxy_);
    if (ret != IME_ERR_OK) {
        WVLOG_E("inputmethod attach failed, errcode=%{public}d", ret);
        return false;
    }
    return true;
}

void ReportImfErrorEvent(int32_t ret, bool isShowKeyboard)
{
    std::string isShowKeyboardStr = isShowKeyboard ? "true" : "false";
    OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(INPUT_METHOD,
        HiSysEventAdapter::EventType::FAULT,
        { ATTACH_CODE, std::to_string(ret), IS_SHOW_KEY_BOARD, isShowKeyboardStr });
}

bool IMFAdapterImpl::Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
    const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener)
{
    if (!listener) {
        WVLOG_E("the listener is nullptr");
        ReportImfErrorEvent(IMF_LISTENER_NULL_POINT, isShowKeyboard);
        return false;
    }
    if (!config || !(config->GetInputAttribute()) || !(config->GetCursorInfo())) {
        WVLOG_E("the config is nullptr");
        ReportImfErrorEvent(IMF_TEXT_CONFIG_NULL_POINT, isShowKeyboard);
        return false;
    }

    if (textEditorProxy_) {
        IMFTextEditorProxyImpl::TextEditorProxyDestroy(textEditorProxy_);
    }
    textEditorProxy_ = IMFTextEditorProxyImpl::TextEditorProxyCreate(listener, config);
    if (!textEditorProxy_) {
        WVLOG_E("new textListener failed");
        ReportImfErrorEvent(IMF_LISTENER_NULL_POINT, isShowKeyboard);
        return false;
    }
    InputMethod_AttachOptions *options = OH_AttachOptions_Create(isShowKeyboard);
    InputMethod_ErrorCode ret = OH_InputMethodController_Attach(textEditorProxy_, options, &inputMethodProxy_);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod attach failed, ret=%{public}d", isShowKeyboard);
    }
    return true;
}

void IMFAdapterImpl::ShowCurrentInput(const IMFAdapterTextInputType& inputType)
{
    InputMethod_ErrorCode ret;

    if (!IMFTextEditorProxyImpl::textConfig_) {
        return ;
    }
    if (inputType == IMFAdapterTextInputType::NUMBER) {
        ret = OH_TextConfig_SetInputType(IMFTextEditorProxyImpl::textConfig_, IME_TEXT_INPUT_TYPE_NUMBER);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Set input type failed ret=%{public}d", ret);
            return;
        }
    } else {
        ret = OH_TextConfig_SetInputType(IMFTextEditorProxyImpl::textConfig_, IME_TEXT_INPUT_TYPE_TEXT);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Set input type failed ret=%{public}d", ret);
            return;
        }
    }
    InputMethod_EnterKeyType enterKeyType;
    InputMethod_TextInputType textInputType;
    ret = OH_TextConfig_GetEnterKeyType(IMFTextEditorProxyImpl::textConfig_, &enterKeyType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Get enter key type failed ret=%{public}d", ret);
        return;
    }
    ret = OH_TextConfig_GetInputType(IMFTextEditorProxyImpl::textConfig_, &textInputType);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Get input type failed ret=%{public}d", ret);
        return;
    }
    if (inputMethodProxy_) {
        ret = OH_InputMethodProxy_NotifyConfigurationChange(inputMethodProxy_,
            enterKeyType, textInputType);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Notify config change failed ret=%{public}d", ret);
            return;
        }
        ret = OH_InputMethodProxy_ShowKeyboard(inputMethodProxy_);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Show key board failed ret=%{public}d", ret);
            return;
        }
    }
}

void IMFAdapterImpl::HideTextInput()
{
    if (inputMethodProxy_) {
        InputMethod_ErrorCode ret = OH_InputMethodProxy_HideKeyboard(inputMethodProxy_);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Hide key board failed ret=%{public}d", ret);
        }
    }
}

void IMFAdapterImpl::Close()
{
    if (inputMethodProxy_) {
        InputMethod_ErrorCode ret = OH_InputMethodController_Detach(inputMethodProxy_);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Inputmethod controller detach failed ret=%{public}d", ret);
        }
    }
}

void IMFAdapterImpl::OnCursorUpdate(const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo)
{
    if (!cursorInfo) {
        WVLOG_E("inputmethod OnCursorUpdate cursorInfo is null");
        return;
    }

    InputMethod_CursorInfo *ohCursorInfo = nullptr;
    if (!IMFTextEditorProxyImpl::textConfig_) {
        WVLOG_W("Inputmethod text config is null");
        return;
    }
    InputMethod_ErrorCode ret = OH_TextConfig_GetCursorInfo(IMFTextEditorProxyImpl::textConfig_, &ohCursorInfo);
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod get cursor info failed ret=%{public}d", ret);
        return;
    }

    ret = OH_CursorInfo_SetRect(ohCursorInfo, cursorInfo->GetLeft(), cursorInfo->GetTop(), cursorInfo->GetWidth(),
        cursorInfo->GetHeight());
    if (ret != IME_ERR_OK) {
        WVLOG_E("Inputmethod set cursor info failed ret=%{public}d", ret);
        return;
    }
    if (!inputMethodProxy_) {
        ret = OH_InputMethodProxy_NotifyCursorUpdate(inputMethodProxy_, ohCursorInfo);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Inputmethod notify cursor update failed ret=%{public}d", ret);
            return;
        }
    }
    WVLOG_D("imfInfo left = %{public}f, top = %{public}f, width = %{public}f, height = %{public}f",
        cursorInfo->GetLeft(), cursorInfo->GetTop(), cursorInfo->GetWidth(), cursorInfo->GetHeight());
}

void IMFAdapterImpl::OnSelectionChange(std::u16string text, int start, int end)
{
    if (inputMethodProxy_) {
        InputMethod_ErrorCode ret = OH_InputMethodProxy_NotifySelectionChange(
            inputMethodProxy_, (char16_t*)text.c_str(), text.length(), start, end);
        if (ret != IME_ERR_OK) {
            WVLOG_E("Inputmethod notify selection change failed ret=%{public}d", ret);
        }
    }
}

void DestroyPrivateCommand(std::vector<InputMethod_PrivateCommand *>& privateCommands)
{
    for (auto it = privateCommands.begin(); it != privateCommands.end(); it++) {
        OH_PrivateCommand_Destroy(*it);
    }
}

bool IMFAdapterImpl::SendPrivateCommand(const std::string& commandKey, const std::string& commandValue)
{
    if (commandKey == AUTO_FILL_CANCEL_PRIVATE_COMMAND) {
        std::vector<InputMethod_PrivateCommand *> privateCommands;
        ParseFillContentJsonValue(commandValue, privateCommands);
        InputMethod_ErrorCode ret = OH_InputMethodProxy_SendPrivateCommand(
            inputMethodProxy_, privateCommands.data(), privateCommands.size());
        DestroyPrivateCommand(privateCommands);
        if (ret != IME_ERR_OK) {
            WVLOG_E("inputmethod SendPrivateCommand failed, errcode=%{public}d", ret);
            return false;
        }
        WVLOG_I("inputmethod  SendPrivateCommand success");
        return true;
    }
    return false;
}

bool IMFAdapterImpl::ParseFillContentJsonValue(const std::string& commandValue,
    std::vector<InputMethod_PrivateCommand *> privateCommands)
{
    Json::Reader reader;
    Json::Value root;

    if (reader.parse(commandValue, root)) {
        WVLOG_E("Inputmethod parse json failed");
        return false;
    }
    if (root.isMember("userName")) {
        std::string key("userName");
        std::string userName = root[key].asString();
        InputMethod_PrivateCommand* command = OH_PrivateCommand_Create((char*)key.c_str(), key.length());
        InputMethod_ErrorCode ret =
            OH_PrivateCommand_SetStrValue(command, (char *)userName.c_str(), userName.length());
        if (ret != IME_ERR_OK) {
            WVLOG_E("Set private command string failed, ret=%{public}d", ret);
            return false;
        }
        privateCommands.push_back(command);
    }
    if (root.isMember("newPassword")) {
        std::string key("newPassword");
        std::string newPassword = root[key].asString();
        InputMethod_PrivateCommand* command = OH_PrivateCommand_Create((char*)key.c_str(), key.length());
        InputMethod_ErrorCode ret =
            OH_PrivateCommand_SetStrValue(command, (char *)newPassword.c_str(), newPassword.length());
        if (ret != IME_ERR_OK) {
            WVLOG_E("Set private command string failed, ret=%{public}d", ret);
            return false;
        }
        privateCommands.push_back(command);
    }
    if (root.isMember("hasAccount")) {
        std::string key("hasAccount");
        std::string hasAccount = root[key].asString();
        InputMethod_PrivateCommand* command = OH_PrivateCommand_Create((char*)key.c_str(), key.length());
        InputMethod_ErrorCode ret =
            OH_PrivateCommand_SetStrValue(command, (char *)hasAccount.c_str(), hasAccount.length());
        if (ret != IME_ERR_OK) {
            WVLOG_E("Set private command string failed, ret=%{public}d", ret);
            return false;
        }
        privateCommands.push_back(command);
    }
    return true;
}
} // namespace OHOS::NWeb
