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

#ifndef AUDIO_AVCENC_INFO_ADAPTER_IMPL_H
#define AUDIO_AVCENC_INFO_ADAPTER_IMPL_H

#include "audio_cenc_info_adapter.h"

#include <vector>
#include <cstdint>

#include "nweb_log.h"

namespace OHOS::NWeb {

class AudioCencInfoAdapterImpl : public AudioCencInfoAdapter {
public:
    AudioCencInfoAdapterImpl() = default;

    ~AudioCencInfoAdapterImpl() override;

    uint8_t* GetKeyId() override;

    uint32_t GetKeyIdLen() override;

    uint8_t* GetIv() override;

    uint32_t GetIvLen() override;

    uint32_t GetAlgo() override;

    uint32_t GetEncryptedBlockCount() override;

    uint32_t GetSkippedBlockCount() override;

    uint32_t GetFirstEncryptedOffset() override;

    std::vector<uint32_t> GetClearHeaderLens() override;

    std::vector<uint32_t> GetPayLoadLens() override;

    uint32_t GetMode() override;

    void SetKeyId(uint8_t* keyId) override;

    void SetKeyIdLen(uint32_t keyIdLen) override;

    void SetIv(uint8_t* iv) override;

    void SetIvLen(uint32_t ivLen) override;

    void SetAlgo(uint32_t algo) override;

    void SetEncryptedBlockCount(uint32_t count) override;

    void SetSkippedBlockCount(uint32_t count) override;

    void SetFirstEncryptedOffset(uint32_t offset) override;

    void SetClearHeaderLens(const std::vector<uint32_t>& lens) override;

    void SetPayLoadLens(const std::vector<uint32_t>& lens) override;

    void SetMode(uint32_t mode) override;

    static void PrintCencInfoData(std::shared_ptr<AudioCencInfoAdapter> cencInfo)
    {
        if (cencInfo) {
            WVLOG_I("AudioCodecDecoder PrintFormatData:algo[%{public}u], encryptedBlockCount[%{public}u],"
                    "skippedBlockCount[%{public}u], firstEncryptedOffset[%{public}u], mode[%{public}u],",
                    cencInfo->GetAlgo(), cencInfo->GetEncryptedBlockCount(), cencInfo->GetSkippedBlockCount(),
                    cencInfo->GetFirstEncryptedOffset(), cencInfo->GetMode());
        }
    }
private:
    uint8_t* keyId_;
    uint32_t keyIdLen_;
    uint8_t* iv_;
    uint32_t ivLen_;
    uint32_t algo_;
    uint32_t encryptedBlockCount_;
    uint32_t skippedBlockCount_;
    uint32_t firstEncryptedOffset_;
    std::vector<uint32_t> clearHeaderLens_;
    std::vector<uint32_t> payLoadLens_;
    uint32_t mode_;
};

} // namespace OHOS::NWeb

#endif // AUDIO_AVCENC_INFO_ADAPTER_IMPL_H