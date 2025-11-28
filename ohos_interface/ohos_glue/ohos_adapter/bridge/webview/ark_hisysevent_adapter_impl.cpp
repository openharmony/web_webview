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

#include "ohos_adapter/bridge/ark_hisysevent_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkHiSysEventAdapterImpl::ArkHiSysEventAdapterImpl(NWeb::HiSysEventAdapter& ref) : real_(ref) {}

int ArkHiSysEventAdapterImpl::Write(
    const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::tuple data = std::make_tuple(s_key1, s_value1);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::tuple data = std::make_tuple(s_key1, s_value1, s_key2, s_value2);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::tuple data = std::make_tuple(s_key1, s_value1, s_key2, s_value2, s_key3, s_value3);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::tuple data = std::make_tuple(s_key1, s_value1, s_key2, s_value2, s_key3, s_value3, s_key4, s_value4);
    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4, const ArkWebString key5,
    const ArkWebString value5)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_value5 = ArkWebStringStructToClass(value5);
    std::tuple data =
        std::make_tuple(s_key1, s_value1, s_key2, s_value2, s_key3, s_value3, s_key4, s_value4, s_key5, s_value5);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4, const ArkWebString key5,
    const ArkWebString value5, const ArkWebString key6, const ArkWebString value6)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_value5 = ArkWebStringStructToClass(value5);
    std::string s_key6 = ArkWebStringStructToClass(key6);
    std::string s_value6 = ArkWebStringStructToClass(value6);
    std::tuple data = std::make_tuple(
        s_key1, s_value1, s_key2, s_value2, s_key3, s_value3, s_key4, s_value4, s_key5, s_value5, s_key6, s_value6);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const int64_t value1, const ArkWebString key2, const int value2, const ArkWebString key3,
    const ArkWebUint16Vector value3, const ArkWebString key4, const int value4)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::vector<uint16_t> v_value3 = ArkWebBasicVectorStructToClass<uint16_t, ArkWebUint16Vector>(value3);
    std::tuple data = std::make_tuple(s_key1, value1, s_key2, value2, s_key3, v_value3, s_key4, value4);
    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const int64_t value1, const ArkWebString key2, const int64_t value2, const ArkWebString key3, const int value3,
    const ArkWebString key4, const int value4, const ArkWebString key5, const int64_t value5, const ArkWebString key6,
    const int value6)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_key6 = ArkWebStringStructToClass(key6);
    std::tuple data =
        std::make_tuple(s_key1, value1, s_key2, value2, s_key3, value3, s_key4, value4, s_key5, value5, s_key6, value6);
    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const int64_t value1, const ArkWebString key2, const int64_t value2, const ArkWebString key3, const uint32_t value3,
    const ArkWebString key4, const int64_t value4, const ArkWebString key5, const int64_t value5,
    const ArkWebString key6, const int64_t value6, const ArkWebString key7, const int64_t value7,
    const ArkWebString key8, const int64_t value8, const ArkWebString key9, const int64_t value9,
    const ArkWebString key10, const int64_t value10, const ArkWebString key11, const int64_t value11,
    const ArkWebString key12, const int64_t value12, const ArkWebString key13, const int64_t value13,
    const ArkWebString key14, const int64_t value14, const ArkWebString key15, const int64_t value15,
    const ArkWebString key16, const int64_t value16, const ArkWebString key17, const int64_t value17,
    const ArkWebString key18, const int64_t value18, const ArkWebString key19, const int64_t value19,
    const ArkWebString key20, const int64_t value20, const ArkWebString key21, const int64_t value21,
    const ArkWebString key22, const int64_t value22, const ArkWebString key23, const int64_t value23)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_key6 = ArkWebStringStructToClass(key6);
    std::string s_key7 = ArkWebStringStructToClass(key7);
    std::string s_key8 = ArkWebStringStructToClass(key8);
    std::string s_key9 = ArkWebStringStructToClass(key9);
    std::string s_key10 = ArkWebStringStructToClass(key10);
    std::string s_key11 = ArkWebStringStructToClass(key11);
    std::string s_key12 = ArkWebStringStructToClass(key12);
    std::string s_key13 = ArkWebStringStructToClass(key13);
    std::string s_key14 = ArkWebStringStructToClass(key14);
    std::string s_key15 = ArkWebStringStructToClass(key15);
    std::string s_key16 = ArkWebStringStructToClass(key16);
    std::string s_key17 = ArkWebStringStructToClass(key17);
    std::string s_key18 = ArkWebStringStructToClass(key18);
    std::string s_key19 = ArkWebStringStructToClass(key19);
    std::string s_key20 = ArkWebStringStructToClass(key20);
    std::string s_key21 = ArkWebStringStructToClass(key21);
    std::string s_key22 = ArkWebStringStructToClass(key22);
    std::string s_key23 = ArkWebStringStructToClass(key23);
    std::tuple data = std::make_tuple(s_key1, value1, s_key2, value2, s_key3, value3, s_key4, value4, s_key5, value5,
        s_key6, value6, s_key7, value7, s_key8, value8, s_key9, value9, s_key10, value10, s_key11, value11, s_key12,
        value12, s_key13, value13, s_key14, value14, s_key15, value15, s_key16, value16, s_key17, value17, s_key18,
        value18, s_key19, value19, s_key20, value20, s_key21, value21, s_key22, value22, s_key23, value23);
    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4, const ArkWebString key5,
    const ArkWebString value5, const ArkWebString key6, const ArkWebString value6, const ArkWebString key7,
    const ArkWebString value7)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_value5 = ArkWebStringStructToClass(value5);
    std::string s_key6 = ArkWebStringStructToClass(key6);
    std::string s_value6 = ArkWebStringStructToClass(value6);
    std::string s_key7 = ArkWebStringStructToClass(key7);
    std::string s_value7 = ArkWebStringStructToClass(value7);
    std::tuple data = std::make_tuple(s_key1, s_value1, s_key2, s_value2, s_key3, s_value3, s_key4, s_value4, s_key5,
        s_value5, s_key6, s_value6, s_key7, s_value7);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const ArkWebString value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4, const ArkWebString key5,
    const ArkWebString value5, const ArkWebString key6, const ArkWebString value6, const ArkWebString key7,
    const ArkWebString value7, const ArkWebString key8, const ArkWebString value8)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_value1 = ArkWebStringStructToClass(value1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::string s_value5 = ArkWebStringStructToClass(value5);
    std::string s_key6 = ArkWebStringStructToClass(key6);
    std::string s_value6 = ArkWebStringStructToClass(value6);
    std::string s_key7 = ArkWebStringStructToClass(key7);
    std::string s_value7 = ArkWebStringStructToClass(value7);
    std::string s_key8 = ArkWebStringStructToClass(key8);
    std::string s_value8 = ArkWebStringStructToClass(value8);
    std::tuple data = std::make_tuple(s_key1, s_value1, s_key2, s_value2, s_key3, s_value3, s_key4, s_value4, s_key5,
        s_value5, s_key6, s_value6, s_key7, s_value7, s_key8, s_value8);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const uint32_t value1, const ArkWebString key2, const uint64_t value2)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::tuple data = std::make_tuple(s_key1, value1, s_key2, value2);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}

int ArkHiSysEventAdapterImpl::Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
    const int32_t value1, const ArkWebString key2, const ArkWebString value2, const ArkWebString key3,
    const ArkWebString value3, const ArkWebString key4, const ArkWebString value4, const ArkWebString key5,
    const int32_t value5)
{
    std::string s_eventName = ArkWebStringStructToClass(eventName);
    std::string s_key1 = ArkWebStringStructToClass(key1);
    std::string s_key2 = ArkWebStringStructToClass(key2);
    std::string s_value2 = ArkWebStringStructToClass(value2);
    std::string s_key3 = ArkWebStringStructToClass(key3);
    std::string s_value3 = ArkWebStringStructToClass(value3);
    std::string s_key4 = ArkWebStringStructToClass(key4);
    std::string s_value4 = ArkWebStringStructToClass(value4);
    std::string s_key5 = ArkWebStringStructToClass(key5);
    std::tuple data = std::make_tuple(s_key1, value1, s_key2, s_value2, s_key3, s_value3,
                                      s_key4, s_value4, s_key5, value5);

    return real_.Write(s_eventName, (OHOS::NWeb::HiSysEventAdapter::EventType)type, data);
}
} // namespace OHOS::ArkWeb
