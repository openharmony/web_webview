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
#include "webview_javascript_result_callback.h"

#include <sys/mman.h>
#include <unistd.h>

#include "webview_log.h"
#include "ohos_adapter_helper.h"

#define MAX_FLOWBUF_DATA_SIZE 52428800 /* 50MB */
#define MAX_ENTRIES 10
#define HEADER_SIZE (MAX_ENTRIES * 8)  /* 10 * (int position + int length) */
#define INDEX_SIZE 2

using namespace OHOS::NWeb;

namespace OHOS::Webview {

std::unordered_map<int32_t, WebviewJavaScriptResultCallBackImpl*> g_webviewJsResultCallbackMap;
std::mutex g_objectMtx;

bool vectorEqual(const std::vector<std::string>& v1, const std::vector<std::string>& v2)
{
    if (v1.size() != v2.size()) {
        return false;
    }
    int size = static_cast<int>(v1.size());
    for (int i = 0; i < size; i++) {
        if (v1[i] != v2[i]) {
            return false;
        }
    }
    return true;
}

WebviewJavaScriptResultCallBackImpl::WebviewJavaScriptResultCallBackImpl(int32_t nwebId) : nwebId_(nwebId)
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webviewJsResultCallbackMap.emplace(nwebId, this);
}

WebviewJavaScriptResultCallBackImpl::~WebviewJavaScriptResultCallBackImpl()
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webviewJsResultCallbackMap.erase(nwebId_);
}

bool WebviewJavaScriptResultCallBackImpl::FindObjectIdInJsTd(
    const std::vector<std::function<char*(const char*)>>& cjFuncs,
    const std::vector<std::string>& methodList, JavaScriptOb::ObjectID& objectId)
{
    objectId = static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBVIEWCONTROLLERERROR);
    for (const auto& pair : objects_) {
        bool result;
        if (pair.second == nullptr) {
            result = false;
        } else {
            result = (pair.second->GetFuncs().size() == cjFuncs.size())
                && vectorEqual(pair.second->GetMethodNames(), methodList);
        }
        if (result) {
            objectId = pair.first;
            return true;
        }
    }
    return false;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBackImpl::AddObject(
    const std::vector<std::function<char*(const char*)>>& cjFuncs)
{
    JavaScriptOb::ObjectID objectId;
    {
        auto new_object = JavaScriptOb::CreateNamed(cjFuncs);
        objectId = nextObjectId_++;
        WEBVIEWLOGD("WebviewJavaScriptResultCallBackImpl::AddObject objectId = "
                "%{public}d",
            static_cast<int32_t>(objectId));
        objects_[objectId] = new_object;
    }
    return objectId;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBackImpl::AddNamedObject(
    const std::vector<std::function<char*(const char*)>>& cjFuncs,
    const std::vector<std::string>& methodList, const std::string& objName)
{
    JavaScriptOb::ObjectID objectId;
    NamedObjectMap::iterator iter = namedObjects_.find(objName);
    bool methodName = FindObjectIdInJsTd(cjFuncs, methodList, objectId);
    if (methodName && iter != namedObjects_.end() && iter->second == objectId) {
        // Nothing to do.
        return objectId;
    }
    if (iter != namedObjects_.end()) {
        RemoveNamedObject(iter->first);
    }
    if (methodName && objects_[objectId] != nullptr) {
        objects_[objectId]->AddName();
    } else {
        objectId = AddObject(cjFuncs);
    }
    namedObjects_[objName] = objectId;
    return objectId;
}

bool WebviewJavaScriptResultCallBackImpl::RemoveNamedObject(const std::string& name)
{
    WEBVIEWLOGD("WebviewJavaScriptResultCallBackImpl::RemoveNamedObject called, "
            "name = %{public}s",
        name.c_str());
    NamedObjectMap::iterator iter = namedObjects_.find(name);
    if (iter == namedObjects_.end()) {
        return false;
    }
    if (objects_[iter->second]) {
        objects_[iter->second]->RemoveName();
    }
    namedObjects_.erase(iter);
    return true;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBackImpl::RegisterJavaScriptProxy(
    const std::vector<std::function<char*(const char*)>>& cjFuncs,
    const std::string& objName, const std::vector<std::string>& methodList)
{
    JavaScriptOb::ObjectID objId = AddNamedObject(cjFuncs, methodList, objName);
    // set up named object method
    if (namedObjects_.find(objName) != namedObjects_.end() && objects_[namedObjects_[objName]]) {
        objects_[namedObjects_[objName]]->SetMethods(methodList);
    }
    WEBVIEWLOGD("WebviewJavaScriptResultCallBackImpl::RegisterJavaScriptProxy called, "
            "objectId = %{public}d",
        static_cast<int32_t>(objId));
    return objId;
}

std::shared_ptr<JavaScriptOb> WebviewJavaScriptResultCallBackImpl::FindObject(JavaScriptOb::ObjectID objectId)
{
    auto iter = objects_.find(objectId);
    if (iter != objects_.end()) {
        return iter->second;
    }
    WEBVIEWLOGE("WebviewJavaScriptResultCallBackImpl::FindObject Unknown object: objectId = "
            "%{public}d",
        objectId);
    return nullptr;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptResultSelf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName,
    int32_t routingId, int32_t objectId)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    WEBVIEWLOGI("WebviewJavaScriptResultCallBackImpl::GetJavaScriptResultSelf");
    std::string argv;
    if (args.size() == 0) {
        argv = "";
    } else {
        argv = args[0]->GetString();
    }
    auto callback = jsObj->FindMethod(method);
    if (!callback) {
        WEBVIEWLOGE("WebviewJavaScriptResultCallBackImpl::ExecuteGetJavaScriptResult callback null");
        return ret;
    }
    auto argCj = MallocCString(argv);
    if (argCj == nullptr) {
        return ret;
    }
    char* cjRet = callback(argCj);
    std::string strVal = std::string(cjRet);
    free(cjRet);
    ret->SetType(NWebValue::Type::STRING);
    ret->SetString(strVal);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName,
    int32_t routingId, int32_t objectId)
{
    WEBVIEWLOGD("GetJavaScriptResult method = %{public}s", method.c_str());
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj || jsObj->HasMethod(method) == -1) {
        return ret;
    }

    return GetJavaScriptResultSelf(args, method, objName, routingId, objectId);
}

