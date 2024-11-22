/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_CTOCPP_REF_COUNTED_H_
#define ARK_WEB_CTOCPP_REF_COUNTED_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_bridge_types.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

template<class ClassName, class BaseName, class StructName>
class ArkWebCToCppRefCounted : public BaseName {
public:
    ArkWebCToCppRefCounted(const ArkWebCToCppRefCounted&) = delete;
    ArkWebCToCppRefCounted& operator=(const ArkWebCToCppRefCounted&) = delete;

    static ArkWebRefPtr<BaseName> Invert(StructName* s);

    static StructName* Revert(ArkWebRefPtr<BaseName> c);

    void IncreRef() const;

    void DecreRef() const;

protected:
    ArkWebCToCppRefCounted() = default;
    virtual ~ArkWebCToCppRefCounted() = default;

    StructName* GetStruct() const;

    void StructIncreRef() const;

    void StructDecreRef() const;

private:
    struct BridgeStruct {
        ArkWebBridgeType type_;
        StructName* struct_;
        ClassName class_;
    };

    static BridgeStruct* GetBridgeStruct(const BaseName* obj);

    ArkWebRefCount ref_count_;

    static inline ArkWebBridgeType kBridgeType;
};

template<class ClassName, class BaseName, class StructName>
ArkWebRefPtr<BaseName> ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::Invert(StructName* s)
{
    if (!s) {
        ARK_WEB_CTOCPP_DEBUG_LOG("capi struct is null,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    BridgeStruct* bridgeStruct = new BridgeStruct;
    if (!bridgeStruct) {
        ARK_WEB_CTOCPP_WARN_LOG("failed to new struct,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    ARK_WEB_CTOCPP_DV_LOG("bridge type is %{public}d,this is %{public}ld,capi "
                          "struct is %{public}ld",
        kBridgeType, (long)&bridgeStruct->class_, (long)s);

    bridgeStruct->type_ = kBridgeType;
    bridgeStruct->struct_ = s;

    ArkWebRefPtr<BaseName> bridgePtr(&bridgeStruct->class_);
    bridgeStruct->class_.StructDecreRef();
    return bridgePtr;
}

template<class ClassName, class BaseName, class StructName>
StructName* ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::Revert(ArkWebRefPtr<BaseName> c)
{
    BridgeStruct* bridgeStruct = GetBridgeStruct(c.get());
    if (!bridgeStruct) {
        ARK_WEB_CTOCPP_DEBUG_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    ARK_WEB_CTOCPP_DV_LOG("bridge type is %{public}d,this is %{public}ld,capi "
                          "struct is %{public}ld,class is %{public}ld",
        kBridgeType, (long)&bridgeStruct->class_, (long)bridgeStruct->struct_, (long)c.get());

    bridgeStruct->class_.StructIncreRef();
    return bridgeStruct->struct_;
}

template<class ClassName, class BaseName, class StructName>
StructName* ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::GetStruct() const
{
    BridgeStruct* bridgeStruct = GetBridgeStruct(this);
    if (!bridgeStruct) {
        ARK_WEB_CTOCPP_INFO_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    return bridgeStruct->struct_;
}

template<class ClassName, class BaseName, class StructName>
typename ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::BridgeStruct*
ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::GetBridgeStruct(const BaseName* obj)
{
    if (!obj) {
        ARK_WEB_CTOCPP_DEBUG_LOG("base class is null,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    BridgeStruct* bridgeStruct = reinterpret_cast<BridgeStruct*>(
        reinterpret_cast<char*>(const_cast<BaseName*>(obj)) - (sizeof(BridgeStruct) - sizeof(ClassName)));
    if (bridgeStruct->type_ != kBridgeType) {
        ARK_WEB_CTOCPP_INFO_LOG("bridge type %{public}d - %{public}d is invalid", bridgeStruct->type_, kBridgeType);
        return nullptr;
    }

    return bridgeStruct;
}

template<class ClassName, class BaseName, class StructName>
void ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::IncreRef() const
{
    ARK_WEB_CTOCPP_REF_LOG("bridge type is %{public}d,ref count is %{public}d,this is %{public}ld", kBridgeType,
        ref_count_.GetRefCount(), (long)this);

    StructIncreRef();

    ref_count_.IncreRef();
}

template<class ClassName, class BaseName, class StructName>
void ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::DecreRef() const
{
    ARK_WEB_CTOCPP_REF_LOG("bridge type is %{public}d,ref count is %{public}d,this is %{public}ld", kBridgeType,
        ref_count_.GetRefCount(), (long)this);

    StructDecreRef();

    if (ref_count_.DecreRef()) {
        ARK_WEB_CTOCPP_DV_LOG("delete ctocpp,bridge type is %{public}d,this is %{public}ld", kBridgeType, (long)this);

        BridgeStruct* bridgeStruct = GetBridgeStruct(this);
        delete bridgeStruct;
    }
}

template<class ClassName, class BaseName, class StructName>
ARK_WEB_NO_SANITIZE void ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::StructIncreRef() const
{
    ARK_WEB_CTOCPP_REF_LOG("bridge type is %{public}d,this is %{public}ld", kBridgeType, (long)this);

    ark_web_base_ref_counted_t* base = reinterpret_cast<ark_web_base_ref_counted_t*>(GetStruct());
    if (base && base->incre_ref) {
        base->incre_ref(base);
    }
}

template<class ClassName, class BaseName, class StructName>
ARK_WEB_NO_SANITIZE void ArkWebCToCppRefCounted<ClassName, BaseName, StructName>::StructDecreRef() const
{
    ARK_WEB_CTOCPP_REF_LOG("bridge type is %{public}d,this is %{public}ld", kBridgeType, (long)this);

    ark_web_base_ref_counted_t* base = reinterpret_cast<ark_web_base_ref_counted_t*>(GetStruct());
    if (base && base->decre_ref) {
        base->decre_ref(base);
    }
}

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CTOCPP_REF_COUNTED_H_
