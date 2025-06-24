/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_WEBVIEW_JAVASCRIPT_RESULT_CALLBACK_IMPL_H
#define NWEB_WEBVIEW_JAVASCRIPT_RESULT_CALLBACK_IMPL_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_parse_utils.h"
#include "nweb.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_log.h"
#include "nweb_value.h"
#include "uv.h"

namespace OHOS::NWeb {
typedef struct RegisterJavaScriptProxyParam {
    napi_env env;
    napi_value obj;
    std::string objName;
    std::vector<std::string> syncMethodList;
    std::vector<std::string> asyncMethodList;
    std::string permission;
} RegisterJavaScriptProxyParam;

class JavaScriptOb {
public:
    // to be compatible with older webcotroller, be sure to the same as ace and core
    enum class JavaScriptObjIdErrorCode : int32_t { WEBCONTROLLERERROR = -2, WEBVIEWCONTROLLERERROR = -1, END = 0 };

    typedef int32_t ObjectID;

    static std::shared_ptr<JavaScriptOb> CreateNamed(
        napi_env env, int32_t containerScopeId, napi_value value, size_t refCount = 1);
    static std::shared_ptr<JavaScriptOb> CreateTransient(
        napi_env env, int32_t containerScopeId, napi_value value, int32_t holder, size_t refCount = 1);

    JavaScriptOb(napi_env env, int32_t containerScopeId, napi_value value, size_t refCount = 1);

    JavaScriptOb(
        napi_env env, int32_t containerScopeId, napi_value value, std::set<int32_t> holders, size_t refCount = 1);

    JavaScriptOb(const JavaScriptOb& job)
    {
        *this = job;
    }

    JavaScriptOb(JavaScriptOb&& job)
    {
        *this = std::move(job);
    }

    JavaScriptOb& operator=(const JavaScriptOb& job)
    {
        if (this != &job) {
            Delete();
            env_ = job.env_;
            isStrongRef_ = job.isStrongRef_;
            if (isStrongRef_) {
                objRef_ = job.objRef_;
                napi_status s = napi_reference_ref(env_, objRef_, nullptr);
                if (s != napi_ok) {
                    WVLOG_E("JavaScriptOb copy assign fail");
                }
            } else {
                napi_status s = CreateNewWeakRef(env_, job.objRef_, &objRef_);
                if (s != napi_ok) {
                    WVLOG_E("JavaScriptOb copy assign fail");
                }
            }
        }
        return *this;
    }

    JavaScriptOb& operator=(JavaScriptOb&& job)
    {
        if (this != &job) {
            Delete();
            env_ = job.env_;
            objRef_ = job.objRef_;
            isStrongRef_ = job.isStrongRef_;
            job.env_ = nullptr;
            job.objRef_ = nullptr;
        }
        return *this;
    }

