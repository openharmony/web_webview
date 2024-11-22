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
import file_parser
import make_file_base


# pylint:disable=variable-type-changed
# pylint:disable=huawei-redefined-outer-name

def ctocpp_make_impl_proto(cls, func):
    proto = 'ARK_WEB_NO_SANITIZE\n'
    parts = func.get_cpp_parts(True)
    if cls.get_name() is None:
        proto += 'ARK_WEB_GLOBAL ' + parts['retval'] + ' ' + func.get_name() + '(' + ', '.join(parts['args']) + ') {'
    else:
        const = ''
        proto += parts['retval'] + ' ' + cls.get_name()
        if isinstance(func, file_parser.obj_function_virtual):
            proto += 'CToCpp'
            if func.is_const():
                const = ' const'
        proto += '::' + func.get_name() + '(' + ', '.join(parts['args']) + ')' + const + ' {'
    return proto


def verify_ctocpp_func_args(func, retval_default):
    result = ''
    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '\n  // Verify param: ' + arg_name + '; type: ' + arg_type

        if arg_type == 'bool_byaddr':
            result += comment + \
                      '\n  if (!' + arg_name + ') {' + \
                      '\n    return' + retval_default + ';' + \
                      '\n  }'

        # check index params
        index_params = arg.parent.get_attrib_list('index_param')
        if not index_params is None and arg_name in index_params:
            result += comment + \
                      '\n  if (' + arg_name + ' < 0) {' + \
                      '\n    return' + retval_default + ';' + \
                      '\n  }'
    return result


def restore_ctocpp_func_args(func):
    result = ''
    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '\n  // Restore param:' + arg_name + '; type: ' + arg_type

        if arg_type == 'bool_byaddr':
            result += comment + \
                      '\n  if (' + arg_name + ') {' + \
                      '\n    *' + arg_name + ' = ' + arg_name + 'Int ? true : false;' + \
                      '\n  }'
        elif arg_type == 'refptr_same_byref' or arg_type == 'refptr_diff_byref':
            ptr_class = arg.get_type().get_ptr_type()
            if arg_type == 'refptr_same_byref':
                assign = ptr_class + 'CToCpp::Invert(' + arg_name + 'Struct)'
            else:
                assign = ptr_class + 'CppToC::Revert(' + arg_name + 'Struct)'
            result += comment + \
                      '\n  if (' + arg_name + 'Struct) {' + \
                      '\n    if (' + arg_name + 'Struct != ' + arg_name + 'Orig) {' + \
                      '\n      ' + arg_name + ' = ' + assign + ';' + \
                      '\n    }' + \
                      '\n  } else {' + \
                      '\n    ' + arg_name + ' = nullptr;' + \
                      '\n  }'
    return result;


def translate_ctocpp_func_args(func):
    params = []
    if isinstance(func, file_parser.obj_function_virtual):
        params.append('_struct')

    result = ''
    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '\n  // Translate param: ' + arg_name + '; type: ' + arg_type

        if arg_type == 'simple_byval' or arg_type == 'simple_byaddr' or arg_type == 'bool_byval':
            if arg_name[0] == '*':
                params.append(arg_name[1:])
            else:
                pos = arg_name.find('[')
                if pos == -1:
                    params.append(arg_name)
                else:
                    params.append(arg_name[0:pos])
        elif arg_type == 'simple_byref' or arg_type == 'simple_byref_const' or \
                arg_type == 'struct_byref_const' or arg_type == 'struct_byref':
            params.append('&' + arg_name)
        elif arg_type == 'bool_byref':
            params.append('&' + arg_name)
        elif arg_type == 'bool_byaddr':
            result += comment + '\n  int ' + arg_name + 'Int = ' + arg_name + '?*' + arg_name + ':0;'
            params.append('&' + arg_name + 'Int')
        elif arg_type == 'refptr_same':
            ptr_class = arg.get_type().get_ptr_type()
            params.append(ptr_class + 'CToCpp::Revert(' + arg_name + ')')
        elif arg_type == 'refptr_diff':
            ptr_class = arg.get_type().get_ptr_type()
            params.append(ptr_class + 'CppToC::Invert(' + arg_name + ')')
        elif arg_type == 'refptr_same_byref' or arg_type == 'refptr_diff_byref':
            ptr_class = arg.get_type().get_ptr_type()
            ptr_struct = arg.get_type().get_result_ptr_type_root()
            if arg_type == 'refptr_same_byref':
                assign = ptr_class + 'CToCpp::Revert(' + arg_name + ')'
            else:
                assign = ptr_class + 'CppToC::Invert(' + arg_name + ')'
            result += comment + \
                      '\n  ' + ptr_struct + '* ' + arg_name + 'Struct = NULL;' + \
                      '\n  if (' + arg_name + '.get()) {' + \
                      '\n    ' + arg_name + 'Struct = ' + assign + ';' + \
                      '\n  }' + \
                      '\n  ' + ptr_struct + '* ' + arg_name + 'Orig = ' + arg_name + 'Struct;'
            params.append('&' + arg_name + 'Struct')
        else:
            raise Exception('Unsupported argument type %s for parameter %s in %s' %
                            (arg_type, arg_name, name))
    return result, params


