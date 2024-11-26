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

def make_cpptoc_impl_proto(name, func, parts, flag):
    if isinstance(func, file_parser.obj_function_virtual):
        proto = parts['retval'] + ' ARK_WEB_CALLBACK'
    elif flag:
        proto = 'ARK_WEB_EXPORT ' + parts['retval']
    else:
        proto = parts['retval']

    proto += ' ' + name + '(' + ', '.join(parts['args']) + ')'
    return proto


def verify_cpptoc_func_args(func, retval_default, macro_retval_default):
    result = ''
    if isinstance(func, file_parser.obj_function_virtual):
        result += '\n  ARK_WEB_CPPTOC_DV_LOG(\"capi struct is %{public}ld\", (long)self);\n' + \
                  '\n  ARK_WEB_CPPTOC_CHECK_PARAM(self, ' + macro_retval_default + ');'

    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '\n  // Verify param: ' + arg_name + '; type: ' + arg_type

        if arg_type == 'bool_byref' or arg_type == 'bool_byref_const' or arg_type == 'simple_byref' or \
                arg_type == 'simple_byref_const' or arg_type == 'struct_byref' or arg_type == 'struct_byref_const' or \
                arg_type == 'refptr_diff_byref':
            result += '\n  ARK_WEB_CPPTOC_CHECK_PARAM(' + arg_name + ', ' + macro_retval_default + ');'
            if arg_type == 'struct_byref_const' or arg_type == 'struct_byref':
                result += '\n  if (!template_util::has_valid_size(' + arg_name + ')) {' + \
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


def restore_cpptoc_func_args(func):
    result = ''
    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '\n  // Restore param: ' + arg_name + '; type: ' + arg_type

        if arg_type == 'struct_byref':
            result += comment + \
                      '\n  if (' + arg_name + ') {' + \
                      '\n    ' + arg_name + 'Obj.DetachTo(*' + arg_name + ');' + \
                      '\n  }'
        elif arg_type == 'refptr_same_byref' or arg_type == 'refptr_diff_byref':
            ptr_class = arg.get_type().get_ptr_type()
            if arg_type == 'refptr_same_byref':
                assign = ptr_class + 'CppToC::Invert(' + arg_name + 'Ptr)'
            else:
                assign = ptr_class + 'CToCpp::Revert(' + arg_name + 'Ptr)'
            result += comment + \
                      '\n  if (' + arg_name + ') {' + \
                      '\n    if (' + arg_name + 'Ptr.get()) {' + \
                      '\n      if (' + arg_name + 'Ptr.get() != ' + arg_name + 'Orig) {' + \
                      '\n        *' + arg_name + ' = ' + assign + ';' + \
                      '\n      }' + \
                      '\n    } else {' + \
                      '\n      *' + arg_name + ' = nullptr;' + \
                      '\n    }' + \
                      '\n  }'
    return result;


