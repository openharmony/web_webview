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

#include "multimedia/player_framework/native_avcodec_base.h"
#include "multimedia/player_framework/native_avcodec_videodecoder.h"
#include <string>

using namespace std;

void CreateVideoDecoderByMime(const std::string& mimetype)
{
    OH_VideoDecoder_CreateByMime(mimetype.c_str());
}

void CreateVideoDecoderByName(const std::string& name)
{
    OH_VideoDecoder_CreateByName(name.c_str());
}
