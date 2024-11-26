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

#ifndef ARK_WEB_CPPTOC_REF_COUNTED_H_
#define ARK_WEB_CPPTOC_REF_COUNTED_H_
#pragma once

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"
#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_bridge_types.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

template<class ClassName, class BaseName, class StructName>
class ArkWebCppToCRefCounted : public ArkWebBaseRefCounted {
public:
    ArkWebCppToCRefCounted(const ArkWebCppToCRefCounted&) = delete;
    ArkWebCppToCRefCounted& operator=(const ArkWebCppToCRefCounted&) = delete;

    static ArkWebRefPtr<BaseName> Get(StructName* s);

    static StructName* Invert(ArkWebRefPtr<BaseName> c);

    static ArkWebRefPtr<BaseName> Revert(StructName* s);

    StructName* GetStruct();

    void IncreRef() const;

    void DecreRef() const;

protected:
    ArkWebCppToCRefCounted();
    virtual ~ArkWebCppToCRefCounted() = default;

private:
    struct BridgeStruct {
        ArkWebBridgeType type_;
        BaseName* class_;
        ArkWebCppToCRefCounted<ClassName, BaseName, StructName>* bridge_;
        StructName struct_;
    };

    static BridgeStruct* GetBridgeStruct(StructName* s);

    static void StructIncreRef(ark_web_base_ref_counted_t* base);

    static void StructDecreRef(ark_web_base_ref_counted_t* base);

    ArkWebRefCount ref_count_;
    BridgeStruct bridge_struct_;

    static inline ArkWebBridgeType kBridgeType;
};

template<class ClassName, class BaseName, class StructName>
ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::ArkWebCppToCRefCounted()
{
    bridge_struct_.type_ = kBridgeType;
    bridge_struct_.bridge_ = this;
    memset(GetStruct(), 0, sizeof(StructName));

    ark_web_base_ref_counted_t* base = reinterpret_cast<ark_web_base_ref_counted_t*>(GetStruct());
    if (base) {
        base->size = sizeof(StructName);
        base->incre_ref = StructIncreRef;
        base->decre_ref = StructDecreRef;
    }
}

template<class ClassName, class BaseName, class StructName>
ArkWebRefPtr<BaseName> ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::Get(StructName* s)
{
    BridgeStruct* bridgeStruct = GetBridgeStruct(s);
    if (!bridgeStruct) {
        ARK_WEB_CPPTOC_INFO_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    return bridgeStruct->class_;
}

template<class ClassName, class BaseName, class StructName>
StructName* ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::Invert(ArkWebRefPtr<BaseName> c)
{
    if (!c.get()) {
        ARK_WEB_CPPTOC_DEBUG_LOG("base class is null,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    ClassName* bridge = new ClassName();
    if (!bridge) {
        ARK_WEB_CPPTOC_WARN_LOG("failed to new class,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }
    StructName* capi_struct = bridge->GetStruct();

    ARK_WEB_CPPTOC_DV_LOG("bridge type is %{public}d,this is %{public}ld,capi "
                          "struct is %{public}ld,class is %{public}ld",
        kBridgeType, (long)bridge, (long)capi_struct, (long)c.get());

    bridge->bridge_struct_.class_ = c.get();
    bridge->IncreRef();
    return capi_struct;
}

template<class ClassName, class BaseName, class StructName>
ArkWebRefPtr<BaseName> ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::Revert(StructName* s)
{
    BridgeStruct* bridgeStruct = GetBridgeStruct(s);
    if (!bridgeStruct) {
        ARK_WEB_CPPTOC_DEBUG_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    ARK_WEB_CPPTOC_DV_LOG("bridge type is %{public}d,this is %{public}ld,capi "
                          "struct is %{public}ld",
        kBridgeType, (long)bridgeStruct->bridge_, (long)s);

    ArkWebRefPtr<BaseName> objectPtr(bridgeStruct->class_);
    bridgeStruct->bridge_->DecreRef();
    return objectPtr;
}

template<class ClassName, class BaseName, class StructName>
StructName* ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::GetStruct()
{
    return &bridge_struct_.struct_;
}

template<class ClassName, class BaseName, class StructName>
typename ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::BridgeStruct*
ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::GetBridgeStruct(StructName* s)
{
    if (!s) {
        ARK_WEB_CPPTOC_DEBUG_LOG("capi struct is null,bridge type is %{public}d", kBridgeType);
        return nullptr;
    }

    BridgeStruct* bridgeStruct =
        reinterpret_cast<BridgeStruct*>(reinterpret_cast<char*>(s) - (sizeof(BridgeStruct) - sizeof(StructName)));
    if (bridgeStruct->type_ != kBridgeType) {
        ARK_WEB_CPPTOC_INFO_LOG("bridge type %{public}d - %{public}d is invalid", bridgeStruct->type_, kBridgeType);
        return nullptr;
    }

    return bridgeStruct;
}

template<class ClassName, class BaseName, class StructName>
void ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::IncreRef() const
{
    ARK_WEB_CPPTOC_REF_LOG("bridge type is %{public}d,ref count is %{public}d,this is %{public}ld", kBridgeType,
        ref_count_.GetRefCount(), (long)this);

    bridge_struct_.class_->IncreRef();

    ref_count_.IncreRef();
}

template<class ClassName, class BaseName, class StructName>
void ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::DecreRef() const
{
    ARK_WEB_CPPTOC_REF_LOG("bridge type is %{public}d,ref count is %{public}d,this is %{public}ld", kBridgeType,
        ref_count_.GetRefCount(), (long)this);

    bridge_struct_.class_->DecreRef();

    if (ref_count_.DecreRef()) {
        ARK_WEB_CPPTOC_DV_LOG("delete cpptoc,bridge type is %{public}d,this is %{public}ld", kBridgeType, (long)this);

        delete this;
    }
}

template<class ClassName, class BaseName, class StructName>
void ARK_WEB_CALLBACK ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::StructIncreRef(
    ark_web_base_ref_counted_t* base)
{
    ARK_WEB_CPPTOC_REF_LOG("bridge type is %{public}d", kBridgeType);

    BridgeStruct* bridgeStruct = GetBridgeStruct(reinterpret_cast<StructName*>(base));
    if (!bridgeStruct) {
        ARK_WEB_CPPTOC_INFO_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return;
    }

    bridgeStruct->bridge_->IncreRef();
}

template<class ClassName, class BaseName, class StructName>
void ARK_WEB_CALLBACK ArkWebCppToCRefCounted<ClassName, BaseName, StructName>::StructDecreRef(
    ark_web_base_ref_counted_t* base)
{
    ARK_WEB_CPPTOC_REF_LOG("bridge type is %{public}d", kBridgeType);

    BridgeStruct* bridgeStruct = GetBridgeStruct(reinterpret_cast<StructName*>(base));
    if (!bridgeStruct) {
        ARK_WEB_CPPTOC_INFO_LOG("failed to get bridge struct,bridge type is %{public}d", kBridgeType);
        return;
    }

    bridgeStruct->bridge_->DecreRef();
}

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CPPTOC_REF_COUNTED_H_