def translate_cpptoc_func_args(func):
    result = ''
    params = []
    args = func.get_arguments()
    for arg in args:
        arg_type = arg.get_arg_type()
        arg_name = arg.get_type().get_name()
        comment = '  // Translate param: ' + arg_name + '; type: ' + arg_type

        if arg_type == 'simple_byval' or arg_type == 'simple_byaddr':
            if arg_name[0] == '*':
                params.append(arg_name[1:])
            else:
                pos = arg_name.find('[')
                if pos == -1:
                    params.append(arg_name)
                else:
                    params.append(arg_name[0:pos])
        elif arg_type == 'simple_byref' or arg_type == 'simple_byref_const':
            params.append('*' + arg_name)
        elif arg_type == 'bool_byval':
            params.append(arg_name)
        elif arg_type == 'bool_byref' or arg_type == 'bool_byaddr':
            params.append('*' + arg_name)
        elif arg_type == 'struct_byref_const':
            struct_type = arg.get_type().get_type()
            result += comment + \
                      '\n  ' + struct_type + ' ' + arg_name + 'Obj;' + \
                      '\n  if (' + arg_name + ') {' + \
                      '\n    ' + arg_name + 'Obj.Set(*' + arg_name + ', false);' + \
                      '\n  }'
            params.append(arg_name + 'Obj')
        elif arg_type == 'struct_byref':
            struct_type = arg.get_type().get_type()
            result += comment + \
                      '\n  ' + struct_type + ' ' + arg_name + 'Obj;' + \
                      '\n  if (' + arg_name + ') {' + \
                      '\n    ' + arg_name + 'Obj.AttachTo(*' + arg_name + ');' + \
                      '\n  }'
            params.append(arg_name + 'Obj')
        elif arg_type == 'refptr_same' or arg_type == 'refptr_diff':
            ptr_class = arg.get_type().get_ptr_type()
            if arg_type == 'refptr_same':
                params.append(ptr_class + 'CppToC::Revert(' + arg_name + ')')
            else:
                params.append(ptr_class + 'CToCpp::Invert(' + arg_name + ')')
        elif arg_type == 'refptr_same_byref' or arg_type == 'refptr_diff_byref':
            ptr_class = arg.get_type().get_ptr_type()
            if arg_type == 'refptr_same_byref':
                assign = ptr_class + 'CppToC::Revert(*' + arg_name + ')'
            else:
                assign = ptr_class + 'CToCpp::Invert(*' + arg_name + ')'
            result += comment + \
                      '\n  ArkWebRefPtr<' + ptr_class + '> ' + arg_name + 'Ptr;' + \
                      '\n  if (' + arg_name + ' && *' + arg_name + ') {' + \
                      '\n    ' + arg_name + 'Ptr = ' + assign + ';' + \
                      '\n  }' + \
                      '\n  ' + ptr_class + '* ' + arg_name + 'Orig = ' + arg_name + 'Ptr.get();'
            params.append(arg_name + 'Ptr')
        else:
            raise Exception('Unsupported argument type %s for parameter %s in %s' %
                            (arg_type, arg_name, name))
    return result, params


def make_cpptoc_function_impl(cls, name, func, defined_names):
    # retrieve the C API prototype parts
    parts = func.get_capi_parts(defined_names, True)
    result = make_cpptoc_impl_proto(name, func, parts, False) + ' {'

    invalid = make_file_base.get_func_invalid_info(name, func)
    if len(invalid) > 0:
        return result + invalid

    retval = func.get_retval()
    retval_default = retval.get_retval_default(True)
    if len(retval_default) > 0:
        retval_default = ' ' + retval_default
        macro_retval_default = retval_default
    else:
        macro_retval_default = 'ARK_WEB_RETURN_VOID'
    result_len = len(result)

    # parameter verification
    result += verify_cpptoc_func_args(func, retval_default, macro_retval_default)
    if len(result) != result_len:
        result += '\n'
        result_len = len(result)

    # parameter translation
    trans, params = translate_cpptoc_func_args(func)
    if len(trans) != 0:
        result += trans + '\n'

    # execution
    result += '\n  // Execute\n  '

    retval_type = retval.get_retval_type()
    if retval_type != 'none':
        # has a return value
        if retval_type == 'simple' or retval_type == 'bool' or retval_type == 'void*' or \
                retval_type == 'uint8_t*' or retval_type == 'uint32_t*' or retval_type == 'char*' or \
                file_parser.check_arg_type_is_struct(retval_type):
            result += 'return '
        else:
            result += retval.get_type().get_type() + ' _retval = '

    if isinstance(func.parent, file_parser.obj_class):
        parent_clsname = func.parent.get_name()
        if isinstance(func, file_parser.obj_function_virtual):
            if cls.get_name() == parent_clsname:
                result += parent_clsname + 'CppToC::Get(self)->'
            else:
                result += cls.get_name() + 'CppToC::Get(reinterpret_cast<' + cls.get_capi_name() + '*>(self))->'
        else:
            result += parent_clsname + '::'
    result += func.get_name() + '('

    if len(params) > 0:
        result += '\n      ' + ',\n      '.join(params)
    result += ');\n'
    result_len = len(result)

    # parameter restoration
    result += restore_cpptoc_func_args(func)
    if len(result) != result_len:
        result += '\n'
        result_len = len(result)

    if retval_type == 'refptr_same':
        result += '\n  // Return type: ' + retval_type + \
                  '\n  return ' + retval.get_type().get_ptr_type() + 'CppToC::Invert(_retval);'
    elif retval_type == 'refptr_diff':
        result += '\n  // Return type: ' + retval_type + \
                  '\n  return ' + retval.get_type().get_ptr_type() + 'CToCpp::Revert(_retval);'

    if len(result) != result_len:
        result += '\n'

    result += '}\n\n'
    return result


