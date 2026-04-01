#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""
Bridge 文件自动生成脚本
主要特性:
- 支持配置文件模式生成
- 支持嵌套类（Listener、Callback、Handler 等）
- 支持 Adapter 和 NWeb 双系统生成
- 通用类型转换（string、vector、shared_ptr、enum）
- 支持 ArkWeb 基类引用计数管理
使用方法:
    python bridge_gen_files.py --config config.json --output output_dir --base-dir base_dir
"""
import argparse
import json
import logging
import os
import re
import sys
import traceback
from datetime import datetime
from pathlib import Path

# 全局 logger 实例（用于输出解析失败日志）
logger = None

SOURCE_DIR = "ohos_interface"
OUTPUT_DIR = None  # 必须由调用方通过 --output-dir 参数指定
ARK_ADAPTER_INCLUDE_REL = "ohos_glue/ohos_adapter/include"
ARK_NWEB_INCLUDE_REL = "ohos_glue/ohos_nweb/include"
NWEB_ADAPTER_INCLUDE = "include/ohos_adapter"
NWEB_NWEB_INCLUDE = "include/ohos_nweb"

# 匹配 ArkTS 接口头文件中的类定义
# 格式: class ArkXXX : public virtual ArkWebBaseRefCounted
ALL_CLASSES_PATTERN = re.compile(
    r'''class\s+                              # class 关键字
        (Ark\w*                               # 类名（以 Ark 开头）
        (?:Adapter|Callback|Result|Handler|   # 后缀类型
        Info|Data|Config|Request|Chooser|
        Params|Details|Options|Event|Wrapper|
        List|Item|Type)?)
        \s*:\s*public\s+                      # : public （必需）
        (?:virtual\s+)?                       # virtual 关键字（可选）
        ArkWebBaseRefCounted                  # 基类名称
        ''',
    re.MULTILINE | re.VERBOSE
)

# 匹配 NWeb 接口头文件中的类定义
# 格式: class OHOS_NWEB_EXPORT NWebXXX 或 class [[clang::...]] NWebXXX
NWEB_CLASS_PATTERN = re.compile(
    r'''(?:class|struct)\s+                   # class 或 struct 关键字
        (?:\[\[clang::lto_visibility_public\]\]\s+)?  # 可选的 clang 属性
        (?:OHOS_NWEB_EXPORT\s+)?              # 可选的导出宏
        ([A-Z][a-zA-Z0-9_]*)                 # 类名（捕获组 1）
        \s*[:\{]                             # 冒号或左大括号（类定义开始）
        ''',
    re.MULTILINE | re.VERBOSE
)

VECTOR_TYPE_CONFIG = {
    'std::vector<uint8_t>': {
        'arkweb_type': 'ArkWebUint8Vector',
        'nweb_type': 'std::vector<uint8_t>',
        'element_type': 'uint8_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebUint8Vector>',
    },
    'std::vector<int8_t>': {
        'arkweb_type': 'ArkWebInt8Vector',
        'nweb_type': 'std::vector<int8_t>',
        'element_type': 'int8_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<int8_t, ArkWebInt8Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<int8_t, ArkWebInt8Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebInt8Vector>',
    },
    'std::vector<uint16_t>': {
        'arkweb_type': 'ArkWebUint16Vector',
        'nweb_type': 'std::vector<uint16_t>',
        'element_type': 'uint16_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<uint16_t, ArkWebUint16Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<uint16_t, ArkWebUint16Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebUint16Vector>',
    },
    'std::vector<int16_t>': {
        'arkweb_type': 'ArkWebInt16Vector',
        'nweb_type': 'std::vector<int16_t>',
        'element_type': 'int16_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<int16_t, ArkWebInt16Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<int16_t, ArkWebInt16Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebInt16Vector>',
    },
    'std::vector<uint32_t>': {
        'arkweb_type': 'ArkWebUint32Vector',
        'nweb_type': 'std::vector<uint32_t>',
        'element_type': 'uint32_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebUint32Vector>',
    },
    'std::vector<int32_t>': {
        'arkweb_type': 'ArkWebInt32Vector',
        'nweb_type': 'std::vector<int32_t>',
        'element_type': 'int32_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebInt32Vector>',
    },
    'std::vector<uint64_t>': {
        'arkweb_type': 'ArkWebUint64Vector',
        'nweb_type': 'std::vector<uint64_t>',
        'element_type': 'uint64_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<uint64_t, ArkWebUint64Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<uint64_t, ArkWebUint64Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebUint64Vector>',
    },
    'std::vector<int64_t>': {
        'arkweb_type': 'ArkWebInt64Vector',
        'nweb_type': 'std::vector<int64_t>',
        'element_type': 'int64_t',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebInt64Vector>',
    },
    'std::vector<bool>': {
        'arkweb_type': 'ArkWebBoolVector',
        'nweb_type': 'std::vector<bool>',
        'element_type': 'bool',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<bool, ArkWebBoolVector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<bool, ArkWebBoolVector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebBoolVector>',
    },
    'std::vector<double>': {
        'arkweb_type': 'ArkWebDoubleVector',
        'nweb_type': 'std::vector<double>',
        'element_type': 'double',
        'class_to_struct': 'ArkWebBasicVectorClassToStruct<double, ArkWebDoubleVector>',
        'struct_to_class': 'ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>',
        'release': 'ArkWebBasicVectorStructRelease<ArkWebDoubleVector>',
    },
    'std::vector<std::string>': {
        'arkweb_type': 'ArkWebStringVector',
        'nweb_type': 'std::vector<std::string>',
        'element_type': 'std::string',
        'class_to_struct': 'ArkWebStringVectorClassToStruct',
        'struct_to_class': 'ArkWebStringVectorStructToClass',
        'release': 'ArkWebStringVectorStructRelease',
    },
}

ADAPTER_VECTOR_TYPE_CONFIG = {
    'std::vector<std::shared_ptr<AudioDeviceDescAdapter>>': {
        'arkweb_type': 'ArkAudioDeviceDescAdapterVector',
        'nweb_type': 'std::vector<std::shared_ptr<AudioDeviceDescAdapter>>',
        'element_type': 'AudioDeviceDescAdapter',
        'adapter_name': 'AudioDeviceDesc',
        'class_to_struct': 'ArkAudioDeviceDescAdapterVectorClassToStruct',
        'struct_to_class': 'ArkAudioDeviceDescAdapterVectorStructToClass',
        'release': 'ArkAudioDeviceDescAdapterVectorStructRelease',
    },
    'std::vector<std::shared_ptr<DisplayAdapter>>': {
        'arkweb_type': 'ArkDisplayAdapterVector',
        'nweb_type': 'std::vector<std::shared_ptr<DisplayAdapter>>',
        'element_type': 'DisplayAdapter',
        'adapter_name': 'Display',
        'class_to_struct': 'ArkDisplayAdapterVectorClassToStruct',
        'struct_to_class': 'ArkDisplayAdapterVectorStructToClass',
        'release': 'ArkDisplayAdapterVectorStructRelease',
    },
    'std::vector<std::shared_ptr<FormatAdapter>>': {
        'arkweb_type': 'ArkFormatAdapterVector',
        'nweb_type': 'std::vector<std::shared_ptr<FormatAdapter>>',
        'element_type': 'FormatAdapter',
        'adapter_name': 'Format',
        'class_to_struct': 'ArkFormatAdapterVectorClassToStruct',
        'struct_to_class': 'ArkFormatAdapterVectorStructToClass',
        'release': 'ArkFormatAdapterVectorStructRelease',
    },
    'std::vector<std::shared_ptr<PasteDataRecordAdapter>>': {
        'arkweb_type': 'ArkPasteRecordVector',
        'nweb_type': 'std::vector<std::shared_ptr<PasteDataRecordAdapter>>',
        'element_type': 'PasteDataRecordAdapter',
        'adapter_name': 'PasteRecord',
        'class_to_struct': 'ArkPasteRecordVectorClassToStruct',
        'struct_to_class': 'ArkPasteRecordVectorStructToClass',
        'release': 'ArkPasteRecordVectorStructRelease',
    },
    'std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>': {
        'arkweb_type': 'ArkVideoDeviceDescriptorAdapterVector',
        'nweb_type': 'std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>',
        'element_type': 'VideoDeviceDescriptorAdapter',
        'adapter_name': 'VideoDeviceDescriptor',
        'class_to_struct': 'ArkVideoDeviceDescriptorAdapterVectorClassToStruct',
        'struct_to_class': 'ArkVideoDeviceDescriptorAdapterVectorStructToClass',
        'release': 'ArkVideoDeviceDescriptorAdapterVectorStructRelease',
    },
}

MAP_TYPE_CONFIG = {
    'std::map<std::string, std::string>': {
        'arkweb_type': 'ArkWebStringMap',
        'nweb_type': 'std::map<std::string, std::string>',
        'class_to_struct': 'ArkWebStringMapClassToStruct',
        'struct_to_class': 'ArkWebStringMapStructToClass',
        'release': 'ArkWebStringMapStructRelease',
    },
    'std::map<std::string, std::vector<std::string>>': {
        'arkweb_type': 'ArkWebStringVectorMap',
        'nweb_type': 'std::map<std::string, std::vector<std::string>>',
        'class_to_struct': 'ArkWebStringVectorMapClassToStruct',
        'struct_to_class': 'ArkWebStringVectorMapStructToClass',
        'release': 'ArkWebStringVectorMapStructRelease',
    },
    'std::map<std::string, std::vector<uint8_t>>': {
        'arkweb_type': 'ArkWebUInt8VectorMap',
        'nweb_type': 'std::map<std::string, std::vector<uint8_t>>',
        'class_to_struct': 'ArkWebUInt8VectorMapClassToStruct',
        'struct_to_class': 'ArkWebUInt8VectorMapStructToClass',
        'release': 'ArkWebUInt8VectorMapStructRelease',
    },
    'std::map<std::string, std::vector<std::pair<std::string, std::string>>>': {
        'arkweb_type': 'ArkWebPairStringVectorMap',
        'nweb_type': 'std::map<std::string, std::vector<std::pair<std::string, std::string>>>',
        'class_to_struct': 'ArkWebPairStringVectorMapClassToStruct',
        'struct_to_class': 'ArkWebPairStringVectorMapStructToClass',
        'release': 'ArkWebPairStringVectorMapStructRelease',
    },
}

LIST_TYPE_CONFIG = {
    'std::list<std::string>': {
        'arkweb_type': 'ArkWebStringList',
        'nweb_type': 'std::list<std::string>',
        'element_type': 'std::string',
        'class_to_struct': 'ArkWebStringListClassToStruct',
        'struct_to_class': 'ArkWebStringListStructToClass',
        'release': 'ArkWebStringListStructRelease',
    },
    'std::list<int32_t>': {
        'arkweb_type': 'ArkWebInt32List',
        'nweb_type': 'std::list<int32_t>',
        'element_type': 'int32_t',
        'class_to_struct': 'ArkWebBasicListClassToStruct<int32_t, ArkWebInt32List>',
        'struct_to_class': 'ArkWebBasicListStructToClass<int32_t, ArkWebInt32List>',
        'release': 'ArkWebBasicListStructRelease<ArkWebInt32List>',
    },
}

PAIR_VECTOR_TYPE_CONFIG = {
    'std::vector<std::pair<std::string, std::string>>': {
        'arkweb_type': 'ArkWebPairStringVector',
        'nweb_type': 'std::vector<std::pair<std::string, std::string>>',
        'first_type': 'std::string',
        'second_type': 'std::string',
        'class_to_struct': 'ArkWebPairStringVectorClassToStruct',
        'struct_to_class': 'ArkWebPairStringVectorStructToClass',
        'release': 'ArkWebPairStringVectorStructRelease',
    },
}

BASIC_TYPES = {
    'void', 'bool', 'char', 'short', 'int', 'long', 'float', 'double',
    'int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t',
    'int64_t', 'uint64_t', 'size_t', 'ssize_t', 'intptr_t', 'uintptr_t',
    'std::string', 'std::vector', 'std::map', 'std::shared_ptr', 'const',
    'unsigned', 'signed'
}

PARAMETER_NAME_MAPPING = {}
ENUM_TO_INT32_CONVERSION = {}
ARKTS_TO_NWEB_CLASS_MAPPING = {}
NWEB_ENUM_OUTPUT_PARAMS = {}
UINT32_ENUM_TYPES = {}

SUFFIX_PROTECT = ["ID", "URL", "URI"]


def to_snake_case(name, _recursing=False):
    """转换类名为 snake_case 文件名（使用简单字符串操作）

    处理规则：
    1. 保护配置的缩写组合（如 ID、URL）
    2. 识别"连续大写字母组+单词"模式（如 MMIDevice → MMI_Device）
    3. 转换为 snake_case
    """
    temp_name = name

    for suffix in SUFFIX_PROTECT:
        i = 0
        while True:
            idx = temp_name.find(suffix, i)
            if idx == -1:
                break
            next_idx = idx + len(suffix)
            if next_idx >= len(temp_name) or temp_name[next_idx].isupper():
                temp_name = f"{temp_name[:idx]}{suffix}_{temp_name[next_idx:]}"
                i = next_idx + 1
            else:
                i = idx + 1

    temp_name = temp_name.replace('NWeb', 'Nweb')

    result = []
    i = 0
    while i < len(temp_name):
        if temp_name[i].isupper():
            j = i
            while j < len(temp_name) and temp_name[j].isupper():
                if j + 1 < len(temp_name) and temp_name[j+1].islower():
                    j += 1
                    break
                j += 1
            if j - i >= 2 and j < len(temp_name) and temp_name[j].islower():
                result.append(f'<{temp_name[i:j-1]}>')
                result.append(temp_name[j-1])
                i = j
                continue
        result.append(temp_name[i])
        i += 1
    temp_name = ''.join(result)

    if not _recursing:
        if temp_name.startswith('ArkWeb'):
            temp_name = f'ark_web{temp_name[6:]}'
        elif temp_name.startswith('Ark'):
            temp_name = f'ark_{temp_name[3:]}'

    result = []
    in_tag = False
    for i, ch in enumerate(temp_name):
        if ch == '<':
            in_tag = True
            result.append(ch)
        elif ch == '>':
            in_tag = False
            result.append(ch)
        elif ch.isupper() and i > 0:
            prev_char = temp_name[i-1]
            if prev_char.isupper() or prev_char == '_':
                result.append(ch)
            elif not in_tag:
                result.append('_')
                result.append(ch)
            else:
                result.append(ch)
        else:
            result.append(ch)
    temp_name = ''.join(result)

    temp_name = temp_name.lower()
    while '<' in temp_name and '>' in temp_name:
        start = temp_name.find('<')
        end = temp_name.find('>', start)
        if start != -1 and end != -1:
            content = temp_name[start+1:end]
            temp_name = f'{temp_name[:start]}{content}_{temp_name[end+1:]}'
        else:
            break
    while '__' in temp_name:
        temp_name = temp_name.replace('__', '_')
    temp_name = temp_name.strip('_')

    return temp_name


def to_snake_case_for_filename(name):
    return to_snake_case(name)


def get_nweb_base_name(ark_class_name):
    if ark_class_name.startswith('ArkWeb'):
        return f'NWeb{ark_class_name[6:]}'
    elif ark_class_name.startswith('Ark'):
        return ark_class_name[3:]
    return ark_class_name


def arkweb_to_nweb_type(ark_class_name):
    if ark_class_name in ARKTS_TO_NWEB_CLASS_MAPPING:
        return ARKTS_TO_NWEB_CLASS_MAPPING[ark_class_name]
    return get_nweb_base_name(ark_class_name)


def strip_class_suffixes(name, suffixes=None):
    if suffixes is None:
        suffixes = ['Adapter', 'Impl', 'Wrapper', 'Callback', 'Handler',
                    'Listener', 'Info', 'Data', 'Config', 'Request', 'Options']
    for suffix in suffixes:
        if name.endswith(suffix):
            return name[:-len(suffix)]
    return name


def _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=True, vector_mappings=None):
    """通用 Vector 配置查找函数

    Args:
        vector_type: 要查找的 Vector 类型
        config_key: 用于匹配的配置键（默认 'arkweb_type'）
        check_adapters: 是否检查 ADAPTER_VECTOR_TYPE_CONFIG（默认 True）
        vector_mappings: 类级别的 vector 映射配置（可选）

    Returns:
        dict: 找到的配置，如果未找到返回 None
    """
    # 优先检查类级别的 vector_mappings 配置
    if vector_mappings and vector_type in vector_mappings:
        return vector_mappings[vector_type]

    # 检查 VECTOR_TYPE_CONFIG 直接匹配
    if isinstance(vector_type, str) and vector_type in VECTOR_TYPE_CONFIG:
        return VECTOR_TYPE_CONFIG[vector_type]

    # 遍历 VECTOR_TYPE_CONFIG 查找
    for config in VECTOR_TYPE_CONFIG.values():
        if config.get(config_key) == vector_type:
            return config

    # 检查 ADAPTER_VECTOR_TYPE_CONFIG
    if check_adapters:
        if isinstance(vector_type, str) and vector_type in ADAPTER_VECTOR_TYPE_CONFIG:
            return ADAPTER_VECTOR_TYPE_CONFIG[vector_type]
        for config in ADAPTER_VECTOR_TYPE_CONFIG.values():
            if config.get(config_key) == vector_type:
                return config

    return None


def get_vector_config(vector_type):
    return VECTOR_TYPE_CONFIG.get(vector_type)


def get_vector_arkweb_type(vector_type):
    config = _find_vector_config(vector_type, config_key='nweb_type', check_adapters=False)
    return config['arkweb_type'] if config else None


def get_vector_nweb_type(vector_type):
    config = _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=False)
    return config['nweb_type'] if config else None


def get_vector_element_type(vector_type):
    """获取 Vector 类型的元素类型"""
    if vector_type is None:
        return None
    config = _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=False)
    if config:
        return config['element_type']
    # 特殊处理：从 ArkWebXXXVector 格式推导元素类型
    if isinstance(vector_type, str) and 'ArkWeb' in vector_type and 'Vector' in vector_type:
        base = vector_type.replace('ArkWeb', '').replace('Vector', '')
        if base:
            return base.lower()
    return None


def get_vector_class_to_struct_func(vector_type, vector_mappings=None):
    config = _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=True, vector_mappings=vector_mappings)
    if config:
        return config['class_to_struct']
    # 默认回退
    if vector_type == 'ArkWebStringVector':
        return 'ArkWebStringVectorClassToStruct'
    return f'ArkWebBasicVectorClassToStruct<{get_vector_element_type(vector_type)}, {vector_type}>'


def get_vector_struct_to_class_func(vector_type):
    config = _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=True)
    if config:
        return config['struct_to_class']
    # 默认回退
    if vector_type == 'ArkWebStringVector':
        return 'ArkWebStringVectorStructToClass'
    return f'ArkWebBasicVectorStructToClass<{get_vector_element_type(vector_type)}, {vector_type}>'


def get_vector_release_func(vector_type):
    config = _find_vector_config(vector_type, config_key='arkweb_type', check_adapters=True)
    if config:
        return config['release']
    # 默认回退
    if vector_type == 'ArkWebStringVector':
        return 'ArkWebStringVectorStructRelease'
    return f'ArkWebBasicVectorStructRelease<{vector_type}>'


def get_map_arkweb_type(map_type):
    if isinstance(map_type, str) and map_type in MAP_TYPE_CONFIG:
        return MAP_TYPE_CONFIG[map_type]['arkweb_type']
    for config in MAP_TYPE_CONFIG.values():
        if config['nweb_type'] == map_type:
            return config['arkweb_type']
    return None


def get_map_nweb_type(map_type):
    if isinstance(map_type, str) and map_type in MAP_TYPE_CONFIG:
        return MAP_TYPE_CONFIG[map_type]['nweb_type']
    for config in MAP_TYPE_CONFIG.values():
        if config['arkweb_type'] == map_type:
            return config['nweb_type']
    return None


def get_map_class_to_struct_func(map_type):
    if isinstance(map_type, str) and map_type in MAP_TYPE_CONFIG:
        return MAP_TYPE_CONFIG[map_type]['class_to_struct']
    for config in MAP_TYPE_CONFIG.values():
        if config['arkweb_type'] == map_type:
            return config['class_to_struct']
    return None


def get_map_struct_to_class_func(map_type):
    if isinstance(map_type, str) and map_type in MAP_TYPE_CONFIG:
        return MAP_TYPE_CONFIG[map_type]['struct_to_class']
    for config in MAP_TYPE_CONFIG.values():
        if config['arkweb_type'] == map_type:
            return config['struct_to_class']
    return None


def get_map_release_func(map_type):
    if isinstance(map_type, str) and map_type in MAP_TYPE_CONFIG:
        return MAP_TYPE_CONFIG[map_type]['release']
    for config in MAP_TYPE_CONFIG.values():
        if config['arkweb_type'] == map_type:
            return config['release']
    return None


def get_list_arkweb_type(list_type):
    if isinstance(list_type, str) and list_type in LIST_TYPE_CONFIG:
        return LIST_TYPE_CONFIG[list_type]['arkweb_type']
    for config in LIST_TYPE_CONFIG.values():
        if config['nweb_type'] == list_type:
            return config['arkweb_type']
    return None


def get_list_nweb_type(list_type):
    if isinstance(list_type, str) and list_type in LIST_TYPE_CONFIG:
        return LIST_TYPE_CONFIG[list_type]['nweb_type']
    for config in LIST_TYPE_CONFIG.values():
        if config['arkweb_type'] == list_type:
            return config['nweb_type']
    return None


def get_list_class_to_struct_func(list_type):
    if isinstance(list_type, str) and list_type in LIST_TYPE_CONFIG:
        return LIST_TYPE_CONFIG[list_type]['class_to_struct']
    for config in LIST_TYPE_CONFIG.values():
        if config['arkweb_type'] == list_type:
            return config['class_to_struct']
    return None


def get_list_struct_to_class_func(list_type):
    if isinstance(list_type, str) and list_type in LIST_TYPE_CONFIG:
        return LIST_TYPE_CONFIG[list_type]['struct_to_class']
    for config in LIST_TYPE_CONFIG.values():
        if config['arkweb_type'] == list_type:
            return config['struct_to_class']
    return None


def get_list_release_func(list_type):
    if isinstance(list_type, str) and list_type in LIST_TYPE_CONFIG:
        return LIST_TYPE_CONFIG[list_type]['release']
    for config in LIST_TYPE_CONFIG.values():
        if config['arkweb_type'] == list_type:
            return config['release']
    return None


def get_pair_vector_arkweb_type(pair_vector_type):
    if isinstance(pair_vector_type, str) and pair_vector_type in PAIR_VECTOR_TYPE_CONFIG:
        return PAIR_VECTOR_TYPE_CONFIG[pair_vector_type]['arkweb_type']
    for config in PAIR_VECTOR_TYPE_CONFIG.values():
        if config['nweb_type'] == pair_vector_type:
            return config['arkweb_type']
    return None


def get_pair_vector_nweb_type(pair_vector_type):
    if isinstance(pair_vector_type, str) and pair_vector_type in PAIR_VECTOR_TYPE_CONFIG:
        return PAIR_VECTOR_TYPE_CONFIG[pair_vector_type]['nweb_type']
    for config in PAIR_VECTOR_TYPE_CONFIG.values():
        if config['arkweb_type'] == pair_vector_type:
            return config['nweb_type']
    return None


def get_pair_vector_class_to_struct_func(pair_vector_type):
    if isinstance(pair_vector_type, str) and pair_vector_type in PAIR_VECTOR_TYPE_CONFIG:
        return PAIR_VECTOR_TYPE_CONFIG[pair_vector_type]['class_to_struct']
    for config in PAIR_VECTOR_TYPE_CONFIG.values():
        if config['arkweb_type'] == pair_vector_type:
            return config['class_to_struct']
    return None


def get_pair_vector_struct_to_class_func(pair_vector_type):
    if isinstance(pair_vector_type, str) and pair_vector_type in PAIR_VECTOR_TYPE_CONFIG:
        return PAIR_VECTOR_TYPE_CONFIG[pair_vector_type]['struct_to_class']
    for config in PAIR_VECTOR_TYPE_CONFIG.values():
        if config['arkweb_type'] == pair_vector_type:
            return config['struct_to_class']
    return None


def get_pair_vector_release_func(pair_vector_type):
    if isinstance(pair_vector_type, str) and pair_vector_type in PAIR_VECTOR_TYPE_CONFIG:
        return PAIR_VECTOR_TYPE_CONFIG[pair_vector_type]['release']
    for config in PAIR_VECTOR_TYPE_CONFIG.values():
        if config['arkweb_type'] == pair_vector_type:
            return config['release']
    return None


def get_nweb_vector_type(arkweb_vec_type):
    """获取 ArkWeb vector 类型对应的 NWeb vector 类型
    例如：ArkWebInt32Vector -> std::vector<int32_t>
    """
    if arkweb_vec_type == 'ArkWebStringVector':
        return 'std::vector<std::string>'
    elif arkweb_vec_type == 'ArkWebInt32Vector':
        return 'std::vector<int32_t>'
    elif arkweb_vec_type == 'ArkWebInt64Vector':
        return 'std::vector<int64_t>'
    elif arkweb_vec_type == 'ArkWebUint32Vector':
        return 'std::vector<uint32_t>'
    elif arkweb_vec_type == 'ArkWebBoolVector':
        return 'std::vector<bool>'
    elif arkweb_vec_type == 'ArkWebDoubleVector':
        return 'std::vector<double>'
    else:
        element_type = get_vector_element_type(arkweb_vec_type)
        # 映射 ArkWeb 类型到 NWeb 类型
        if element_type.startswith('Ark'):
            # 去掉 Ark 前缀
            nweb_element = element_type[3:]
        else:
            nweb_element = element_type
        return f'std::vector<{nweb_element}>'


def _is_vector_type(type_str):
    type_str = type_str.strip()
    if type_str in VECTOR_TYPE_CONFIG:
        return True
    if type_str in ADAPTER_VECTOR_TYPE_CONFIG:
        return True
    if 'std::vector' in type_str:
        return True
    if 'ArkWeb' in type_str and 'Vector' in type_str:
        return True
    if type_str.startswith('Ark') and 'AdapterVector' in type_str:
        return True
    if type_str.startswith('Ark') and 'Vector' in type_str:
        return True
    return False


def _is_pure_arkweb_string_param(param_str):
    """检查参数是否是纯 ArkWebString 类型（不包括 ArkWebStringVector 等派生类型）"""
    param_str = param_str.strip()
    if 'ArkWebString' not in param_str:
        return False
    if 'Vector' in param_str:
        return False
    pattern = r'(?:const\s+)?ArkWebString(?:&)?(?=\s+\w)'
    return bool(re.search(pattern, param_str))


def _is_string_type(type_str):
    """检查是否是字符串类型（精确匹配，不包括 std::map 等包含 std::string 的类型）"""
    type_str = type_str.strip()
    pure_string_pattern = r'^(const\s+)?std::string(&)?$'
    if re.match(pure_string_pattern, type_str):
        return True
    return type_str == 'ArkWebString' or type_str.startswith('ArkWebString&') or type_str.startswith('const ArkWebString')


def _is_shared_ptr_type(type_str):
    type_str = type_str.strip()
    return 'std::shared_ptr' in type_str or 'ArkWebRefPtr' in type_str


def _is_enum_type(type_str):
    """通用枚举类型检测（使用排除法）"""
    type_str = type_str.strip()

    base_type = type_str.split('::')[-1] if '::' in type_str else type_str

    for basic_type in BASIC_TYPES:
        if base_type == basic_type:
            return False
    if '::std::' in type_str or type_str.startswith('std::'):
        return False
    if 'OHOS::NWeb::' in type_str:
        return True
    nweb_enum_patterns = ['Adapter', 'Type', 'Mode', 'Code', 'State', 'Status',
                          'Flag', 'Reason', 'Format', 'Source', 'Option', 'Level',
                          'Orientation', 'Id', 'Subtype']
    for pattern in nweb_enum_patterns:
        if type_str.endswith(pattern) and not type_str.startswith('void'):
            return True
    return False


def _is_basic_integer_type(type_str):
    """检查是否是基本整数类型（需要 static_cast 的类型）

    包括: int, int32_t, uint32_t, int8_t, uint8_t, int16_t, uint16_t, int64_t, uint64_t
    """
    type_str = type_str.strip()
    basic_integer_types = {
        'int', 'int8_t', 'uint8_t', 'int16_t', 'uint16_t',
        'int32_t', 'uint32_t', 'int64_t', 'uint64_t'
    }
    return type_str in basic_integer_types


def _is_basic_type(type_str):
    """检查是否是基本类型（用于判断是否需要添加命名空间）

    注意：OpenHarmony 编码规范推荐使用定宽整数类型（int32_t, uint32_t 等），
    但为了保持与现有 NWeb 接口的兼容性，此处仍支持非定宽类型（int, short, long 等）。
    新增接口建议使用定宽类型。
    """
    if not type_str:
        return True
    type_str = type_str.strip()

    if type_str == 'void':
        return True

    # 基本类型集合（包括非定宽整数类型，保持兼容性）
    # 注意：int/short/long 等非定宽类型不符合 OpenHarmony 规范，但保留用于兼容现有接口
    # 参考类型映射表：NON_FIXED_WIDTH_TYPE_MAP
    basic_cpp_types = {'bool', 'char', 'char16_t', 'char32_t', 'wchar_t',
                      'short', 'int', 'long', 'long long',  # 非定宽类型（兼容性）
                      'unsigned short', 'unsigned int', 'unsigned long', 'unsigned long long',
                      'float', 'double'}
    if type_str in basic_cpp_types:
        return True

    # 定宽整数类型（符合 OpenHarmony 规范）
    if any(t in type_str for t in ['int8_t', 'uint8_t', 'int16_t', 'uint16_t',
                                      'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
                                      'intptr_t', 'uintptr_t', 'size_t', 'ssize_t']):
        return True

    # 去除修饰符后再次检查
    base_type = type_str.replace('const', '').replace('&', '').replace('*', '').strip()
    if base_type in basic_cpp_types:
        return True

    # 检查基础类型中是否包含定宽整数类型
    if any(t in base_type for t in ['int8_t', 'uint8_t', 'int16_t', 'uint16_t',
                                      'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
                                      'intptr_t', 'uintptr_t', 'size_t', 'ssize_t']):
        return True

    return False


# 非定宽整数类型到定宽类型的映射（仅供参考）
# 用于未来代码规范迁移时查阅
# 基于 OpenHarmony 编码规范（LP64 数据模型）
NON_FIXED_WIDTH_TYPE_MAP = {
    # 有符号类型
    'short': 'int16_t',
    'int': 'int32_t',
    'long': 'int64_t',        # 64 位系统
    'long long': 'int64_t',
    # 无符号类型
    'unsigned short': 'uint16_t',
    'unsigned int': 'uint32_t',
    'unsigned long': 'uint64_t',  # 64 位系统
    'unsigned long long': 'uint64_t',
}


def get_fixed_width_type(type_str):
    """将非定宽整数类型转换为定宽整数类型（仅供参考，不自动转换）

    Args:
        type_str: 输入类型字符串

    Returns:
        str: 对应的定宽类型字符串，如果输入已经是定宽类型或不需要转换，则返回原类型
    """
    type_str = type_str.strip()
    return NON_FIXED_WIDTH_TYPE_MAP.get(type_str, type_str)


def _needs_type_conversion(type_str):
    return (_is_string_type(type_str) or
            _is_vector_type(type_str) or
            _is_shared_ptr_type(type_str) or
            _is_enum_type(type_str))


def _is_vector_shared_ptr_return_type(return_type):
    """检查返回值是否是 std::vector<std::shared_ptr<...>> 类型
    需要从 ArkTS vector 类型（如 ArkAudioDeviceDescAdapterVector）转换为 NWeb vector 类型
    Args:
        return_type: 返回值类型字符串
    Returns:
        tuple: (is_vector_shared_ptr, element_type, arkts_vector_type, conversion_func, release_func)
    """
    pattern = r'std::vector\s*<\s*std::shared_ptr\s*<\s*(?:OHOS::NWeb::)?([^>]+)\s*>\s*>'
    match = re.search(pattern, return_type)
    if match:
        element_type = match.group(1).strip()
        # AudioDeviceDescAdapter -> ArkAudioDeviceDescAdapterVector
        arkts_element_type = f'Ark{element_type}'
        arkts_vector_type = f'{arkts_element_type}Vector'
        conversion_func = f'{arkts_vector_type}StructToClass'
        release_func = f'{arkts_vector_type}StructRelease'
        return True, element_type, arkts_vector_type, conversion_func, release_func
    return False, None, None, None, None


def _detect_reference_type_adapters(helper_file_path):
    """从 ohos_adapter_helper.h 中检测使用引用类型的 adapter

    通过解析 helper 文件，自动识别哪些 adapter 应该使用引用类型（而不是指针类型）。
    规则：
    - 如果 getter 方法返回 Adapter&（引用类型），则该 adapter 使用引用类型
    - 如果 create 方法返回 std::unique_ptr/std::shared_ptr（指针类型），则该 adapter 使用指针类型

    Args:
        helper_file_path: ohos_adapter_helper.h 文件路径

    Returns:
        set: 使用引用类型的 adapter 完整类名集合（含 Adapter 后缀）
    """
    reference_type_adapters = set()

    try:
        with open(helper_file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # 匹配返回引用类型的 adapter
        # 格式: virtual XXXAdapter& GetXXX() = 0;
        # 或: virtual XXXAdapter& GetXXXInstance() = 0;
        # 或: virtual XXXAdapter& GetXXX() = 0;
        ref_pattern = re.compile(r'virtual\s+(\w+Adapter)&\s+\w+\s*\(\s*\)\s*=\s*0\s*;')
        matches = ref_pattern.findall(content)

        for match in matches:
            # 直接使用完整的类名（包含 Adapter 后缀）
            if match.endswith('Adapter'):
                reference_type_adapters.add(match)

    except FileNotFoundError:
        print(f"警告: 无法找到 helper 文件: {helper_file_path}")
    except Exception as e:
        print(f"警告: 解析 helper 文件时出错: {e}")

    return reference_type_adapters


def _parse_vector_headers(vector_headers, base_dir):
    """从 vector 头文件中解析 Vector 类型映射

    Args:
        vector_headers: vector 头文件路径列表
        base_dir: 基础目录

    Returns:
        dict: Vector 类型映射，格式为 {arkweb_type: {nweb_type, class_to_struct}}
    """
    vector_mappings = {}

    for header_path in vector_headers:
        full_path = os.path.join(base_dir, header_path)
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()

            # 匹配 typedef struct { ... } ArkXXXVector; 格式
            # 例如: typedef struct { ... } ArkVideoDeviceDescriptorAdapterVector;
            typedef_pattern = re.compile(r'typedef\s+struct\s+\{[^}]*\}\s+(\w+Vector)\s*;', re.DOTALL)
            matches = typedef_pattern.findall(content)

            for vector_type in matches:
                # 推导转换函数名：ArkVideoDeviceDescriptorAdapterVector -> ArkVideoDeviceDescriptorAdapterVectorClassToStruct
                class_to_struct = f"{vector_type}ClassToStruct"

                # 从 vector 类型名推导元素类型
                # ArkVideoDeviceDescriptorAdapterVector -> VideoDeviceDescriptorAdapter
                element_type = None
                if vector_type.endswith('AdapterVector'):
                    # 去掉 Ark 前缀和 Vector 后缀
                    element_type = vector_type[3:-6]  # ArkXXXAdapterVector -> XXXAdapter
                elif vector_type.endswith('Vector'):
                    # 去掉 Ark 前缀和 Vector 后缀
                    element_type = vector_type[3:-6]  # ArkXXXVector -> XXX

                # 构建 NWeb vector 类型
                if element_type:
                    nweb_type = f"std::vector<std::shared_ptr<{element_type}>>"

                    vector_mappings[vector_type] = {
                        'nweb_type': nweb_type,
                        'class_to_struct': class_to_struct
                    }
                    print(f"  [解析 Vector] {vector_type} -> {class_to_struct}")

        except FileNotFoundError:
            print(f"警告: 无法找到 vector 头文件: {full_path}")
        except Exception as e:
            print(f"警告: 解析 vector 头文件时出错 ({header_path}): {e}")

    return vector_mappings


def _is_basic_vector_output_param(param):
    """检查参数是否是基本类型或枚举类型的 vector 输出参数（std::vector<basic_type>&，不带 const）
    支持格式:
    - std::vector<T>& name (基本类型)
    - std::vector<OHOS::NWeb::T>& name (枚举类型)
    Args:
        param: 参数字符串
    Returns:
        tuple: (is_output, element_type, param_name, arkts_vector_type, underlying_type, conversion_func, release_func)
    """
    # 基本类型列表
    basic_types = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t',
                   'int64_t', 'uint64_t', 'bool', 'double', 'float', 'char']
    # 支持两种格式:
    # 1. std::vector<T>& name (基本类型)
    # 2. std::vector<OHOS::NWeb::T>& name (枚举类型)
    pattern = r'(?:const\s+)?std::vector\s*<\s*(?:(\w+)|(OHOS::NWeb::(\w+)))\s*>\s*&\s*(\w+)'
    match = re.match(pattern, param.strip())
    if match:
        element_type = match.group(1) if match.group(1) else match.group(3)
        param_name = match.group(4)
        underlying_type = None
        arkts_vector_type = None
        if element_type in basic_types:
            underlying_type = element_type
            type_map = {
                'int8_t': 'Int8', 'uint8_t': 'Uint8',
                'int16_t': 'Int16', 'uint16_t': 'Uint16',
                'int32_t': 'Int32', 'uint32_t': 'Uint32',
                'int64_t': 'Int64', 'uint64_t': 'Uint64',
                'bool': 'Bool', 'double': 'Double', 'float': 'Float', 'char': 'Char'
            }
            mapped_type = type_map.get(element_type, element_type)
            arkts_vector_type = f'ArkWeb{mapped_type}Vector'
        elif element_type.endswith('Adapter'):
            underlying_type = NWEB_ENUM_OUTPUT_PARAMS.get(element_type, 'int32_t')
            type_map = {
                'int8_t': 'Int8', 'uint8_t': 'Uint8',
                'int16_t': 'Int16', 'uint16_t': 'Uint16',
                'int32_t': 'Int32', 'uint32_t': 'Uint32',
                'int64_t': 'Int64', 'uint64_t': 'Uint64',
                'bool': 'Bool', 'double': 'Double', 'float': 'Float', 'char': 'Char'
            }
            mapped_type = type_map.get(underlying_type, underlying_type)
            arkts_vector_type = f'ArkWeb{mapped_type}Vector'
        else:
            return False, None, None, None, None, None, None
        conversion_func = f'ArkWebBasicVectorStructToClass<{underlying_type}, {arkts_vector_type}>'
        release_func = f'ArkWebBasicVectorStructRelease<{arkts_vector_type}>'
        return True, element_type, param_name, arkts_vector_type, underlying_type, conversion_func, release_func
    return False, None, None, None, None, None, None


def _is_enum_ref_param(param):
    """检查参数是否是枚举引用参数（用于 wrapper 文件生成）"""
    param = param.strip()
    if param.startswith('const'):
        return False, None, None
    parts = param.rsplit('&', 1)
    if len(parts) != 2:
        return False, None, None
    type_part = parts[0].strip()
    name_part = parts[1].strip()
    if not _is_enum_type(type_part):
        return False, None, None
    if 'OHOS::NWeb::' not in type_part and 'NWeb::' not in type_part:
        type_part = f'OHOS::NWeb::{type_part}'
    return True, type_part, name_part


def _extract_param_names(param_str):
    """从参数字符串中提取参数名列表"""
    if not param_str or param_str.strip() == 'void':
        return []
    param_names = []
    params = param_str.split(',')
    for i, param in enumerate(params):
        param = param.strip()
        if not param:
            continue
        func_ptr_match = re.search(r'(\**)\s*(\w+)\s*\)\s*\(', param)
        if func_ptr_match:
            param_name = func_ptr_match.group(2)
            if param_name:
                param_names.append(param_name)
            else:
                param_names.append(f'param{i}')
            continue
        ptr_match = re.search(r'(\w+\s*\*+)\s*(\w+)$', param)
        if ptr_match:
            param_name = ptr_match.group(2)
            if param_name:
                param_names.append(param_name)
            else:
                param_names.append(f'param{i}')
            continue
        parts = param.rsplit(' ', 1)
        if len(parts) == 2:
            param_name = parts[1].strip()
            param_name = param_name.rstrip('&')
            if param_name:
                param_names.append(param_name)
            else:
                param_names.append(f'param{i}')
        else:
            param_names.append(f'param{i}')
    return param_names


def _is_void_star_param(param):
    """检查参数是否是 void* 类型"""
    param = param.strip()
    return bool(re.search(r'\bvoid\s*\*\s*', param))


def _is_function_pointer_param(param):
    """检查参数是否是函数指针类型"""
    param = param.strip()
    return bool(re.search(r'\(\s*\*\s*\w+\s*\)\s*\(', param))


def _extract_shared_ptr_inner_type(type_str):
    """提取 shared_ptr 内部类型"""
    type_str = type_str.strip()
    patterns = [
        r'std::shared_ptr<(.+?)>',
        r'ArkWebRefPtr<(.+?)>'
    ]
    for pattern in patterns:
        match = re.search(pattern, type_str)
        if match:
            inner_type = match.group(1).strip()
            for prefix in ['OHOS::NWeb::', 'NWeb::', 'OHOS::', 'ArkWeb::']:
                if inner_type.startswith(prefix):
                    inner_type = inner_type[len(prefix):]
            return inner_type
    return None


def _is_output_enum_param(param_type, param_name):
    """检查参数是否是需要特殊处理的输出枚举参数"""
    if '&' not in param_type:
        return False
    if 'const ' in param_type:
        return False
    base_type = param_type.split('::')[-1] if '::' in param_type else param_type
    base_type = base_type.rstrip('&').strip()
    return NWEB_ENUM_OUTPUT_PARAMS.get(base_type, False)


def _get_output_enum_temp_var_name(param_name):
    """为输出枚举参数生成临时变量名
    Args:
        param_name: 原始参数名
    Returns:
        str: 临时变量名（如 uType for type, uNetConnectSubtype for netConnectSubtype）
    """
    # 使用 u 前缀 + 首字母大写的参数名（驼峰式）
    if not param_name:
        return 'uParam'
    # 首字母大写
    capitalized = param_name[0].upper() + param_name[1:] if len(param_name) > 1 else param_name.upper()
    return f'u{capitalized}'


def _is_uint32_enum(param_type):
    """检查枚举类型是否是 uint32_t 底层类型
    Args:
        param_type: 参数类型字符串
    Returns:
        bool: 是否是 uint32_t 底层类型的枚举
    """
    if '::' in param_type:
        base_type = param_type.split('::')[-1]
    else:
        base_type = param_type
    # 移除const和引用修饰符
    base_type = base_type.replace('const ', '').strip().rstrip('&')
    return UINT32_ENUM_TYPES.get(base_type, False)


def _is_basic_type_output_param(param):
    """检查参数是否是基本类型的输出参数（基本类型&，不带 const）
    用于处理像 GetCurrentExposureMode(OHOS::NWeb::ExposureModeAdapter& exposureModeAdapter) 这样的参数
    Args:
        param: 参数字符串
    Returns:
        tuple: (is_output, param_type, param_name)
    """
    # 基本类型列表（枚举类型）
    basic_types = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t',
                   'int64_t', 'uint64_t', 'bool', 'double', 'float']
    # 但要排除 std::vector<> 的情况
    if 'std::vector' in param:
        return False, None, None
    pattern = r'([A-Za-z0-9_:]+)\s*&\s*(\w+)'
    match = re.match(pattern, param.strip())
    if match:
        param_type = match.group(1)
        param_name = match.group(2)
        # 去掉命名空间前缀
        base_type = param_type.split('::')[-1] if '::' in param_type else param_type
        if base_type in basic_types or 'Adapter' in param_type:
            return True, param_type, param_name
    return False, None, None


def _needs_enum_to_int32_conversion(param_type):
    """检查枚举类型是否需要转换为 int32_t
    Args:
        param_type: 参数类型字符串（可能包含命名空间）
    Returns:
        bool: 是否需要转换为 int32_t
    """
    # 例如: "OHOS::NWeb::ResSchedStatusAdapter" -> "ResSchedStatusAdapter"
    if '::' in param_type:
        base_type = param_type.split('::')[-1]
    else:
        base_type = param_type
    # 移除const和引用修饰符
    base_type = base_type.replace('const ', '').strip().rstrip('&')
    # 或者是包含 OHOS::NWeb:: 命名空间的枚举类型
    if 'OHOS::NWeb::' in param_type:
        return True
    return ENUM_TO_INT32_CONVERSION.get(base_type, False)


def _format_param_for_call(param_name, param_type):
    """格式化方法调用参数，处理必要的类型转换
    Args:
        param_name: 参数名
        param_type: 参数类型
    Returns:
        str: 格式化后的参数字符串（可能包含类型转换）
    """
    if _needs_enum_to_int32_conversion(param_type):
        if _is_uint32_enum(param_type):
            return f"static_cast<uint32_t>({param_name})"
        return f"static_cast<int32_t>({param_name})"
    return param_name


def _get_shared_ptr_impl_class(inner_type):
    """生成 shared_ptr 参数对应的 impl 类名
    例如: MMIListenerAdapter -> ArkMMIListenerAdapterImpl
    """
    if inner_type.startswith('Ark'):
        return f'{inner_type}Impl'
    return f'Ark{inner_type}Impl'


def _check_shared_ptr_params(params_str):
    """检查参数列表中的 shared_ptr 参数
    Returns:
        list: 布尔值列表，表示每个参数是否是 shared_ptr 类型
    """
    if not params_str or params_str.strip() == 'void':
        return []
    result = []
    params = params_str.split(',')
    for param in params:
        param = param.strip()
        if not param:
            continue
        result.append(_is_shared_ptr_type(param))
    return result


class InterfaceParser:

    def __init__(self, base_dir):
        self.base_dir = Path(base_dir)

    def parse_arkts_interface(self, file_path, class_name=None):
        full_path = self.base_dir / file_path
        if not full_path.exists():
            return None
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except (FileNotFoundError, PermissionError, UnicodeDecodeError, IOError):
            return None
        # 查找类定义
        for match in ALL_CLASSES_PATTERN.finditer(content):
            found_class_name = match.group(1)
            class_start_pos = match.start()
            if class_name and found_class_name != class_name:
                continue
            source = self._extract_source_attribute(content[:class_start_pos])
            methods = self._extract_arkts_methods(content, found_class_name)
            if methods:
                return {
                    'class_name': found_class_name,
                    'source': source,
                    'methods': methods,
                    'file_path': file_path
                }
        # 解析失败时输出诊断信息
        if logger:
            has_match = ALL_CLASSES_PATTERN.search(content) is not None
            if not has_match:
                logger.error(f"解析失败: {file_path}")
                logger.error(f"  原因: 正则表达式未匹配到任何类定义")
                if 'class Ark' in content:
                    logger.error(f"  提示: 文件包含 'class Ark'，但继承格式可能不匹配")
                elif 'class' in content:
                    logger.error(f"  提示: 文件包含 class，但类名不以 'Ark' 开头")
                else:
                    logger.error(f"  提示: 文件不包含 class 定义")
            elif class_name:
                logger.error(f"解析失败: {file_path}")
                logger.error(f"  原因: 未找到指定的类 '{class_name}'")
        return None

    def parse_nweb_interface(self, file_path, class_name=None):
        full_path = self.base_dir / file_path
        if not full_path.exists():
            return None
        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()
        except (FileNotFoundError, PermissionError, UnicodeDecodeError, IOError):
            return None
        # 查找 NWeb 类定义
        for match in NWEB_CLASS_PATTERN.finditer(content):
            found_class_name = match.group(1)
            if class_name and found_class_name != class_name:
                continue
            methods = self._extract_nweb_methods(content, found_class_name)
            if methods:
                return {
                    'class_name': found_class_name,
                    'methods': methods,
                    'file_path': file_path,
                    'namespace': 'OHOS::NWeb',
                    'is_nweb_interface': True
                }
        # 解析失败时输出诊断信息
        if logger:
            has_match = NWEB_CLASS_PATTERN.search(content) is not None
            if not has_match:
                logger.error(f"解析失败: {file_path}")
                logger.error(f"  原因: 正则表达式未匹配到任何类定义")
                if 'class NWeb' in content or 'class OHOS_NWEB_EXPORT' in content:
                    logger.error(f"  提示: 文件包含类定义，但格式可能不匹配")
                elif 'class' in content:
                    logger.error(f"  提示: 文件包含 class，但格式可能不匹配")
                else:
                    logger.error(f"  提示: 文件不包含 class 定义")
            elif class_name:
                logger.error(f"解析失败: {file_path}")
                logger.error(f"  原因: 未找到指定的类 '{class_name}'")
        return None

    def _extract_source_attribute(self, content):
        source_matches = list(re.finditer(r'/\*--ark web\(source=(\w+)\)--\*/', content))
        if source_matches:
            return source_matches[-1].group(1)
        return 'webcore'

    def _extract_arkts_methods(self, content, class_name):
        methods = []
        # 查找类定义的开始位置
        class_start_pattern = re.compile(
            r'class\s+' + re.escape(class_name) + r'\s*:\s*public\s+(?:virtual\s+)?ArkWebBaseRefCounted\s*\{'
        )
        class_start = class_start_pattern.search(content)
        if not class_start:
            return methods
        start_pos = class_start.end()
        brace_count = 1
        pos = start_pos
        while pos < len(content) and brace_count > 0:
            if content[pos] == '{':
                brace_count += 1
            elif content[pos] == '}':
                brace_count -= 1
            pos += 1
        class_body = content[start_pos:pos - 1]
        # 查找带 /*--ark web()--*/ 标记的方法
        lines = class_body.split('\n')
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            if '/*--ark web()--*/' in line or '/*-- ark web() --*/' in line:
                if 'virtual ' in line or 'static ' in line:
                    method_def = line.split('/*--')[0].strip()
                    j = i + 1
                    while j < len(lines) and ';' not in method_def:
                        method_def += ' ' + lines[j].strip()
                        j += 1
                    method_info = self._parse_method_definition(method_def.strip())
                    if method_info:
                        methods.append(method_info)
                    i = j
                    continue
                else:
                    # 标记在单独一行，查找方法定义
                    start_idx = i + 1
                    while start_idx < len(lines) and start_idx < i + 10:
                        next_line = lines[start_idx].strip()
                        if next_line.startswith('virtual ') or next_line.startswith('static '):
                            break
                        start_idx += 1
                    # 构建方法定义
                    method_def = ""
                    for k in range(start_idx, len(lines)):
                        method_def += lines[k]
                        if ';' in lines[k]:
                            break
                    method_info = self._parse_method_definition(method_def.strip())
                    if method_info:
                        methods.append(method_info)
                    i = start_idx + 1
                    continue
            i += 1
        return methods

    def _extract_nweb_methods(self, content, class_name):
        methods = []
        # 查找类定义
        class_pattern = re.compile(
            r'(?:class|struct)\s+(?:\[\[clang::lto_visibility_public\]\]\s+)?(?:OHOS_NWEB_EXPORT\s+)?' + re.escape(class_name) + r'\s*\{'
        )
        class_start = class_pattern.search(content)
        if not class_start:
            return methods
        start_pos = class_start.end()
        brace_count = 1
        pos = start_pos
        while pos < len(content) and brace_count > 0:
            if content[pos] == '{':
                brace_count += 1
            elif content[pos] == '}':
                brace_count -= 1
            pos += 1
        class_body = content[start_pos:pos - 1]
        # 查找所有方法（虚函数、静态方法）
        # 按行处理，避免多行匹配问题
        lines = class_body.split('\n')
        for i, line in enumerate(lines):
            line = line.strip()

            # 处理静态方法
            if line.startswith('static '):
                static_pattern = re.compile(
                    r'^static\s+(.+?)\s+(\w+)\s*\(([^)]*)\)\s*[;{]\s*$'
                )
                match = static_pattern.match(line)
                if match:
                    return_type = match.group(1).strip()
                    method_name = match.group(2)
                    params = match.group(3).strip()
                    # 检查返回类型有效性
                    if ('~' not in return_type and '{' not in return_type and
                        '}' not in return_type and ';' not in return_type):
                        methods.append({
                            'return_type': return_type,
                            'name': method_name,
                            'params': params,
                            'is_static': True,
                            'is_const': False
                        })
                continue

            # 处理虚函数
            if not line.startswith('virtual '):
                continue
            # 跳过析构函数
            if '~' in line and ('~' + class_name) in line:
                continue

            # 尝试匹配虚函数声明（支持多行格式）
            # 格式1: virtual ReturnType MethodName(params)...[const]...[= 0]...;  （单行）
            # 格式2: virtual ReturnType MethodName(params)...[const]...        （多行，{ 在下一行）
            # 格式3: virtual ReturnType MethodName(params)...[const]...{        （单行，方法体紧跟）
            virtual_pattern = re.compile(
                r'^virtual\s+(.+?)\s+(\w+)\s*\(([^)]*)\)(\s+const)?(\s*=\s*0)?\s*(?:;|\\{)?\s*$'
            )
            match = virtual_pattern.match(line)
            if match:
                return_type = match.group(1).strip()
                method_name = match.group(2)
                params = match.group(3).strip()
                is_const = match.group(4) is not None
                is_pure_virtual = match.group(5) is not None

                # 再次检查返回类型有效性
                if ('~' not in return_type and '{' not in return_type and
                    '}' not in return_type and ';' not in return_type):

                    # 如果不是纯虚函数（没有 = 0），检查下一行是否有 {
                    if not is_pure_virtual:
                        # 查看下一行（去除空白字符）
                        if i + 1 < len(lines):
                            next_line = lines[i + 1].strip()
                            # 如果下一行以 { 开头，说明这是非纯虚函数（有默认实现）
                            if next_line.startswith('{'):
                                methods.append({
                                    'return_type': return_type,
                                    'name': method_name,
                                    'params': params,
                                    'is_static': False,
                                    'is_const': is_const
                                })
                            # 如果下一行是其他内容（如其他声明），跳过此方法
                            # 这可能只是前向声明或特殊情况
                        else:
                            # 已经是最后一行，且不是纯虚函数
                            # 可能是语法错误，跳过
                            pass
                    else:
                        # 纯虚函数（= 0），直接添加
                        methods.append({
                            'return_type': return_type,
                            'name': method_name,
                            'params': params,
                            'is_static': False,
                            'is_const': is_const
                        })
        return methods

    def _parse_method_definition(self, method_def):
        """解析方法定义"""
        static_match = re.search(
            r'static\s+(.*?)\s+(\w+)\s*\((.*?)\)\s*;',
            method_def,
            re.DOTALL
        )
        if static_match:
            return {
                'return_type': static_match.group(1).strip(),
                'name': static_match.group(2),
                'params': static_match.group(3).strip(),
                'is_static': True
            }
        virtual_match = re.search(
            r'virtual\s+([^{]+?)\s+(\w+)\s*\(([^)]*)\)\s*(?:override\s*)?(?:=\s*0\s*)?;',
            method_def,
            re.DOTALL
        )
        if virtual_match:
            return {
                'return_type': virtual_match.group(1).strip(),
                'name': virtual_match.group(2),
                'params': virtual_match.group(3).strip(),
                'is_static': False
            }
        return None


class BridgeFileGenerator:
    """Bridge 文件生成器"""
    COPYRIGHT_HEADER = """/*
 * Copyright (c) {year} Huawei Device Co., Ltd.
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
"""
    MACRO_HEADER_MAP = {
        'IMPLEMENT_REFCOUNTING': 'base/include/ark_web_base_ref_counted.h',
        'CHECK_REF_PTR_IS_NULL': 'base/bridge/ark_web_bridge_macros.h',
        'CHECK_SHARED_PTR_IS_NULL': 'base/bridge/ark_web_bridge_macros.h',
        'ARK_WEB_BRIDGE_DV_LOG': 'base/bridge/ark_web_bridge_macros.h',
        'ARK_WEB_BRIDGE_INFO_LOG': 'base/bridge/ark_web_bridge_macros.h',
        'ARK_WEB_BRIDGE_WARN_LOG': 'base/bridge/ark_web_bridge_macros.h',
        'ARK_WEB_BRIDGE_ERROR_LOG': 'base/bridge/ark_web_bridge_macros.h',
    }

    def __init__(self, source_dir, output_dir, config_data=None):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.generated_files = []
        self.stats = {
            'wrapper_h': 0,
            'wrapper_cpp': 0,
            'impl_h': 0,
            'impl_cpp': 0
        }
        # 确定路径前缀
        output_dir_str = str(self.output_dir)
        path_parts = Path(output_dir_str).parts
        is_symlink_target = 'glue' in path_parts or output_dir_str.endswith('/glue') or '/glue/' in output_dir_str
        if ('ohos_glue' in path_parts or
            output_dir_str.endswith('ohos_glue') or
            output_dir_str.endswith('ohos_glue/') or
            '/ohos_glue/' in output_dir_str or
            is_symlink_target):
            self.bridge_path_prefix = ''
        else:
            self.bridge_path_prefix = 'ohos_glue/'
        # 构建枚举类型映射
        self.enum_type_map, self.nweb_return_type_map = self._build_enum_type_mapping()
        # 解析结构体映射配置
        self.struct_mappings = self._parse_struct_mappings(config_data) if config_data else {}
        # 解析 using 别名配置
        self.using_aliases = self._parse_using_aliases(config_data) if config_data else {}

    def generate_class_files(self, arkts_interface, nweb_interface, class_config):
        """为单个类生成所有 bridge 文件"""
        ark_class = arkts_interface['class_name']
        nweb_class = nweb_interface['class_name'] if nweb_interface else arkweb_to_nweb_type(ark_class)
        system = class_config['system']
        source = arkts_interface.get('source', 'webcore')
        if source == 'webcore':
            impl_dir = 'webcore'
            wrapper_dir = 'webview'
        else:
            impl_dir = 'webview'
            wrapper_dir = 'webcore'

        # 自动检测是否应该使用引用类型（从 ohos_adapter_helper.h）
        use_reference_type = class_config.get('use_reference_type', None)
        if use_reference_type is None:
            # 如果配置中没有明确指定，则从 helper 文件中自动检测
            # 脚本位于 ohos_interface/ohos_glue/scripts/，需要向上2层到 ohos_interface/
            helper_file = os.path.join(os.path.dirname(__file__), '../../include/ohos_adapter/ohos_adapter_helper.h')
            reference_type_adapters = _detect_reference_type_adapters(helper_file)
            # 检查当前 adapter 是否在引用类型列表中
            if nweb_class in reference_type_adapters:
                use_reference_type = True
                print(f"  [自动检测] {nweb_class} 使用引用类型（从 ohos_adapter_helper.h 检测）")
            else:
                use_reference_type = False
            # 将检测结果保存到配置中，供后续使用
            class_config['use_reference_type'] = use_reference_type

        # 解析 vector_headers，获取 Vector 类型映射
        vector_headers = class_config.get('vector_headers', [])
        if vector_headers:
            # base_dir 可能在配置文件中或命令行参数中
            base_dir = class_config.get('base_dir', '.')
            vector_mappings = _parse_vector_headers(vector_headers, base_dir)
            # 将解析结果保存到配置中
            class_config['vector_mappings'] = vector_mappings

        if 'impl' in class_config['generate']:
            impl_h, impl_cpp = self._generate_impl_files(
                ark_class, nweb_class, arkts_interface, nweb_interface, system, impl_dir, class_config
            )
            self._write_files(ark_class, impl_h, impl_cpp, system, impl_dir, 'impl')
        if 'wrapper' in class_config['generate']:
            wrapper_h, wrapper_cpp = self._generate_wrapper_files(
                ark_class, nweb_class, arkts_interface, nweb_interface, system, wrapper_dir, class_config
            )
            self._write_files(ark_class, wrapper_h, wrapper_cpp, system, wrapper_dir, 'wrapper')

    def print_summary(self):
        """打印生成摘要"""
        total_files = self.stats['wrapper_h'] + self.stats['wrapper_cpp'] + self.stats['impl_h'] + self.stats['impl_cpp']
        print(f"\n{'=' * 60}")
        print("生成完成！")
        print("=" * 60)
        print(f"Wrapper 头文件: {self.stats['wrapper_h']}")
        print(f"Wrapper 实现文件: {self.stats['wrapper_cpp']}")
        print(f"Impl 头文件: {self.stats['impl_h']}")
        print(f"Impl 实现文件: {self.stats['impl_cpp']}")
        print(f"总计: {total_files} 个文件")
        print("=" * 60)

    def _generate_impl_files(self, ark_class, nweb_class, arkts_interface,
                            nweb_interface, system, output_subdir, class_config):
        """生成 impl 文件（头文件和实现）"""
        snake_name = to_snake_case_for_filename(ark_class)
        year = self._extract_copyright_year_from_nweb(nweb_interface, class_config)
        nweb_header_path = class_config.get('nweb', '')
        arkweb_header_path = class_config.get('arkweb', '')
        # 检查是否使用引用类型（而不是指针类型）
        use_reference_type = class_config.get('use_reference_type', False)
        # nweb: "include/ohos_nweb/nweb_console_log.h" -> "include/nweb_console_log.h" (ohos_nweb侧)
        #       "include/ohos_adapter/location_adapter.h" -> "location_adapter.h" (ohos_adapter侧)
        # arkweb: "ohos_glue/ohos_adapter/include/ark_location_info.h" -> "ohos_adapter/include/ark_location_info.h"
        if nweb_header_path:
            parts = nweb_header_path.split('/')
            # 期望格式: include/ohos_nweb/filename.h 或 include/ohos_adapter/filename.h
            if len(parts) >= 3 and parts[0] == 'include' and parts[1].startswith('ohos_'):
                # 根据侧别决定是否加 include/ 前缀
                if system == 'ohos_adapter':
                    nweb_header_include = parts[2]  # filename.h (无 include/ 前缀)
                else:  # ohos_nweb
                    nweb_header_include = f"{parts[0]}/{parts[2]}"  # include/filename.h
            else:
                nweb_header_include = nweb_header_path
        else:
            if system == 'ohos_adapter':
                nweb_header_include = f"ohos_{to_snake_case(nweb_class)}.h"
            else:  # ohos_nweb
                nweb_header_include = f"include/ohos_{to_snake_case(nweb_class)}.h"
        # 移除 ohos_glue/ 前缀
        if arkweb_header_path.startswith('ohos_glue/'):
            arkweb_header_include = arkweb_header_path.replace('ohos_glue/', '', 1)
        else:
            arkweb_header_include = arkweb_header_path
        # include 顺序：
        # 1. ArkWeb 接口头文件（基类）
        # 2. NWeb 接口头文件（real_ 成员变量类型）
        header = self.COPYRIGHT_HEADER.format(year=year)
        # 根据 use_reference_type 决定使用引用类型还是指针类型
        if use_reference_type:
            nweb_ref_type = f'OHOS::NWeb::{nweb_class}&'
            nweb_member_type = f'OHOS::NWeb::{nweb_class}&'
        else:
            nweb_ref_type = f'std::shared_ptr<OHOS::NWeb::{nweb_class}>'
            nweb_member_type = f'std::shared_ptr<OHOS::NWeb::{nweb_class}>'
        header += f"""#ifndef {snake_name.upper()}_IMPL_H
#define {snake_name.upper()}_IMPL_H
#include "{arkweb_header_include}"
#include "{nweb_header_include}"
namespace OHOS::ArkWeb {{

class {ark_class}Impl : public {ark_class} {{

public:
    explicit {ark_class}Impl({nweb_ref_type} ref);
    ~{ark_class}Impl() = default;

"""
        for method in arkts_interface['methods']:
            return_type = method['return_type']
            name = method['name']
            params = method['params']
            is_static = method.get('is_static', False)
            if is_static:
                continue
            nweb_params = self._add_nweb_namespace_to_params(params)
            header += f"    {return_type} {name}({nweb_params}) override;\n"
        header += f"""
private:
    {nweb_member_type} real_;
    IMPLEMENT_REFCOUNTING({ark_class}Impl);
}};
}} // namespace OHOS::ArkWeb
#endif // {snake_name.upper()}_IMPL_H
"""
        macro_headers = self._collect_macro_headers(header)
        if macro_headers:
            lines = header.split('\n')
            insert_pos = 0
            for i, line in enumerate(lines):
                if line.strip().startswith('#include'):
                    insert_pos = i + 1
            for h in sorted(macro_headers):
                lines.insert(insert_pos, h)
                insert_pos += 1
            lines.insert(insert_pos, '')
            header = '\n'.join(lines)
        impl_include = f"{system}/bridge/{to_snake_case(ark_class)}_impl.h"
        wrapper_includes = set()
        for method in arkts_interface['methods']:
            params = method['params']
            refptr_params = self._parse_refptr_params_for_impl(params)
            for param_name, ark_type, wrapper_class in refptr_params:
                wrapper_snake = to_snake_case(wrapper_class)
                wrapper_includes.add(f'{system}/bridge/{wrapper_snake}.h')
        impl = self.COPYRIGHT_HEADER.format(year=year)
        impl += f"""#include "{impl_include}"
#include "base/include/ark_web_types.h"
#include "base/bridge/ark_web_bridge_macros.h"
"""
        for wrapper_include in sorted(wrapper_includes):
            impl += f'#include "{wrapper_include}"\n'
        if self._has_struct_mappings(ark_class):
            struct_utils_header = f"{system}/bridge/ark_web_{output_subdir}_struct_utils.h"
            impl += f'#include "{struct_utils_header}"\n'
        impl += f"""
namespace OHOS::ArkWeb {{

{ark_class}Impl::{ark_class}Impl({nweb_ref_type} ref) : real_(ref) {{}}

"""
        for method in arkts_interface['methods']:
            if method.get('is_static', False):
                continue
            impl += self._generate_impl_method(ark_class, nweb_class, method, use_reference_type=use_reference_type, class_config=class_config)
        impl += "\n} // namespace OHOS::ArkWeb\n"
        return header, impl

    def _parse_refptr_params_for_impl(self, params_str):
        """解析 impl 方法参数字符串，提取 ArkWebRefPtr 参数信息"""
        if not params_str or params_str.strip() == 'void':
            return []
        refptr_params = []
        for param in params_str.split(','):
            param = param.strip()
            if not param:
                continue
            match = re.match(r'const\s+ArkWebRefPtr<(\w+)>\s+(\w+)', param)
            if not match:
                match = re.match(r'ArkWebRefPtr<(\w+)>\s+(\w+)', param)
            if match:
                ark_type = match.group(1)
                param_name = match.group(2)
                wrapper_class = f'{ark_type}Wrapper'
                refptr_params.append((param_name, ark_type, wrapper_class))
        return refptr_params

    def _generate_impl_method(self, ark_class, nweb_class, method, use_reference_type=False, class_config=None):
        """生成 impl 方法实现（应用所有通用规则）

        Args:
            ark_class: ArkWeb 类名
            nweb_class: NWeb 类名
            method: 方法信息字典
            use_reference_type: 是否使用引用类型（默认 False，使用指针类型）
            class_config: 类配置字典，包含 vector_mappings 等配置
        """
        if class_config is None:
            class_config = {}
        # 获取类级别的 vector_mappings 配置
        vector_mappings = class_config.get('vector_mappings', {})
        return_type = method['return_type']
        name = method['name']
        params = method['params']
        is_static = method.get('is_static', False)
        param_names = _extract_param_names(params)
        param_str = ', '.join(param_names) if param_names else ''
        refptr_params = self._parse_refptr_params_for_impl(params)
        # 构建 NWeb 方法键（用于查询枚举映射）
        nweb_key = f"{nweb_class}::{name}"
        # 应用参数名映射
        mapped_params = self._apply_param_name_mapping(params)
        # 根据引用类型决定成员访问方式
        member_access = '.' if use_reference_type else '->'
        # 格式化函数签名：参数数量 > 3 时每个参数单独一行
        param_list = [p.strip() for p in mapped_params.split(',') if p.strip()]
        if len(param_list) > 3:
            formatted_params = ',\n    '.join(param_list)
            result = f"{return_type} {ark_class}Impl::{name}(\n    {formatted_params})\n{{\n"
        else:
            result = f"{return_type} {ark_class}Impl::{name}({mapped_params})\n{{\n"
        return_needs_wrap, wrapped_type, wrap_impl_class = self._check_refptr_return_conversion(return_type)
        has_arkweb_string_conv, arkweb_conv_code, arkweb_converted_param_str, out_params_info = \
            self._check_arkweb_string_params(params)
        # 构建方法键用于查询输出枚举参数配置
        nweb_class_for_config = ark_class
        if nweb_class_for_config.startswith('Ark'):
            nweb_class_for_config = nweb_class_for_config[3:]
        method_key = f"{nweb_class_for_config}::{name}"
        call_params = []
        param_list = [p.strip() for p in params.split(',') if p.strip()]
        # 检测是否有 vector 输出参数
        has_vector_out_param = False
        vector_out_params = []
        # 检测是否有枚举输出参数
        has_enum_out_param = False
        enum_out_params = []
        for i, (pname, param_def) in enumerate(zip(param_names, param_list)):
            param = param_def.strip()
            # 检查是否是 ArkWebString 参数（但不是 ArkWebStringVector 等派生类型）
            if has_arkweb_string_conv and _is_pure_arkweb_string_param(param):
                # ArkWebString 参数需要转换为 std::string
                call_params.append(f"ArkWebStringStructToClass({pname})")
                continue
            if _is_vector_type(param):
                vec_type = self._extract_vector_type_from_param(param)
                is_output = '&' in param and 'const' not in param
                if is_output:
                    has_vector_out_param = True
                    vector_out_params.append((pname, vec_type))
                elif vec_type:
                    call_params.append(f"{get_vector_struct_to_class_func(vec_type)}({pname})")
                else:
                    call_params.append(pname)
            else:
                # 检测枚举输出参数：引用参数 + 枚举类型
                is_ref = '&' in param
                is_const = 'const' in param
                # 检查是否是枚举类型参数
                is_enum_param = False
                enum_type = None
                if nweb_key in self.enum_type_map and pname in self.enum_type_map[nweb_key]:
                    enum_type = self.enum_type_map[nweb_key][pname]
                    is_enum_param = True
                # 如果是枚举类型的输出参数（引用 + 非 const + 枚举类型）
                if is_ref and not is_const and is_enum_param and enum_type:
                    has_enum_out_param = True
                    # 记录枚举输出参数：(参数名, NWeb枚举类型, ArkTS参数类型)
                    enum_out_params.append((pname, enum_type, param_def))
                elif is_enum_param and ('int32_t' in param or 'uint32_t' in param):
                    # 枚举输入参数：需要从 uint32_t/int32_t 转换为枚举类型
                    if 'OHOS::NWeb::' not in enum_type:
                        enum_type_full = f"OHOS::NWeb::{enum_type}"
                    else:
                        enum_type_full = enum_type
                    call_params.append(f"static_cast<{enum_type_full}>({pname})")
                else:
                    call_params.append(pname)
        is_void = return_type.strip() == 'void'
        # 对于静态方法，使用 NWeb::ClassName::Method() 调用；对于实例方法，使用 real_.MethodName() 调用
        if is_static:
            # 静态方法：OHOS::NWeb::ClassName::MethodName(...)
            method_call_target = f'OHOS::NWeb::{nweb_class}'
            static_member_access = '::'
        else:
            # 实例方法：根据 use_reference_type 决定访问方式
            method_call_target = 'real_'
            static_member_access = member_access  # 使用之前定义的 member_access 变量
        nweb_return_type = self.nweb_return_type_map.get(nweb_key, return_type)
        if refptr_params:
            for param_name, ark_type, wrapper_class in refptr_params:
                result += f"    if (CHECK_REF_PTR_IS_NULL({param_name})) {{\n"
                args = []
                for pname in param_names:
                    if pname == param_name:
                        args.append("nullptr")
                    else:
                        needs_string_conv = False
                        if has_arkweb_string_conv:
                            for i, (orig_pname, param_def) in enumerate(zip(param_names, param_list)):
                                if orig_pname == pname and 'ArkWebString' in param_def:
                                    is_output = '&' in param_def and 'const' not in param_def
                                    if not is_output:  # 输入参数需要转换
                                        needs_string_conv = True
                                    break
                        if needs_string_conv:
                            args.append(f"ArkWebStringStructToClass({pname})")
                        else:
                            args.append(pname)
                args_str = ', '.join(args) if args else ''
                if is_void:
                    if args_str:
                        result += f"        {method_call_target}{static_member_access}{name}({args_str});\n"
                    else:
                        result += f"        {method_call_target}{static_member_access}{name}();\n"
                    result += "        return;\n"
                else:
                    # 使用 static_cast 替代 C 风格转换，根据包装函数的返回类型决定转换类型
                    if args_str:
                        if _is_enum_type(nweb_return_type) and _is_basic_integer_type(return_type):
                            result += f"        return static_cast<{return_type}>({method_call_target}{static_member_access}{name}({args_str}));\n"
                        else:
                            result += f"        return {method_call_target}{static_member_access}{name}({args_str});\n"
                    else:
                        if _is_enum_type(nweb_return_type) and _is_basic_integer_type(return_type):
                            result += f"        return static_cast<{return_type}>({method_call_target}{static_member_access}{name}());\n"
                        else:
                            result += f"        return {method_call_target}{static_member_access}{name}();\n"
                result += f"    }}\n\n"
            # 构建参数列表，RefPtr 参数包装为 std::make_shared
            args = []
            for pname in param_names:
                is_refptr = False
                wrapper_class_for_param = None
                for pparam_name, park_type, pwrapper_class in refptr_params:
                    if pname == pparam_name:
                        is_refptr = True
                        wrapper_class_for_param = pwrapper_class
                        break
                needs_string_conv = False
                if has_arkweb_string_conv:
                    # 检查当前参数是否是 ArkWebString 输入参数
                    for orig_pname, orig_param_def in zip(param_names, param_list):
                        if orig_pname == pname and _is_pure_arkweb_string_param(orig_param_def):
                            is_output = '&' in orig_param_def and 'const' not in orig_param_def
                            if not is_output:  # 输入参数需要转换
                                needs_string_conv = True
                            break
                if is_refptr and wrapper_class_for_param:
                    # RefPtr 参数，包装为 std::make_shared<Wrapper>(param)
                    if needs_string_conv:
                        args.append(f"std::make_shared<{wrapper_class_for_param}>(ArkWebStringStructToClass({pname}))")
                    else:
                        args.append(f"std::make_shared<{wrapper_class_for_param}>({pname})")
                elif needs_string_conv:
                    args.append(f"ArkWebStringStructToClass({pname})")
                else:
                    # 普通参数
                    args.append(pname)
            call_param_str = ', '.join(args) if args else ''
        # 确定最终使用的调用参数字符串
        if not refptr_params:
            if has_arkweb_string_conv:
                call_param_str = arkweb_converted_param_str
            else:
                call_param_str = ', '.join(call_params) if call_params else ''
        if arkweb_conv_code:
            result += arkweb_conv_code
        # 处理输出参数：vector 和 enum 输出参数
        if has_vector_out_param or has_enum_out_param:
            # 为 vector 输出参数创建临时变量
            for out_param_name, vec_type in vector_out_params:
                if vec_type:
                    nweb_vec_type = get_nweb_vector_type(vec_type)
                    result += f"    {nweb_vec_type} {out_param_name}_temp;\n"
                else:
                    continue
            # 为 enum 输出参数创建临时变量
            for out_param_name, enum_type, _ in enum_out_params:
                if 'OHOS::NWeb::' not in enum_type:
                    enum_type_full = f"OHOS::NWeb::{enum_type}"
                else:
                    enum_type_full = enum_type
                result += f"    {enum_type_full} {out_param_name}_temp;\n"
            # 修改 call_param_str，使用临时变量
            temp_call_params = []
            for pname in param_names:
                is_output = False
                # 检查 vector 输出参数
                for out_param_name, vec_type in vector_out_params:
                    if pname == out_param_name:
                        temp_call_params.append(f"{pname}_temp")
                        is_output = True
                        break
                # 检查 enum 输出参数
                if not is_output:
                    for out_param_name, enum_type, _ in enum_out_params:
                        if pname == out_param_name:
                            temp_call_params.append(f"{pname}_temp")
                            is_output = True
                            break
                if not is_output:
                    temp_call_params.append(pname)
            temp_call_param_str = ', '.join(temp_call_params) if temp_call_params else ''
        if is_void:
            if has_vector_out_param or has_enum_out_param:
                # 使用临时变量模式
                if temp_call_param_str:
                    result += f"    {method_call_target}{static_member_access}{name}({temp_call_param_str});\n"
                else:
                    result += f"    {method_call_target}{static_member_access}{name}();\n"
                # 处理 vector 输出参数赋值
                for out_param_name, vec_type in vector_out_params:
                    conv_func = get_vector_class_to_struct_func(vec_type)
                    result += f"    {out_param_name} = {conv_func}({out_param_name}_temp);\n"
                # 处理 enum 输出参数赋值
                for out_param_name, enum_type, param_def in enum_out_params:
                    # 检测枚举的底层类型
                    if _is_uint32_enum(enum_type):
                        result += f"    {out_param_name} = static_cast<uint32_t>({out_param_name}_temp);\n"
                    else:
                        result += f"    {out_param_name} = static_cast<int32_t>({out_param_name}_temp);\n"
                result += f"}}\n\n"
            else:
                if call_param_str:
                    result += f"    {method_call_target}{static_member_access}{name}({call_param_str});\n"
                else:
                    result += f"    {method_call_target}{static_member_access}{name}();\n"
                for out_param in out_params_info:
                    if not out_param.get('is_enum_out', False):
                        result += f"    {out_param['name']} = ArkWebStringClassToStruct({out_param['temp_var']});\n"
                result += f"}}\n\n"
        elif has_vector_out_param or has_enum_out_param:
            if temp_call_param_str:
                result += f"    {return_type} ret = {method_call_target}{static_member_access}{name}({temp_call_param_str});\n"
            else:
                result += f"    {return_type} ret = {method_call_target}{static_member_access}{name}();\n"
            # 处理 vector 输出参数赋值
            for out_param_name, vec_type in vector_out_params:
                conv_func = get_vector_class_to_struct_func(vec_type)
                result += f"    {out_param_name} = {conv_func}({out_param_name}_temp);\n"
            # 处理 enum 输出参数赋值
            for out_param_name, enum_type, param_def in enum_out_params:
                if _is_uint32_enum(enum_type):
                    result += f"    {out_param_name} = static_cast<uint32_t>({out_param_name}_temp);\n"
                else:
                    result += f"    {out_param_name} = static_cast<int32_t>({out_param_name}_temp);\n"
            result += "    return ret;\n"
            result += f"}}\n\n"
        elif return_needs_wrap and wrapped_type and wrap_impl_class:
            var_name = 'result' if 'result' in return_type.lower() else 'info'
            if call_param_str:
                result += f"    {wrapped_type} {var_name} = {method_call_target}{static_member_access}{name}({call_param_str});\n"
            else:
                result += f"    {wrapped_type} {var_name} = {method_call_target}{static_member_access}{name}();\n"
            result += f"    if (CHECK_SHARED_PTR_IS_NULL({var_name})) {{\n"
            result += "        return nullptr;\n"
            result += f"    }}\n"
            result += f"    return new {wrap_impl_class}({var_name});\n"
            result += f"}}\n\n"
        elif _is_vector_type(return_type):
            vec_type = self._extract_vector_type_from_return(return_type)
            conv_func = get_vector_class_to_struct_func(vec_type, vector_mappings=vector_mappings)
            if call_param_str:
                result += f"    return {conv_func}({method_call_target}{static_member_access}{name}({call_param_str}));\n"
            else:
                result += f"    return {conv_func}({method_call_target}{static_member_access}{name}());\n"
            result += f"}}\n\n"
        elif _is_string_type(return_type):
            if call_param_str:
                result += f"    return ArkWebStringClassToStruct({method_call_target}{static_member_access}{name}({call_param_str}));\n"
            else:
                result += f"    return ArkWebStringClassToStruct({method_call_target}{static_member_access}{name}());\n"
            result += f"}}\n\n"
        elif _is_enum_type(nweb_return_type):
            # 根据包装函数的返回类型决定转换类型
            # 如果 return_type 是基本整数类型（int, int32_t, uint32_t 等），使用 static_cast
            # 否则直接返回（处理使用别名的枚举情况）
            cast_type = return_type if _is_basic_integer_type(return_type) else None
            if call_param_str:
                if cast_type:
                    result += f"    return static_cast<{cast_type}>({method_call_target}{static_member_access}{name}({call_param_str}));\n"
                else:
                    result += f"    return {method_call_target}{static_member_access}{name}({call_param_str});\n"
            else:
                if cast_type:
                    result += f"    return static_cast<{cast_type}>({method_call_target}{static_member_access}{name}());\n"
                else:
                    result += f"    return {method_call_target}{static_member_access}{name}();\n"
            result += f"}}\n\n"
        else:
            # 检查是否是结构体类型（需要转换）
            struct_mapping_info = self._get_struct_mapping_info(ark_class, return_type)
            if struct_mapping_info:
                class_to_struct_func = struct_mapping_info['class_to_struct']
                if call_param_str:
                    result += f"    return {class_to_struct_func}({method_call_target}{static_member_access}{name}({call_param_str}));\n"
                else:
                    result += f"    return {class_to_struct_func}({method_call_target}{static_member_access}{name}());\n"
                result += f"}}\n\n"
            else:
                if call_param_str:
                    result += f"    return {method_call_target}{static_member_access}{name}({call_param_str});\n"
                else:
                    result += f"    return {method_call_target}{static_member_access}{name}();\n"
                result += f"}}\n\n"
        return result

    def _generate_wrapper_files(self, ark_class, nweb_class, arkts_interface,
                                nweb_interface, system, output_subdir, class_config):
        """生成 wrapper 文件（头文件和实现）"""
        snake_name = to_snake_case_for_filename(ark_class)
        year = self._extract_copyright_year_from_nweb(nweb_interface, class_config)
        nweb_header_path = class_config.get('nweb', '')
        arkweb_header_path = class_config.get('arkweb', '')
        # nweb: "include/ohos_nweb/nweb_console_log.h" -> "include/nweb_console_log.h" (ohos_nweb侧)
        #       "include/ohos_adapter/location_adapter.h" -> "location_adapter.h" (ohos_adapter侧)
        # arkweb: "ohos_glue/ohos_adapter/include/ark_location_info.h" -> "ohos_adapter/include/ark_location_info.h"
        if nweb_header_path:
            parts = nweb_header_path.split('/')
            # 期望格式: include/ohos_nweb/filename.h 或 include/ohos_adapter/filename.h
            if len(parts) >= 3 and parts[0] == 'include' and parts[1].startswith('ohos_'):
                # 根据侧别决定是否加 include/ 前缀
                if system == 'ohos_adapter':
                    nweb_header_include = parts[2]  # filename.h (无 include/ 前缀)
                else:  # ohos_nweb
                    nweb_header_include = f"{parts[0]}/{parts[2]}"  # include/filename.h
            else:
                nweb_header_include = nweb_header_path
        else:
            if system == 'ohos_adapter':
                nweb_header_include = f"ohos_{to_snake_case(nweb_class)}.h"
            else:  # ohos_nweb
                nweb_header_include = f"include/ohos_{to_snake_case(nweb_class)}.h"
        # 移除 ohos_glue/ 前缀
        if arkweb_header_path.startswith('ohos_glue/'):
            arkweb_header_include = arkweb_header_path.replace('ohos_glue/', '', 1)
        else:
            arkweb_header_include = arkweb_header_path
        # ============ 阶段1: 分析类型依赖 ============
        dependent_types = self._collect_wrapper_dependent_types(nweb_interface, ark_class, nweb_class)
        header = self.COPYRIGHT_HEADER.format(year=year)
        header += f"""#ifndef {snake_name.upper()}_WRAPPER_H
#define {snake_name.upper()}_WRAPPER_H
#include "{nweb_header_include}"
#include "{arkweb_header_include}"
#include "base/include/ark_web_types.h"
namespace OHOS::ArkWeb {{
"""
        using_aliases = set()
        for method in nweb_interface['methods']:
            return_type = method['return_type']
            needs_alias, alias_name, actual_type = self._needs_using_alias(return_type, nweb_class)
            if needs_alias:
                using_aliases.add((alias_name, actual_type))

        config_key = self._find_using_alias_config_key(ark_class)
        if config_key and config_key in self.using_aliases:
            for nweb_type, alias_name in self.using_aliases[config_key].items():
                actual_type = nweb_type
                using_aliases.add((alias_name, actual_type))
        if using_aliases:
            for alias_name, actual_type in sorted(using_aliases):
                header += f"using {alias_name} = {actual_type};\n"
            header += "\n"
        else:
            header += "\n"
        header += f"""class {ark_class}Wrapper : public OHOS::NWeb::{nweb_class} {{
public:
    explicit {ark_class}Wrapper(ArkWebRefPtr<{ark_class}> ref);
    ~{ark_class}Wrapper() = default;

"""
        for method in nweb_interface['methods']:
            return_type = method['return_type']
            name = method['name']
            params = method['params']
            is_static = method.get('is_static', False)
            if name.startswith('~') or name == f'~{nweb_class}':
                continue
            if is_static:
                continue
            arkweb_method = None
            nweb_param_count = len(params.split(',')) if params and params.strip() != 'void' else 0

            for ark_method in arkts_interface.get('methods', []):
                if ark_method['name'] == name:
                    ark_param_count = len(ark_method['params'].split(',')) if ark_method['params'] and ark_method['params'].strip() != 'void' else 0
                    if ark_param_count == nweb_param_count:
                        arkweb_method = ark_method
                        break

            nweb_params = self._add_nweb_namespace_to_params(params, arkweb_method)
            header_params = nweb_params

            needs_alias, alias_name, actual_type = self._needs_using_alias(return_type, nweb_class)
            if needs_alias:
                display_return_type = alias_name
            else:
                display_return_type = self._add_namespace_to_type(return_type, nweb_class)
            header += f"    {display_return_type} {name}({header_params}) override;\n"
        header += f"""
private:
    ArkWebRefPtr<{ark_class}> ctocpp_;
}};
}} // namespace OHOS::ArkWeb
#endif // {snake_name.upper()}_WRAPPER_H
"""
        wrapper_include = f"{system}/bridge/{to_snake_case(ark_class)}_wrapper.h"
        impl = self.COPYRIGHT_HEADER.format(year=year)
        impl += f"""#include "{wrapper_include}"
#include "base/bridge/ark_web_bridge_macros.h"
"""
        if dependent_types:
            for dep_type in sorted(dependent_types):
                if dep_type.startswith('Ark'):
                    dep_type_without_ark = dep_type[3:]
                else:
                    dep_type_without_ark = dep_type
                dep_snake = to_snake_case(dep_type_without_ark)
                dep_header = f"{system}/bridge/ark_{dep_snake}_impl.h"
                impl += f'#include "{dep_header}"\n'
            impl += '\n'
        if self._has_struct_mappings(ark_class):
            struct_utils_header = f"{system}/bridge/ark_web_{output_subdir}_struct_utils.h"
            impl += f'#include "{struct_utils_header}"\n'
            impl += '\n'
        static_methods = []
        virtual_methods = []
        for method in nweb_interface['methods']:
            if method.get('is_static', False):
                static_methods.append(method)
            else:
                virtual_methods.append(method)

        if static_methods:
            impl += f"""
namespace OHOS::NWeb {{

"""
            for method in static_methods:
                arkweb_method = None
                nweb_param_count = len(method['params'].split(',')) if method['params'] and method['params'].strip() != 'void' else 0
                for ark_method in arkts_interface.get('methods', []):
                    if ark_method['name'] == method['name']:
                        ark_param_count = len(ark_method['params'].split(',')) if ark_method['params'] and ark_method['params'].strip() != 'void' else 0
                        if ark_param_count == nweb_param_count:
                            arkweb_method = ark_method
                            break
                impl += self._generate_nweb_static_method(nweb_class, method, arkweb_method)
            impl += "\n} // namespace OHOS::NWeb\n"

        impl += f"""
namespace OHOS::ArkWeb {{

{ark_class}Wrapper::{ark_class}Wrapper(ArkWebRefPtr<{ark_class}> ref) : ctocpp_(ref) {{}}

"""
        for method in virtual_methods:
            arkweb_method = None
            nweb_param_count = len(method['params'].split(',')) if method['params'] and method['params'].strip() != 'void' else 0

            for ark_method in arkts_interface.get('methods', []):
                if ark_method['name'] == method['name']:
                    ark_param_count = len(ark_method['params'].split(',')) if ark_method['params'] and ark_method['params'].strip() != 'void' else 0
                    if ark_param_count == nweb_param_count:
                        arkweb_method = ark_method
                        break
            impl += self._generate_wrapper_method(ark_class, nweb_class, method, arkweb_method)
        impl += "\n} // namespace OHOS::ArkWeb\n"
        return header, impl

    def _generate_nweb_static_method(self, nweb_class, method, arkweb_method=None):
        return_type = method['return_type']
        name = method['name']
        params = method['params']
        param_names = _extract_param_names(params)

        param_list = []
        if params and params.strip() != 'void':
            for i, param in enumerate(params.split(',')):
                param = param.strip()
                parts = param.rsplit(' ', 1)
                if len(parts) == 2:
                    param_type, param_name = parts[0].strip(), parts[1].strip()
                    param_names_list = list(param_names) if param_names else []
                    if i < len(param_names_list):
                        param_name = param_names_list[i]
                    param_list.append(f'{param_type} {param_name}')
                else:
                    param_list.append(param)

        params_str = ', '.join(param_list) if param_list else ''

        arkweb_call_params = []
        if params and params.strip() != 'void':
            param_names_list = list(param_names) if param_names else []
            for i, param_name in enumerate(param_names_list):
                param_type = None
                if params:
                    for j, p in enumerate(params.split(',')):
                        p = p.strip()
                        parts = p.rsplit(' ', 1)
                        if len(parts) == 2 and i == j:
                            param_type = parts[0].strip()
                            break
                if param_type and _is_enum_type(param_type):
                    arkweb_call_params.append(f'(int32_t){param_name}')
                else:
                    arkweb_call_params.append(param_name)

        arkweb_call_str = ', '.join(arkweb_call_params) if arkweb_call_params else ''

        result = f"{return_type} {nweb_class}::{name}({params_str})\n{{\n"
        if arkweb_call_str:
            result += f"    return OHOS::ArkWeb::Ark{nweb_class}::{name}({arkweb_call_str});\n"
        else:
            result += f"    return OHOS::ArkWeb::Ark{nweb_class}::{name}();\n"
        result += '}\n\n'
        return result

    def _generate_wrapper_method(self, ark_class, nweb_class, method, arkweb_method=None):
        return_type = method['return_type']
        name = method['name']
        params = method['params']
        param_names = _extract_param_names(params)

        nweb_params_for_cpp = self._add_nweb_namespace_to_params_for_cpp(params, arkweb_method)
        nweb_params_str = nweb_params_for_cpp
        arkweb_params = self._convert_arkweb_params_to_nweb(params, ark_class, name)
        needs_using_alias, using_alias_name, using_actual_type = self._needs_using_alias(return_type, nweb_class)
        if needs_using_alias:
            display_return_type = using_alias_name
        else:
            display_return_type = self._add_namespace_to_type(return_type, nweb_class)
        result = f"{display_return_type} {ark_class}Wrapper::{name}({nweb_params_for_cpp})\n{{\n"
        is_void = return_type.strip() == 'void'
        return_arkweb_type = self._extract_vector_type_from_return(return_type)
        if return_arkweb_type:
            nweb_params_list = self._extract_params_with_types(nweb_params_str)
            converted_params = []
            conversions = []
            release_codes = []
            for i, nweb_param in enumerate(nweb_params_list):
                needs_str_conv, str_name, str_type = self._needs_string_conversion(nweb_param)
                needs_ptr_check, ptr_name, ref_type, impl_class, nweb_type = self._needs_shared_ptr_check(nweb_param)
                needs_vec_conv, vec_name, vec_type, vec_conv_func = self._needs_vector_conversion(nweb_param)
                if needs_str_conv:
                    ark_var_name = f'ark_{str_name}'
                    conversions.append(f'    ArkWebString {ark_var_name} = ArkWebStringClassToStruct({str_name});')
                    converted_params.append(ark_var_name)
                    release_codes.append(f'    ArkWebStringStructRelease({ark_var_name});')
                elif needs_vec_conv:
                    ark_vec_name = f'ark_{vec_name}'
                    conversions.append(f'    {vec_type} {ark_vec_name} = {vec_conv_func}({vec_name});')
                    converted_params.append(ark_vec_name)
                    if vec_type == 'ArkWebStringVector':
                        release_codes.append(f'    ArkWebStringVectorStructRelease({ark_vec_name});')
                    else:
                        if vec_type in VECTOR_TYPE_CONFIG:
                            release_func = VECTOR_TYPE_CONFIG[vec_type]['release']
                        else:
                            # 回退到基本类型推断
                            elem_type = get_vector_element_type(vec_type)
                            release_func = f'ArkWebBasicVectorStructRelease<{vec_type}>'
                        release_codes.append(f'    {release_func}({ark_vec_name});')
                elif needs_ptr_check:
                    converted_params.append(ptr_name)
                else:
                    # 普通参数 - 但需要特殊处理 void* 和函数指针
                    param_name = None
                    if i < len(param_names):
                        param_name = param_names[i]
                    else:
                        parts = nweb_param.split()
                        if parts:
                            param_name = parts[-1].strip('&')
                    if param_name:
                        # 检查是否是 void* 参数 - 直接传递，不解引用
                        if _is_void_star_param(nweb_param):
                            converted_params.append(param_name)
                        # 检查是否是函数指针参数 - 直接传递指针，不调用
                        elif _is_function_pointer_param(nweb_param):
                            converted_params.append(param_name)
                        else:
                            converted_params.append(param_name)
            conv_func = get_vector_struct_to_class_func(return_arkweb_type)
            release_func = get_vector_release_func(return_arkweb_type)
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            if converted_param_str:
                result += f'    {return_arkweb_type} ret = ctocpp_->{name}({converted_param_str});\n'
            else:
                result += f'    {return_arkweb_type} ret = ctocpp_->{name}();\n'
            if release_codes:
                result += '\n'.join(release_codes) + '\n'
            result += f'    auto result = {conv_func}(ret);\n'
            result += f'    {release_func}(ret);\n'
            result += '    return result;\n'
            result += '}\n\n'
            return result
        # 处理 Map 返回值类型
        return_arkweb_map_type = self._extract_map_type_from_return(return_type)
        if return_arkweb_map_type:
            nweb_params_list = self._extract_params_with_types(nweb_params_str)
            converted_params = []
            conversions = []
            release_codes = []
            for i, nweb_param in enumerate(nweb_params_list):
                needs_str_conv, str_name, str_type = self._needs_string_conversion(nweb_param)
                needs_ptr_check, ptr_name, ref_type, impl_class, nweb_type = self._needs_shared_ptr_check(nweb_param)
                needs_vec_conv, vec_name, vec_type, vec_conv_func = self._needs_vector_conversion(nweb_param)
                if needs_str_conv:
                    ark_var_name = f'ark_{str_name}'
                    conversions.append(f'    ArkWebString {ark_var_name} = ArkWebStringClassToStruct({str_name});')
                    converted_params.append(ark_var_name)
                    release_codes.append(f'    ArkWebStringStructRelease({ark_var_name});')
                elif needs_vec_conv:
                    ark_vec_name = f'ark_{vec_name}'
                    conversions.append(f'    {vec_type} {ark_vec_name} = {vec_conv_func}({vec_name});')
                    converted_params.append(ark_vec_name)
                    if vec_type == 'ArkWebStringVector':
                        release_codes.append(f'    ArkWebStringVectorStructRelease({ark_vec_name});')
                    else:
                        if vec_type in VECTOR_TYPE_CONFIG:
                            release_func = VECTOR_TYPE_CONFIG[vec_type]['release']
                        else:
                            # 回退到基本类型推断
                            elem_type = get_vector_element_type(vec_type)
                            release_func = f'ArkWebBasicVectorStructRelease<{vec_type}>'
                        release_codes.append(f'    {release_func}({ark_vec_name});')
                elif needs_ptr_check:
                    converted_params.append(ptr_name)
                else:
                    # 普通参数 - 但需要特殊处理 void* 和函数指针
                    param_name = None
                    if i < len(param_names):
                        param_name = param_names[i]
                    else:
                        parts = nweb_param.split()
                        if parts:
                            param_name = parts[-1].strip('&')
                    if param_name:
                        # 检查是否是 void* 参数 - 直接传递，不解引用
                        if _is_void_star_param(nweb_param):
                            converted_params.append(param_name)
                        # 检查是否是函数指针参数 - 直接传递指针，不调用
                        elif _is_function_pointer_param(nweb_param):
                            converted_params.append(param_name)
                        else:
                            converted_params.append(param_name)
            conv_func = get_map_struct_to_class_func(return_arkweb_map_type)
            release_func = get_map_release_func(return_arkweb_map_type)
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            if converted_param_str:
                result += f'    {return_arkweb_map_type} ret = ctocpp_->{name}({converted_param_str});\n'
            else:
                result += f'    {return_arkweb_map_type} ret = ctocpp_->{name}();\n'
            if release_codes:
                result += '\n'.join(release_codes) + '\n'
            result += f'    auto result = {conv_func}(ret);\n'
            result += f'    {release_func}(ret);\n'
            result += '    return result;\n'
            result += '}\n\n'
            return result
        # 处理 List 返回值类型
        return_arkweb_list_type = self._extract_list_type_from_return(return_type)
        if return_arkweb_list_type:
            nweb_params_list = self._extract_params_with_types(nweb_params_str)
            converted_params = []
            conversions = []
            release_codes = []
            for i, nweb_param in enumerate(nweb_params_list):
                needs_str_conv, str_name, str_type = self._needs_string_conversion(nweb_param)
                needs_ptr_check, ptr_name, ref_type, impl_class, nweb_type = self._needs_shared_ptr_check(nweb_param)
                needs_vec_conv, vec_name, vec_type, vec_conv_func = self._needs_vector_conversion(nweb_param)
                if needs_str_conv:
                    ark_var_name = f'ark_{str_name}'
                    conversions.append(f'    ArkWebString {ark_var_name} = ArkWebStringClassToStruct({str_name});')
                    converted_params.append(ark_var_name)
                    release_codes.append(f'    ArkWebStringStructRelease({ark_var_name});')
                elif needs_vec_conv:
                    ark_vec_name = f'ark_{vec_name}'
                    conversions.append(f'    {vec_type} {ark_vec_name} = {vec_conv_func}({vec_name});')
                    converted_params.append(ark_vec_name)
                    if vec_type == 'ArkWebStringVector':
                        release_codes.append(f'    ArkWebStringVectorStructRelease({ark_vec_name});')
                    else:
                        if vec_type in VECTOR_TYPE_CONFIG:
                            release_func = VECTOR_TYPE_CONFIG[vec_type]['release']
                        else:
                            # 回退到基本类型推断
                            elem_type = get_vector_element_type(vec_type)
                            release_func = f'ArkWebBasicVectorStructRelease<{vec_type}>'
                        release_codes.append(f'    {release_func}({ark_vec_name});')
                elif needs_ptr_check:
                    converted_params.append(ptr_name)
                else:
                    # 普通参数 - 但需要特殊处理 void* 和函数指针
                    param_name = None
                    if i < len(param_names):
                        param_name = param_names[i]
                    else:
                        parts = nweb_param.split()
                        if parts:
                            param_name = parts[-1].strip('&')
                    if param_name:
                        # 检查是否是 void* 参数 - 直接传递，不解引用
                        if _is_void_star_param(nweb_param):
                            converted_params.append(param_name)
                        # 检查是否是函数指针参数 - 直接传递指针，不调用
                        elif _is_function_pointer_param(nweb_param):
                            converted_params.append(param_name)
                        else:
                            converted_params.append(param_name)
            conv_func = get_list_struct_to_class_func(return_arkweb_list_type)
            release_func = get_list_release_func(return_arkweb_list_type)
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            if converted_param_str:
                result += f'    {return_arkweb_list_type} ret = ctocpp_->{name}({converted_param_str});\n'
            else:
                result += f'    {return_arkweb_list_type} ret = ctocpp_->{name}();\n'
            if release_codes:
                result += '\n'.join(release_codes) + '\n'
            result += f'    auto result = {conv_func}(ret);\n'
            result += f'    {release_func}(ret);\n'
            result += '    return result;\n'
            result += '}\n\n'
            return result
        # 处理 Pair Vector 返回值类型
        return_arkweb_pair_vector_type = self._extract_pair_vector_type_from_return(return_type)
        if return_arkweb_pair_vector_type:
            nweb_params_list = self._extract_params_with_types(nweb_params_str)
            converted_params = []
            conversions = []
            release_codes = []
            for i, nweb_param in enumerate(nweb_params_list):
                needs_str_conv, str_name, str_type = self._needs_string_conversion(nweb_param)
                needs_ptr_check, ptr_name, ref_type, impl_class, nweb_type = self._needs_shared_ptr_check(nweb_param)
                needs_vec_conv, vec_name, vec_type, vec_conv_func = self._needs_vector_conversion(nweb_param)
                if needs_str_conv:
                    ark_var_name = f'ark_{str_name}'
                    conversions.append(f'    ArkWebString {ark_var_name} = ArkWebStringClassToStruct({str_name});')
                    converted_params.append(ark_var_name)
                    release_codes.append(f'    ArkWebStringStructRelease({ark_var_name});')
                elif needs_vec_conv:
                    ark_vec_name = f'ark_{vec_name}'
                    conversions.append(f'    {vec_type} {ark_vec_name} = {vec_conv_func}({vec_name});')
                    converted_params.append(ark_vec_name)
                    if vec_type == 'ArkWebStringVector':
                        release_codes.append(f'    ArkWebStringVectorStructRelease({ark_vec_name});')
                    else:
                        if vec_type in VECTOR_TYPE_CONFIG:
                            release_func = VECTOR_TYPE_CONFIG[vec_type]['release']
                        else:
                            # 回退到基本类型推断
                            elem_type = get_vector_element_type(vec_type)
                            release_func = f'ArkWebBasicVectorStructRelease<{vec_type}>'
                        release_codes.append(f'    {release_func}({ark_vec_name});')
                elif needs_ptr_check:
                    converted_params.append(ptr_name)
                else:
                    # 普通参数 - 但需要特殊处理 void* 和函数指针
                    param_name = None
                    if i < len(param_names):
                        param_name = param_names[i]
                    else:
                        parts = nweb_param.split()
                        if parts:
                            param_name = parts[-1].strip('&')
                    if param_name:
                        # 检查是否是 void* 参数 - 直接传递，不解引用
                        if _is_void_star_param(nweb_param):
                            converted_params.append(param_name)
                        # 检查是否是函数指针参数 - 直接传递指针，不调用
                        elif _is_function_pointer_param(nweb_param):
                            converted_params.append(param_name)
                        else:
                            converted_params.append(param_name)
            conv_func = get_pair_vector_struct_to_class_func(return_arkweb_pair_vector_type)
            release_func = get_pair_vector_release_func(return_arkweb_pair_vector_type)
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            if converted_param_str:
                result += f'    {return_arkweb_pair_vector_type} ret = ctocpp_->{name}({converted_param_str});\n'
            else:
                result += f'    {return_arkweb_pair_vector_type} ret = ctocpp_->{name}();\n'
            if release_codes:
                result += '\n'.join(release_codes) + '\n'
            result += f'    auto result = {conv_func}(ret);\n'
            result += f'    {release_func}(ret);\n'
            result += '    return result;\n'
            result += '}\n\n'
            return result
        needs_return_conv = _is_string_type(return_type)
        arkweb_return_type = 'ArkWebString' if needs_return_conv else None
        needs_return_wrap, wrapper_class, ark_return_type = self._needs_return_wrapper(return_type)
        needs_using_alias, using_alias_name, using_actual_type = self._needs_using_alias(return_type, nweb_class)
        # 分析参数
        conversions = []
        shared_ptr_checks = []
        converted_params = []
        release_codes = []
        vector_output_params = []  # 存储vector输出参数信息
        enum_ref_params = []  # 存储枚举引用参数信息
        nweb_params_list = self._extract_params_with_types(nweb_params_str)
        for i, nweb_param in enumerate(nweb_params_list):
            is_vec_out, elem_type, p_name, arkts_vec_type, underlying_type, conv_func, rel_func = self._is_vector_output_param(nweb_param)
            needs_str_conv, str_name, str_type = self._needs_string_conversion(nweb_param)
            needs_ptr_check, ptr_name, ref_type, impl_class, nweb_type = self._needs_shared_ptr_check(nweb_param)
            needs_vec_conv, vec_name, vec_type, vec_conv_func = self._needs_vector_conversion(nweb_param)
            is_enum_ref, enum_type, enum_param_name = _is_enum_ref_param(nweb_param)
            if is_enum_ref:
                # 枚举引用参数：需要创建临时 uint32_t 变量
                enum_ref_params.append((enum_param_name, enum_type))
                converted_params.append(enum_param_name)  # 使用原始参数名
            elif is_vec_out:
                # 1. 创建临时 ArkWeb vector 变量
                # 2. 在调用时使用临时变量
                # 4. 释放临时变量
                vector_output_params.append((p_name, arkts_vec_type, underlying_type, conv_func, rel_func))
                converted_params.append(p_name)  # 普通参数列表中使用原始参数名
            elif needs_str_conv:
                ark_var_name = f'ark_{str_name}'
                # 检查是否是字符串输出参数（std::string& 不带 const）
                if self._is_string_output_param(nweb_param):
                    # 输出参数：不先转换，创建空的 ArkWebString 变量
                    conversions.append(f'    ArkWebString {ark_var_name};')
                    converted_params.append(ark_var_name)
                    # 添加输出参数信息，以便在方法调用后转换
                    vector_output_params.append((str_name, 'ArkWebString', 'std::string', 'ArkWebStringStructToClass', 'ArkWebStringStructRelease'))
                else:
                    # 输入参数：先转换为 ArkWebString
                    conversions.append(f'    ArkWebString {ark_var_name} = ArkWebStringClassToStruct({str_name});')
                    converted_params.append(ark_var_name)
                    release_codes.append(f'    ArkWebStringStructRelease({ark_var_name});')
            elif needs_vec_conv:
                ark_vec_name = f'ark_{vec_name}'
                conversions.append(f'    {vec_type} {ark_vec_name} = {vec_conv_func}({vec_name});')
                converted_params.append(ark_vec_name)
                if vec_type == 'ArkWebStringVector':
                    release_codes.append(f'    ArkWebStringVectorStructRelease({ark_vec_name});')
                else:
                    if vec_type in VECTOR_TYPE_CONFIG:
                        release_func = VECTOR_TYPE_CONFIG[vec_type]['release']
                    else:
                        # 回退到基本类型推断
                        elem_type = get_vector_element_type(vec_type)
                        release_func = f'ArkWebBasicVectorStructRelease<{vec_type}>'
                    release_codes.append(f'    {release_func}({ark_vec_name});')
            elif needs_ptr_check:
                shared_ptr_checks.append((ptr_name, impl_class))
                converted_params.append(ptr_name)
            else:
                # 普通参数 - 但需要特殊处理 void* 和函数指针
                param_name = None
                if i < len(param_names):
                    param_name = param_names[i]
                else:
                    parts = nweb_param.split()
                    if parts:
                        param_name = parts[-1].strip('&')
                if param_name:
                    # 检查是否是 void* 参数 - 直接传递，不解引用
                    if _is_void_star_param(nweb_param):
                        converted_params.append(param_name)
                    # 检查是否是函数指针参数 - 直接传递指针，不调用
                    elif _is_function_pointer_param(nweb_param):
                        converted_params.append(param_name)
                    else:
                        converted_params.append(param_name)
        if shared_ptr_checks:
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
                if not is_void:
                    result += f'    {return_type} result;\n\n'
                for ptr_name, impl_class in shared_ptr_checks:
                    result += f'    if (CHECK_SHARED_PTR_IS_NULL({ptr_name})) {{\n'
                    null_args = []
                    for j, cp in enumerate(converted_params):
                        if cp == ptr_name:
                            null_args.append('nullptr')
                        else:
                            null_args.append(cp)
                    null_args_str = ', '.join(null_args)
                    if is_void:
                        result += f'        ctocpp_->{name}({null_args_str});\n'
                    else:
                        result += f'        result = ctocpp_->{name}({null_args_str});\n'
                    result += f'    }} else {{\n'
                # 非空分支：创建 impl 对象
                impl_creates = []
                for j, cp in enumerate(converted_params):
                    is_ptr = False
                    for ptr_name, impl_class in shared_ptr_checks:
                        if cp == ptr_name:
                            is_ptr = True
                            impl_creates.append(f'new {impl_class}({ptr_name})')
                            break
                    if not is_ptr:
                        impl_creates.append(cp)
                impl_args_str = ', '.join(impl_creates)
                if is_void:
                    result += f'        ctocpp_->{name}({impl_args_str});\n'
                else:
                    result += f'        result = ctocpp_->{name}({impl_args_str});\n'
                # 关闭所有 else 分支
                for _ in shared_ptr_checks:
                    result += f'    }}\n'
                if release_codes:
                    result += '\n' + '\n'.join(release_codes) + '\n'
                if not is_void:
                    result += '    return result;\n'
            else:
                # 有 shared_ptr 参数检查的情况
                # 如果返回值是枚举类型，在开始处定义 result 变量
                if not is_void and _is_enum_type(return_type):
                    result += f'    int32_t result;\n'
                # 检查每个 shared_ptr 参数
                for ptr_name, impl_class in shared_ptr_checks:
                    result += f'    if (CHECK_SHARED_PTR_IS_NULL({ptr_name})) {{\n'
                    null_args = []
                    for j, cp in enumerate(converted_params):
                        if cp == ptr_name:
                            null_args.append('nullptr')
                        else:
                            null_args.append(cp)
                    null_args_str = ', '.join(null_args)
                    if is_void:
                        result += f'        ctocpp_->{name}({null_args_str});\n'
                        result += '        return;\n'
                    else:
                        # 检查是否是枚举返回值类型
                        if _is_enum_type(return_type):
                            result += f'        result = ctocpp_->{name}({null_args_str});\n'
                            if 'OHOS::NWeb::' in return_type:
                                result += f'        return ({return_type})result;\n'
                            else:
                                result += f'        return (OHOS::NWeb::{return_type})result;\n'
                        else:
                            result += f'        return ctocpp_->{name}({null_args_str});\n'
                    result += f'    }}\n'
                impl_creates = []
                for j, cp in enumerate(converted_params):
                    is_ptr = False
                    for ptr_name, impl_class in shared_ptr_checks:
                        if cp == ptr_name:
                            is_ptr = True
                            impl_creates.append(f'new {impl_class}({ptr_name})')
                            break
                    if not is_ptr:
                        impl_creates.append(cp)
                impl_args_str = ', '.join(impl_creates)
                if is_void:
                    result += f'    ctocpp_->{name}({impl_args_str});\n'
                else:
                    # 检查是否是枚举返回值类型
                    if _is_enum_type(return_type):
                        result += f'    result = ctocpp_->{name}({impl_args_str});\n'
                        if 'OHOS::NWeb::' in return_type:
                            result += f'    return ({return_type})result;\n'
                        else:
                            result += f'    return (OHOS::NWeb::{return_type})result;\n'
                    else:
                        result += f'    return ctocpp_->{name}({impl_args_str});\n'
            result += '}\n\n'
            return result
        # 处理枚举引用参数
        if enum_ref_params:
            # 为每个枚举引用参数创建临时 uint32_t 变量
            for param_name, enum_type in enum_ref_params:
                result += f'    uint32_t {param_name}_temp = static_cast<uint32_t>({param_name});\n'
            result += '\n'
            # 构建调用参数列表（使用临时变量）
            arkts_call_params = []
            for cp in converted_params:
                is_enum = False
                for param_name, enum_type in enum_ref_params:
                    if cp == param_name:
                        arkts_call_params.append(f'{param_name}_temp')
                        is_enum = True
                        break
                if not is_enum:
                    arkts_call_params.append(cp)
            # 调用 ArkWeb 方法
            arkts_call_str = ', '.join(arkts_call_params) if arkts_call_params else ''
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            if is_void:
                result += f'    ctocpp_->{name}({arkts_call_str});\n'
            else:
                result += f'    {return_type} result = ctocpp_->{name}({arkts_call_str});\n'
            # 将结果转换回枚举类型
            for param_name, enum_type in enum_ref_params:
                # 确保枚举类型有完整的命名空间前缀
                if 'OHOS::NWeb::' not in enum_type:
                    full_enum_type = f'OHOS::NWeb::{enum_type}'
                else:
                    full_enum_type = enum_type
                result += f'    {param_name} = static_cast<{full_enum_type}>({param_name}_temp);\n'
            if release_codes:
                result += '\n' + '\n'.join(release_codes) + '\n'
            if not is_void:
                result += '    return result;\n'
            result += '}\n\n'
            return result
        if vector_output_params:
            # 为每个 vector 输出参数创建临时变量（只创建不在 conversions 中的）
            created_vars = set()
            # 检查 conversions 列表中已经创建的变量名
            for conv_line in conversions:
                if '=' in conv_line:
                    # 赋值语句，提取变量名
                    var_name = conv_line.strip().split()[1].rstrip(';')
                    created_vars.add(var_name)
                elif conv_line.strip().endswith(';'):
                    # 声明语句，提取变量名
                    parts = conv_line.strip().split()
                    if len(parts) >= 2:
                        var_name = parts[1].rstrip(';')
                        created_vars.add(var_name)
            # 只为未创建的变量创建声明
            for p_name, arkts_vec_type, underlying_type, conv_func, rel_func in vector_output_params:
                # 使用参数名生成唯一的临时变量名
                temp_var_name = f'ark_{p_name}'
                if temp_var_name not in created_vars:
                    result += f'    {arkts_vec_type} {temp_var_name};\n'
            result += '\n'
            # 构建 ArkTS 调用参数列表（使用临时变量）
            arkts_call_params = []
            for i, cp in enumerate(converted_params):
                is_output = False
                for p_name, arkts_vec_type, underlying_type, conv_func, rel_func in vector_output_params:
                    if cp == p_name:
                        temp_var_name = f'ark_{p_name}'
                        arkts_call_params.append(temp_var_name)
                        is_output = True
                        break
                if not is_output:
                    arkts_call_params.append(cp)
            if conversions:
                result += '\n'.join(conversions) + '\n\n'
            # 调用 ArkTS 方法
            arkts_call_str = ', '.join(arkts_call_params) if arkts_call_params else ''
            if is_void:
                result += f'    ctocpp_->{name}({arkts_call_str});\n'
            else:
                result += f'    {return_type} result = ctocpp_->{name}({arkts_call_str});\n'
            for p_name, arkts_vec_type, underlying_type, conv_func, rel_func in vector_output_params:
                temp_var_name = f'ark_{p_name}'
                result += f'    {p_name} = {conv_func}({temp_var_name});\n'
            # 释放 vector 临时变量
            for p_name, arkts_vec_type, underlying_type, conv_func, rel_func in vector_output_params:
                temp_var_name = f'ark_{p_name}'
                result += f'    {rel_func}({temp_var_name});\n'
            # 释放其他临时变量
            if release_codes:
                result += '\n'
                result += '\n'.join(release_codes) + '\n'
            if not is_void:
                result += '    return result;\n'
            result += '}\n\n'
            return result
        if conversions:
            result += '\n'.join(conversions) + '\n\n'
            if is_void:
                result += f'    ctocpp_->{name}({", ".join(converted_params)});\n'
            else:
                # 检测返回值是否是枚举类型，使用基础类型作为变量类型
                if _is_enum_type(return_type):
                    # 枚举类型：用 int32_t 作为变量类型，最后转换
                    result += f'    int32_t result = ctocpp_->{name}({", ".join(converted_params)});\n'
                else:
                    # 非枚举类型：直接使用 return_type
                    result += f'    {return_type} result = ctocpp_->{name}({", ".join(converted_params)});\n'
            result += '\n'
            result += '\n'.join(release_codes) + '\n'
            if not is_void:
                # 枚举类型需要转换返回值
                if _is_enum_type(return_type):
                    if 'OHOS::NWeb::' in return_type:
                        result += f'    return ({return_type})result;\n'
                    else:
                        result += f'    return (OHOS::NWeb::{return_type})result;\n'
                else:
                    result += '    return result;\n'
        elif needs_return_wrap:
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            # 使用简单的变量名
            if 'Info' in ark_return_type:
                var_name = 'ark_info'
            elif 'Callback' in ark_return_type:
                var_name = 'ark_callback'
            elif 'Adapter' in ark_return_type:
                base_name = ark_return_type.replace('Ark', '').replace('Adapter', '').lower()
                var_name = f'ark_{base_name}' if base_name else 'ark_adapter'
            else:
                var_name = 'ark_result'
            result += f'    ArkWebRefPtr<{ark_return_type}> {var_name} = ctocpp_->{name}({converted_param_str});\n'
            result += f'    if (CHECK_REF_PTR_IS_NULL({var_name})) {{\n'
            result += '        return nullptr;\n'
            result += f'    }}\n'
            result += f'    return std::make_shared<{wrapper_class}>({var_name});\n'
        elif needs_return_conv:
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            result += f'    {arkweb_return_type} str = ctocpp_->{name}({converted_param_str});\n'
            result += f'    {return_type} result = ArkWebStringStructToClass(str);\n'
            result += '    ArkWebStringStructRelease(str);\n'
            result += '    return result;\n'
        elif needs_using_alias:
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            result += f'    return static_cast<{using_alias_name}>(ctocpp_->{name}({converted_param_str}));\n'
        else:
            converted_param_str = ', '.join(converted_params) if converted_params else ''
            if is_void:
                result += f'    ctocpp_->{name}({converted_param_str});\n'
            else:
                # 检查是否是枚举返回值类型，如果是则需要转换
                if _is_enum_type(return_type):
                    # ArkWeb 侧返回 int32_t，需要转换为 NWeb 枚举类型
                    result += f'    int32_t result = ctocpp_->{name}({converted_param_str});\n'
                    if 'OHOS::NWeb::' in return_type:
                        result += f'    return ({return_type})result;\n'
                    else:
                        result += f'    return (OHOS::NWeb::{return_type})result;\n'
                else:
                    # 检查是否是结构体类型（需要转换）
                    # 在 wrapper 侧，返回值类型是 NWeb 类型（如 DateTime），需要查找对应的映射
                    struct_mapping_info = self._get_struct_mapping_info_by_nweb_type(ark_class, return_type)
                    if struct_mapping_info:
                        struct_to_class_func = struct_mapping_info['struct_to_class']
                        result += f'    return {struct_to_class_func}(ctocpp_->{name}({converted_param_str}));\n'
                    else:
                        result += f'    return ctocpp_->{name}({converted_param_str});\n'
        result += '}\n\n'
        return result

    def _collect_wrapper_dependent_types(self, nweb_interface, ark_class, nweb_class):
        """收集 wrapper 方法中使用的所有依赖类型
        返回: set - 依赖的 ArkWeb 类型名集合（不含当前类）
        """
        dependent_types = set()
        for method in nweb_interface.get('methods', []):
            return_type = method.get('return_type', '')
            dependent_types.update(self._extract_arkweb_types(return_type, ark_class))
            # 分析参数类型
            params = method.get('params', '')
            dependent_types.update(self._extract_arkweb_types(params, ark_class))
        # 移除当前类自身
        dependent_types.discard(ark_class)
        return dependent_types

    def _convert_arkweb_to_nweb_type(self, arkweb_type):
        """将 ArkWeb 类型转换为 NWeb 类型，保留 const 和引用修饰符"""
        # 保留 const 前缀和引用后缀
        has_const = arkweb_type.startswith('const ')
        has_ref = arkweb_type.endswith('&')
        type_to_convert = arkweb_type
        if has_const:
            type_to_convert = type_to_convert[6:]  # 去除 'const '
        if has_ref:
            type_to_convert = type_to_convert[:-1]  # 去除 '&'
        type_to_convert = type_to_convert.strip()
        if 'ArkWebRefPtr<' in type_to_convert:
            start = type_to_convert.find('ArkWebRefPtr<') + len('ArkWebRefPtr<')
            end = type_to_convert.find('>', start)
            if end > start:
                ref_type = type_to_convert[start:end].strip()
                # 去掉 Ark 前缀
                if ref_type.startswith('Ark'):
                    nweb_type = ref_type[3:]
                else:
                    nweb_type = ref_type
                converted = f'std::shared_ptr<OHOS::NWeb::{nweb_type}>'
                # 恢复 const 和引用修饰符
                if has_const:
                    converted = 'const ' + converted
                if has_ref:
                    converted = converted + '&'
                return converted
        type_map = [
            ('ArkWebStringVector', 'std::vector<std::string>'),
            ('ArkWebString', 'std::string'),
        ]
        for ark_type, nweb_type in type_map:
            if ark_type in type_to_convert:
                converted = type_to_convert.replace(ark_type, nweb_type)
                # 恢复 const 和引用修饰符
                if has_const:
                    converted = 'const ' + converted
                if has_ref:
                    converted = converted + '&'
                return converted
        converted = type_to_convert
        if has_const:
            converted = 'const ' + converted
        if has_ref:
            converted = converted + '&'
        return converted

    def _convert_nweb_params_to_static(self, params_str, param_names):
        """将 NWeb 参数列表转换为静态方法使用的参数（枚举转 int32_t）

        Args:
            params_str: NWeb 参数字符串
            param_names: 参数名列表

        Returns:
            str: 转换后的参数字符串
        """
        if not params_str or params_str.strip() == 'void':
            return params_str

        result_params = []
        for i, param in enumerate(params_str.split(',')):
            param = param.strip()
            # 分离类型和名称
            parts = param.rsplit(' ', 1)
            if len(parts) == 2:
                param_type, param_name = parts[0].strip(), parts[1].strip()
                # 从参数名列表中获取正确的参数名
                param_names_list = list(param_names) if param_names else []
                if i < len(param_names_list):
                    param_name = param_names_list[i]
                else:
                    param_name = f'param{i}'
                # 转换枚举类型为 int32_t（ArkWeb 静态方法使用 int32_t）
                base_type = param_type.split('::')[-1]
                if _is_enum_type(param_type):
                    param_type = 'int32_t'
                result_params.append(f'{param_type} {param_name}')
            else:
                result_params.append(param)
        return ', '.join(result_params)

    def _convert_arkweb_params_to_nweb(self, params_str, wrapper_class_name=None, method_name=None):
        """将参数列表中的 ArkWeb 类型转换为 NWeb 类型，保留参数名"""
        if not params_str or params_str.strip() == 'void':
            return params_str
        result = []
        for param in params_str.split(','):
            param = param.strip()
            if not param:
                continue
            # 分离类型和参数名
            parts = param.split()
            if len(parts) >= 2:
                # 最后一部分是参数名（可能带有 &）
                param_name_part = parts[-1]
                param_name = param_name_part.rstrip('&').strip()
                # 前面的部分是类型
                type_part = ' '.join(parts[:-1])
                converted_type = self._convert_arkweb_to_nweb_type(type_part)
                # 重新组合
                if '&' in param_name_part:
                    result.append(f'{converted_type}& {param_name}')
                else:
                    result.append(f'{converted_type} {param_name}')
            else:
                # 单个词，可能是类型或参数名
                result.append(param)
        return ', '.join(result)

    def _add_namespace_to_type(self, type_str, nweb_class):
        """为类型添加 OHOS::NWeb:: 命名空间前缀

        处理以下情况：
        1. 基本类型：int32_t, bool, void 等 -> 保持不变
        2. 标准库类型：std::string, std::vector<T> 等 -> 保持不变
        3. 模板类型：std::shared_ptr<T>, std::vector<T> 等 -> 为 T 添加命名空间
        4. 自定义类型：AudioDeviceDescAdapter -> OHOS::NWeb::AudioDeviceDescAdapter

        Args:
            type_str: 类型字符串
            nweb_class: NWeb 类名
        Returns:
            str: 添加命名空间后的类型字符串
        """
        if not type_str:
            return type_str

        # 如果已经有 OHOS::NWeb:: 前缀，直接返回
        if 'OHOS::NWeb::' in type_str:
            return type_str

        # 如果有 NWeb:: 前缀，替换为 OHOS::NWeb::
        if 'NWeb::' in type_str:
            result = type_str.replace('NWeb::', 'OHOS::NWeb::')
            return result

        # 检查是否是基本类型
        if _is_basic_type(type_str) or type_str == 'void':
            return type_str

        # 检查是否是标准库类型（以 std:: 开头）
        if type_str.startswith('std::'):
            # 对于模板类型，需要处理模板参数
            # 匹配 std::shared_ptr<T>, std::vector<T>, std::vector<std::shared_ptr<T>> 等
            template_match = re.match(r'(std::[a-z_]+<)(.+)(>)', type_str)
            if template_match:
                template_prefix = template_match.group(1)
                template_args = template_match.group(2)
                template_suffix = template_match.group(3)


                # 递归处理模板参数
                processed_args = self._process_template_args(template_args)
                result = f'{template_prefix}{processed_args}{template_suffix}'
                return result

            # 非模板的 std:: 类型（如 std::string）
            return type_str

        # 自定义类型，添加 OHOS::NWeb:: 前缀
        result = f'OHOS::NWeb::{type_str}'
        return result

    def _process_template_args(self, args_str):
        """处理模板参数，为每个参数添加命名空间

        Args:
            args_str: 模板参数字符串，可能包含多个参数（如 T, U 或 T, std::vector<U>）
        Returns:
            str: 处理后的模板参数字符串
        """
        # 简单处理：按逗号分割，但需要考虑嵌套的尖括号

        # 处理单个模板参数或多个参数
        # 由于可能有嵌套的模板（如 std::vector<std::shared_ptr<T>>），需要仔细解析

        # 尝试匹配常见的模板参数模式
        # 1. 单个参数：如 AudioDeviceDescAdapter
        # 2. 多个参数：如 T, U（暂时不处理，因为当前代码不使用）

        # 处理 std::shared_ptr<T> 或 T 的情况
        args_str = args_str.strip()

        # 如果参数以 std:: 开头，递归处理
        if args_str.startswith('std::'):
            return self._add_namespace_to_type(args_str, '')

        # 如果参数包含 >，可能是嵌套模板（如 std::shared_ptr<T>::value_type）
        # 这种情况比较复杂，暂时只处理简单情况

        # 如果已经包含命名空间，直接返回
        if 'OHOS::NWeb::' in args_str:
            return args_str
        if 'NWeb::' in args_str:
            return args_str.replace('NWeb::', 'OHOS::NWeb::')

        # 否则添加命名空间（检查是否是非基本类型的自定义类型）
        if not _is_basic_type(args_str) and not args_str.startswith('std::'):
            return f'OHOS::NWeb::{args_str}'

        return args_str

    def _add_nweb_namespace_to_params(self, params_str, arkweb_method=None):
        """为参数类型添加 OHOS::NWeb:: 命名空间前缀（用于 wrapper 方法声明）

        修复：正确处理 Type& name 格式的参数（如 NetConnectType& type）
        如果 nweb 侧没有参数名，则使用 arkweb 侧的参数名

        Args:
            params_str: nweb 参数字符串
            arkweb_method: arkweb 方法信息字典（用于获取参数名）
        """
        # 从 arkweb 方法中提取参数名列表
        arkweb_param_names = []
        if arkweb_method and 'params' in arkweb_method:
            arkweb_param_names = _extract_param_names(arkweb_method['params'])

        if not params_str or params_str.strip() == 'void':
            return params_str
        result = []
        for param in params_str.split(','):
            param = param.strip()
            if not param:
                continue
            # 分离类型和参数名
            # 支持 "Type name" 和 "Type& name" 两种格式
            # 注意：参数名可能是 C++ 关键字，需要处理
            parts = param.split()
            if len(parts) >= 2:
                # 参数名是最后一部分（可能带有 &）
                param_name_part = parts[-1]
                param_name = param_name_part.rstrip('&').strip()
                # 类型是参数名之前的所有部分
                type_part = ' '.join(parts[:-1])
                # 如果参数名部分有 &，则类型也包含 &
                if '&' in param_name_part and '&' not in type_part:
                    type_part = type_part + '&'
                # 保存 const 修饰符和引用符号
                has_const = type_part.startswith('const')
                has_ref = type_part.endswith('&')
                if has_const:
                    type_part = type_part[6:].strip()  # 移除 'const'
                if has_ref:
                    type_part = type_part[:-1].strip()  # 移除 '&'

                # 对于 std::string 类型，不需要添加命名空间前缀
                is_std_string = (type_part == 'std::string')

                if not is_std_string and 'NWeb::' not in type_part and 'OHOS::ArkWeb::' not in type_part and 'OHOS::NWeb::' not in type_part:
                    # 1. std::shared_ptr<T> -> std::shared_ptr<OHOS::NWeb::T>
                    # 2. 枚举类型 -> OHOS::NWeb::EnumType
                    # 3. 基本类型（int32_t, bool 等）保持不变
                    # 4. std::string 保持不变

                    # 优先检查是否是 shared_ptr 类型
                    shared_ptr_match = re.match(r'(std::shared_ptr<)([^>]+)(>)', type_part)
                    if shared_ptr_match:
                        inner_type = shared_ptr_match.group(2).strip()
                        if '::' not in inner_type:
                            # 为所有非基本类型添加 OHOS::NWeb:: 命名空间
                            inner_type = f'OHOS::NWeb::{inner_type}'
                        type_part = f'std::shared_ptr<{inner_type}>'
                    elif _is_enum_type(type_part):
                        # 枚举类型：添加命名空间前缀
                        type_part = f'OHOS::NWeb::{type_part}'
                # 恢复 const 修饰符和引用符号
                if has_ref:
                    type_part = type_part + '&'
                if has_const:
                    type_part = f'const {type_part}'
                # 重新组合
                result.append(f'{type_part} {param_name}')
            else:
                # 没有显式参数名的情况（如 "std::shared_ptr<VpnListener>"）
                # 检查是否是 shared_ptr 类型
                shared_ptr_match = re.match(r'(std::shared_ptr<)([^>]+)(>)', param)
                if shared_ptr_match:
                    # 为内部类型添加命名空间前缀（如果需要）
                    inner_type = shared_ptr_match.group(2).strip()
                    if '::' not in inner_type:
                        # 为所有非基本类型添加 OHOS::NWeb:: 命名空间
                        inner_type = f'OHOS::NWeb::{inner_type}'
                    # 优先使用 arkweb 侧的参数名，否则生成默认参数名
                    param_index = len(result)
                    if param_index < len(arkweb_param_names):
                        param_name = arkweb_param_names[param_index]
                    else:
                        param_name = f'param{param_index}'
                    type_part = f'std::shared_ptr<{inner_type}>'
                    result.append(f'{type_part} {param_name}')
                else:
                    # 其他类型，检查是否需要添加命名空间前缀
                    type_part = param.rstrip('&').strip()
                    has_ref = param.endswith('&')
                    if has_ref:
                        type_part = type_part[:-1].strip()
                    # 保存 const 修饰符
                    has_const = type_part.startswith('const')
                    if has_const:
                        type_part = type_part[6:].strip()
                    # 检查是否需要添加命名空间前缀
                    # 对于 std::string 类型，不需要添加命名空间前缀
                    is_std_string = (type_part == 'std::string')
                    if not is_std_string and 'NWeb::' not in type_part and 'OHOS::ArkWeb::' not in type_part and 'OHOS::NWeb::' not in type_part:
                        if _is_enum_type(type_part):
                            type_part = f'OHOS::NWeb::{type_part}'
                    # 恢复 const 修饰符和引用符号
                    if has_ref:
                        type_part = type_part + '&'
                    if has_const:
                        type_part = f'const {type_part}'
                    # 优先使用 arkweb 侧的参数名，否则生成默认参数名
                    param_index = len(result)
                    if param_index < len(arkweb_param_names):
                        param_name = arkweb_param_names[param_index]
                    else:
                        param_name = f'param{param_index}'
                    result.append(f'{type_part} {param_name}')
        return ', '.join(result)

    def _add_nweb_namespace_to_params_for_cpp(self, params_str, arkweb_method=None):
        """为参数类型添加 OHOS::NWeb:: 命名空间前缀（用于 wrapper cpp 方法签名，与头文件保持一致）

        Args:
            params_str: nweb 参数字符串
            arkweb_method: arkweb 方法信息字典（用于获取参数名）
        """
        # wrapper cpp 的参数类型应该与 wrapper 头文件保持一致
        return self._add_nweb_namespace_to_params(params_str, arkweb_method)

    def _check_refptr_return_conversion(self, return_type):
        """检查返回值是否需要 RefPtr 包装
        Args:
            return_type: 返回值类型字符串
        Returns:
            tuple: (needs_wrap, wrapped_type, impl_class)
                - needs_wrap: 是否需要包装
                - wrapped_type: 包装后的类型（NWeb 侧完整类型）
                - impl_class: impl 类名
        """
        if 'ArkWebRefPtr<' in return_type:
            match = re.search(r'ArkWebRefPtr<(.+?)>', return_type)
            if match:
                inner_type = match.group(1).strip()
                # 将 ArkWeb 侧类型转换为 NWeb 侧类型
                # ArkDisplayAdapter -> DisplayAdapter, ArkXXX -> XXX
                nweb_inner_type = inner_type
                if inner_type.startswith('Ark'):
                    nweb_inner_type = inner_type[3:]  # 去掉 'Ark' 前缀
                # 包装为 std::shared_ptr<OHOS::NWeb::NWebType>
                # 因为 impl 文件在 namespace OHOS::ArkWeb 中，需要完整命名空间
                wrapped_type = f'std::shared_ptr<OHOS::NWeb::{nweb_inner_type}>'
                impl_class = inner_type + 'Impl'
                return (True, wrapped_type, impl_class)
        return (False, None, None)

    def _check_arkweb_string_params(self, params_str):
        """检查参数中是否包含 ArkWebString，需要转换为 std::string
        Args:
            params_str: 参数字符串
        Returns:
            tuple: (has_conversion, conv_code, converted_param_str, out_params_info)
                - has_conversion: 是否有转换
                - conv_code: 转换代码
                - converted_param_str: 转换后的参数字符串
                - out_params_info: 输出参数信息列表
        """
        if not params_str or params_str.strip() == 'void':
            return (False, '', '', [])
        param_names = _extract_param_names(params_str)
        param_list = [p.strip() for p in params_str.split(',') if p.strip()]
        has_arkweb_string = False
        conv_code_parts = []
        converted_parts = []
        out_params_info = []
        for i, (pname, param_def) in enumerate(zip(param_names, param_list)):
            if 'ArkWebString' in param_def:
                has_arkweb_string = True
                if '&' in param_def and 'const' not in param_def:
                    # 输出参数
                    temp_var_name = f'n{pname[0].upper() if pname else ""}{pname[1:] if len(pname) > 1 else ""}'
                    conv_code_parts.append(f'    std::string {temp_var_name};')
                    out_params_info.append({
                        'name': pname,
                        'type': param_def,
                        'temp_var': temp_var_name,
                        'is_enum_out': False,
                    })
                    converted_parts.append(temp_var_name)
                else:
                    # 输入参数
                    converted_parts.append(f'ArkWebStringStructToClass({pname})')
            else:
                converted_parts.append(pname)
        if has_arkweb_string:
            conv_code = '\n'.join(conv_code_parts) if conv_code_parts else ''
            if conv_code:
                conv_code += '\n'
            return (True, conv_code, ', '.join(converted_parts), out_params_info)
        return (False, '', '', [])

    def _needs_return_wrapper(self, nweb_return_type):
        """
        检查 wrapper 方法的返回类型是否需要将 ArkWebRefPtr 包装成 std::shared_ptr<Wrapper>
        支持两种格式:
        1. std::shared_ptr<OHOS::NWeb::XXXAdapter> (带完整命名空间)
        2. std::shared_ptr<XXXAdapter> (不带命名空间)
        """
        # 检查带完整命名空间的类型: std::shared_ptr<OHOS::NWeb::XXXAdapter>
        if 'std::shared_ptr<OHOS::NWeb::' in nweb_return_type:
            start = nweb_return_type.find('std::shared_ptr<OHOS::NWeb::') + len('std::shared_ptr<OHOS::NWeb::')
            end = nweb_return_type.find('>', start)
            if end > start:
                nweb_type = nweb_return_type[start:end].strip()
                # 特殊类型映射
                type_mapping = {
                    'WebLocationInfo': 'ArkLocationInfo',
                }
                if nweb_type in type_mapping:
                    ark_type = type_mapping[nweb_type]
                else:
                    if nweb_type.startswith('Web'):
                        ark_type = 'Ark' + nweb_type
                    else:
                        ark_type = 'Ark' + nweb_type
                wrapper_class = ark_type + 'Wrapper'
                return True, wrapper_class, ark_type

        # 新增：检查不带命名空间的类型: std::shared_ptr<XXXAdapter>
        # 这处理了类似 std::shared_ptr<AudioDeviceDescAdapter> 的情况
        elif 'std::shared_ptr<' in nweb_return_type:
            match = re.match(r'std::shared_ptr<([A-Z][a-zA-Z0-9_]*)>', nweb_return_type)
            if match:
                nweb_type = match.group(1)
                # 检查是否是 Adapter 类型或其他需要包装的 ArkWeb 类型
                # 包括 Adapter, Callback, Handler, Info, Data, Config, Request, Chooser, Params, Details, Options, Event, List, Item, Type
                arkweb_type_patterns = ('Adapter', 'Callback', 'Handler', 'Info', 'Data', 'Config', 'Request', 'Chooser', 'Params', 'Details', 'Options', 'Event', 'List', 'Item', 'Type')
                if nweb_type.endswith(arkweb_type_patterns):
                    # 特殊类型映射
                    type_mapping = {
                        'WebLocationInfo': 'ArkLocationInfo',
                    }
                    if nweb_type in type_mapping:
                        ark_type = type_mapping[nweb_type]
                    else:
                        if nweb_type.startswith('Web'):
                            ark_type = 'Ark' + nweb_type
                        else:
                            ark_type = 'Ark' + nweb_type
                    wrapper_class = ark_type + 'Wrapper'
                    return True, wrapper_class, ark_type

        return False, None, None

    def _needs_string_conversion(self, param_str):
        """检查参数是否需要字符串转换
        Args:
            param_str: 参数字符串，如 "const std::string& msg"
        Returns:
            tuple: (needs_conv, param_name, param_type)
                - needs_conv: 是否需要转换
                - param_name: 参数名
                - param_type: 参数类型
        """
        param_str = param_str.strip()

        # 匹配字符串类型参数（包含参数名）
        # 模式: (const)? std::string (&)? 参数名
        # 例如: "const std::string& msg", "std::string& name", "const std::string msg"
        pattern = r'^(const\s+)?std::string(\&)?\s+(\w+)$'
        match = re.match(pattern, param_str)

        if match:
            # 提取参数名（最后一个捕获组）
            param_name = match.group(3)
            # 重构类型部分（去掉参数名）
            const_part = match.group(1) if match.group(1) else ''
            ref_part = match.group(2) if match.group(2) else '&'
            param_type = f'{const_part}std::string{ref_part}'
            return (True, param_name, param_type)

        # 也检查不带参数名的纯类型字符串（兼容性）
        if _is_string_type(param_str):
            parts = param_str.split()
            if parts:
                param_name = parts[-1].strip('&').strip()
                return (True, param_name, param_str)

        return (False, None, None)

    def _is_string_output_param(self, param_str):
        """检查参数是否是字符串输出参数（std::string& 不带 const）
        Args:
            param_str: 参数字符串
        Returns:
            bool: 是否是字符串输出参数
        """
        param_str = param_str.strip()
        # 检查是否是 std::string& 格式（有 & 但没有 const）
        return ('std::string&' in param_str and 'const std::string&' not in param_str)

    def _needs_shared_ptr_check(self, param_str):
        """检查参数是否需要智能指针空指针检查
        Args:
            param_str: 参数字符串，如 "std::shared_ptr<OHOS::NWeb::MMIListenerAdapter> eventCallback"
        Returns:
            tuple: (needs_check, param_name, ref_type, impl_class, nweb_type)
                - needs_check: 是否需要检查
                - param_name: 参数名
                - ref_type: ArkWebRefPtr 类型
                - impl_class: impl 类名
                - nweb_type: NWeb 类型
        """
        param_str = param_str.strip()
        if _is_shared_ptr_type(param_str):
            parts = param_str.split()
            if parts:
                param_name = parts[-1].strip('>').strip()
                inner_type = _extract_shared_ptr_inner_type(param_str)
                if inner_type:
                    impl_class = _get_shared_ptr_impl_class(inner_type)
                    return (True, param_name, f'ArkWebRefPtr<{inner_type}>', impl_class, inner_type)
        return (False, None, None, None, None)

    def _needs_vector_conversion(self, param_str):
        """检查参数是否需要 vector 转换（输入参数）
        Args:
            param_str: 参数字符串
        Returns:
            tuple: (needs_conv, param_name, vec_type, conv_func)
                - needs_conv: 是否需要转换
                - param_name: 参数名
                - vec_type: vector 类型（如 ArkWebStringVector）
                - conv_func: 转换函数名
        """
        param_str = param_str.strip()
        vec_type = self._extract_vector_type_from_param(param_str)
        if vec_type:
            parts = param_str.split()
            if parts:
                param_name = parts[-1].strip('>').strip()
                conv_func = get_vector_class_to_struct_func(vec_type)
                return (True, param_name, vec_type, conv_func)
        return (False, None, None, None)

    def _is_vector_output_param(self, param_str):
        """检查参数是否是 vector 输出参数（std::vector<T>&，不带 const）
        Args:
            param_str: 参数字符串
        Returns:
            tuple: (is_output, element_type, param_name, arkts_vec_type, underlying_type, class_to_struct_func, struct_to_class_func, release_func)
        """
        # 使用已有的 _is_basic_vector_output_param 函数
        return _is_basic_vector_output_param(param_str)

    def _extract_params_with_types(self, params_str):
        """提取参数及其类型
        Args:
            params_str: 参数字符串，如 "int32_t type, const std::string& msg"
        Returns:
            参数类型列表，如 ["int32_t type", "const std::string& msg"]
        """
        if not params_str or params_str.strip() == 'void':
            return []
        params = []
        # 分割参数（考虑模板和括号）
        param_str = params_str.strip()
        brace_count = 0
        current_param = ""
        for char in param_str:
            if char == '<':
                brace_count += 1
                current_param += char
            elif char == '>':
                brace_count -= 1
                current_param += char
            elif char == ',' and brace_count == 0:
                params.append(current_param.strip())
                current_param = ""
            else:
                current_param += char
        if current_param.strip():
            params.append(current_param.strip())
        return params

    def _extract_vector_type_from_param(self, param_str):
        """从参数字符串中提取 Vector 类型（返回 ArkWeb 类型）"""
        param_str = param_str.strip()
        # 检查 ArkWeb 类型匹配
        for config in VECTOR_TYPE_CONFIG.values():
            arkweb_type = config['arkweb_type']
            if arkweb_type in param_str:
                return arkweb_type
            if config['nweb_type'] in param_str:
                return arkweb_type
        return None

    def _extract_vector_type_from_return(self, return_type):
        """从返回类型中提取 Vector 类型（返回 ArkWeb 类型）"""
        return_type = return_type.strip()

        # 首先检查 ArkWeb 类型直接匹配
        for config in VECTOR_TYPE_CONFIG.values():
            if config['arkweb_type'] in return_type:
                return config['arkweb_type']

        # 检查 NWeb 类型匹配（包括 std::vector<element_type> 格式）
        for config in VECTOR_TYPE_CONFIG.values():
            nweb_type = config['nweb_type']
            element_type = config['element_type']
            if nweb_type in return_type or f'std::vector<{element_type}>' in return_type:
                return config['arkweb_type']

        # 检查 Adapter Vector 类型
        for config in ADAPTER_VECTOR_TYPE_CONFIG.values():
            if config['arkweb_type'] in return_type:
                return config['arkweb_type']

        # 检查 Adapter Vector 的 NWeb 类型变体
        for config in ADAPTER_VECTOR_TYPE_CONFIG.values():
            nweb_type = config['nweb_type']
            element_type = config['element_type']
            # 检查 NWeb 类型的各种变体
            if nweb_type in return_type:
                return config['arkweb_type']
            # 检查 std::vector<std::shared_ptr<XXXAdapter>> 格式
            if f'std::vector<std::shared_ptr<{element_type}>>' in return_type:
                return config['arkweb_type']
            # 检查简化版 std::vector<std::shared_ptr<XXX>> (去掉 Adapter 后缀)
            adapter_name = config.get('adapter_name', '')
            if adapter_name and f'std::vector<std::shared_ptr<{adapter_name}Adapter>>' in return_type:
                return config['arkweb_type']
        return None

    def _extract_map_type_from_return(self, return_type):
        """从返回类型中提取 Map 类型（返回 ArkWeb 类型）"""
        return_type = return_type.strip()
        for map_type, config in MAP_TYPE_CONFIG.items():
            arkweb_type = config['arkweb_type']
            if arkweb_type in return_type:
                return arkweb_type  # 返回 ArkWeb 类型
        for map_type, config in MAP_TYPE_CONFIG.items():
            nweb_type = config['nweb_type']
            if nweb_type in return_type:
                return config['arkweb_type']  # 返回 ArkWeb 类型
        return None

    def _extract_list_type_from_return(self, return_type):
        """从返回类型中提取 List 类型（返回 ArkWeb 类型）"""
        return_type = return_type.strip()
        for list_type, config in LIST_TYPE_CONFIG.items():
            arkweb_type = config['arkweb_type']
            if arkweb_type in return_type:
                return arkweb_type  # 返回 ArkWeb 类型
        for list_type, config in LIST_TYPE_CONFIG.items():
            nweb_type = config['nweb_type']
            if nweb_type in return_type:
                return config['arkweb_type']  # 返回 ArkWeb 类型
        return None

    def _extract_pair_vector_type_from_return(self, return_type):
        """从返回类型中提取 Pair Vector 类型（返回 ArkWeb 类型）"""
        return_type = return_type.strip()
        for pair_vector_type, config in PAIR_VECTOR_TYPE_CONFIG.items():
            arkweb_type = config['arkweb_type']
            if arkweb_type in return_type:
                return arkweb_type  # 返回 ArkWeb 类型
        for pair_vector_type, config in PAIR_VECTOR_TYPE_CONFIG.items():
            nweb_type = config['nweb_type']
            if nweb_type in return_type:
                return config['arkweb_type']  # 返回 ArkWeb 类型
        return None

    def _extract_arkweb_types(self, type_string, exclude_class):
        """从类型字符串中提取 ArkWeb 类型名
        例如:
        - "std::shared_ptr<ArkAudioCallback>" → {"ArkAudioCallback"}
        - "ArkWebRefPtr<ArkMMIListenerAdapter>" → {"ArkMMIListenerAdapter"}
        - "std::shared_ptr<MMIListenerAdapter>" → {"ArkMMIListenerAdapter"} (添加Ark前缀)
        - "int32_t" → {}
        """
        types = set()
        ref_ptr_pattern = r'(?:ArkWebRefPtr|std::shared_ptr)\s*<\s*([A-Za-z][A-Za-z0-9_]*)\s*>'
        for match in re.finditer(ref_ptr_pattern, type_string):
            type_name = match.group(1)
            # 例如: MMIListenerAdapter -> ArkMMIListenerAdapter
            if not type_name.startswith('Ark'):
                type_name = f"Ark{type_name}"
            if type_name != exclude_class:
                types.add(type_name)
        return types

    def _extract_copyright_year_from_nweb(self, nweb_interface, class_config):
        """从 NWeb 接口文件提取版权年份"""
        nweb_header_path = class_config.get('nweb', '')
        if nweb_header_path:
            full_path = Path(self.source_dir) / nweb_header_path
            if full_path.exists():
                try:
                    with open(full_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                    match = re.search(r'Copyright.*\(c\)\s*(\d{4})', content)
                    if match:
                        return match.group(1)
                except (FileNotFoundError, PermissionError, UnicodeDecodeError, IOError):
                    pass
        return datetime.now().year

    def _get_default_value_for_type(self, return_type):
        """获取类型的默认值
        Args:
            return_type: 返回值类型字符串
        Returns:
            默认值字符串
        """
        return_type = return_type.strip()
        if return_type == 'void':
            return ''
        elif return_type == 'bool':
            return 'false'
        elif return_type in ['int', 'int32_t']:
            return '0'
        elif return_type in ['unsigned', 'uint32_t']:
            return '0U'
        elif return_type in ['int64_t', 'long']:
            return '0L'
        elif return_type == 'float':
            return '0.0f'
        elif return_type == 'double':
            return '0.0'
        elif 'ArkWebString' in return_type:
            return 'ArkWebString()'
        elif 'ArkWebRefPtr' in return_type:
            return 'nullptr'
        elif 'ArkWeb' in return_type and 'Vector' in return_type:
            return f'{return_type}()'
        else:
            return '0'

    def _apply_param_name_mapping(self, params_str):
        """应用参数名映射，修正不清晰的参数名
        Args:
            params_str: 参数字符串，如 "int32_t type, const std::string& msg"
        Returns:
            映射后的参数字符串
        """
        return params_str

    def _write_files(self, ark_class, header_content, impl_content, system, subdir, file_type):
        """写入生成的文件"""
        snake_name = to_snake_case(ark_class)
        if self.bridge_path_prefix:
            base_dir = self.output_dir / self.bridge_path_prefix / system / 'bridge' / subdir
        else:
            base_dir = self.output_dir / system / 'bridge' / subdir
        base_dir.mkdir(parents=True, exist_ok=True)
        # 写入头文件
        header_path = base_dir / f"{snake_name}_{file_type}.h"
        if header_path.exists():
            with open(header_path, 'r', encoding='utf-8') as f:
                existing_content = f.read()
            if existing_content != header_content:
                with open(header_path, 'w', encoding='utf-8') as f:
                    f.write(header_content)
                self.generated_files.append(str(header_path))
                print(f"Generated: {header_path}", flush=True)
        else:
            with open(header_path, 'w', encoding='utf-8') as f:
                f.write(header_content)
            self.generated_files.append(str(header_path))
            print(f"Generated: {header_path}", flush=True)
        self.stats[f'{file_type}_h'] += 1
        # 写入实现文件
        impl_path = base_dir / f"{snake_name}_{file_type}.cpp"
        if impl_path.exists():
            with open(impl_path, 'r', encoding='utf-8') as f:
                existing_content = f.read()
            if existing_content != impl_content:
                with open(impl_path, 'w', encoding='utf-8') as f:
                    f.write(impl_content)
                self.generated_files.append(str(impl_path))
                print(f"Generated: {impl_path}", flush=True)
        else:
            with open(impl_path, 'w', encoding='utf-8') as f:
                f.write(impl_content)
            self.generated_files.append(str(impl_path))
            print(f"Generated: {impl_path}", flush=True)
        self.stats[f'{file_type}_cpp'] += 1

    def _collect_macro_headers(self, code_content):
        """
        扫描代码内容，收集使用的宏对应的头文件
        实现"用谁就 include 谁"规则，确保生成的代码直接包含它使用的所有宏、类型、函数的头文件
        不依赖间接包含（传递依赖），使每个生成的文件都能独立编译
        Args:
            code_content: 生成的代码内容
        Returns:
            set: 需要包含的头文件集合（格式: '#include "path/to/header.h"'）
        """
        headers = set()
        for macro, header in self.MACRO_HEADER_MAP.items():
            if macro in code_content:
                headers.add(f'#include "{header}"')
        return headers

    def _build_enum_type_mapping(self):
        """扫描 NWeb 接口定义文件，建立枚举类型映射
        通过分析 NWeb 接口定义中的方法签名，学习 int32_t 到实际枚举类型的映射关系
        Returns:
            tuple: (param_mapping, return_mapping)
                  - param_mapping: {class_method: param_type_mapping}
                    格式: {"NWebClassName::MethodName": {"param_name": "NWebEnumType"}}
                  - return_mapping: {class_method: return_type}
                    格式: {"NWebClassName::MethodName": "ReturnType"}
        """
        mapping = {}
        return_mapping = {}
        # 扫描 NWeb 接口定义文件
        nweb_include_dir = self.source_dir / 'include'
        if not nweb_include_dir.exists():
            return mapping, return_mapping
        # 扫描 ohos_adapter 和 ohos_nweb 目录
        for component in ['ohos_adapter', 'ohos_nweb']:
            include_dir = nweb_include_dir / component
            if not include_dir.exists():
                continue
            for header_file in include_dir.glob('*.h'):
                try:
                    with open(header_file, 'r', encoding='utf-8') as f:
                        content = f.read()
                    # 查找所有接口类定义和方法签名
                    class_pattern = r'class\s+(?:OHOS_NWEB_EXPORT\s+)?([A-Z]\w*(?:Adapter|Callback|Listener|Info|Config|Options|Params|Data|Log|Source|Type|State|Mode|Result|Event|Request|Response|Handler|Manager|Client|Proxy|Wrapper|Helper|Exception|Error))\s*[:\{]'
                    for class_match in re.finditer(class_pattern, content):
                        nweb_class = class_match.group(1)
                        class_start = class_match.end()
                        # 查找类定义的结束位置（下一个 class 关键字或文件结尾）
                        rest_of_content = content[class_start:]
                        next_class_match = re.search(r'\n\s*class\s+\w', rest_of_content)
                        if next_class_match:
                            class_end = class_start + next_class_match.start()
                            class_body = content[class_start:class_end]
                        else:
                            class_body = rest_of_content
                        method_pattern = r'virtual\s+(.+?)\s+(\w+)\s*\(([^)]*)\)\s*[=;{]'
                        for method_match in re.finditer(method_pattern, class_body):
                            return_type = method_match.group(1).strip()
                            method_name = method_match.group(2)
                            params_str = method_match.group(3)
                            key = f"{nweb_class}::{method_name}"
                            return_mapping[key] = return_type
                            for param in params_str.split(','):
                                param = param.strip()
                                if not param:
                                    continue
                                # 例如: "AudioCaptureSourceTypeAdapter type"
                                # 或: "NetConnectType& type" (带引用)
                                # 先去除尾部的引用符号，再匹配
                                param_for_match = param.rstrip('&').strip()
                                param_match = re.match(r'(.+?)\s+(\w+)$', param_for_match)
                                if param_match:
                                    param_type = param_match.group(1).strip()
                                    param_name = param_match.group(2)
                                    # 通用化枚举类型检测（排除法）
                                    # 基本类型列表
                                    basic_types = {
                                        'int8_t', 'uint8_t', 'int16_t', 'uint16_t',
                                        'int32_t', 'uint32_t', 'int64_t', 'uint64_t',
                                        'bool', 'char', 'wchar_t', 'float', 'double', 'size_t',
                                        'void'
                                    }
                                    # 标准库类型模式
                                    std_patterns = ('std::', '::std::')
                                    # 先去除 const，再去除引用/指针符号
                                    param_type_clean = param_type.replace('const', '').replace('Const', '')
                                    param_type_base = param_type_clean.rstrip('*&').strip()
                                    is_basic_type = param_type_base in basic_types or any(
                                        param_type_base == t or param_type_base.startswith(t + ' ') or param_type_base.startswith(t + '*')
                                        for t in basic_types
                                    )
                                    has_std_type = any(p in param_type for p in std_patterns)
                                    # 通用化检测：不是基本类型、不是标准库类型 → 当作枚举类型
                                    is_enum_type = not is_basic_type and not has_std_type
                                    if is_enum_type:
                                        # 清理枚举类型，移除 const 和引用修饰符
                                        enum_type_clean = param_type_clean.rstrip('*&').strip()
                                        key = f"{nweb_class}::{method_name}"
                                        if key not in mapping:
                                            mapping[key] = {}
                                        mapping[key][param_name] = enum_type_clean
                except Exception:
                    pass
        return mapping, return_mapping

    def _parse_struct_mappings(self, config_data):
        """从配置数据中解析结构体映射
        Args:
            config_data: 配置数据字典，包含 classes 配置
        Returns:
            dict: 结构体映射字典，格式为 {class_name: {arkweb_type: mapping_info}}
        """
        struct_mappings = {}
        if not config_data or 'classes' not in config_data:
            return struct_mappings

        classes = config_data['classes']
        # 兼容两种格式：字典格式和数组格式
        if isinstance(classes, dict):
            for class_name, class_config in classes.items():
                if 'struct_mappings' in class_config:
                    struct_mappings[class_name] = class_config['struct_mappings']
        elif isinstance(classes, list):
            for class_config in classes:
                class_name = class_config.get('name')
                if class_name and 'struct_mappings' in class_config:
                    struct_mappings[class_name] = class_config['struct_mappings']

        return struct_mappings

    def _parse_using_aliases(self, config_data):
        """从配置数据中解析 using 别名映射
        Args:
            config_data: 配置数据字典，包含 classes 配置
        Returns:
            dict: using 别名映射字典，格式为 {class_name: {nweb_type: arkweb_alias}}
        """
        using_aliases = {}
        if not config_data or 'classes' not in config_data:
            return using_aliases

        classes = config_data['classes']
        # 兼容两种格式：字典格式和数组格式
        if isinstance(classes, dict):
            for class_name, class_config in classes.items():
                if 'using_aliases' in class_config:
                    using_aliases[class_name] = class_config['using_aliases']
        elif isinstance(classes, list):
            for class_config in classes:
                class_name = class_config.get('name')
                if class_name and 'using_aliases' in class_config:
                    using_aliases[class_name] = class_config['using_aliases']

        return using_aliases

    def _find_using_alias_config_key(self, ark_class):
        """查找 using 别名的配置键，支持多种前缀格式
        Args:
            ark_class: ArkWeb 类名（如 "ArkWebConsoleLog"）
        Returns:
            str or None: 找到的配置键，如果未找到返回 None
        """
        # 首先检查是否直接存在
        if ark_class in self.using_aliases:
            return ark_class

        # 生成所有可能的配置键变体（按优先级排序）
        possible_keys = []

        # 基于前缀的各种变换
        if ark_class.startswith('ArkWeb'):
            base = ark_class[6:]  # 去掉 "ArkWeb"
            # 按优先级：精确匹配 > 去掉前缀 > 添加前缀
            possible_keys.extend([
                'Ark' + base,                       # ArkConsoleLog
                'NWeb' + base,                      # NWebConsoleLog
                'Web' + base,                       # WebConsoleLog
                base,                              # ConsoleLog（去掉前缀）
            ])
        elif ark_class.startswith('Ark'):
            base = ark_class[3:]  # 去掉 "Ark"
            possible_keys.extend([
                'ArkWeb' + base,                    # ArkWebConsoleLog（尝试添加 Web）
                'NWeb' + base,                      # NWebConsoleLog
                'Web' + base,                       # WebConsoleLog
                'Ark' + base,                       # ArkConsoleLog（直接用 Ark）
                base,                              # ConsoleLog（去掉前缀）
            ])
        else:
            # 没有 Ark 前缀，尝试添加前缀
            possible_keys.extend([
                'ArkWeb' + ark_class,                # ArkWebConsoleLog
                'Ark' + ark_class,                   # ArkConsoleLog
                'NWeb' + ark_class,                  # NWebConsoleLog
            ])

        # 去重并过滤空字符串，保持优先级顺序
        seen = set()
        unique_keys = []
        for key in possible_keys:
            if key and key not in seen:
                seen.add(key)
                unique_keys.append(key)

        # 按优先级查找第一个存在的键
        for key in unique_keys:
            if key in self.using_aliases:
                return key

        return None

    def _get_using_alias_info(self, ark_class, nweb_type):
        """获取指定类型的 using 别名信息
        Args:
            ark_class: ArkWeb 类名
            nweb_type: NWeb 类型名称（短名称，不含命名空间）
        Returns:
            tuple or None: (alias_name, actual_type) 如果未找到返回 None
        """
        config_key = self._find_using_alias_config_key(ark_class)
        if not config_key:
            return None

        aliases = self.using_aliases[config_key]
        alias_info = aliases.get(nweb_type)

        if not alias_info:
            return None

        # 支持两种格式：
        # 1. 简单格式：字符串别名
        # 2. 扩展格式：包含 alias 和 actual_type 的对象
        if isinstance(alias_info, str):
            # 简单格式：只返回别名，实际类型需要在其他地方推导
            return alias_info, None
        elif isinstance(alias_info, dict):
            # 扩展格式：同时包含别名和实际类型
            return alias_info.get('alias'), alias_info.get('actual_type')

        return None

    def _needs_using_alias(self, return_type, nweb_class):
        """检查返回类型是否需要使用 using 别名（使用类级别配置）
        Args:
            return_type: 返回类型字符串
            nweb_class: NWeb 类名
        Returns:
            tuple: (needs_alias, alias_name, actual_type)
        """
        # 查找配置键
        ark_class = 'Ark' + nweb_class if not nweb_class.startswith('NWeb') else 'Ark' + nweb_class[4:]
        config_key = self._find_using_alias_config_key(ark_class)

        # 如果配置了 using_aliases，使用别名
        if config_key and config_key in self.using_aliases:
            aliases = self.using_aliases[config_key]
            # 1. 优先匹配完整类型路径
            if return_type in aliases:
                return True, aliases[return_type], return_type

            # 2. 尝试补全为完整路径后匹配
            full_type = f'OHOS::NWeb::{nweb_class}::{return_type}'
            if full_type in aliases:
                return True, aliases[full_type], full_type

            # 3. 尝试基础类型名称匹配
            base_type = return_type.split('::')[-1]
            if base_type in aliases:
                actual_type = return_type if 'OHOS::NWeb::' in return_type else f'OHOS::NWeb::{return_type}'
                return True, aliases[base_type], actual_type

        # 没有配置，返回 False，使用完整命名空间类型
        return False, None, None

    def _find_struct_mapping_key(self, ark_class):
        """查找结构体映射的配置键，支持多种前缀格式
        Args:
            ark_class: ArkWeb 类名（如 "ArkWebDateTimeChooser"）
        Returns:
            str or None: 找到的配置键，如果未找到返回 None
        """
        # 首先检查是否直接存在
        if ark_class in self.struct_mappings:
            return ark_class

        # 生成所有可能的配置键变体（按优先级排序）
        possible_keys = []

        # 基于前缀的各种变换
        if ark_class.startswith('ArkWeb'):
            base = ark_class[6:]  # 去掉 "ArkWeb"
            # 按优先级：精确匹配 > 去掉前缀 > 添加前缀
            possible_keys.extend([
                'Ark' + base,                       # ArkDateTimeChooser（精确匹配 Ark 部分）
                base,                              # DateTimeChooser（去掉前缀）
                'NWeb' + base,                      # NWebDateTimeChooser
                'Web' + base,                       # WebDateTimeChooser
                ark_class,                         # ArkWebDateTimeChooser（完整）
            ])
        elif ark_class.startswith('Ark'):
            base = ark_class[3:]  # 去掉 "Ark"
            possible_keys.extend([
                'ArkWeb' + base,                    # ArkWebDateTimeChooser（尝试添加 Web）
                'Ark' + base,                       # ArkDateTimeChooser（直接用 Ark）
                base,                              # DateTimeChooser（去掉前缀）
                'NWeb' + base,                      # NWebDateTimeChooser
                'Web' + base,                       # WebDateTimeChooser
                ark_class,                         # ArkMMIAdapter（完整）
            ])
        else:
            # 没有 Ark 前缀，尝试添加前缀
            possible_keys.extend([
                'ArkWeb' + ark_class,                # ArkWebDateTimeChooser
                'Ark' + ark_class,                   # ArkDateTimeChooser
                'NWeb' + ark_class,                  # NWebDateTimeChooser
            ])

        # 去重并过滤空字符串，保持优先级顺序
        seen = set()
        unique_keys = []
        for key in possible_keys:
            if key and key not in seen:
                seen.add(key)
                unique_keys.append(key)

        # 按优先级查找第一个存在的键
        for key in unique_keys:
            if key in self.struct_mappings:
                return key

        return None

    def _get_struct_mapping_info(self, ark_class, return_type):
        """检查返回值是否是已配置的结构体类型
        Args:
            ark_class: ArkWeb 类名（如 "DateTimeChooser" 或 "ArkWebDateTimeChooser"）
            return_type: 返回值类型（如 "ArkWebDateTime"）
        Returns:
            dict or None: 如果找到映射，返回包含 nweb_type, class_to_struct, struct_to_class 的字典
        """
        # 查找配置键
        config_key = self._find_struct_mapping_key(ark_class)
        if not config_key:
            return None

        struct_mappings = self.struct_mappings[config_key]
        if return_type in struct_mappings:
            return struct_mappings[return_type]
        return None

    def _get_struct_mapping_info_by_nweb_type(self, ark_class, nweb_type):
        """根据 NWeb 类型查找结构体映射信息（用于 wrapper 侧）
        Args:
            ark_class: ArkWeb 类名（如 "DateTimeChooser" 或 "ArkWebDateTimeChooser"）
            nweb_type: NWeb 类型（如 "DateTime" 或 "OHOS::NWeb::DateTime"）
        Returns:
            dict or None: 如果找到映射，返回包含 nweb_type, class_to_struct, struct_to_class 的字典
        """
        # 清理 nweb_type，移除可能的命名空间前缀
        clean_nweb_type = nweb_type
        if 'OHOS::NWeb::' in clean_nweb_type:
            clean_nweb_type = clean_nweb_type.replace('OHOS::NWeb::', '')
        elif 'NWeb::' in clean_nweb_type:
            clean_nweb_type = clean_nweb_type.replace('NWeb::', '')

        # 首先尝试使用完整的类名查找
        if ark_class in self.struct_mappings:
            struct_mappings = self.struct_mappings[ark_class]
            for arkweb_type, mapping_info in struct_mappings.items():
                mapping_nweb_type = mapping_info.get('nweb_type', '')
                # 清理映射中的 nweb_type
                if 'OHOS::NWeb::' in mapping_nweb_type:
                    mapping_nweb_type = mapping_nweb_type.replace('OHOS::NWeb::', '')
                elif 'NWeb::' in mapping_nweb_type:
                    mapping_nweb_type = mapping_nweb_type.replace('NWeb::', '')
                if clean_nweb_type == mapping_nweb_type:
                    return mapping_info

        # 尝试不同的类名变换来查找
        possible_keys = []

        # 1. 去掉 Ark 前缀
        if ark_class.startswith('Ark'):
            possible_keys.append(ark_class[3:])

        # 2. 去掉 ArkWeb 前缀（注意：ArkWeb 是 6 个字符）
        if ark_class.startswith('ArkWeb'):
            possible_keys.append(ark_class[6:])

        # 3. 去掉 Ark 并加上 Web 前缀
        if ark_class.startswith('Ark') and not ark_class.startswith('ArkWeb'):
            possible_keys.append('Web' + ark_class[3:])

        for key in possible_keys:
            if key in self.struct_mappings:
                struct_mappings = self.struct_mappings[key]
                for arkweb_type, mapping_info in struct_mappings.items():
                    mapping_nweb_type = mapping_info.get('nweb_type', '')
                    # 清理映射中的 nweb_type
                    if 'OHOS::NWeb::' in mapping_nweb_type:
                        mapping_nweb_type = mapping_nweb_type.replace('OHOS::NWeb::', '')
                    elif 'NWeb::' in mapping_nweb_type:
                        mapping_nweb_type = mapping_nweb_type.replace('NWeb::', '')
                    if clean_nweb_type == mapping_nweb_type:
                        return mapping_info

        return None

    def _has_struct_mappings(self, ark_class):
        """检查类是否有结构体映射配置
        Args:
            ark_class: ArkWeb 类名
        Returns:
            bool: 是否有结构体映射配置
        """
        config_key = self._find_struct_mapping_key(ark_class)
        return config_key is not None


def load_config(config_path):
    config_file = Path(config_path)
    if not config_file.exists():
        raise FileNotFoundError(f"配置文件不存在: {config_path}")
    with open(config_file, 'r', encoding='utf-8') as f:
        config = json.load(f)
    required_fields = ['version', 'classes']
    for field in required_fields:
        if field not in config:
            raise ValueError(f"配置文件缺少必需字段: {field}")
    classes_dict = {}
    for cls in config['classes']:
        required_class_fields = ['name', 'system', 'arkweb', 'nweb', 'generate']
        for field in required_class_fields:
            if field not in cls:
                raise ValueError(f"类配置缺少必需字段: {field}")
        if cls['system'] not in ['ohos_adapter', 'ohos_nweb']:
            raise ValueError(f"无效的 system 值: {cls['system']}")
        valid_generate = ['impl', 'wrapper']
        for item in cls['generate']:
            if item not in valid_generate:
                raise ValueError(f"无效的 generate 值: {item}")
        classes_dict[cls['name']] = cls
    return {
        'version': config['version'],
        'base_dir': config.get('base_dir'),  # 可选，可通过 --base-dir 参数覆盖
        'description': config.get('description', ''),
        'classes': classes_dict
    }


def validate_config(config_data, base_dir):
    result = {
        'valid': True,
        'errors': [],
        'warnings': [],
        'statistics': {
            'total_classes': len(config_data['classes']),
            'arkweb_exists': 0,
            'nweb_exists': 0
        }
    }
    base_path = Path(base_dir)
    for class_name, class_config in config_data['classes'].items():
        arkweb_path = base_path / class_config['arkweb']
        if not arkweb_path.exists():
            result['errors'].append(f"{class_name}: ArkTS 头文件缺失 - {arkweb_path}")
        else:
            result['statistics']['arkweb_exists'] += 1
        nweb_path = base_path / class_config['nweb']
        if not nweb_path.exists():
            result['errors'].append(f"{class_name}: NWeb 头文件缺失 - {nweb_path}")
        else:
            result['statistics']['nweb_exists'] += 1
    if result['errors']:
        result['valid'] = False
    return result


def main():
    # 配置日志系统（默认输出 ERROR 级别，用于诊断解析失败）
    global logger
    logger = logging.getLogger('bridge_gen')
    log_level = os.environ.get('BRIDGE_GEN_LOG_LEVEL', 'ERROR').upper()
    if log_level == 'DEBUG':
        logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
    elif log_level == 'INFO':
        logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    else:
        logging.basicConfig(level=logging.ERROR, format='%(levelname)s: %(message)s')

    parser = argparse.ArgumentParser(
        description='Bridge 文件自动生成脚本（精简版）',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
    python bridge_gen_files.py --config config.json --output ohos_interface_tmp_gen
    python bridge_gen_files.py --config config.json --output ohos_interface_tmp_gen --validate
        """
    )
    parser.add_argument('--config', required=True, help='配置文件路径（JSON 格式）')
    parser.add_argument('--output', required=True, help='输出目录')
    parser.add_argument('--system', required=False, help='过滤系统（ohos_adapter 或 ohos_nweb）')
    parser.add_argument('--validate', action='store_true', help='仅验证配置文件，不生成文件')
    parser.add_argument('--base-dir', required=False, help='指定 base_dir（必需，除非配置文件中已定义）')
    args = parser.parse_args()
    # 加载配置
    try:
        config = load_config(args.config)
        if args.base_dir:
            config['base_dir'] = args.base_dir
            print(f"[配置] 使用命令行指定 base_dir: {args.base_dir}")
        elif not config['base_dir']:
            print("✗ 错误: 必须通过 --base-dir 参数指定 base_dir")
            return 1
        print(f"✓ 加载配置: {args.config} (v{config['version']}, {len(config['classes'])}个类, base_dir={config['base_dir']})")
    except Exception as e:
        print(f"✗ 加载配置文件失败: {e}")
        return 1
    # 按系统过滤类
    if args.system:
        system_filter = args.system
        classes_dict = {name: cls for name, cls in config['classes'].items() if cls['system'] == system_filter}
        config['classes'] = classes_dict
        if len(config['classes']) == 0:
            print(f"\n警告: 系统 '{system_filter}' 没有匹配的类")
            return 0
        print(f"  过滤系统: {system_filter} ({len(config['classes'])}个类)")
    validation = validate_config(config, config['base_dir'])
    if not validation['valid']:
        print("\n✗ 配置验证失败:")
        for error in validation['errors']:
            print(f"  - {error}")
        return 1
    print(f"✓ 配置验证通过 (ArkTS: {validation['statistics']['arkweb_exists']}, NWeb: {validation['statistics']['nweb_exists']})")
    if args.validate:
        print("\n验证完成（--validate 模式，未生成文件）")
        return 0
    parser = InterfaceParser(config['base_dir'])
    generator = BridgeFileGenerator(config['base_dir'], args.output, config)
    print(f"\n开始生成 {len(config['classes'])} 个类的文件...")
    for class_name, class_config in config['classes'].items():
        generate_types = ', '.join(class_config['generate'])
        print(f"\n处理类: {class_name} ({class_config['system']}, {generate_types})")
        # 确定类名
        arkts_class_name = class_name
        if not class_name.startswith('Ark'):
            if class_config['system'] == 'ohos_nweb':
                if class_name.startswith('NWeb'):
                    arkts_class_name = 'Ark' + class_name
                elif not class_name.startswith('ArkWeb'):
                    arkts_class_name = 'ArkWeb' + class_name
            elif class_config['system'] == 'ohos_adapter' and not class_name.startswith('Ark'):
                arkts_class_name = 'Ark' + class_name
        # 去掉 ArkNWeb 前缀中的 NWeb
        if arkts_class_name.startswith('ArkNWeb'):
            arkts_class_name = 'ArkWeb' + arkts_class_name[7:]
        arkts_interface = parser.parse_arkts_interface(class_config['arkweb'], arkts_class_name)
        nweb_interface = parser.parse_nweb_interface(class_config['nweb'], class_name)
        if not arkts_interface and not arkts_class_name.endswith('Adapter'):
            arkts_class_name_with_adapter = arkts_class_name + 'Adapter'
            print(f"  尝试使用类名: {arkts_class_name_with_adapter}")
            arkts_interface = parser.parse_arkts_interface(class_config['arkweb'], arkts_class_name_with_adapter)
            if arkts_interface:
                arkts_class_name = arkts_class_name_with_adapter
        if not arkts_interface:
            print("  ✗ 解析 ArkTS 接口失败")
            continue
        nweb_class_name = class_name
        if not nweb_interface:
            # 智能类名推导：根据 system 类型优先尝试不同的类名变换
            system = class_config['system']
            nweb_attempts = []  # 尝试的类名列表

            # 根据 system 类型确定尝试顺序
            if system == 'ohos_adapter':
                # ohos_adapter: 优先添加 Adapter 后缀
                if not nweb_class_name.endswith('Adapter'):
                    nweb_attempts.append(nweb_class_name + 'Adapter')
                # 尝试添加 NWeb 前缀
                if not nweb_class_name.startswith('NWeb'):
                    nweb_attempts.append('NWeb' + nweb_class_name)
                # 同时添加 NWeb 前缀和 Adapter 后缀
                if not nweb_class_name.startswith('NWeb') and not nweb_class_name.endswith('Adapter'):
                    nweb_attempts.append('NWeb' + nweb_class_name + 'Adapter')
            elif system == 'ohos_nweb':
                # ohos_nweb: 优先添加 NWeb 前缀
                if not nweb_class_name.startswith('NWeb'):
                    nweb_attempts.append('NWeb' + nweb_class_name)
                # 尝试添加 Adapter 后缀
                if not nweb_class_name.endswith('Adapter'):
                    nweb_attempts.append(nweb_class_name + 'Adapter')
                # 同时添加 NWeb 前缀和 Adapter 后缀
                if not nweb_class_name.startswith('NWeb') and not nweb_class_name.endswith('Adapter'):
                    nweb_attempts.append('NWeb' + nweb_class_name + 'Adapter')

            # 按顺序尝试每个候选类名
            for attempt_name in nweb_attempts:
                print(f"  尝试使用NWeb类名: {attempt_name}")
                nweb_interface = parser.parse_nweb_interface(class_config['nweb'], attempt_name)
                if nweb_interface:
                    nweb_class_name = attempt_name
                    break
        if not nweb_interface:
            print("  ✗ 解析 NWeb 接口失败")
            continue
        print(f"  ✓ 接口解析成功 (ArkTS: {len(arkts_interface['methods'])}方法, NWeb: {len(nweb_interface['methods'])}方法)")
        # 将全局 base_dir 复制到 class_config 中，供 vector_headers 解析使用
        class_config['base_dir'] = config['base_dir']
        try:
            generator.generate_class_files(arkts_interface, nweb_interface, class_config)
        except Exception as e:
            print(f"  ✗ 文件生成失败: {e}", flush=True)
            traceback.print_exc()
    # 打印摘要
    generator.print_summary()
    return 0
if __name__ == '__main__':
    sys.exit(main())
