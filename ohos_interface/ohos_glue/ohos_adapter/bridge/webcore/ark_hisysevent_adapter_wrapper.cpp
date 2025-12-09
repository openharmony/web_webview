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

#include "ohos_adapter/bridge/ark_hisysevent_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkHiSysEventAdapterWrapper::ArkHiSysEventAdapterWrapper(ArkWebRefPtr<ArkHiSysEventAdapter> ref) : ctocpp_(ref) {}

int ArkHiSysEventAdapterWrapper::Write(
    const std::string& eventName, ArkEventType type, const std::tuple<const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));

    int result = ctocpp_->Write(
        ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3, ark_value3);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3,
        ark_value3, ark_key4, ark_value4);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_value5 = ArkWebStringClassToStruct(std::get<9>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3,
        ark_value3, ark_key4, ark_value4, ark_key5, ark_value5);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_value5);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_value5 = ArkWebStringClassToStruct(std::get<9>(data));
    ArkWebString ark_key6 = ArkWebStringClassToStruct(std::get<10>(data));
    ArkWebString ark_value6 = ArkWebStringClassToStruct(std::get<11>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3,
        ark_value3, ark_key4, ark_value4, ark_key5, ark_value5, ark_key6, ark_value6);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_value5);
    ArkWebStringStructRelease(ark_key6);
    ArkWebStringStructRelease(ark_value6);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int, const std::string,
        const std::vector<uint16_t>, const std::string, const int>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebUint16Vector ark_value3 = ArkWebBasicVectorClassToStruct<uint16_t, ArkWebUint16Vector>(std::get<5>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, std::get<1>(data), ark_key2,
        std::get<3>(data), ark_key3, ark_value3, ark_key4, std::get<7>(data));

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebBasicVectorStructRelease(ark_value3);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string, const int,
        const std::string, const int, const std::string, const int64_t, const std::string, const int>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_key6 = ArkWebStringClassToStruct(std::get<10>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, std::get<1>(data), ark_key2,
        std::get<3>(data), ark_key3, std::get<5>(data), ark_key4, std::get<7>(data), ark_key5, std::get<9>(data),
        ark_key6, std::get<11>(data));

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_key6);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const uint32_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
        const int64_t, const std::string, const int64_t, const std::string, const int64_t>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_key6 = ArkWebStringClassToStruct(std::get<10>(data));
    ArkWebString ark_key7 = ArkWebStringClassToStruct(std::get<12>(data));
    ArkWebString ark_key8 = ArkWebStringClassToStruct(std::get<14>(data));
    ArkWebString ark_key9 = ArkWebStringClassToStruct(std::get<16>(data));
    ArkWebString ark_key10 = ArkWebStringClassToStruct(std::get<18>(data));
    ArkWebString ark_key11 = ArkWebStringClassToStruct(std::get<20>(data));
    ArkWebString ark_key12 = ArkWebStringClassToStruct(std::get<22>(data));
    ArkWebString ark_key13 = ArkWebStringClassToStruct(std::get<24>(data));
    ArkWebString ark_key14 = ArkWebStringClassToStruct(std::get<26>(data));
    ArkWebString ark_key15 = ArkWebStringClassToStruct(std::get<28>(data));
    ArkWebString ark_key16 = ArkWebStringClassToStruct(std::get<30>(data));
    ArkWebString ark_key17 = ArkWebStringClassToStruct(std::get<32>(data));
    ArkWebString ark_key18 = ArkWebStringClassToStruct(std::get<34>(data));
    ArkWebString ark_key19 = ArkWebStringClassToStruct(std::get<36>(data));
    ArkWebString ark_key20 = ArkWebStringClassToStruct(std::get<38>(data));
    ArkWebString ark_key21 = ArkWebStringClassToStruct(std::get<40>(data));
    ArkWebString ark_key22 = ArkWebStringClassToStruct(std::get<42>(data));
    ArkWebString ark_key23 = ArkWebStringClassToStruct(std::get<44>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, std::get<1>(data), ark_key2,
        std::get<3>(data), ark_key3, std::get<5>(data), ark_key4, std::get<7>(data), ark_key5, std::get<9>(data),
        ark_key6, std::get<11>(data), ark_key7, std::get<13>(data), ark_key8, std::get<15>(data), ark_key9,
        std::get<17>(data), ark_key10, std::get<19>(data), ark_key11, std::get<21>(data), ark_key12, std::get<23>(data),
        ark_key13, std::get<25>(data), ark_key14, std::get<27>(data), ark_key15, std::get<29>(data), ark_key16,
        std::get<31>(data), ark_key17, std::get<33>(data), ark_key18, std::get<35>(data), ark_key19, std::get<37>(data),
        ark_key20, std::get<39>(data), ark_key21, std::get<41>(data), ark_key22, std::get<43>(data), ark_key23,
        std::get<45>(data));

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_key6);
    ArkWebStringStructRelease(ark_key7);
    ArkWebStringStructRelease(ark_key8);
    ArkWebStringStructRelease(ark_key9);
    ArkWebStringStructRelease(ark_key10);
    ArkWebStringStructRelease(ark_key11);
    ArkWebStringStructRelease(ark_key12);
    ArkWebStringStructRelease(ark_key13);
    ArkWebStringStructRelease(ark_key14);
    ArkWebStringStructRelease(ark_key15);
    ArkWebStringStructRelease(ark_key16);
    ArkWebStringStructRelease(ark_key17);
    ArkWebStringStructRelease(ark_key18);
    ArkWebStringStructRelease(ark_key19);
    ArkWebStringStructRelease(ark_key20);
    ArkWebStringStructRelease(ark_key21);
    ArkWebStringStructRelease(ark_key22);
    ArkWebStringStructRelease(ark_key23);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_value5 = ArkWebStringClassToStruct(std::get<9>(data));
    ArkWebString ark_key6 = ArkWebStringClassToStruct(std::get<10>(data));
    ArkWebString ark_value6 = ArkWebStringClassToStruct(std::get<11>(data));
    ArkWebString ark_key7 = ArkWebStringClassToStruct(std::get<12>(data));
    ArkWebString ark_value7 = ArkWebStringClassToStruct(std::get<13>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3,
        ark_value3, ark_key4, ark_value4, ark_key5, ark_value5, ark_key6, ark_value6, ark_key7, ark_value7);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_value5);
    ArkWebStringStructRelease(ark_key6);
    ArkWebStringStructRelease(ark_value6);
    ArkWebStringStructRelease(ark_key7);
    ArkWebStringStructRelease(ark_value7);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string,
        const std::string>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_value1 = ArkWebStringClassToStruct(std::get<1>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));
    ArkWebString ark_value5 = ArkWebStringClassToStruct(std::get<9>(data));
    ArkWebString ark_key6 = ArkWebStringClassToStruct(std::get<10>(data));
    ArkWebString ark_value6 = ArkWebStringClassToStruct(std::get<11>(data));
    ArkWebString ark_key7 = ArkWebStringClassToStruct(std::get<12>(data));
    ArkWebString ark_value7 = ArkWebStringClassToStruct(std::get<13>(data));
    ArkWebString ark_key8 = ArkWebStringClassToStruct(std::get<14>(data));
    ArkWebString ark_value8 = ArkWebStringClassToStruct(std::get<15>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, ark_value1, ark_key2, ark_value2, ark_key3,
        ark_value3, ark_key4, ark_value4, ark_key5, ark_value5, ark_key6, ark_value6, ark_key7, ark_value7, ark_key8,
        ark_value8);

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_value1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);
    ArkWebStringStructRelease(ark_key5);
    ArkWebStringStructRelease(ark_value5);
    ArkWebStringStructRelease(ark_key6);
    ArkWebStringStructRelease(ark_value6);
    ArkWebStringStructRelease(ark_key7);
    ArkWebStringStructRelease(ark_value7);
    ArkWebStringStructRelease(ark_key8);
    ArkWebStringStructRelease(ark_value8);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, ArkEventType type,
    const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));

    int result = ctocpp_->Write(
        ark_event_name, (uint32_t)type, ark_key1, std::get<1>(data), ark_key2, std::get<3>(data));

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_key2);

    return result;
}