def make_cpptoc_function_body(cls, funcs, prefix, defined_names):
    new_list = []
    old_list = make_file_base.get_func_name_list(funcs)

    impl = ''
    for func in funcs:
        suffix = ''
        new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)
        if new_list.count(func.get_capi_name()) != 0:
            suffix = str(new_list.count(func.get_capi_name()))

        name, _ = make_file_base.get_func_pointer_name(cls, func, prefix, suffix)
        impl += make_cpptoc_function_impl(cls, name, func, defined_names)
    return impl


def cpptoc_make_function_assign(cls, prefix, header):
    funcs = make_file_base.get_class_func_list(cls, header);
    new_list = []
    old_list = make_file_base.get_func_name_list(funcs)

    insert = ''
    assign = ''
    for func in funcs:
        suffix = ''
        new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)
        if new_list.count(func.get_capi_name()) != 0:
            suffix = str(new_list.count(func.get_capi_name()))

        hash_name = make_file_base.get_func_hash_name(func, prefix)
        var_name = make_file_base.get_func_variable_name(func, suffix)
        func_name, _ = make_file_base.get_func_pointer_name(cls, func, prefix, suffix)
        insert += '  funcMemberMap[\"' + hash_name + '\"] = reinterpret_cast<void*>(' + func_name + ');\n  '
        assign += '  GetStruct()->' + var_name + ' = ' + func_name + ';\n'
    return assign, insert


def make_cpptoc_static_function_impl(cls, funcs, defined_names):
    new_list = []
    old_list = make_file_base.get_func_name_list(funcs)

    impl = '#ifdef __cplusplus\nextern "C" {\n#endif // __cplusplus\n\n'
    for func in funcs:
        suffix = ''
        new_list = make_file_base.get_func_name_count(func.get_capi_name(), old_list, new_list)
        if new_list.count(func.get_capi_name()) != 0:
            suffix = str(new_list.count(func.get_capi_name()))
        parts = func.get_capi_parts(defined_names, True)
        func_name, _ = make_file_base.get_func_pointer_name(cls, func, '', suffix)
        impl += make_cpptoc_impl_proto(func_name + '_static', func, parts, True) + ' {\n' + \
                '  ARK_WEB_CPPTOC_DV_LOG();\n\n'

        retval = func.get_retval()
        retval_type = retval.get_retval_type()
        if retval_type != 'none':
            impl += '  return '
        impl += 'OHOS::ArkWeb::' + func_name + '('

        params = []
        args = func.get_arguments()
        for arg in args:
            arg_name = arg.get_type().get_name()
            params.append(arg_name)

        if len(params) > 0:
            impl += '\n      ' + ',\n      '.join(params)
        impl += ');\n}\n\n'

    impl += '#ifdef __cplusplus\n}\n#endif // __cplusplus'
    return impl


def cpptoc_make_unwrap_derived(cls, header, clsname):
    impl = ''
    derived_classes = make_file_base.get_derived_classes(cls, header)
    for clsname in derived_classes:
        impl += '  if (type == ' + file_parser.get_wrapper_type_enum(clsname) + ') {\n' + \
                '    return ' + clsname + 'CppToC::Revert(reinterpret_cast<' + \
                file_parser.get_capi_name(clsname, True) + '*>(s));\n' + \
                '  }\n'
    return impl


