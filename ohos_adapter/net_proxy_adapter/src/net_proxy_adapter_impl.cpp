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

#include "net_proxy_adapter_impl.h"

#include <vector>

#include "nweb_log.h"

namespace OHOS::NWeb {
static constexpr const char* DEFAULT_HTTP_PROXY_HOST = "NONE";
static constexpr const char* DEFAULT_HTTP_PROXY_EXCLUSION_LIST = "NONE";
static constexpr const char* EMPTY_HTTP_PROXY_HOST = "";

namespace Base64 {
static std::string BASE64_CHARS = /* NOLINT */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static constexpr const uint32_t CHAR_ARRAY_LENGTH_THREE = 3;
static constexpr const uint32_t CHAR_ARRAY_LENGTH_FOUR = 4;

enum BASE64_ENCODE_CONSTANT : uint8_t {
    BASE64_ENCODE_MASK1 = 0xfc,
    BASE64_ENCODE_MASK2 = 0x03,
    BASE64_ENCODE_MASK3 = 0x0f,
    BASE64_ENCODE_MASK4 = 0x3f,
    BASE64_ENCODE_MASK5 = 0xf0,
    BASE64_ENCODE_MASK6 = 0xc0,
    BASE64_ENCODE_OFFSET2 = 2,
    BASE64_ENCODE_OFFSET4 = 4,
    BASE64_ENCODE_OFFSET6 = 6,
    BASE64_ENCODE_INDEX0 = 0,
    BASE64_ENCODE_INDEX1 = 1,
    BASE64_ENCODE_INDEX2 = 2,
};

enum BASE64_DECODE_CONSTANT : uint8_t {
    BASE64_DECODE_MASK1 = 0x30,
    BASE64_DECODE_MASK2 = 0xf,
    BASE64_DECODE_MASK3 = 0x3c,
    BASE64_DECODE_MASK4 = 0x3,
    BASE64_DECODE_OFFSET2 = 2,
    BASE64_DECODE_OFFSET4 = 4,
    BASE64_DECODE_OFFSET6 = 6,
    BASE64_DECODE_INDEX0 = 0,
    BASE64_DECODE_INDEX1 = 1,
    BASE64_DECODE_INDEX2 = 2,
    BASE64_DECODE_INDEX3 = 3,
};

static inline bool IsBase64Char(const char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

static inline void MakeCharFour(const std::array<uint8_t, CHAR_ARRAY_LENGTH_THREE>& charArrayThree,
    std::array<uint8_t, CHAR_ARRAY_LENGTH_FOUR>& charArrayFour)
{
    const uint8_t table[CHAR_ARRAY_LENGTH_FOUR] = {
        static_cast<uint8_t>((charArrayThree[BASE64_ENCODE_INDEX0] & BASE64_ENCODE_MASK1) >> BASE64_ENCODE_OFFSET2),
        static_cast<uint8_t>(((charArrayThree[BASE64_ENCODE_INDEX0] & BASE64_ENCODE_MASK2) << BASE64_ENCODE_OFFSET4) +
                             ((charArrayThree[BASE64_ENCODE_INDEX1] & BASE64_ENCODE_MASK5) >> BASE64_ENCODE_OFFSET4)),
        static_cast<uint8_t>(((charArrayThree[BASE64_ENCODE_INDEX1] & BASE64_ENCODE_MASK3) << BASE64_ENCODE_OFFSET2) +
                             ((charArrayThree[BASE64_ENCODE_INDEX2] & BASE64_ENCODE_MASK6) >> BASE64_ENCODE_OFFSET6)),
        static_cast<uint8_t>(charArrayThree[BASE64_ENCODE_INDEX2] & BASE64_ENCODE_MASK4),
    };
    for (size_t index = 0; index < CHAR_ARRAY_LENGTH_FOUR; ++index) {
        charArrayFour[index] = table[index];
    }
}

static inline void MakeCharTree(const std::array<uint8_t, CHAR_ARRAY_LENGTH_FOUR>& charArrayFour,
    std::array<uint8_t, CHAR_ARRAY_LENGTH_THREE>& charArrayThree)
{
    const uint8_t table[CHAR_ARRAY_LENGTH_THREE] = {
        static_cast<uint8_t>((charArrayFour[BASE64_DECODE_INDEX0] << BASE64_DECODE_OFFSET2) +
                             ((charArrayFour[BASE64_DECODE_INDEX1] & BASE64_DECODE_MASK1) >> BASE64_DECODE_OFFSET4)),
        static_cast<uint8_t>(((charArrayFour[BASE64_DECODE_INDEX1] & BASE64_DECODE_MASK2) << BASE64_DECODE_OFFSET4) +
                             ((charArrayFour[BASE64_DECODE_INDEX2] & BASE64_DECODE_MASK3) >> BASE64_DECODE_OFFSET2)),
        static_cast<uint8_t>(((charArrayFour[BASE64_DECODE_INDEX2] & BASE64_DECODE_MASK4) << BASE64_DECODE_OFFSET6) +
                             charArrayFour[BASE64_DECODE_INDEX3]),
    };
    for (size_t index = 0; index < CHAR_ARRAY_LENGTH_THREE; ++index) {
        charArrayThree[index] = table[index];
    }
}

std::string Encode(const std::string& source)
{
    auto it = source.begin();
    std::string ret;
    size_t index = 0;
    std::array<uint8_t, CHAR_ARRAY_LENGTH_THREE> charArrayThree = { 0 };
    std::array<uint8_t, CHAR_ARRAY_LENGTH_FOUR> charArrayFour = { 0 };

    while (it != source.end()) {
        charArrayThree[index] = *it;
        ++index;
        ++it;
        if (index != CHAR_ARRAY_LENGTH_THREE) {
            continue;
        }
        MakeCharFour(charArrayThree, charArrayFour);
        std::for_each(charArrayFour.begin(), charArrayFour.end(), [&ret](uint8_t idx) { ret += BASE64_CHARS[idx]; });
        index = 0;
    }
    if (index == 0) {
        return ret;
    }

    for (auto i = index; i < CHAR_ARRAY_LENGTH_THREE; ++i) {
        charArrayThree[i] = 0;
    }
    MakeCharFour(charArrayThree, charArrayFour);

    for (size_t i = 0; i < index + 1; ++i) {
        ret += BASE64_CHARS[charArrayFour[i]];
    }

    while (index < CHAR_ARRAY_LENGTH_THREE) {
        ret += '=';
        ++index;
    }
    return ret;
}

std::string Decode(const std::string& encoded)
{
    auto it = encoded.begin();
    size_t index = 0;
    std::array<uint8_t, CHAR_ARRAY_LENGTH_THREE> charArrayThree = { 0 };
    std::array<uint8_t, CHAR_ARRAY_LENGTH_FOUR> charArrayFour = { 0 };
    std::string ret;

    while (it != encoded.end() && IsBase64Char(*it)) {
        charArrayFour[index] = *it;
        ++index;
        ++it;
        if (index != CHAR_ARRAY_LENGTH_FOUR) {
            continue;
        }
        for (index = 0; index < CHAR_ARRAY_LENGTH_FOUR; ++index) {
            charArrayFour[index] = BASE64_CHARS.find(static_cast<char>(charArrayFour[index]));
        }
        MakeCharTree(charArrayFour, charArrayThree);
        std::for_each(
            charArrayThree.begin(), charArrayThree.end(), [&ret](uint8_t idx) { ret += static_cast<char>(idx); });
        index = 0;
    }
    if (index == 0) {
        return ret;
    }

    for (auto i = index; i < CHAR_ARRAY_LENGTH_FOUR; ++i) {
        charArrayFour[i] = 0;
    }
    for (unsigned char& i : charArrayFour) {
        std::string::size_type idx = BASE64_CHARS.find(static_cast<char>(i));
        if (idx != std::string::npos) {
            i = static_cast<unsigned char>(idx);
        }
    }
    MakeCharTree(charArrayFour, charArrayThree);

    for (size_t i = 0; i < index - 1; i++) {
        ret += static_cast<char>(charArrayThree[i]);
    }
    return ret;
}
} // namespace Base64

std::shared_ptr<NetProxyEventCallbackAdapter> NetProxyAdapterImpl::cb_ = nullptr;
CommonEvent_SubscribeInfo *NetProxyAdapterImpl::commonEventSubscribeInfo_= nullptr;
CommonEvent_Subscriber *NetProxyAdapterImpl::commonEventSubscriber_= nullptr;

NetProxyAdapterImpl& NetProxyAdapterImpl::GetInstance()
{
    static NetProxyAdapterImpl instance;
    return instance;
}

NetProxyAdapterImpl::~NetProxyAdapterImpl() 
{
    if (commonEventSubscriber_ != nullptr) {
        OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
    }
    if (commonEventSubscribeInfo_ != nullptr) {
        OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
    }
    commonEventSubscribeInfo_ = nullptr;
    commonEventSubscriber_ = nullptr;
}

void NetProxyAdapterImpl::RegNetProxyEvent(std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback)
{
    WVLOG_I("reg netproxy event");
    cb_ = std::move(eventCallback);
    if (!cb_) {
        WVLOG_E("reg netproxy event, callback is null");
    }
}

void NetProxyAdapterImpl::Changed()
{
    if (listen_) {
        return;
    }
    WVLOG_I("start NetProxy changed");
    listen_ = true;

    std::string host;
    uint16_t port;
    std::string pacUrl;
    std::vector<std::string> exclusionList;
    std::string exclusion;

    GetProperty(host, port, pacUrl, exclusion);
    exclusionList.push_back(exclusion);
    if (cb_) {
        cb_->Changed(host, port, "", exclusionList);
    }
}

bool NetProxyAdapterImpl::StartListen()
{
    WVLOG_I("start netproxy listen");
    Changed();
    if (!cb_) {
        WVLOG_E("start netproxy listen, callback is null");
        return false;
    }
    if (commonEventSubscriber_ == NULL) {
        const char *events[] = {
            COMMON_EVENT_HTTP_PROXY_CHANGE,
        };

        int count = sizeof(events) / sizeof(events[0]);
        commonEventSubscribeInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
        if (commonEventSubscribeInfo_ == NULL) {
            WVLOG_E("Create SubscribeInfo failed.");
            return false;
        }

        commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(commonEventSubscribeInfo_,
            OnReceiveEvent);
        if (commonEventSubscriber_ == NULL) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
            commonEventSubscribeInfo_ = NULL;
            WVLOG_E("Create Subscriber failed.");
            return false;
        }

        CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
        if (ret != COMMONEVENT_ERR_OK) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
            OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
            commonEventSubscribeInfo_ = NULL;
            commonEventSubscriber_ = NULL;
            WVLOG_E("Subscribe failed. ret = %{public}d", ret);
            return false;
        }
    }