int ArkHiSysEventAdapterWrapper::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int32_t, const std::string, const std::string,
        const std::string, const std::string, const std::string, const std::string,
        const std::string, const int32_t>& data)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString ark_event_name = ArkWebStringClassToStruct(eventName);
    ArkWebString ark_key1 = ArkWebStringClassToStruct(std::get<0>(data));
    ArkWebString ark_key2 = ArkWebStringClassToStruct(std::get<2>(data));
    ArkWebString ark_value2 = ArkWebStringClassToStruct(std::get<3>(data));
    ArkWebString ark_key3 = ArkWebStringClassToStruct(std::get<4>(data));
    ArkWebString ark_value3 = ArkWebStringClassToStruct(std::get<5>(data));
    ArkWebString ark_key4 = ArkWebStringClassToStruct(std::get<6>(data));
    ArkWebString ark_value4 = ArkWebStringClassToStruct(std::get<7>(data));
    ArkWebString ark_key5 = ArkWebStringClassToStruct(std::get<8>(data));

    int result = ctocpp_->Write(ark_event_name, (uint32_t)type, ark_key1, std::get<1>(data), ark_key2, ark_value2,
        ark_key3, ark_value3, ark_key4, ark_value4, ark_key5, std::get<9>(data));

    ArkWebStringStructRelease(ark_event_name);
    ArkWebStringStructRelease(ark_key1);
    ArkWebStringStructRelease(ark_key2);
    ArkWebStringStructRelease(ark_value2);
    ArkWebStringStructRelease(ark_key3);
    ArkWebStringStructRelease(ark_value3);
    ArkWebStringStructRelease(ark_key4);
    ArkWebStringStructRelease(ark_value4);
    ArkWebStringStructRelease(ark_key5);

    return result;
}

} // namespace OHOS::ArkWeb
