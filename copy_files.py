#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2021 Huawei Device Co., Ltd.
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

import sys
import os
import argparse
import shutil

WORK_SPACE = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(WORK_SPACE)))

sys.path.append(os.path.join(PROJECT_DIR, 'build'))
sys.path.append(os.path.join(PROJECT_DIR, 'build/hb'))
from hb.util import log_util  # noqa: E402
from scripts.util import file_utils  # noqa: E402
from scripts.util import build_utils  # noqa: E402

INTERFACE_DIR = os.path.join(WORK_SPACE, 'ohos_interface')
INTERFACE_INCLUDE_DIR = os.path.join(INTERFACE_DIR, 'include')
INTERFACE_OHOS_GLUE_DIR = os.path.join(INTERFACE_DIR, 'ohos_glue')


def copy_dir(src_dir: str, dst_dir: str):
    log_util.LogUtil.hb_info("begin to copy dir from '{}' to '{}'".format(src_dir, dst_dir))
    if os.path.isdir(dst_dir):
        shutil.rmtree(dst_dir)

    if os.path.isdir(src_dir) and os.listdir(src_dir):
        shutil.copytree(src_dir, dst_dir)

    source_files = []
    for root, dirs, files in os.walk(src_dir):
        for name in files:
            source_files.append(os.path.join(root, name))
    return source_files


def copy_files(src_dir: str, dst_dir: str):
    log_util.LogUtil.hb_info("begin to copy files from '{}' to '{}'".format(src_dir, dst_dir))
    source_files = []
    for item in os.listdir(src_dir):
        src_file = os.path.join(src_dir, item)
        dst_file = os.path.join(dst_dir, item)
        if os.path.isfile(src_file):
            source_files.append(src_file)
            shutil.copy2(src_file, dst_file)
    return source_files


def copy_include():
    log_util.LogUtil.hb_info("begin to copy include dir")
    nweb_include = os.path.join('ohos_nweb', 'include')
    include_source_files = copy_files(os.path.join(INTERFACE_INCLUDE_DIR, 'ohos_nweb'),
            os.path.join(WORK_SPACE, nweb_include))

    adapter_include = os.path.join('ohos_adapter', 'interfaces')
    include_source_files += copy_dir(os.path.join(INTERFACE_INCLUDE_DIR, 'ohos_adapter'),
            os.path.join(WORK_SPACE, adapter_include))
    return include_source_files


def copy_glue_base(glue_dir: str):
    log_util.LogUtil.hb_info("begin to copy glue base dir")
    base_dir = os.path.join(glue_dir, 'base')
    base_source_files = copy_dir(os.path.join(INTERFACE_OHOS_GLUE_DIR, 'base'), base_dir)

    script_dir = os.path.join(glue_dir, 'scripts')
    base_source_files += copy_dir(os.path.join(INTERFACE_OHOS_GLUE_DIR, 'scripts'), script_dir)
    return base_source_files


def copy_glue_module(glue_dir: str, module_name: str):
    dir_name = 'ohos_' + module_name;
    log_util.LogUtil.hb_info("begin to copy glue '{}' dir".format(dir_name))
    dst_dir = os.path.join(glue_dir, dir_name)
    if os.path.isdir(dst_dir):
        shutil.rmtree(dst_dir)

    src_dir = os.path.join(INTERFACE_OHOS_GLUE_DIR, dir_name)
    module_source_files = copy_dir(os.path.join(src_dir, 'include'), os.path.join(dst_dir, 'include'))
    module_source_files += copy_dir(os.path.join(os.path.join(src_dir, 'bridge'), 'webview'),
            os.path.join(dst_dir, 'bridge'))
    module_source_files += copy_dir(os.path.join(os.path.join(src_dir, 'cpptoc'), 'webview'),
            os.path.join(dst_dir, 'cpptoc'))
    module_source_files += copy_dir(os.path.join(os.path.join(src_dir, 'ctocpp'), 'webview'),
            os.path.join(dst_dir, 'ctocpp'))
    return module_source_files


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--command-type', required=True)
    parser.add_argument('--ohos-glue-dir', required=True)
    parser.add_argument('--outfile', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    if args.command_type == "include":
        source_file_list = copy_include()
    elif args.command_type == "base":
        source_file_list = copy_glue_base(args.ohos_glue_dir)
    else:
        source_file_list = copy_glue_module(args.ohos_glue_dir, args.command_type)

    file_utils.write_file(args.outfile, '\n'.join(source_file_list))

    if args.depfile:
        _dep_files = []
        _dep_files.extend(source_file_list)
        _dep_files.sort()
        build_utils.write_depfile(args.depfile,
                                  args.outfile,
                                  _dep_files,
                                  add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main())