    StartListenAppProxy();
    return true;
}

void NetProxyAdapterImpl::AppProxyChange(NetConn_HttpProxy *receiveHttpProxy)
{
    if (receiveHttpProxy == nullptr) {
        WVLOG_E("receiveHttpProxy null.");
        return;
    }
    WVLOG_D("App netproxy config change, receive host is %{public}s, port is %{public}d.",
        receiveHttpProxy->host, receiveHttpProxy->port);

    NetConn_HttpProxy *httpProxy = receiveHttpProxy;
    std::string host;
    host.assign(httpProxy->host);
    if (host == EMPTY_HTTP_PROXY_HOST) {
        NetConn_HttpProxy tempHttpProxy;
        int32_t ret = OH_NetConn_GetDefaultHttpProxy(&tempHttpProxy);
        if (ret != 0) {
            WVLOG_E("NetProxyAdapter::OH_NetConn_GetDefaultHttpProxy failed, errorCode = %{public}d", ret);
            return;
        }
        httpProxy = &tempHttpProxy;
        host.assign(httpProxy->host);
    }

    if (host == DEFAULT_HTTP_PROXY_HOST) {
        WVLOG_W("get netproxy property failed, host is null.");
        host = std::string();
    }

    std::vector<std::string> exclusionList;
    for (int i = 0; i < httpProxy->exclusionListSize; i++) {
        exclusionList.push_back(httpProxy->exclusionList[i]);
    }

    uint16_t port = httpProxy->port;
    for (auto it : exclusionList) {
        WVLOG_D("App netproxy config change, exclusion is %{public}s.", it.c_str());
    }
    if (cb_) {
        WVLOG_D("App netproxy config change, host is %{public}s, port is %{public}d.", host.c_str(), port);
        cb_->Changed(host, port, "", exclusionList);
    }
}

