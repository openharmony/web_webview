#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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

from __future__ import absolute_import

import os
import base64
import hashlib
import file_parser
import system_util


# pylint:disable=huawei-redefined-outer-name

# ------------------------------------------------------------------------------------------------------#


def get_copyright():
    content = \
        """/*
         * Copyright (c) $YEAR$ Huawei Device Co., Ltd.
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

    # add the copyright year
    return content.replace('$YEAR$', system_util.get_year())


# ------------------------------------------------------------------------------------------------------#


def make_def_file(cls, class_name):
    directory = cls.get_file_directory()

    def_name = ''
    if not directory is None:
        def_name += directory + '_'

    def_name += file_parser.get_capi_name(class_name, False)
    return def_name


# ------------------------------------------------------------------------------------------------------#


def get_base_name(ident, header, class_name):
    base_name = header.get_base_class_name(class_name)
    if base_name == 'ArkWebBaseScoped':
        base_name = 'ArkWeb' + ident + 'Scoped'
    else:
        base_name = 'ArkWeb' + ident + 'RefCounted'
    return base_name


# ------------------------------------------------------------------------------------------------------#


def make_include_file(cls, ident, header, dir_name, class_name):
    # include the headers for this class
    capi_path = os.path.join(dir_name, 'capi', cls.get_capi_file_name())
    include_path = os.path.join(dir_name, 'include', cls.get_file_name())
    content = '#include "' + capi_path + '"\n' + \
              '#include "' + include_path + '"\n'

    base_name = header.get_base_class_name(class_name)
    if base_name == 'ArkWebBaseScoped':
        base_file = ident.lower() + '_scoped.h'
    else:
        base_file = ident.lower() + '_ref_counted.h'

    base_path = os.path.join('base', ident.lower(), 'ark_web_' + base_file)
    content += '#include "' + base_path + '"\n'
    return {'content': content, 'base_name': get_base_name(ident, header, class_name)}


# ------------------------------------------------------------------------------------------------------#


def make_class_define(cls, ident, base_class, class_name, funcs_body):
    capi_name = cls.get_capi_name()
    content = 'class ' + class_name + ident + '\n' + \
              '    : public ' + base_class + '<' + class_name + ident + ', ' + class_name + ', ' + capi_name + '> {\n' + \
              '  public:\n' + \
              '    ' + class_name + ident + '();\n' + \
              '    virtual ~' + class_name + ident + '();\n\n'

    if len(funcs_body) != 0:
        content += funcs_body
    content += '};\n'
    return content


# ------------------------------------------------------------------------------------------------------#


def make_wrapper_type(cls, ident):
    clsname = cls.get_name()
    content = 'template<>\n' + \
              'ArkWebBridgeType ArkWeb' + ident + 'RefCounted<' + clsname + ident + ', ' + clsname + ', ' + \
              cls.get_capi_name() + '>\n::kBridgeType = ' + file_parser.get_wrapper_type_enum(clsname) + ';'
    return content


# ------------------------------------------------------------------------------------------------------#


def get_derived_classes(cls, header):
    # identify all classes that derive form class
    derived_classes = []
    class_name = cls.get_name()
    all_classes = header.get_classes()
    for cur_cls in all_classes:
        if cur_cls.get_name() == class_name:
            continue
        if cur_cls.has_parent(class_name):
            derived_classes.append(cur_cls.get_name())

    derived_classes = sorted(derived_classes)
    return derived_classes


# ------------------------------------------------------------------------------------------------------#


def get_func_hash_name(func, prefix):
    args = func.get_arguments()
    func_digest = prefix + '_' + func.get_capi_name() + ','.join(arg.get_raw_type() for arg in args)
    return base64.b64encode(hashlib.md5(func_digest.encode()).digest(), altchars=b'$_').decode().rstrip('=')


# ------------------------------------------------------------------------------------------------------#


def get_func_name_list(funcs):
    name_list = []
    for func in funcs:
        name_list.append(func.get_capi_name())
    return name_list


# ------------------------------------------------------------------------------------------------------#


def get_func_name_count(name, old_list, new_list):
    if old_list.count(name) != 1:
        new_list.append(name)
    return new_list


# ------------------------------------------------------------------------------------------------------#


def get_class_func_list(cls, header):
    func_list = []
    parent_cls = cls
    while True:
        func_list.extend(parent_cls.get_virtual_funcs())

        parent_clsname = parent_cls.get_parent_name()
        if file_parser.is_base_class(parent_clsname):
            break

        parent_cls = header.get_class(parent_clsname)
        if parent_cls is None:
            raise Exception('Class does not exist: ' + parent_clsname)
    return func_list


# ------------------------------------------------------------------------------------------------------#


def get_func_pointer_name(cls, func, prefix, suffix):
    if len(prefix) == 0:
        func_name = func.get_capi_name() + suffix
    else:
        func_name = prefix + '_' + func.get_capi_name() + suffix
    func_type = cls.get_name() + func.get_name() + 'Func' + suffix
    return func_name, func_type


# ------------------------------------------------------------------------------------------------------#


def get_func_variable_name(func, suffix):
    if file_parser.check_func_name_is_key_work(func.get_capi_name()):
        if len(suffix) == 0:
            var_name = func.get_capi_name() + '0'
        else:
            var_name = func.get_capi_name() + suffix
    else:
        var_name = func.get_capi_name() + suffix
    return var_name


# ------------------------------------------------------------------------------------------------------#


def get_func_invalid_info(name, func):
    invalid = []
    args = func.get_arguments()
    for arg in args:
        if arg.get_arg_type() == 'invalid':
            invalid.append(arg.get_name())

    retval = func.get_retval()
    retval_type = retval.get_retval_type()
    if retval_type == 'invalid':
        invalid.append('(return value)')

    if len(invalid) == 0:
        return ''

    file_parser.notify(name + ' could not be autogenerated')
    # code could not be auto-generated
    result = '\n  // BEGIN DELETE BEFORE MODIFYING'
    result += '\n  // COULD NOT IMPLEMENT DUE TO: ' + ', '.join(invalid)
    result += '\n  #pragma message("Warning: " __FILE__ ": ' + name + ' is not implemented")'
    result += '\n  // END DELETE BEFORE MODIFYING'
    result += '\n}\n\n'
    return result