def cpptoc_make_include_file(cls, body, header, dir_name):
    result = file_parser.format_translation_includes(header, dir_name, body)
    result += '#include "base/cpptoc/ark_web_cpptoc_macros.h"\n'
    if dir_name == 'ohos_nweb':
        if cls.is_webview_side():
            result += '#include "ohos_nweb/bridge/ark_web_nweb_webview_bridge_helper.h"\n'
        else:
            result += '#include "ohos_nweb/bridge/ark_web_nweb_webcore_bridge_helper.h"\n'
    else:
        if cls.is_webview_side():
            result += '#include "ohos_adapter/bridge/ark_web_adapter_webview_bridge_helper.h"\n'
        else:
            result += '#include "ohos_adapter/bridge/ark_web_adapter_webcore_bridge_helper.h"\n'
    return result


def cpptoc_make_class_function(cls, prefix, header, dir_name):
    assign, insert = cpptoc_make_function_assign(cls, prefix, header)
    result = cls.get_name() + 'CppToC::' + cls.get_name() + 'CppToC() {' + \
             '\n' + assign + \
             '\n  static std::once_flag flag;' + \
             '\n  std::call_once(flag, [] {' + \
             '\n    std::map<std::string, void*> funcMemberMap;' + \
             '\n  ' + insert
    if dir_name == 'ohos_nweb':
        if cls.is_webview_side():
            result += '  ArkWebNWebWebviewBridgeHelper::GetInstance().RegisterFuncMember('
        else:
            result += '  ArkWebNWebWebcoreBridgeHelper::GetInstance().RegisterFuncMember('
    else:
        if cls.is_webview_side():
            result += '  ArkWebAdapterWebviewBridgeHelper::GetInstance().RegisterFuncMember('
        else:
            result += '  ArkWebAdapterWebcoreBridgeHelper::GetInstance().RegisterFuncMember('
    result += file_parser.get_wrapper_type_enum(cls.get_name()) + ', funcMemberMap);\n  });\n}\n' + \
              '\n' + cls.get_name() + 'CppToC::~' + cls.get_name() + 'CppToC() {\n}\n\n'
    return result


def cpptoc_make_c_function_body(cls, header, dir_name, defined_names):
    funcs = make_file_base.get_class_func_list(cls, header)
    prefix = file_parser.get_capi_name(cls.get_name(), False)
    result = make_cpptoc_function_body(cls, funcs, prefix, defined_names)
    if len(result) > 0:
        result = make_cpptoc_function_body(cls, cls.get_static_funcs(), '', defined_names) + \
                 'namespace {\n\n' + result + '}  // namespace\n\n' + \
                 cpptoc_make_class_function(cls, prefix, header, dir_name)
    else:
        result = make_cpptoc_function_body(cls, cls.get_static_funcs(), '', defined_names) + \
                 cpptoc_make_class_function(cls, prefix, header, dir_name)
    return result


def make_cpptoc_impl_file(header, dir_path, dir_name, clsname):
    defined_names = header.get_defined_structs()
    cls = header.get_class(clsname, defined_names)
    if cls is None:
        raise Exception('Class does not exist: ' + clsname)

    defined_names.append(cls.get_capi_name())
    function_body = cpptoc_make_c_function_body(cls, header, dir_name, defined_names)

    unwrap_derived = cpptoc_make_unwrap_derived(cls, header, clsname)
    include_file = cpptoc_make_include_file(cls, function_body + unwrap_derived, header, dir_name)

    content = make_file_base.get_copyright() + '\n' + include_file + '\n' + \
              'namespace OHOS::ArkWeb {\n\n' + \
              function_body + '\n' + \
              '\n' + make_file_base.make_wrapper_type(cls, 'CppToC') + \
              '\n\n} // namespace OHOS::ArkWeb\n\n'

    if len(cls.get_static_funcs()) > 0:
        content += make_cpptoc_static_function_impl(cls, cls.get_static_funcs(), defined_names)

    absolute_dir = os.path.join(os.path.join(dir_path, dir_name), 'cpptoc')
    absolute_path = os.path.join(absolute_dir, file_parser.get_capi_name(clsname, False) + '_cpptoc.cpp')
    return (content, absolute_path)