void NetProxyAdapterImpl::StartListenAppProxy()
{
    uint32_t callbackId;
    int32_t res = OH_NetConn_RegisterAppHttpProxyCallback(AppProxyChange, &callbackId);
    if (res == 0) {
        appProxyCallbackId_ = callbackId;
    }
    WVLOG_D("App netproxy config change, fromNet , appId is %{public}d.", appProxyCallbackId_);
}

void NetProxyAdapterImpl::StopListen()
{
    WVLOG_I("stop netproxy listen");
    if (commonEventSubscriber_) {
        CommonEvent_ErrCode errorCode = OH_CommonEvent_UnSubscribe(commonEventSubscriber_);
        if (errorCode != COMMONEVENT_ERR_OK) {
            WVLOG_E("stop netproxy listen, unsubscribe common event failed");
        }
        if (commonEventSubscriber_ != nullptr) {
           OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
        }
        if (commonEventSubscribeInfo_ != nullptr) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
        }
        commonEventSubscribeInfo_ = nullptr;
        commonEventSubscriber_ = nullptr;
    }

    WVLOG_D("App netproxy,UnregisterAppHttpProxyCallback, appId is %{public}d.", appProxyCallbackId_);
    OH_NetConn_UnregisterAppHttpProxyCallback(appProxyCallbackId_);
}