def ctocpp_make_static_param(cls, funcs, prefix):
    new_list = []
    old_list = make_file_base.get_func_name_list(funcs)

    impl = ''
    for func in funcs:
        new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)

        suffix = ''
        if new_list.count(func.get_capi_name()) > 0:
            suffix = str(new_list.count(func.get_capi_name()))

        parts = func.get_capi_parts()
        func_name, func_type = make_file_base.get_func_pointer_name(cls, func, prefix, suffix)
        impl += 'using ' + func_type + ' = ' + parts['retval'] + ' (*)(' + ', '.join(parts['args']) + ');\n' + \
                'static ' + func_type + ' ' + func_name + ' = reinterpret_cast<' + func_type + '>(ARK_WEB_INIT_ADDR);\n\n'
    return impl


def ctocpp_make_static_params(cls, header):
    prefix = file_parser.get_capi_name(cls.get_name(), False)
    result = ctocpp_make_static_param(cls, cls.get_static_funcs(), '')
    result += ctocpp_make_static_param(cls, make_file_base.get_class_func_list(cls, header), prefix)
    return result


def ctocpp_get_static_funcion(cls, func, suffix, dir_name, retval_default):
    func_name, func_type = make_file_base.get_func_pointer_name(cls, func, '', suffix)
    result = '\n  ARK_WEB_CTOCPP_DV_LOG();\n' + \
             '\n  void* func_pointer = reinterpret_cast<void*>(' + func_name + ');' + \
             '\n  if (func_pointer == ARK_WEB_INIT_ADDR) {' + \
             '\n    static const char* funcName = \"' + func_name + '_static\";' + \
             '\n    ' + func_name + ' = reinterpret_cast<' + func_type + '>('
    if dir_name == 'ohos_nweb':
        if cls.is_webview_side():
            result += 'ArkWebNWebWebcoreBridgeHelper::GetInstance().LoadFuncSymbol(funcName));'
        else:
            result += 'ArkWebNWebWebviewBridgeHelper::GetInstance().LoadFuncSymbol(funcName));'
    else:
        if cls.is_webview_side():
            result += 'ArkWebAdapterWebcoreBridgeHelper::GetInstance().LoadFuncSymbol(funcName));'
        else:
            result += 'ArkWebAdapterWebviewBridgeHelper::GetInstance().LoadFuncSymbol(funcName));'
    result += '\n  }\n' + \
              '\n  if (!' + func_name + ') {' + \
              '\n    ARK_WEB_CTOCPP_WARN_LOG("failed to load func ' + func_name + '_static");' + \
              '\n    return ' + retval_default + ';' + \
              '\n  }\n'
    return result, func_name


