/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef WEBVIEW_JAVA_SCRIPT_RESULT_CALLBACK_H
#define WEBVIEW_JAVA_SCRIPT_RESULT_CALLBACK_H

#include <string>

#include "webview_ffi.h"
#include "nweb.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_web_message.h"
#include "webview_log.h"


namespace OHOS::Webview {

class JavaScriptOb {
public:
    // to be compatible with older webcotroller, be sure to the same as ace and core
    enum class JavaScriptObjIdErrorCode : int32_t { WEBCONTROLLERERROR = -2, WEBVIEWCONTROLLERERROR = -1, END = 0 };

    typedef int32_t ObjectID;

    static std::shared_ptr<JavaScriptOb> CreateNamed(std::vector<std::function<char*(const char*)>> cjFuncs)
    {
        return std::make_shared<JavaScriptOb>(cjFuncs);
    }
    
    explicit JavaScriptOb(std::vector<std::function<char*(const char*)>> cjFuncs) : cjFuncs_(cjFuncs), namesCount_(0)
    {}

    bool IsNamed()
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

    std::vector<std::string> GetMethodNames()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return methods_;
    }

    std::string GetPermission()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return permission_;
    }

    std::vector<std::function<char*(const char*)>> GetFuncs()
    {
        return cjFuncs_;
    }

    int HasMethod(const std::string& methodName)
    {
        if (methodName.empty()) {
            WEBVIEWLOGE("HasMethod methodName null");
            return -1;
        }
        std::unique_lock<std::mutex> lock(mutex_);
        if (!isMethodsSetup_) {
            return -1;
        }
        int size = static_cast<int>(methods_.size());
        for (int i = 0; i < size; i++) {
            if (methodName == methods_[i]) {
                return i;
            }
        }
        return -1;
    }

    std::function<char*(const char*)> FindMethod(const std::string& methodName)
    {
        auto index = HasMethod(methodName);
        if (index != -1 && index < static_cast<int>(cjFuncs_.size())) {
            return cjFuncs_[index];
        }
        return nullptr;
    }

    void SetMethods(std::vector<std::string> methods_name)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        methods_ = methods_name;
        isMethodsSetup_ = true;
    }

    void SetPermission(std::string permission)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        permission_ = permission;
    }

private:

    std::vector<std::function<char*(const char*)>> cjFuncs_;

    std::vector<std::string> methods_;

    int namesCount_;

    bool isMethodsSetup_ = false;

    std::mutex mutex_;

    // allow list
    std::string permission_;
};

class WebviewJavaScriptResultCallBackImpl : public NWeb::NWebJavaScriptResultCallBack {
public:
    typedef std::unordered_map<std::string, JavaScriptOb::ObjectID> NamedObjectMap;
    typedef std::unordered_map<JavaScriptOb::ObjectID, std::shared_ptr<JavaScriptOb>> ObjectMap;
    
    WebviewJavaScriptResultCallBackImpl() {}

    explicit WebviewJavaScriptResultCallBackImpl(int32_t nwebId);

    ~WebviewJavaScriptResultCallBackImpl() override;

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptResult(std::vector<std::shared_ptr<NWeb::NWebValue>> args,
        const std::string& method, const std::string& objName, int32_t routingId, int32_t objectId) override;

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptResultFlowbuf(std::vector<std::shared_ptr<NWeb::NWebValue>> args,
        const std::string& method, const std::string& objName, int fd, int32_t routingId, int32_t objectId) override;

    bool HasJavaScriptObjectMethods(int32_t objectId, const std::string& methodName) override;

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptObjectMethods(int32_t objectId) override;

    std::shared_ptr<JavaScriptOb> FindObject(JavaScriptOb::ObjectID objectId);

    bool FindObjectIdInJsTd(
        const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::vector<std::string>& methodList, JavaScriptOb::ObjectID& objectId);

    JavaScriptOb::ObjectID AddObject(const std::vector<std::function<char*(const char*)>>& cjFuncs);

    JavaScriptOb::ObjectID RegisterJavaScriptProxy(
        const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::string& objName, const std::vector<std::string>& methodList, const std::string& permission = "");

    void RemoveJavaScriptObjectHolder(int32_t holder, JavaScriptOb::ObjectID objectId) override;

    void RemoveTransientJavaScriptObject() override;

    bool DeleteJavaScriptRegister(const std::string &objName);

    void GetJavaScriptResultV2(const std::vector<std::shared_ptr<NWeb::NWebHapValue>>& args, const std::string& method,
        const std::string& objectName, int32_t routingId, int32_t objectId,
        std::shared_ptr<NWeb::NWebHapValue> result) override;

    void GetJavaScriptResultFlowbufV2(const std::vector<std::shared_ptr<NWeb::NWebHapValue>>& args,
        const std::string& method, const std::string& objectName, int fd, int32_t routingId, int32_t objectId,
        std::shared_ptr<NWeb::NWebHapValue> result) override;

    void GetJavaScriptObjectMethodsV2(int32_t objectId, std::shared_ptr<NWeb::NWebHapValue> result) override;

    int32_t GetNWebId()
    {
        return nwebId_;
    }

private:
    bool RemoveNamedObject(const std::string& name);

    JavaScriptOb::ObjectID AddNamedObject(const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::vector<std::string>& methodList, const std::string& objName);

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptResultSelf(std::vector<std::shared_ptr<NWeb::NWebValue>> args,
        const std::string& method, const std::string& objName, int32_t routingId, int32_t objectId);

    bool ConstructArgv(void* ashmem, std::vector<std::shared_ptr<NWeb::NWebValue>> args,
    	std::vector<std::string>& argv, std::shared_ptr<JavaScriptOb> jsObj, int32_t routingId);

    char* FlowbufStrAtIndex(void* mem, int flowbufIndex, int* argIndex, int* strLen);

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptResultSelfHelper(std::shared_ptr<JavaScriptOb> jsObj,
        const std::string& method, int32_t routingId, std::vector<std::string> argv);

    std::shared_ptr<NWeb::NWebValue> GetJavaScriptResultSelfFlowbuf(std::vector<std::shared_ptr<NWeb::NWebValue>> args,
        const std::string& method, const std::string& objName, int fd, int32_t routingId, int32_t objectId);

    void ConstructArgvV2(void* ashmem, const std::vector<std::shared_ptr<NWeb::NWebHapValue>>& args,
        std::vector<std::string>& argv, std::shared_ptr<JavaScriptOb> jsObj, int32_t routingId);
    
    void GetJavaScriptResultSelfHelperV2(std::shared_ptr<JavaScriptOb> jsObj, const std::string& method,
        int32_t routingId, const std::vector<std::string>& argv, std::shared_ptr<NWeb::NWebHapValue> result);
    
    int32_t nwebId_ = -1;

    JavaScriptOb::ObjectID nextObjectId_ = 1;
    NamedObjectMap namedObjects_;
    ObjectMap objects_;
};

} // namespace OHOS::Webview
#endif