    ~JavaScriptOb()
    {
        Delete();
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    void SetContainerScopeId(int32_t newId)
    {
        containerScopeId_ = newId;
    }

    int32_t GetContainerScopeId() const
    {
        return containerScopeId_;
    }

    bool IsEmpty() const
    {
        return !objRef_;
    }

    bool IsStrongRef()
    {
        return isStrongRef_;
    }

    napi_value GetValue() const
    {
        napi_value result = nullptr;
        napi_get_reference_value(env_, objRef_, &result);
        return result;
    }

    void ToWeakRef()
    {
        if (!isStrongRef_ || !objRef_) {
            return;
        }

        if (Release() == 0) {
            isStrongRef_ = false;
            return;
        }

        isStrongRef_ = false;
        napi_status s = CreateNewWeakRef(env_, objRef_, &objRef_);
        if (s != napi_ok) {
            WVLOG_E("JavaScriptOb ToWeakRef fail");
        }
    }

    bool IsNamed() const
    {
        return namesCount_ > 0;
    }
    void AddName()
    {
        ++namesCount_;
    }
    void RemoveName()
    {
        --namesCount_;
    }

    bool HasHolders()
    {
        return !holders_.empty();
    }
    void AddHolder(int32_t holder)
    {
        holders_.insert(holder);
    }
    void RemoveHolder(int32_t holder)
    {
        holders_.erase(holder);
    }

    std::vector<std::string> GetMethodNames()
    {
        if (!isMethodsSetup_) {
            SetUpMethods();
        }
        return methods_;
    }

    std::vector<std::string> GetSyncMethodNames()
    {
        if (!isMethodsSetup_) {
            SetUpMethods();
        }
        if (asyncMethods_.empty()) {
            return methods_;
        }

        std::vector<std::string> syncMethodNames;
        for (const auto& method : methods_) {
            auto it = std::find(asyncMethods_.begin(), asyncMethods_.end(), method);
            if (it == asyncMethods_.end()) {
                syncMethodNames.emplace_back(method);
            }
        }
        return syncMethodNames;
    }

    std::vector<std::string> GetAsyncMethodNames()
    {
        return asyncMethods_;
    }

    std::string GetPermission()
    {
        return permission_;
    }

    bool HasMethod(const std::string& methodName)
    {
        if (methodName.empty()) {
            WVLOG_E("HasMethod methodName null");
            return false;
        }

        if (!isMethodsSetup_) {
            SetUpMethods();
        }
        for (std::vector<std::string>::iterator iter = methods_.begin(); iter != methods_.end(); ++iter) {
            if (*iter == methodName) {
                return true;
            }
        }
        return false;
    }

    napi_value FindMethod(const std::string& methodName)
    {
        if (HasMethod(methodName)) {
            bool hasFunc = false;
            napi_value result = nullptr;
            napi_valuetype valueType = napi_undefined;
            napi_value obj = GetValue();
            if (!obj) {
                WVLOG_E("JavaScriptOb FindMethod obj null");
                return nullptr;
            }
            napi_status s = napi_has_named_property(env_, obj, methodName.c_str(), &hasFunc);
            if (s != napi_ok) {
                WVLOG_E("JavaScriptOb FindMethod fail");
                return nullptr;
            }
            if (!hasFunc) {
                WVLOG_E("JavaScriptOb FindMethod fail");
                return nullptr;
            }
            s = napi_get_named_property(env_, obj, methodName.c_str(), &result);
            if (s != napi_ok) {
                WVLOG_E("JavaScriptOb FindMethod fail");
                return nullptr;
            }
            napi_typeof(env_, result, &valueType);
            if (valueType != napi_function) {
                WVLOG_E("JavaScriptOb FindMethod not function");
                return nullptr;
            }
            return result;
        }

        return nullptr;
    }

    void SetUpMethods()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (isMethodsSetup_) {
            return;
        }
        napi_value propertyNames;
        napi_value obj = GetValue();
        napi_status s = napi_get_all_property_names(env_, obj, napi_key_include_prototypes, napi_key_all_properties,
            napi_key_numbers_to_strings, &propertyNames);
        if (s != napi_ok) {
            WVLOG_E("JavaScriptOb SetUpMethods fail");
            return;
        }
        uint32_t size;
        s = napi_get_array_length(env_, propertyNames, &size);
        if (s != napi_ok) {
            WVLOG_E("JavaScriptOb SetUpMethods fail");
            return;
        }
        for (uint32_t i = 0; i < size; i++) {
            napi_value napiKeyTmp;
            s = napi_get_element(env_, propertyNames, i, &napiKeyTmp);
            if (s != napi_ok) {
                WVLOG_E("JavaScriptOb SetUpMethods fail");
                return;
            }
            napi_valuetype valueType = napi_undefined;
            napi_value napiValueTmp;
            s = napi_get_property(env_, obj, napiKeyTmp, &napiValueTmp);
            if (s != napi_ok) {
                WVLOG_E("JavaScriptOb SetUpMethods fail");
                return;
            }
            napi_typeof(env_, napiValueTmp, &valueType);
            if (valueType != napi_function) {
                continue;
            }
            std::string methodName;
            if (NapiParseUtils::ParseString(env_, napiKeyTmp, methodName)) {
                methods_.push_back(methodName);
            }
        }
        isMethodsSetup_ = true;
    }

