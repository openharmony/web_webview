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

#include "hisysevent_adapter.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "cJSON.h"

namespace OHOS::NWeb {
constexpr char INPUT_METHOD[] = "INPUT_METHOD";
constexpr char ATTACH_CODE[] = "ATTACH_CODE";
constexpr char IS_SHOW_KEY_BOARD[] = "IS_SHOW_KEY_BOARD";
constexpr int32_t IMF_LISTENER_NULL_POINT = 1;
constexpr int32_t IMF_TEXT_CONFIG_NULL_POINT = 2;
const std::string AUTO_FILL_CANCEL_PRIVATE_COMMAND = "autofill.cancel";

IMFTextListenerAdapterImpl::IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter>& listener)
    : listener_(listener) {};

IMFTextListenerAdapterImpl::~IMFTextListenerAdapterImpl() = default;

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

void IMFTextListenerAdapterImpl::SendKeyEventFromInputMethod(const MiscServices::KeyEvent& event)
{
    (void)event;
    if (listener_) {
        listener_->SendKeyEventFromInputMethod();
    }
}

void IMFTextListenerAdapterImpl::SendKeyboardStatus(const MiscServices::KeyboardStatus& keyboardStatus)
{
    if (listener_) {
        auto status = IMFAdapterKeyboardStatus::NONE;
        if (keyboardStatus == MiscServices::KeyboardStatus::SHOW) {
            status = IMFAdapterKeyboardStatus::SHOW;
        } else if (keyboardStatus == MiscServices::KeyboardStatus::HIDE) {
            status = IMFAdapterKeyboardStatus::HIDE;
        }
        listener_->SendKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::SendFunctionKey(const MiscServices::FunctionKey& functionKey)
{
    if (listener_) {
        std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> adapterFunction =
            std::make_shared<IMFAdapterFunctionKeyAdapterImpl>();
        switch (functionKey.GetEnterKeyType()) {
            case MiscServices::EnterKeyType::UNSPECIFIED:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::UNSPECIFIED);
                break;
            case MiscServices::EnterKeyType::NONE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NONE);
                break;
            case MiscServices::EnterKeyType::GO:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::GO);
                break;
            case MiscServices::EnterKeyType::SEARCH:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::SEARCH);
                break;
            case MiscServices::EnterKeyType::SEND:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::SEND);
                break;
            case MiscServices::EnterKeyType::NEXT:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NEXT);
                break;
            case MiscServices::EnterKeyType::DONE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::DONE);
                break;
            case MiscServices::EnterKeyType::PREVIOUS:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::PREVIOUS);
                break;
            case MiscServices::EnterKeyType::NEW_LINE:
                adapterFunction->SetEnterKeyType(IMFAdapterEnterKeyType::NEW_LINE);
                break;
            default:
                WVLOG_E("unknown functionKey");
                break;
        }
        listener_->SendFunctionKey(adapterFunction);
    }
}

