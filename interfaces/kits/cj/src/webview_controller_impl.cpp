/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webview_controller_impl.h"

#include <cstdint>
#include <nweb_helper.h>

#include "application_context.h"
#include "arkweb_scheme_handler.h"
#include "bundle_mgr_proxy.h"
#include "cj_common_ffi.h"
#include "ffi_remote_data.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "nweb_precompile_callback.h"
#include "nweb_store_web_archive_callback.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "web_errors.h"
#include "web_native_media_player.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"
#include "webview_log.h"
#include "webview_utils.h"
#include "nweb_message_ext.h"

namespace OHOS::Webview {
    constexpr int MAX_CUSTOM_SCHEME_SIZE = 10;
    constexpr int MAX_CUSTOM_SCHEME_NAME_LENGTH = 32;
    std::unordered_map<int32_t, WebviewControllerImpl*> g_webview_controller_map;
    std::string WebviewControllerImpl::customeSchemeCmdLine_ = "";
    bool WebviewControllerImpl::existNweb_ = false;
    bool WebviewControllerImpl::webDebuggingAccess_ = false;

    // WebMessagePortImpl
    WebMessagePortImpl::WebMessagePortImpl(int32_t nwebId, std::string port, bool isExtentionType)
        : nwebId_(nwebId), portHandle_(port), isExtentionType_(isExtentionType)
    {}