    void SetMethods(std::vector<std::string> methods_name)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        methods_ = methods_name;
        isMethodsSetup_ = true;
    }

    void SetAsyncMethods(std::vector<std::string> async_methods_name)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        asyncMethods_ = async_methods_name;
    }

    void SetPermission(std::string permission)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        permission_ = permission;
    }

private:
    static napi_status CreateNewWeakRef(napi_env env, napi_ref ref, napi_ref* new_ref)
    {
        napi_value val = nullptr;
        napi_status sts = napi_get_reference_value(env, ref, &val);
        if (sts != napi_ok)
            return sts;
        return napi_create_reference(env, val, 0, new_ref);
    }

    void Delete()
    {
        if (objRef_ && Release() == 0) {
            WVLOG_D("JavaScriptOb delete called");
            napi_delete_reference(env_, objRef_);
            objRef_ = nullptr;
        }
    }

    uint32_t Release()
    {
        if (!objRef_ || !isStrongRef_) {
            return 0;
        }
        uint32_t refCount = 0;
        napi_status s = napi_reference_unref(env_, objRef_, &refCount);
        if (s != napi_ok) {
            WVLOG_E("JavaScriptOb Release fail");
        }
        return refCount;
    }

    napi_env env_ = nullptr;
    int32_t containerScopeId_ = -1;

    napi_ref objRef_ = nullptr;
    bool isStrongRef_ = true;

    // methods_ contains sync methods and async methods.
    std::vector<std::string> methods_;
    std::vector<std::string> asyncMethods_;

    // allow list
    std::string permission_;

    // An object must be kept in retainedObjectSet_ either if it has
    // names or if it has a non-empty holders set.
    int namesCount_;
    std::set<int32_t> holders_;
    bool isMethodsSetup_ = false;
    std::mutex mutex_;
};

class WebviewJavaScriptResultCallBack : public NWebJavaScriptResultCallBack {
public:
    typedef std::unordered_map<std::string, JavaScriptOb::ObjectID> NamedObjectMap;
    typedef std::unordered_map<JavaScriptOb::ObjectID, std::shared_ptr<JavaScriptOb>> ObjectMap;
    typedef int32_t ObjectID;
    struct H5Bundle {
        int32_t nwebId;
        int32_t frameRoutingId;
        int32_t h5Id;
        std::string funcName;
    };

    enum class NapiJsCallBackParmFlag : int32_t { ISOBJECT = 1, END = 2 };

    struct NapiJsCallBackInParm {
        WebviewJavaScriptResultCallBack* webJsResCb = nullptr;
        int32_t nwebId = -1;
        int32_t frameRoutingId = -1;
        int32_t objId = -1;
        int32_t containerScopeId = -1;
        std::string objName;
        std::string methodName;
        void* data = nullptr;
    };

    struct NapiJsCallBackOutParm {
        napi_status status;
        int32_t errCode = -1;
        NapiJsCallBackParmFlag flag;
        void* ret = nullptr;
    };

    struct NapiJsCallBackParm {
        // environment
        napi_env env = nullptr;
        int32_t containerScopedId = -1;

        std::mutex mutex;
        //  sync
        std::condition_variable condition;
        bool ready = false;
        // async
        napi_async_work asyncWork = nullptr;
        napi_deferred deferred = nullptr;
        napi_ref callbackRef = nullptr;
        // input
        void* input = nullptr;
        // out
        void* out = nullptr;
    };

    WebviewJavaScriptResultCallBack() {}

    explicit WebviewJavaScriptResultCallBack(int32_t nwebId);

    ~WebviewJavaScriptResultCallBack() override;