def ctocpp_get_virtual_funcion(cls, func, suffix, dir_name, macro_retval_default):
    result = '\n  ARK_WEB_CTOCPP_DV_LOG(\"capi struct is %{public}ld\", (long)this);\n'

    # determine how the struct should be referenced
    if cls.get_name() == func.parent.get_name():
        result += '\n  ' + file_parser.get_capi_name(cls.get_name(), True) + '* _struct = GetStruct();'
    else:
        result += '\n  ' + func.parent.get_capi_name() + '* _struct = reinterpret_cast<' + \
                  func.parent.get_capi_name() + '*>(GetStruct());'

    prefix = file_parser.get_capi_name(cls.get_name(), False)
    hash_name = make_file_base.get_func_hash_name(func, prefix)
    var_name = make_file_base.get_func_variable_name(func, suffix)
    func_name, func_type = make_file_base.get_func_pointer_name(cls, func, prefix, suffix)
    if isinstance(func, file_parser.obj_function_virtual):
        result += '\n  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, ' + macro_retval_default + ');\n' + \
                  '\n  void* func_pointer = reinterpret_cast<void*>(' + func_name + ');' + \
                  '\n  if (func_pointer == ARK_WEB_INIT_ADDR) {' + \
                  '\n    static const std::string funcName = \"' + hash_name + '\";' + \
                  '\n    func_pointer = '
        if dir_name == 'ohos_nweb':
            if cls.is_webview_side():
                result += 'ArkWebNWebWebcoreBridgeHelper::GetInstance().CheckFuncMemberForCaller('
            else:
                result += 'ArkWebNWebWebviewBridgeHelper::GetInstance().CheckFuncMemberForCaller('
        else:
            if cls.is_webview_side():
                result += 'ArkWebAdapterWebcoreBridgeHelper::GetInstance().CheckFuncMemberForCaller('
            else:
                result += 'ArkWebAdapterWebviewBridgeHelper::GetInstance().CheckFuncMemberForCaller('
        result += file_parser.get_wrapper_type_enum(cls.get_name()) + ', funcName);'
        result += '\n    if (func_pointer == ARK_WEB_INIT_ADDR) {' + \
                  '\n      ARK_WEB_CTOCPP_DV_LOG(\"failed to find func member ' + func_name + '\");' + \
                  '\n      if (ARK_WEB_FUNC_MEMBER_MISSING(_struct, ' + var_name + ')) {' + \
                  '\n        ' + func_name + ' = nullptr;' + \
                  '\n      } else {' + \
                  '\n        ' + func_name + ' = _struct->' + var_name + ';' + \
                  '\n      }' + \
                  '\n    } else {' + \
                  '\n      ' + func_name + ' = reinterpret_cast<' + func_type + '>(func_pointer);' + \
                  '\n    }' + \
                  '\n  }\n' + \
                  '\n  ARK_WEB_CTOCPP_CHECK_FUNC_POINTER(' + func_name + ',' + macro_retval_default + ');\n'
    return result, func_name


def ctocpp_make_function_impl(cls, func, suffix, dir_name):
    # build the C++ prototype  # build the C++ prototype
    result = ctocpp_make_impl_proto(cls, func)

    invalid = make_file_base.get_func_invalid_info(func.get_name(), func)
    if len(invalid) > 0:
        return result + invalid

    retval = func.get_retval()
    retval_default = retval.get_retval_default(False)
    if len(retval_default) > 0:
        macro_retval_default = retval_default
        retval_default = ' ' + retval_default
    else:
        macro_retval_default = 'ARK_WEB_RETURN_VOID'

    if isinstance(func, file_parser.obj_function_virtual):
        func_body, func_name = ctocpp_get_virtual_funcion(cls, func, suffix, dir_name, macro_retval_default)
    else:
        func_body, func_name = ctocpp_get_static_funcion(cls, func, suffix, dir_name, retval_default)

    result += func_body
    result_len = len(result)

    # parameter verification
    result += verify_ctocpp_func_args(func, retval_default)
    if len(result) != result_len:
        result += '\n'
        result_len = len(result)

    # parameter translation
    trans, params = translate_ctocpp_func_args(func)
    if len(trans) != 0:
        result += trans + '\n'

    # execution
    result += '\n  // Execute\n  '

    retval_type = retval.get_retval_type()
    if retval_type != 'none':
        # has a return value
        if retval_type == 'simple' or retval_type == 'bool' or retval_type == 'void*' or retval_type == 'uint8_t*' or \
                retval_type == 'uint32_t*' or retval_type == 'char*' or file_parser.check_arg_type_is_struct(
            retval_type):
            result += 'return '
        elif retval_type == 'refptr_same' or retval_type == 'refptr_diff':
            ptr_struct = retval.get_type().get_result_ptr_type_root()
            result += ptr_struct + '* _retval = '
        else:
            raise Exception('Unsupported return type %s in %s' % (retval_type, func.get_name()))

    result += func_name + '('

    if len(params) > 0:
        if not isinstance(func, file_parser.obj_function_virtual):
            result += '\n      '
        result += ',\n      '.join(params)

    result += ');\n'
    result_len = len(result)

    # parameter restoration
    result += restore_ctocpp_func_args(func)
    if len(result) != result_len:
        result += '\n'
        result_len = len(result)

    if retval_type == 'refptr_same':
        result += '\n  // Return type: ' + retval_type + \
                  '\n  return ' + retval.get_type().get_ptr_type() + 'CToCpp::Invert(_retval);'
    elif retval_type == 'refptr_diff':
        result += '\n  // Return type: ' + retval_type + \
                  '\n  return ' + retval.get_type().get_ptr_type() + 'CppToC::Revert(_retval);'

    if len(result) != result_len:
        result += '\n'

    result += '}\n\n'
    return result


