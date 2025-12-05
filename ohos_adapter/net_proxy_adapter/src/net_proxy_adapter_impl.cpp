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
#include "parameter.h"

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

NetProxyEventSubscriber::NetProxyEventSubscriber(
    EventFwk::CommonEventSubscribeInfo& in, std::shared_ptr<NetProxyEventCallbackAdapter> cb)
    : EventFwk::CommonEventSubscriber(in), eventCallback_(cb)
{}

NetProxyAdapterImpl& NetProxyAdapterImpl::GetInstance()
{
    static NetProxyAdapterImpl instance;
    return instance;
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
    if (commonEventSubscriber_) {
        return true;
    }
    WVLOG_I("start netproxy listen");
    Changed();
    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    skill.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE);
    EventFwk::CommonEventSubscribeInfo info(skill);
    info.SetPriority(1); //The higher the value, the higher the priority
    if (!cb_) {
        WVLOG_E("start netproxy listen, callback is null");
        return false;
    }
    commonEventSubscriber_ = std::make_shared<NetProxyEventSubscriber>(info, cb_);
    if (!commonEventSubscriber_) {
        WVLOG_E("start netproxy listen, common event subscriber is null");
        return false;
    }

    bool ret = EventFwk::CommonEventManager::SubscribeCommonEvent(commonEventSubscriber_);
    if (ret == false) {
        WVLOG_E("start netproxy listen, subscribe common event failed");
        return false;
    }

    StartListenAppProxy();
    return true;
}

void NetProxyAdapterImpl::StartListenAppProxy()
{
    std::function<void(const NetManagerStandard::HttpProxy& httpProxy)> callback =
        [this](const NetManagerStandard::HttpProxy& receiveHttpProxy) {
            NetManagerStandard::HttpProxy httpProxy = receiveHttpProxy;
            WVLOG_D("App netproxy config change, receive host is %{public}s, port is %{public}d",
                httpProxy.GetHost().c_str(), httpProxy.GetPort());
            std::string host;
            host.assign(httpProxy.GetHost());
            if (host == EMPTY_HTTP_PROXY_HOST) {
                WVLOG_D("App netproxy config change, clear proxy");
                NetManagerStandard::HttpProxy tempHttpProxy;
                int32_t ret = NetManagerStandard::NetConnClient::GetInstance().GetDefaultHttpProxy(tempHttpProxy);
                if (ret != NetManagerStandard::NET_CONN_SUCCESS) {
                    WVLOG_E("App netproxy config change, get default http proxy from OH network failed");
                    return;
                }
                WVLOG_D("App netproxy config clear, GetDefaultHttpProxy host is %{public}s, port is %{public}d",
                    tempHttpProxy.GetHost().c_str(), tempHttpProxy.GetPort());
                httpProxy = tempHttpProxy;
                host.assign(httpProxy.GetHost());
            }

            if (host == DEFAULT_HTTP_PROXY_HOST) {
                WVLOG_W("get netproxy property failed, host is null");
                host = std::string();
            }
            std::vector<std::string> exclusionList;
            auto exclusion = httpProxy.GetExclusionList();
            exclusionList.assign(exclusion.begin(), exclusion.end());
            uint16_t port = httpProxy.GetPort();

            for (auto it : exclusionList) {
                WVLOG_D("App netproxy config change, exclusion is %{public}s", it.c_str());
            }
            if (cb_) {
                WVLOG_D("App netproxy config change, host is %{public}s, port is %{public}d", host.c_str(), port);
                cb_->Changed(host, port, "", exclusionList);
            }
        };
    uint32_t appId;
    NetManagerStandard::NetConnClient::GetInstance().RegisterAppHttpProxyCallback(callback, appId);
    appProxyCallbackId_ = appId;
    WVLOG_D("App netproxy config change, fromNet , appId =is %{public}d", appId);
}

void NetProxyAdapterImpl::StopListen()
{
    WVLOG_I("stop netproxy listen");
    if (commonEventSubscriber_) {
        bool result = EventFwk::CommonEventManager::UnSubscribeCommonEvent(commonEventSubscriber_);
        if (result) {
            commonEventSubscriber_ = nullptr;
        } else {
            WVLOG_E("stop netproxy listen, unsubscribe common event failed");
        }
    }

    WVLOG_D("App netproxy,UnregisterAppHttpProxyCallback, appId is %{public}d", appProxyCallbackId_);
    NetManagerStandard::NetConnClient::GetInstance().UnregisterAppHttpProxyCallback(appProxyCallbackId_);
}

void NetProxyEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    const std::string action = data.GetWant().GetAction();
    WVLOG_D("netproxy config change, netproxy action: %{public}s", action.c_str());
    if (action != EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE) {
        WVLOG_E("netproxy config change, action error, action is %{public}s", action.c_str());
        return;
    }

    NetManagerStandard::HttpProxy httpProxy;
    int32_t ret = NetManagerStandard::NetConnClient::GetInstance().GetDefaultHttpProxy(httpProxy);
    if (ret != NetManagerStandard::NET_CONN_SUCCESS) {
        WVLOG_E("netproxy config change, get default http proxy from OH network failed");
        return;
    }

    WVLOG_D("netproxy config change, host is %{public}s, port is %{public}d", httpProxy.GetHost().c_str(),
        httpProxy.GetPort());
    for (auto it : httpProxy.GetExclusionList()) {
        WVLOG_D("netproxy config change, exclusion is %{public}s", it.c_str());
    }

    std::string host;
    uint16_t port;
    std::vector<std::string> exclusionList;
    host.assign(httpProxy.GetHost());
    port = httpProxy.GetPort();
    auto exclusion = httpProxy.GetExclusionList();
    exclusionList.assign(exclusion.begin(), exclusion.end());

    if (!eventCallback_) {
        WVLOG_E("netproxy config change, event callback is null");
        return;
    }

    eventCallback_->Changed(host, port, "", exclusionList);
}

void NetProxyAdapterImpl::GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion)
{
    std::string httpProxyExclusions;
    NetManagerStandard::HttpProxy httpProxy;
    int32_t ret = NetManagerStandard::NetConnClient::GetInstance().GetDefaultHttpProxy(httpProxy);
    if (ret != NetManagerStandard::NET_CONN_SUCCESS) {
        WVLOG_E("netproxy config change, get default http proxy from OH network failed");
        return;
    }

    host = httpProxy.GetHost();
    if (host == DEFAULT_HTTP_PROXY_HOST) {
        WVLOG_E("get netproxy property failed, host is null");
        host = std::string();
    }

    for (const auto& s : httpProxy.GetExclusionList()) {
        httpProxyExclusions.append(s + ",");
    }
    if (!httpProxyExclusions.empty()) {
        httpProxyExclusions.pop_back();
    }

    exclusion = httpProxyExclusions;
    if (exclusion == DEFAULT_HTTP_PROXY_EXCLUSION_LIST) {
        WVLOG_E("get netproxy property failed, exclusion is null");
        exclusion = std::string();
    }

    port = httpProxy.GetPort();

    WVLOG_D("get netproxy property, host is %{public}s, port is %{public}d, exclusion is %{public}s", host.c_str(),
        port, exclusion.c_str());
}
} // namespace OHOS::NWeb