    std::shared_ptr<NWebValue> GetJavaScriptResult(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName, int32_t routingId, int32_t objectId) override;

    std::shared_ptr<NWebValue> GetJavaScriptResultFlowbuf(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName, int fd, int32_t routingId, int32_t objectId) override;

    bool HasJavaScriptObjectMethods(int32_t objectId, const std::string& methodName) override;

    std::shared_ptr<NWebValue> GetJavaScriptObjectMethods(int32_t objectId) override;

    std::shared_ptr<JavaScriptOb> FindObject(JavaScriptOb::ObjectID objectId);

    void RemoveJavaScriptObjectHolder(int32_t holder, JavaScriptOb::ObjectID objectId) override;

    void RemoveJavaScriptObjectHolderInJsTd(int32_t holder, JavaScriptOb::ObjectID objectId);

    void RemoveTransientJavaScriptObject() override;

    void RemoveTransientJavaScriptObjectInJsTd();

    bool FindObjectIdInJsTd(napi_env env, napi_value object, JavaScriptOb::ObjectID* objectId);

    std::unordered_map<std::string, std::shared_ptr<JavaScriptOb>> GetNamedObjects();

    ObjectMap GetObjectMap();

    JavaScriptOb::ObjectID AddObject(napi_env env, const napi_value& object, bool methodName, int32_t holder);

    void SetUpAnnotateMethods(JavaScriptOb::ObjectID objId, std::vector<std::string>& methodNameList);

    JavaScriptOb::ObjectID RegisterJavaScriptProxy(RegisterJavaScriptProxyParam& param);

    bool DeleteJavaScriptRegister(const std::string& objName);

    void CallH5FunctionInternal(napi_env env, H5Bundle& bundle, std::vector<std::shared_ptr<NWebValue>> nwebArgs);

    int32_t GetNWebId()
    {
        return nwebId_;
    }

    void UpdateInstanceId(int32_t newId);
private:
    bool RemoveNamedObject(const std::string& name);

    JavaScriptOb::ObjectID AddNamedObject(napi_env env, napi_value& obj, const std::string& objName);

    std::shared_ptr<NWebValue> PostGetJavaScriptResultToJsThread(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName, int32_t routingId, int32_t objectId);

    bool PostHasJavaScriptObjectMethodsToJsThread(int32_t objectId, const std::string& methodName);
    std::shared_ptr<NWebValue> PostGetJavaScriptObjectMethodsToJsThread(int32_t objectId);

    void PostRemoveJavaScriptObjectHolderToJsThread(int32_t holder, JavaScriptOb::ObjectID objectId);

    std::shared_ptr<NWebValue> GetJavaScriptResultSelf(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName, int32_t routingId, int32_t objectId);

    bool ConstructArgv(void* ashmem, std::vector<std::shared_ptr<NWebValue>> args,
    	std::vector<napi_value>& argv, std::shared_ptr<JavaScriptOb> jsObj, int32_t routingId);

    std::shared_ptr<NWebValue> GetJavaScriptResultSelfHelper(std::shared_ptr<JavaScriptOb> jsObj,
        const std::string& method, int32_t routingId, napi_handle_scope scope, std::vector<napi_value> argv);

    char* FlowbufStrAtIndex(void* mem, int flowbufIndex, int* argIndex, int* strLen);

    std::shared_ptr<NWebValue> GetJavaScriptResultSelfFlowbuf(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName, int fd, int32_t routingId, int32_t objectId);
    
    void PostRemoveTransientJavaScriptObjectToJsThread(std::shared_ptr<JavaScriptOb> jsObj);

    int32_t nwebId_ = -1;

    JavaScriptOb::ObjectID nextObjectId_ = 1;
    NamedObjectMap namedObjects_;
    ObjectMap objects_;
    std::unordered_set<std::shared_ptr<JavaScriptOb>> retainedObjectSet_;
};
}
#endif