    ErrCode WebMessagePortImpl::ClosePort()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        nweb_ptr->ClosePort(portHandle_);
        portHandle_.clear();
        return NWebError::NO_ERROR;
    }

    ErrCode WebMessagePortImpl::PostPortMessage(std::shared_ptr<NWeb::NWebMessage> data)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        if (portHandle_.empty()) {
            WEBVIEWLOGE("can't post message, message port already closed");
            return NWebError::CAN_NOT_POST_MESSAGE;
        }
        nweb_ptr->PostPortMessage(portHandle_, data);
        return NWebError::NO_ERROR;
    }

    ErrCode WebMessagePortImpl::SetPortMessageCallback(
        std::shared_ptr<NWeb::NWebMessageValueCallback> callback)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        if (portHandle_.empty()) {
            WEBVIEWLOGE("can't register message port callback event, message port already closed");
            return NWebError::CAN_NOT_REGISTER_MESSAGE_EVENT;
        }
        nweb_ptr->SetPortMessageCallback(portHandle_, callback);
        return NWebError::NO_ERROR;
    }

    std::string WebMessagePortImpl::GetPortHandle() const
    {
        return portHandle_;
    }

    void NWebMessageCallbackImpl::OnReceiveValue(std::shared_ptr<NWeb::NWebMessage> result)
    {
        WEBVIEWLOGD("message port received msg");
        if (result == nullptr) {
            return;
        }
        NWeb::NWebValue::Type type = result->GetType();
        if (type == NWeb::NWebValue::Type::STRING) {
            std::string msgStr = result->GetString();
            char* message = MallocCString(msgStr);
            RetWebMessage ret = {.messageStr = message, .messageArr = {.head = nullptr, .size = 0}};
            callback_(ret);
            free(message);
        } else if (type == NWeb::NWebValue::Type::BINARY) {
            std::vector<uint8_t> msgArr = result->GetBinary();
            uint8_t* result = VectorToCArrUI8(msgArr);
            if (result == nullptr) {
                return;
            }
            RetWebMessage ret = {.messageStr = nullptr, .messageArr = CArrUI8{result, msgArr.size()}};
            callback_(ret);
            free(result);
        }
    }

    void NWebMessageCallbackImpl::OnReceiveValueV2(std::shared_ptr<NWeb::NWebHapValue> value)
    {
        std::shared_ptr<NWeb::NWebMessage> message = ConvertNwebHap2NwebMessage(value);
        OnReceiveValue(message);
    }

    void NWebWebMessageExtCallbackImpl::OnReceiveValue(std::shared_ptr<NWeb::NWebMessage> result)
    {
        WEBVIEWLOGD("message port received msg");
        WebMessageExtImpl *webMessageExt = OHOS::FFI::FFIData::Create<WebMessageExtImpl>(result);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("new WebMessageExt failed.");
            return;
        }
        callback_(webMessageExt->GetID());
    }

    void NWebWebMessageExtCallbackImpl::OnReceiveValueV2(std::shared_ptr<NWeb::NWebHapValue> value)
    {
        WEBVIEWLOGD("message port received msg");
        if (value == nullptr) {
            return;
        }
        WebMessageExtImpl *webMessageExt = OHOS::FFI::FFIData::Create<WebMessageExtImpl>(value);
        if (webMessageExt == nullptr) {
            WEBVIEWLOGE("new WebMessageExt failed.");
            return;
        }
        callback_(webMessageExt->GetID());
    }

    WebviewControllerImpl::WebviewControllerImpl(int32_t nwebId) : nwebId_(nwebId)
    {
        if (IsInit()) {
            std::unique_lock<std::mutex> lk(webMtx_);
            g_webview_controller_map.emplace(nwebId, this);
        }
    }

    bool WebviewControllerImpl::IsInit()
    {
        return NWeb::NWebHelper::Instance().GetNWeb(nwebId_) ? true : false;
    }

    void WebviewControllerImpl::SetWebId(int32_t nwebId)
    {
        nwebId_ = nwebId;
        std::unique_lock<std::mutex> lk(webMtx_);
        g_webview_controller_map.emplace(nwebId, this);

        if (webTag_.empty()) {
            WEBVIEWLOGI("native webtag is empty, don't care because it's not a native instance");
            return;
        }

        auto nweb_ptr = OHOS::NWeb::NWebHelper::Instance().GetNWeb(nwebId);
        if (nweb_ptr) {
            OH_NativeArkWeb_BindWebTagToWebInstance(webTag_.c_str(), nweb_ptr);
            NWeb::NWebHelper::Instance().SetWebTag(nwebId_, webTag_.c_str());
        }
    }

    void WebviewControllerImpl::InnerSetHapPath(const std::string &hapPath)
    {
        hapPath_ = hapPath;
    }

    int32_t WebviewControllerImpl::GetWebId() const
    {
        int32_t webId = -1;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            webId = static_cast<int32_t>(nweb_ptr->GetWebId());
        }
        return webId;
    }

    int32_t WebviewControllerImpl::LoadUrl(std::string url)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        return nweb_ptr->Load(url);
    }

    int32_t WebviewControllerImpl::LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        return nweb_ptr->Load(url, httpHeaders);
    }

    ErrCode WebviewControllerImpl::LoadData(std::string data, std::string mimeType, std::string encoding,
        std::string baseUrl, std::string historyUrl)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        if (baseUrl.empty() && historyUrl.empty()) {
            return nweb_ptr->LoadWithData(data, mimeType, encoding);
        }
        return nweb_ptr->LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
    }

    int32_t WebviewControllerImpl::PreFetchPage(std::string url)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        std::map<std::string, std::string> httpHeaders;
        nweb_ptr->PrefetchPage(url, httpHeaders);
        return NWebError::NO_ERROR;
    }

    int32_t WebviewControllerImpl::PreFetchPage(std::string url, std::map<std::string, std::string> httpHeaders)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->PrefetchPage(url, httpHeaders);
        return NWebError::NO_ERROR;
    }

    void WebviewControllerImpl::Refresh()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->Reload();
        }
    }

    int32_t WebviewControllerImpl::SetAudioMuted(bool mute)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->SetAudioMuted(mute);
        return NWebError::NO_ERROR;
    }

    std::string WebviewControllerImpl::GetUserAgent()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return "";
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return "";
        }
        return setting->DefaultUserAgent();
    }

    bool WebviewControllerImpl::AccessForward()
    {
        bool access = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            access = nweb_ptr->IsNavigateForwardAllowed();
        }
        return access;
    }

    bool WebviewControllerImpl::AccessBackward()
    {
        bool access = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            access = nweb_ptr->IsNavigatebackwardAllowed();
        }
        return access;
    }

    int32_t WebviewControllerImpl::SetCustomUserAgent(const std::string& userAgent)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return NWebError::INIT_ERROR;
        }
        setting->PutUserAgent(userAgent);
        return NWebError::NO_ERROR;
    }

    std::string WebviewControllerImpl::GetCustomUserAgent() const
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return "";
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return "";
        }
        return setting->UserAgent();
    }

    void WebviewControllerImpl::RunJavaScript(std::string script,
        const std::function<void(RetDataCString)>& callbackRef)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            callbackRef(ret);
            return;
        }
        auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(callbackRef);
        nweb_ptr->ExecuteJavaScript(script, callbackImpl, false);
    }

    void WebviewControllerImpl::RunJavaScriptExt(std::string script,
        const std::function<void(RetDataI64)>& callbackRef)
    {
        RetDataI64 ret = { .code = NWebError::INIT_ERROR, .data = 0 };
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            callbackRef(ret);
            return;
        }
        auto callbackImpl = std::make_shared<WebviewJavaScriptExtExecuteCallback>(callbackRef);
        nweb_ptr->ExecuteJavaScript(script, callbackImpl, true);
    }

    std::string WebviewControllerImpl::GetUrl()
    {
        std::string url = "";
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            url = nweb_ptr->GetUrl();
        }
        return url;
    }

    std::string WebviewControllerImpl::GetOriginalUrl()
    {
        std::string url = "";
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            url = nweb_ptr->GetOriginalUrl();
        }
        return url;
    }

    void WebviewControllerImpl::ScrollPageUp(bool top)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->PageUp(top);
        }
        return;
    }

    void WebviewControllerImpl::ScrollPageDown(bool bottom)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->PageDown(bottom);
        }
        return;
    }

    void WebviewControllerImpl::ScrollTo(float x, float y)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->ScrollTo(x, y);
        }
        return;
    }

    void WebviewControllerImpl::ScrollBy(float deltaX, float deltaY)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->ScrollBy(deltaX, deltaY);
        }
        return;
    }

    void WebviewControllerImpl::ScrollToWithAnime(float x, float y, int32_t duration)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->ScrollToWithAnime(x, y, duration);
        }
        return;
    }

    void WebviewControllerImpl::ScrollByWithAnime(float deltaX, float deltaY, int32_t duration)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->ScrollByWithAnime(deltaX, deltaY, duration);
        }
        return;
    }

    void WebviewControllerImpl::Forward()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->NavigateForward();
        }
        return;
    }

    void WebviewControllerImpl::Backward()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->NavigateBack();
        }
        return;
    }

    int32_t WebviewControllerImpl::BackOrForward(int32_t step)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->NavigateBackOrForward(step);
        return NWebError::NO_ERROR;
    }

    int32_t WebviewControllerImpl::GetProgress()
    {
        int32_t progress = 0;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            progress = nweb_ptr->PageLoadProgress();
        }
        return progress;
    }

    int32_t WebviewControllerImpl::GetPageHeight()
    {
        int32_t pageHeight = 0;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            pageHeight = nweb_ptr->ContentHeight();
        }
        return pageHeight;
    }

    std::string WebviewControllerImpl::GetTitle()
    {
        std::string title = "";
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            title = nweb_ptr->Title();
        }
        return title;
    }

    int32_t WebviewControllerImpl::Zoom(float factor)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        ErrCode result = NWebError::NO_ERROR;
        result = nweb_ptr->Zoom(factor);

        return result;
    }

    int32_t WebviewControllerImpl::ZoomIn()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        ErrCode result = NWebError::NO_ERROR;
        result = nweb_ptr->ZoomIn();

        return result;
    }

    int32_t WebviewControllerImpl::ZoomOut()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        ErrCode result = NWebError::NO_ERROR;
        result = nweb_ptr->ZoomOut();

        return result;
    }

    int32_t WebviewControllerImpl::RequestFocus()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->OnFocus();
        ErrCode result = NWebError::NO_ERROR;
        return result;
    }

    void WebviewControllerImpl::ClearHistory()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->DeleteNavigateHistory();
        }
    }

    bool WebviewControllerImpl::AccessStep(int32_t step)
    {
        bool access = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            access = nweb_ptr->CanNavigateBackOrForward(step);
        }
        return access;
    }

    void WebviewControllerImpl::OnActive()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            WEBVIEWLOGD("WebviewControllerImpl::OnActive start")
            nweb_ptr->OnContinue();
        }
    }

    void WebviewControllerImpl::OnInactive()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            WEBVIEWLOGD("WebviewControllerImpl::OnInactive start")
            nweb_ptr->OnPause();
        }
    }

    int WebviewControllerImpl::GetHitTest()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return ConverToWebHitTestType(nweb_ptr->GetHitTestResult()->GetType());
        }
        return static_cast<int>(WebHitTestType::UNKNOWN);
    }

    std::shared_ptr<NWeb::HitTestResult> WebviewControllerImpl::GetHitTestValue()
    {
        std::shared_ptr<NWeb::HitTestResult> nwebResult;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nwebResult = nweb_ptr->GetHitTestResult();
            if (nwebResult) {
                nwebResult->SetType(ConverToWebHitTestType(nwebResult->GetType()));
            }
        }
        return nwebResult;
    }

    int WebviewControllerImpl::ConverToWebHitTestType(int hitType)
    {
        WebHitTestType webHitType;
        switch (hitType) {
            case NWeb::HitTestResult::UNKNOWN_TYPE:
                webHitType = WebHitTestType::UNKNOWN;
                break;
            case NWeb::HitTestResult::ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP;
                break;
            case NWeb::HitTestResult::PHONE_TYPE:
                webHitType = WebHitTestType::PHONE;
                break;
            case NWeb::HitTestResult::GEO_TYPE:
                webHitType = WebHitTestType::MAP;
                break;
            case NWeb::HitTestResult::EMAIL_TYPE:
                webHitType = WebHitTestType::EMAIL;
                break;
            case NWeb::HitTestResult::IMAGE_TYPE:
                webHitType = WebHitTestType::IMG;
                break;
            case NWeb::HitTestResult::IMAGE_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP_IMG;
                break;
            case NWeb::HitTestResult::SRC_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP;
                break;
            case NWeb::HitTestResult::SRC_IMAGE_ANCHOR_TYPE:
                webHitType = WebHitTestType::HTTP_IMG;
                break;
            case NWeb::HitTestResult::EDIT_TEXT_TYPE:
                webHitType = WebHitTestType::EDIT;
                break;
            default:
                webHitType = WebHitTestType::UNKNOWN;
                break;
        }
        return static_cast<int>(webHitType);
    }

    void WebviewControllerImpl::StoreWebArchiveCallback(std::string baseName, bool autoName,
        const std::function<void(RetDataCString)>& callbackRef)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            callbackRef(ret);
            return;
        }
        auto callbackImpl = std::make_shared<NWeb::NWebStoreWebArchiveCallback>();
        callbackImpl->SetCallBack([cjCallback = callbackRef](std::string result) {
            RetDataCString ret = { .code = NWebError::INVALID_RESOURCE, .data = nullptr };
            if (result.empty()) {
                cjCallback(ret);
                return;
            }
            ret.code = NWebError::NO_ERROR;
            ret.data = MallocCString(result);
            if (ret.data == nullptr) {
                ret.code = NWebError::NEW_OOM;
            }
            cjCallback(ret);
        });
        nweb_ptr->StoreWebArchive(baseName, autoName, callbackImpl);
        return;
    }

    void WebviewControllerImpl::EnableSafeBrowsing(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->EnableSafeBrowsing(enable);
        }
        return;
    }

    bool WebviewControllerImpl::IsSafeBrowsingEnabled()
    {
        bool is_safe_browsing_enabled = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            is_safe_browsing_enabled = nweb_ptr->IsSafeBrowsingEnabled();
        }
        return is_safe_browsing_enabled;
    }

    int WebviewControllerImpl::GetSecurityLevel()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return static_cast<int>(SecurityLevel::NONE);
        }

        int nwebSecurityLevel = nweb_ptr->GetSecurityLevel();
        SecurityLevel securityLevel;
        switch (nwebSecurityLevel) {
            case static_cast<int>(CoreSecurityLevel::NONE):
                securityLevel = SecurityLevel::NONE;
                break;
            case static_cast<int>(CoreSecurityLevel::SECURE):
                securityLevel = SecurityLevel::SECURE;
                break;
            case static_cast<int>(CoreSecurityLevel::WARNING):
                securityLevel = SecurityLevel::WARNING;
                break;
            case static_cast<int>(CoreSecurityLevel::DANGEROUS):
                securityLevel = SecurityLevel::DANGEROUS;
                break;
            default:
                securityLevel = SecurityLevel::NONE;
                break;
        }

        return static_cast<int>(securityLevel);
    }

    bool WebviewControllerImpl::IsIncognitoMode()
    {
        bool incognitoMode = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            incognitoMode = nweb_ptr->IsIncognitoMode();
        }
        return incognitoMode;
    }

    void WebviewControllerImpl::RemoveCache(bool includeDiskFiles)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            WEBVIEWLOGD("WebviewControllerImpl::RemoveCache start")
            nweb_ptr->RemoveCache(includeDiskFiles);
        }
    }

    std::shared_ptr<OHOS::NWeb::NWebHistoryList> WebviewControllerImpl::GetHistoryList()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return nullptr;
        }
        return nweb_ptr->GetHistoryList();
    }

    bool WebviewControllerImpl::GetFavicon(const void **data, size_t &width, size_t &height,
        NWeb::ImageColorType &colorType, NWeb::ImageAlphaType &alphaType) const
    {
        bool isGetFavicon = false;
        if (!data) {
            WEBVIEWLOGE("WebviewControllerImpl::GetFavicon data is nullptr");
            return isGetFavicon;
        }
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            isGetFavicon = nweb_ptr->GetFavicon(data, width, height, colorType, alphaType);
        }
        return isGetFavicon;
    }

    int32_t WebHistoryListImpl::GetListSize()
    {
        int32_t listSize = 0;

        if (!sptrHistoryList_) {
            return listSize;
        }
        listSize = sptrHistoryList_->GetListSize();
        return listSize;
    }

    int32_t WebHistoryListImpl::GetCurrentIndex()
    {
        int32_t currentIndex = 0;

        if (!sptrHistoryList_) {
            return currentIndex;
        }
        currentIndex = sptrHistoryList_->GetCurrentIndex();
        return currentIndex;
    }

    std::shared_ptr<OHOS::NWeb::NWebHistoryItem> WebHistoryListImpl::GetItem(int32_t index)
    {
        if (!sptrHistoryList_) {
            return nullptr;
        }
        return sptrHistoryList_->GetItem(index);
    }

    void WebviewControllerImpl::SetNWebJavaScriptResultCallBack()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        if (javaScriptResultCb_ && (javaScriptResultCb_->GetNWebId() == nwebId_)) {
            return;
        }

        javaScriptResultCb_ = std::make_shared<WebviewJavaScriptResultCallBackImpl>(nwebId_);
        nweb_ptr->SetNWebJavaScriptResultCallBack(javaScriptResultCb_);
    }

    void WebviewControllerImpl::RegisterJavaScriptProxy(const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::string& objName, const std::vector<std::string>& methodList)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy nweb_ptr is null");
            return;
        }
        JavaScriptOb::ObjectID objId =
            static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBCONTROLLERERROR);

        if (!javaScriptResultCb_) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy javaScriptResultCb_ is null");
            return;
        }

        if (methodList.empty()) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy methodList is empty");
            return;
        }

        objId = javaScriptResultCb_->RegisterJavaScriptProxy(cjFuncs, objName, methodList);

        nweb_ptr->RegisterArkJSfunction(objName, methodList, objId);
    }

    void WebviewControllerImpl::RegisterJavaScriptProxyEx(const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::string& objName, const std::vector<std::string>& methodList, char* permission)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy nweb_ptr is null");
            return;
        }
        JavaScriptOb::ObjectID objId =
            static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBCONTROLLERERROR);

        if (!javaScriptResultCb_) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy javaScriptResultCb_ is null");
            return;
        }

        if (methodList.empty()) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy methodList is empty");
            return;
        }
        if (!permission) {
            WEBVIEWLOGE("WebviewControllerImpl::RegisterJavaScriptProxy permission is nullptr");
            return;
        }

        objId = javaScriptResultCb_->RegisterJavaScriptProxy(cjFuncs, objName, methodList);

        nweb_ptr->RegisterArkJSfunction(objName, methodList, std::vector<std::string>(),
            objId, permission);
    }

    void WebviewControllerImpl::Stop()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->Stop();
        }
        return;
    }

    void WebviewControllerImpl::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            WEBVIEWLOGE("WebviewControllerImpl::void SetBackForwardCacheOptions nweb_ptr is null");
            return;
        }
        nweb_ptr->SetBackForwardCacheOptions(size, timeToLive);
    }

    void WebviewControllerImpl::SlideScroll(float vx, float vy)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->SlideScroll(vx, vy);
        }
        return;
    }

    void WebviewControllerImpl::PutNetworkAvailable(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->PutNetworkAvailable(enable);
        }
        return;
    }

    void WebviewControllerImpl::ClearClientAuthenticationCache()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->ClearClientAuthenticationCache();
        }
        return;
    }

    void WebviewControllerImpl::ClearSslCache()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->ClearSslCache();
        }
        return;
    }

    void WebviewControllerImpl::SearchNext(bool forward)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->FindNext(forward);
        }
        return;
    }

    void WebviewControllerImpl::ClearMatches()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->ClearMatches();
        }
        return;
    }

    void WebviewControllerImpl::SearchAllAsync(std::string str)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->FindAllAsync(str);
        }
        return;
    }

    ErrCode WebviewControllerImpl::DeleteJavaScriptRegister(const std::string& objName,
        const std::vector<std::string>& methodList)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->UnregisterArkJSfunction(objName, methodList);
        }
        if (javaScriptResultCb_) {
            bool ret = javaScriptResultCb_->DeleteJavaScriptRegister(objName);
            if (!ret) {
                return NWebError::CANNOT_DEL_JAVA_SCRIPT_PROXY;
            }
        }
        return NWebError::NO_ERROR;
    }

    int32_t WebviewControllerImpl::PostUrl(std::string& url, std::vector<char>& postData)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        return nweb_ptr->PostUrl(url, postData);
    }

    std::vector<std::string> WebviewControllerImpl::CreateWebMessagePorts()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            std::vector<std::string> empty;
            return empty;
        }
        return nweb_ptr->CreateWebMessagePorts();
    }

    ErrCode WebviewControllerImpl::PostWebMessage(std::string& message,
        std::vector<std::string>& ports, std::string& targetUrl)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        nweb_ptr->PostWebMessage(message, ports, targetUrl);
        return NWebError::NO_ERROR;
    }

    std::vector<uint8_t> WebviewControllerImpl::SerializeWebState()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            return nweb_ptr->SerializeWebState();
        }
        std::vector<uint8_t> empty;
        return empty;
    }

    bool WebviewControllerImpl::RestoreWebState(const std::vector<uint8_t> &state) const
    {
        bool isRestored = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            isRestored = nweb_ptr->RestoreWebState(state);
        }
        return isRestored;
    }

    bool WebviewControllerImpl::GetCertChainDerData(std::vector<std::string> &certChainDerData) const
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            WEBVIEWLOGE("GetCertChainDerData failed, nweb ptr is null");
            return false;
        }

        return nweb_ptr->GetCertChainDerData(certChainDerData, true);
    }

    ErrCode WebviewControllerImpl::HasImagesCallback(const std::function<void(RetDataBool)>& callbackRef)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        if (callbackRef == nullptr) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        auto callbackImpl = std::make_shared<WebviewHasImageCallback>(callbackRef);
        nweb_ptr->HasImages(callbackImpl);
        return NWebError::NO_ERROR;
    }

    bool WebviewControllerImpl::TerminateRenderProcess()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->TerminateRenderProcess();
    }

    void WebviewControllerImpl::CloseAllMediaPresentations()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->CloseAllMediaPresentations();
    }

    void WebviewControllerImpl::PauseAllMedia()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->PauseAllMedia();
    }

    void WebviewControllerImpl::ResumeAllMedia()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->ResumeAllMedia();
    }

    void WebviewControllerImpl::StopAllMedia()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->StopAllMedia();
    }

    void WebviewControllerImpl::SetPrintBackground(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->SetPrintBackground(enable);
    }

    bool WebviewControllerImpl::GetPrintBackground()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->GetPrintBackground();
    }

    bool WebviewControllerImpl::GetScrollable()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return true;
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return true;
        }
        return setting->GetScrollable();
    }

    void WebviewControllerImpl::SetScrollable(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return;
        }
        setting->SetScrollable(enable);
    }

    void WebviewControllerImpl::SetScrollable(bool enable, int32_t scrollType)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return;
        }
        return setting->SetScrollable(enable, scrollType);
    }

    bool WebviewControllerImpl::ScrollByWithResult(float deltaX, float deltaY) const
    {
        bool enabled = false;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            enabled = nweb_ptr->ScrollByWithResult(deltaX, deltaY);
        }
        return enabled;
    }

    void WebviewControllerImpl::EnableAdsBlock(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->EnableAdsBlock(enable);
    }

    bool WebviewControllerImpl::IsAdsBlockEnabled()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->IsAdsBlockEnabled();
    }

    bool WebviewControllerImpl::IsAdsBlockEnabledForCurPage()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->IsAdsBlockEnabledForCurPage();
    }

    bool WebviewControllerImpl::IsIntelligentTrackingPreventionEnabled()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->IsIntelligentTrackingPreventionEnabled();
    }

    void WebviewControllerImpl::EnableIntelligentTrackingPrevention(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->EnableIntelligentTrackingPrevention(enable);
        }
        return;
    }

    int32_t WebviewControllerImpl::GetMediaPlaybackState()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return static_cast<int32_t>(MediaPlaybackState::NONE);
        }
        return nweb_ptr->GetMediaPlaybackState();
    }

    std::string WebviewControllerImpl::GetLastJavascriptProxyCallingFrameUrl()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return "";
        }
        return nweb_ptr->GetLastJavascriptProxyCallingFrameUrl();
    }

    void WebviewControllerImpl::StartCamera()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->StartCamera();
    }

    void WebviewControllerImpl::StopCamera()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->StopCamera();
    }

    void WebviewControllerImpl::CloseCamera()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        nweb_ptr->CloseCamera();
    }

    std::string WebviewControllerImpl::GetSurfaceId()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return "";
        }
        std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
        if (!setting) {
            return "";
        }
        return setting->GetSurfaceId();
    }

    void WebviewControllerImpl::InjectOfflineResources(const std::vector<std::string>& urlList,
        const std::vector<uint8_t>& resource, const std::map<std::string, std::string>& response_headers,
        const uint32_t type)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        if (urlList.size() == 0) {
            return;
        }
        std::string originUrl = urlList[0];
        if (urlList.size() == 1) {
            nweb_ptr->InjectOfflineResource(originUrl, originUrl, resource, response_headers, type);
            return;
        }

        for (size_t i = 1; i < urlList.size(); i++) {
            nweb_ptr->InjectOfflineResource(urlList[i], originUrl, resource, response_headers, type);
        }
    }

    int32_t WebviewControllerImpl::SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        int ret = NWebError::NO_ERROR;
        switch (nweb_ptr->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsg)) {
            case static_cast<int>(UrlListSetResult::INIT_ERROR):
                ret = NWebError::INIT_ERROR;
                break;
            case static_cast<int>(UrlListSetResult::PARAM_ERROR):
                ret = NWebError::PARAM_CHECK_ERROR;
                break;
            case static_cast<int>(UrlListSetResult::SET_OK):
                ret = NWebError::NO_ERROR;
                break;
            default:
                ret = NWebError::PARAM_CHECK_ERROR;
                break;
        }
        return ret;
    }

    void WebviewControllerImpl::SetPathAllowingUniversalAccess(
        const std::vector<std::string>& pathList, std::string& errorPath)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        if (moduleName_.empty()) {
            WEBVIEWLOGD("need to get module nme for path");
            if (!GetHapModuleInfo()) {
                WEBVIEWLOGE("GetHapModuleInfo failed")
                moduleName_.clear();
                return;
            }
        }
        nweb_ptr->SetPathAllowingUniversalAccess(pathList, moduleName_, errorPath);
    }

    bool WebviewControllerImpl::GetHapModuleInfo()
    {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            WEBVIEWLOGE("get SystemAbilityManager failed");
            return false;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (remoteObject == nullptr) {
            WEBVIEWLOGE("get Bundle Manager failed");
            return false;
        }
        auto bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
        if (bundleMgr == nullptr) {
            WEBVIEWLOGE("get Bundle Manager failed");
            return false;
        }
        AppExecFwk::BundleInfo bundleInfo;
        if (bundleMgr->GetBundleInfoForSelf(
            static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE), bundleInfo) != 0) {
            WEBVIEWLOGE("get bundle info failed");
            return false;
        }
        moduleName_ = bundleInfo.moduleNames;
        return true;
    }

    bool WebviewControllerImpl::SetWebSchemeHandler(const char* scheme, WebSchemeHandlerImpl* handler)
    {
        if (!handler || !scheme) {
            WEBVIEWLOGE("WebviewControllerImpl::SetWebSchemeHandler schemeHandler or scheme is nullptr");
            return false;
        }
        ArkWeb_SchemeHandler* schemeHandler =
            const_cast<ArkWeb_SchemeHandler*>(WebSchemeHandlerImpl::GetArkWebSchemeHandler(handler));
        if (schemeHandler == nullptr) {
            WEBVIEWLOGE("schemeHandler is nullptr");
            return false;
        }
        return OH_ArkWeb_SetSchemeHandler(scheme, webTag_.c_str(), schemeHandler);
    }

    int32_t WebviewControllerImpl::ClearWebSchemeHandler()
    {
        return OH_ArkWeb_ClearSchemeHandlers(webTag_.c_str());
    }

    bool WebviewControllerImpl::SetWebServiceWorkerSchemeHandler(const char* scheme, WebSchemeHandlerImpl* handler)
    {
        if (!handler || !scheme) {
            WEBVIEWLOGE("WebviewControllerImpl::SetWebServiceWorkerSchemeHandler schemeHandler or scheme is nullptr");
            return false;
        }
        ArkWeb_SchemeHandler* schemeHandler =
            const_cast<ArkWeb_SchemeHandler*>(WebSchemeHandlerImpl::GetArkWebSchemeHandler(handler));
        return OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler);
    }

    int32_t WebviewControllerImpl::ClearWebServiceWorkerSchemeHandler()
    {
        return OH_ArkWebServiceWorker_ClearSchemeHandlers();
    }

    void WebviewControllerImpl::OnCreateNativeMediaPlayer(std::function<int64_t(int64_t, CMediaInfo)> callback)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return;
        }
        auto callbackImpl = std::make_shared<NWebCreateNativeMediaPlayerCallbackImpl>(nwebId_, callback);
        nweb_ptr->OnCreateNativeMediaPlayer(callbackImpl);
    }

    int32_t WebviewControllerImpl::PrecompileJavaScript(std::string url, std::string script,
        std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebPrecompileCallback>();
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        if (url.empty() || script.empty()) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        nweb_ptr->PrecompileJavaScript(url, script, cacheOptions, callbackImpl);
        return NWebError::NO_ERROR;
    }

    int32_t WebviewControllerImpl::WebPageSnapshot(const char* id, NWeb::PixelUnit type,
        int32_t width, int32_t height, const NWeb::WebSnapshotCallback callback)
    {
        if (!id) {
            WEBVIEWLOGE(" WebviewControllerImpl::WebPageSnapshot id is nullptr");
            return NWebError::INIT_ERROR;
        }
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }

        bool init = nweb_ptr->WebPageSnapshot(id, type, width, height, std::move(callback));
        if (!init) {
            return NWebError::INIT_ERROR;
        }
        return NWebError::NO_ERROR;
    }

    bool CheckSchemeName(const std::string& schemeName)
    {
        if (schemeName.empty() || schemeName.size() > MAX_CUSTOM_SCHEME_NAME_LENGTH) {
            WEBVIEWLOGE("Invalid scheme name length");
            return false;
        }
        for (auto it = schemeName.begin(); it != schemeName.end(); it++) {
            char chr = *it;
            if (!((chr >= 'a' && chr <= 'z') || (chr >= '0' && chr <= '9') ||
                (chr == '.') || (chr == '+') || (chr == '-'))) {
                WEBVIEWLOGE("invalid character %{public}c", chr);
                return false;
            }
        }
        return true;
    }

    void SetCustomizeSchemeOption(Scheme& scheme)
    {
        std::map<int, std::function<bool(const Scheme&)>> schemeProperties = {
            {0, [](const Scheme& scheme) { return scheme.isStandard; }},
            {1, [](const Scheme& scheme) { return scheme.isLocal; }},
            {2, [](const Scheme& scheme) { return scheme.isDisplayIsolated; }},
            {3, [](const Scheme& scheme) { return scheme.isSecure; }},
            {4, [](const Scheme& scheme) { return scheme.isSupportCORS; }},
            {5, [](const Scheme& scheme) { return scheme.isCspBypassing; }},
            {6, [](const Scheme& scheme) { return scheme.isSupportFetch; }},
            {7, [](const Scheme& scheme) { return scheme.isCodeCacheSupported; }}
        };

        for (const auto& property : schemeProperties) {
            if (property.second(scheme)) {
                scheme.option += 1 << property.first;
            }
        }
    }

    bool SetCustomizeScheme(CScheme cscheme, Scheme& scheme)
    {
        scheme.isSupportCORS = cscheme.isSupportCORS;
        scheme.isSupportFetch = cscheme.isSupportFetch;
        scheme.isStandard = cscheme.isStandard;
        scheme.isLocal = cscheme.isLocal;
        scheme.isDisplayIsolated = cscheme.isDisplayIsolated;
        scheme.isSecure = cscheme.isSecure;
        scheme.isCspBypassing = cscheme.isCspBypassing;
        scheme.isCodeCacheSupported = cscheme.isCodeCacheSupported;
        scheme.name = std::string(cscheme.name);
        if (!CheckSchemeName(scheme.name)) {
            return false;
        }
        SetCustomizeSchemeOption(scheme);
        return true;
    }

    int32_t WebviewControllerImpl::CustomizeSchemesArrayDataHandler(CArrScheme schemes)
    {
        int64_t arrayLength = schemes.size;
        if (arrayLength > MAX_CUSTOM_SCHEME_SIZE) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::vector<Scheme> schemeVector;
        for (int64_t i = 0; i < arrayLength; ++i) {
            Scheme scheme;
            bool result = SetCustomizeScheme(schemes.head[i], scheme);
            if (!result) {
                return NWebError::PARAM_CHECK_ERROR;
            }
            schemeVector.push_back(scheme);
        }
        int32_t registerResult;
        for (auto it = schemeVector.begin(); it != schemeVector.end(); ++it) {
            registerResult = OH_ArkWeb_RegisterCustomSchemes(it->name.c_str(), it->option);
            if (registerResult != NO_ERROR) {
                return registerResult;
            }
        }
        return NWebError::NO_ERROR;
    }

    std::shared_ptr<NWeb::HitTestResult> WebviewControllerImpl::GetLastHitTest()
    {
        std::shared_ptr<NWeb::HitTestResult> nwebResult;
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nwebResult = nweb_ptr->GetLastHitTestResult();
            if (nwebResult) {
                nwebResult->SetType(ConverToWebHitTestType(nwebResult->GetType()));
            }
        }
        return nwebResult;
    }

    void* WebviewControllerImpl::CreateWebPrintDocumentAdapter(const std::string &jobName)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return nullptr;
        }
        return nweb_ptr->CreateWebPrintDocumentAdapter(jobName);
    }

    void WebviewControllerImpl::GetScrollOffset(float* offset_x, float* offset_y)
    {
        if (!offset_x || !offset_y) {
            WEBVIEWLOGE("WebviewControllerImpl::GetScrollOffset offset_x or offset_y is nullptr");
            return;
        }
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (nweb_ptr) {
            nweb_ptr->GetScrollOffset(offset_x, offset_y);
        }
    }
    
    int32_t WebviewControllerImpl::AvoidVisibleViewportBottom(int32_t avoidHeight)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->AvoidVisibleViewportBottom(avoidHeight);
        return NWebError::NO_ERROR;
    }

    int32_t WebviewControllerImpl::SetErrorPageEnabled(bool enable)
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return NWebError::INIT_ERROR;
        }
        nweb_ptr->SetErrorPageEnabled(enable);
        return NWebError::NO_ERROR;
    }

    bool WebviewControllerImpl::GetErrorPageEnabled()
    {
        auto nweb_ptr = NWeb::NWebHelper::Instance().GetNWeb(nwebId_);
        if (!nweb_ptr) {
            return false;
        }
        return nweb_ptr->GetErrorPageEnabled();
    }
}
