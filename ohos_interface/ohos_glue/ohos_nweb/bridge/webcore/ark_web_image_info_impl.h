/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_IMAGE_INFO_IMPL_H_
#define ARK_WEB_IMAGE_INFO_IMPL_H_
 
#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_image_info.h"
 
#include "base/include/ark_web_types.h"
 
namespace OHOS::ArkWeb {
 
class ArkWebImageInfoImpl : public ArkWebImageInfo {
    IMPLEMENT_REFCOUNTING(ArkWebImageInfoImpl);
public:
    ArkWebImageInfoImpl(std::shared_ptr<OHOS::NWeb::NWebImageInfo> nweb_image_info);
    ~ArkWebImageInfoImpl() = default;
 
    int32_t GetWidth() override;
 
    int32_t GetHeight() override;
 
    int32_t GetAlphaType() override;
 
    int32_t GetColorType() override;
 
    uint32_t *GetData() override;
 
    uint32_t GetDataSize() override;
 
    void SetWidth(int32_t width) override;
 
    void SetHeight(int32_t height) override;
 
    void SetAlphaType(int32_t alphaType) override;
 
    void SetColorType(int32_t colorType) override;
 
    void SetData(uint32_t *data) override;
 
    void SetDataSize(uint32_t dataSize) override;
 
private:
    std::shared_ptr<OHOS::NWeb::NWebImageInfo> real_;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_IMAGE_INFO_IMPL_H_
