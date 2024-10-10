#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2022 Huawei Device Co., Ltd.
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
#


from __future__ import print_function
import argparse
import os.path
import subprocess
import sys


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--protoc", required=True,
                        help="Relative path to compiler.")
    parser.add_argument("--protos-dir", required=True,
                        help="protos in dir.")
    parser.add_argument("--cpp-out",
                        help="Output directory for standard C++ generator.")
    parser.add_argument("protos", nargs="+",
                        help="Input protobuf definition file(s).")
    options = parser.parse_args(argv)

    protos_dir = os.path.relpath(options.protos_dir)
    proto_files = options.protos

    # Generate protoc cmd.
    protoc_cmd = [os.path.realpath(options.protoc)]
    if options.cpp_out:
        cpp_out = options.cpp_out
        protoc_cmd += ["--cpp_out", cpp_out]

    protoc_cmd += ["--proto_path", protos_dir]
    protoc_cmd += [os.path.join(protos_dir, name) for name in proto_files]

    ret = subprocess.call(protoc_cmd)
    if ret != 0:
        raise RuntimeError("Protoc failed.")

if __name__ == "__main__":
    main(sys.argv[1:])