char* WebviewJavaScriptResultCallBackImpl::FlowbufStrAtIndex(
    void* mem, int flowbufIndex, int* argIndex, int* strLen)
{
    int* header = static_cast<int*>(mem); // Cast the memory block to int* for easier access
    int offset = 0;
    if (argIndex == nullptr) {
        return nullptr;
    }
    if (flowbufIndex >=  MAX_ENTRIES) {
        *argIndex = -1;
        return nullptr;
    }

    int* entry = header + (flowbufIndex * INDEX_SIZE);
    if (entry == nullptr) {
        return nullptr;
    }
    if (*(entry + 1) == 0) { // Check if length is 0, indicating unused entry
        *argIndex = -1;
        return nullptr;
    }

    int i = 0;
    for (i = 0; i < flowbufIndex; i++) {
        offset += *(header + (i * INDEX_SIZE) + 1);
    }
    if (strLen == nullptr) {
        return nullptr;
    }
    *strLen = *(header + (i * INDEX_SIZE) + 1) - 1;

    *argIndex = *entry;

    char* dataSegment = static_cast<char*>(mem) + HEADER_SIZE;
    char* currentString = dataSegment + offset;
    return currentString;
}

bool WebviewJavaScriptResultCallBackImpl::ConstructArgv(void* ashmem,
    std::vector<std::shared_ptr<NWebValue>> args,
    std::vector<std::string>& argv,
    std::shared_ptr<JavaScriptOb> jsObj,
    int32_t routingId)
{
    int argIndex = -1;
    int currIndex = 0;
    int flowbufIndex = 0;
    int strLen = 0;
    char* flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
    flowbufIndex++;
    while (argIndex == currIndex) {
        argv.push_back(std::string(flowbufStr));
        currIndex ++;
        flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
        flowbufIndex++;
    }

    for (std::shared_ptr<NWebValue> input : args) {
        while (argIndex == currIndex) {
            argv.push_back(std::string(flowbufStr));
            currIndex ++;
            flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
            flowbufIndex++;
        }
        argv.push_back(input->GetString());
        currIndex++;
    }

    while (argIndex == currIndex) {
        argv.push_back(std::string(flowbufStr));
        currIndex ++;
        flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
        flowbufIndex++;
    }
    return true;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptResultSelfHelper(
    std::shared_ptr<JavaScriptOb> jsObj,
    const std::string& method,
    int32_t routingId,
    std::vector<std::string> argv)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    auto callback = jsObj->FindMethod(method);
    if (!callback) {
        WEBVIEWLOGE("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult callback null");
        return ret;
    }
    std::string arg;
    if (argv.size() == 0) {
        arg = "";
    } else {
        arg = argv[0];
    }
    auto argCj = MallocCString(arg);
    if (argCj == nullptr) {
        return ret;
    }
    char* cjRet = callback(argCj);
    std::string strVal = std::string(cjRet);
    free(cjRet);
    ret->SetType(NWebValue::Type::STRING);
    ret->SetString(strVal);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptResultSelfFlowbuf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName, int fd,
    int32_t routingId, int32_t objectId)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        return ret;
    }
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
    if (!ashmem) {
        return ret;
    }

    std::vector<std::string> argv = {};
    if (!ConstructArgv(ashmem, args, argv, jsObj, routingId)) {
        return ret;
    }
    close(fd);

    ret = GetJavaScriptResultSelfHelper(jsObj, method, routingId, argv);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptResultFlowbuf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName, int fd,
    int32_t routingId, int32_t objectId)
{
    (void)objName; // to be compatible with older webcotroller, classname may be empty
    WEBVIEWLOGD("GetJavaScriptResult method = %{public}s", method.c_str());
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj || jsObj->HasMethod(method) == -1) {
        return ret;
    }
    return GetJavaScriptResultSelfFlowbuf(args, method, objName, fd, routingId, objectId);
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBackImpl::GetJavaScriptObjectMethods(int32_t objectId)
{
    auto ret = std::make_shared<NWebValue>(NWebValue::Type::LIST);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    auto methods = jsObj->GetMethodNames();
    for (auto& method : methods) {
        ret->AddListValue(NWebValue(method));
    }
    return ret;
}

bool WebviewJavaScriptResultCallBackImpl::HasJavaScriptObjectMethods(int32_t objectId, const std::string& methodName)
{
    bool ret = false;
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return false;
    }
    if (jsObj->HasMethod(methodName) != -1) {
        ret = true;
    } else {
        WEBVIEWLOGD("WebviewJavaScriptResultCallBackImpl::HasJavaScriptObjectMethods cannot find "
                "object");
    }
    return ret;
}

void WebviewJavaScriptResultCallBackImpl::RemoveJavaScriptObjectHolder(int32_t holder, JavaScriptOb::ObjectID objectId)
{}

void WebviewJavaScriptResultCallBackImpl::RemoveTransientJavaScriptObject()
{}

} // namespace OHOS::Webview