void NetProxyAdapterImpl::OnReceiveEvent(const CommonEvent_RcvData *data)
{
    if (strcmp(OH_CommonEvent_GetEventFromRcvData(data), COMMON_EVENT_HTTP_PROXY_CHANGE)) {
        return;
    }

    NetConn_HttpProxy httpProxy;
    int32_t ret = OH_NetConn_GetDefaultHttpProxy(&httpProxy);
    if (ret != 0) {
        WVLOG_E("NetProxyAdapter::OH_NetConn_GetDefaultHttpProxy failed, errorCode = %{public}d", ret);
        return;
    }

    std::string host;
    uint16_t port;
    std::vector<std::string> exclusionList;
    host.assign(httpProxy.host);
    port = httpProxy.port;
    for (int i = 0; i < httpProxy.exclusionListSize; i++) {
        exclusionList.push_back(httpProxy.exclusionList[i]);
    }
    cb_->Changed(host, port, "", exclusionList);
}

void NetProxyAdapterImpl::GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion)
{
    std::string httpProxyExclusions;
    NetConn_HttpProxy httpProxy;
    int32_t ret = OH_NetConn_GetDefaultHttpProxy(&httpProxy);
    if (ret != 0) {
        WVLOG_E("NetProxyAdapter::OH_NetConn_GetDefaultHttpProxy failed, errorCode = %{public}d", ret);
        return;
    }

    host = httpProxy.host;
    if (host == DEFAULT_HTTP_PROXY_HOST) {
        WVLOG_E("get netproxy property failed, host is null");
        host = std::string();
    }

    for (int i = 0; i < httpProxy.exclusionListSize; i++) {
        httpProxyExclusions.append(httpProxy.exclusionList[i]);
        httpProxyExclusions.append(",");
    }

    if (!httpProxyExclusions.empty()) {
        httpProxyExclusions.pop_back();
    }

    exclusion = httpProxyExclusions;
    if (exclusion == DEFAULT_HTTP_PROXY_EXCLUSION_LIST) {
        WVLOG_E("get netproxy property failed, exclusion is null");
        exclusion = std::string();
    }

    port = httpProxy.port;

    WVLOG_D("get netproxy property, host is %{public}s, port is %{public}d, exclusion is %{public}s", host.c_str(),
        port, exclusion.c_str());
}
} // namespace OHOS::NWeb