void IMFTextListenerAdapterImpl::SetKeyboardStatus(bool status)
{
    if (listener_) {
        listener_->SetKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::MoveCursor(const MiscServices::Direction direction)
{
    if (listener_) {
        IMFAdapterDirection adapterDirection;
        switch (direction) {
            case MiscServices::Direction::UP: {
                adapterDirection = IMFAdapterDirection::UP;
                break;
            }
            case MiscServices::Direction::DOWN: {
                adapterDirection = IMFAdapterDirection::DOWN;
                break;
            }
            case MiscServices::Direction::LEFT: {
                adapterDirection = IMFAdapterDirection::LEFT;
                break;
            }
            case MiscServices::Direction::RIGHT: {
                adapterDirection = IMFAdapterDirection::RIGHT;
                break;
            }
            default: {
                adapterDirection = IMFAdapterDirection::NONE;
            }
        }
        listener_->MoveCursor(adapterDirection);
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

int32_t IMFTextListenerAdapterImpl::SetPreviewText(const std::u16string& text, const MiscServices::Range& range)
{
    if (listener_) {
        return listener_->SetPreviewText(text, range.start, range.end);
    }
    return -1;
}

void IMFTextListenerAdapterImpl::FinishTextPreview()
{
    if (listener_) {
        listener_->FinishTextPreview();
    }
}

int32_t IMFTextListenerAdapterImpl::ReceivePrivateCommand(
    const std::unordered_map<std::string, MiscServices::PrivateDataValue>& privateCommand)
{
    WVLOG_I("ReceivePrivateCommand");
    auto item = privateCommand.find(PREVIEW_TEXT_STYLE_KEY);
    if (item != privateCommand.end()) {
        bool isNeedUnderline = false;
        MiscServices::PrivateDataValue data = item->second;
        std::string previewStyle = std::get<std::string>(data);
        if (previewStyle == PREVIEW_TEXT_STYLE_UNDERLINE) {
            isNeedUnderline = true;
        }

        if (listener_) {
            listener_->SetNeedUnderLine(isNeedUnderline);
        }
    }

    item = privateCommand.find(AUTO_FILL_PARAMS_USERNAME);
    if (item != privateCommand.end()) {
        if (listener_) {
            std::string content = std::get<std::string>(item->second);
            listener_->AutoFillWithIMFEvent(true, false, false, content);
        }
    }

    item = privateCommand.find(AUTO_FILL_PARAMS_OTHERACCOUNT);
    if (item != privateCommand.end()) {
        if (listener_) {
            std::string content = std::string("");
            listener_->AutoFillWithIMFEvent(false, true, false, content);
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
    if (!textListener_) {
        textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
        if (!textListener_) {
            WVLOG_E("new textListener failed");
            return false;
        }
    }
    int32_t ret = MiscServices::InputMethodController::GetInstance()->Attach(textListener_, isShowKeyboard);
    if (ret != 0) {
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

    if ((textListener_ != nullptr) && isResetListener) {
        textListener_ = nullptr;
        WVLOG_I("attach node is changed, need reset listener");
    }

    if (!textListener_) {
        textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
        if (!textListener_) {
            WVLOG_E("new textListener failed");
            ReportImfErrorEvent(IMF_LISTENER_NULL_POINT, isShowKeyboard);
            return false;
        }
    }
    MiscServices::InputAttribute inputAttribute = { .inputPattern = config->GetInputAttribute()->GetInputPattern(),
        .enterKeyType = config->GetInputAttribute()->GetEnterKeyType(),
        .isTextPreviewSupported = true };

    MiscServices::CursorInfo imfInfo = { .left = config->GetCursorInfo()->GetLeft(),
        .top = config->GetCursorInfo()->GetTop(),
        .width = config->GetCursorInfo()->GetWidth(),
        .height = config->GetCursorInfo()->GetHeight() };

    MiscServices::TextConfig textConfig = { .inputAttribute = inputAttribute,
        .cursorInfo = imfInfo,
        .windowId = config->GetWindowId(),
        .positionY = config->GetPositionY(),
        .height = config->GetHeight() };
    WVLOG_I(
        "web inputmethod attach, isShowKeyboard=%{public}d, textConfig=%{public}s",
        isShowKeyboard,
        textConfig.ToString().c_str());
    int32_t ret = MiscServices::InputMethodController::GetInstance()->Attach(textListener_, isShowKeyboard, textConfig);
    if (ret != 0) {
        WVLOG_E("inputmethod attach failed, errcode=%{public}d", ret);
        ReportImfErrorEvent(ret, isShowKeyboard);
        return false;
    }
    return true;
}

bool IMFAdapterImpl::AttachWithRequestKeyboardReason(std::shared_ptr<IMFTextListenerAdapter> listener,
    bool isShowKeyboard, const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener,
    int32_t requestKeyboardReason)
{
    if (!AttachParamsCheck(listener, isShowKeyboard, config, isResetListener)) {
        return false;
    }

    MiscServices::InputAttribute inputAttribute = { .inputPattern = config->GetInputAttribute()->GetInputPattern(),
        .enterKeyType = config->GetInputAttribute()->GetEnterKeyType(),
        .isTextPreviewSupported = true };

    MiscServices::CursorInfo imfInfo = { .left = config->GetCursorInfo()->GetLeft(),
        .top = config->GetCursorInfo()->GetTop(),
        .width = config->GetCursorInfo()->GetWidth(),
        .height = config->GetCursorInfo()->GetHeight() };

    MiscServices::TextConfig textConfig = { .inputAttribute = inputAttribute,
        .cursorInfo = imfInfo,
        .windowId = config->GetWindowId(),
        .positionY = config->GetPositionY(),
        .height = config->GetHeight() };
    OHOS::MiscServices::AttachOptions attachOptions;
    attachOptions.isShowKeyboard = isShowKeyboard;
    attachOptions.requestKeyboardReason = static_cast<OHOS::MiscServices::RequestKeyboardReason>(requestKeyboardReason);
    WVLOG_I(
        "web inputmethod attach, isShowKeyboard=%{public}d, requestKeyboardReason=%{public}d, textConfig=%{public}s",
        isShowKeyboard,
        requestKeyboardReason,
        textConfig.ToString().c_str());
    int32_t ret = MiscServices::InputMethodController::GetInstance()->Attach(textListener_, attachOptions, textConfig);
    if (ret != 0) {
        WVLOG_E("inputmethod attach failed, errcode=%{public}d", ret);
        ReportImfErrorEvent(ret, isShowKeyboard);
        return false;
    }
    return true;
}

void IMFAdapterImpl::ShowCurrentInput(const IMFAdapterTextInputType& inputType)
{
    MiscServices::Configuration config;
    if (inputType == IMFAdapterTextInputType::NUMBER) {
        config.SetTextInputType(MiscServices::TextInputType::NUMBER);
    } else {
        config.SetTextInputType(MiscServices::TextInputType::TEXT);
    }
    MiscServices::InputMethodController::GetInstance()->OnConfigurationChange(config);
    MiscServices::InputMethodController::GetInstance()->ShowCurrentInput();
}

void IMFAdapterImpl::HideTextInput()
{
    MiscServices::InputMethodController::GetInstance()->HideTextInput();
}

void IMFAdapterImpl::Close()
{
    MiscServices::InputMethodController::GetInstance()->Close();
}

void IMFAdapterImpl::OnCursorUpdate(const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo)
{
    if (!cursorInfo) {
        WVLOG_E("inputmethod OnCursorUpdate cursorInfo is null");
        return;
    }

    MiscServices::CursorInfo imfInfo = { .left = cursorInfo->GetLeft(),
        .top = cursorInfo->GetTop(),
        .width = cursorInfo->GetWidth(),
        .height = cursorInfo->GetHeight() };
    WVLOG_D("imfInfo left = %{public}f, top = %{public}f, width = %{public}f, height = %{public}f", imfInfo.left,
        imfInfo.top, imfInfo.width, imfInfo.height);
    MiscServices::InputMethodController::GetInstance()->OnCursorUpdate(imfInfo);
}

void IMFAdapterImpl::OnSelectionChange(std::u16string text, int start, int end)
{
    MiscServices::InputMethodController::GetInstance()->OnSelectionChange(text, start, end);
}

bool IMFAdapterImpl::SendPrivateCommand(const std::string& commandKey, const std::string& commandValue)
{
    if (commandKey == AUTO_FILL_CANCEL_PRIVATE_COMMAND) {
        std::unordered_map<std::string, MiscServices::PrivateDataValue> privateCommand;
        ParseFillContentJsonValue(commandValue, privateCommand);
        int32_t ret = MiscServices::InputMethodController::GetInstance()->SendPrivateCommand(privateCommand);
        if (ret != 0) {
            WVLOG_E("inputmethod SendPrivateCommand failed, errcode=%{public}d", ret);
            return false;
        }
        WVLOG_I("inputmethod  SendPrivateCommand success");
        return true;
    }
    return false;
}

bool IMFAdapterImpl::ParseFillContentJsonValue(const std::string& commandValue,
    std::unordered_map<std::string, std::variant<std::string, bool, int32_t>>& map)
{
    cJSON* sourceJson = cJSON_Parse(commandValue.c_str());
    if (sourceJson == nullptr || cJSON_IsNull(sourceJson)) {
        cJSON_Delete(sourceJson);
        return false;
    }
    if (cJSON_HasObjectItem(sourceJson, "userName")) {
        cJSON* userName = cJSON_GetObjectItem(sourceJson, "userName");
        if (userName != nullptr && cJSON_IsString(userName) && userName->valuestring != nullptr) {
            map.insert(std::make_pair("userName", userName->valuestring));
        }
    }
    if (cJSON_HasObjectItem(sourceJson, "hasAccount")) {
        cJSON* hasAccount = cJSON_GetObjectItem(sourceJson, "hasAccount");
        if (hasAccount != nullptr && cJSON_IsString(hasAccount) && hasAccount->valuestring != nullptr) {
            map.insert(std::make_pair("hasAccount", hasAccount->valuestring));
        }
    }
    cJSON_Delete(sourceJson);
    return true;
}

bool IMFAdapterImpl::AttachParamsCheck(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
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

    if ((textListener_ != nullptr) && isResetListener) {
        textListener_ = nullptr;
        WVLOG_I("attach node is changed, need reset listener");
    }

    if (!textListener_) {
        textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
        if (!textListener_) {
            WVLOG_E("new textListener failed");
            ReportImfErrorEvent(IMF_LISTENER_NULL_POINT, isShowKeyboard);
            return false;
        }
    }
    return true;
}

void IMFTextListenerAdapterImpl::NotifyPanelStatusInfo(const MiscServices::PanelStatusInfo& info)
{
    WVLOG_I("IMFTextListenerAdapterImpl::NotifyPanelStatusInfo, visible:%{public}d", info.visible);
    MiscServices::Trigger triggerFrom = info.trigger;
    if (listener_ && (triggerFrom == MiscServices::Trigger::IME_APP)) {
        WVLOG_I("IMFTextListenerAdapterImpl::NotifyPanelStatusInfo, info.IME_APP");
        listener_->KeyboardUpperRightCornerHide();
    }
    listener_->WebSetImeShow(info.visible);
}
} // namespace OHOS::NWeb