def ctocpp_make_function_body(cls, funcs, dir_name):
    new_list = []
    old_list = make_file_base.get_func_name_list(funcs)

    impl = ''
    for func in funcs:
        new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)

        suffix = ''
        if new_list.count(func.get_capi_name()) > 0:
            suffix = str(new_list.count(func.get_capi_name()))

        impl += ctocpp_make_function_impl(cls, func, suffix, dir_name)
    return impl


def ctocpp_make_functions_body(cls, header, dir_name):
    clsname = cls.get_name();
    result = ctocpp_make_function_body(cls, cls.get_static_funcs(), dir_name)
    result += ctocpp_make_function_body(cls, make_file_base.get_class_func_list(cls, header), dir_name)
    result += clsname + 'CToCpp::' + clsname + 'CToCpp() {\n}\n\n' + \
              clsname + 'CToCpp::~' + clsname + 'CToCpp() {\n}\n\n'
    return result


def ctocpp_make_include_files(cls, body, header, dir_name):
    result = file_parser.format_translation_includes(header, dir_name, body)
    result += '#include "base/ctocpp/ark_web_ctocpp_macros.h"\n'
    if dir_name == 'ohos_nweb':
        if cls.is_webview_side():
            result += '#include "ohos_nweb/bridge/ark_web_nweb_webcore_bridge_helper.h"\n'
        else:
            result += '#include "ohos_nweb/bridge/ark_web_nweb_webview_bridge_helper.h"\n'
    else:
        if cls.is_webview_side():
            result += '#include "ohos_adapter/bridge/ark_web_adapter_webcore_bridge_helper.h"\n'
        else:
            result += '#include "ohos_adapter/bridge/ark_web_adapter_webview_bridge_helper.h"\n'
    return result


def ctocpp_make_unwrap_derived(cls, header, clsname):
    impl = ''
    derived_classes = make_file_base.get_derived_classes(cls, header)
    for clsname in derived_classes:
        impl += '  if (type == ' + file_parser.get_wrapper_type_enum(clsname) + ') {\n' + \
                '    return reinterpret_cast<' + file_parser.get_capi_name(clsname, True) + '*>(' + \
                clsname + 'CToCpp::Revert(reinterpret_cast<' + clsname + '*>(c)));\n' + \
                '  }\n'
    return impl


def make_ctocpp_impl_file(header, dir_path, dir_name, clsname):
    cls = header.get_class(clsname)
    if cls is None:
        raise Exception('Class does not exist: ' + clsname)

    static_param = ctocpp_make_static_params(cls, header)
    function_body = ctocpp_make_functions_body(cls, header, dir_name)

    unwrapderived = ctocpp_make_unwrap_derived(cls, header, clsname)
    includes = ctocpp_make_include_files(cls, function_body + unwrapderived, header, dir_name)

    content = make_file_base.get_copyright() + '\n' + includes + '\n' + \
              'namespace OHOS::ArkWeb {\n\n' + \
              static_param + function_body + \
              make_file_base.make_wrapper_type(cls, 'CToCpp') + \
              '\n\n} // namespace OHOS::ArkWeb\n'

    absolute_dir = os.path.join(os.path.join(dir_path, dir_name), 'ctocpp')
    absolute_path = os.path.join(absolute_dir, file_parser.get_capi_name(clsname, False) + '_ctocpp.cpp')
    return (content, absolute_path)
