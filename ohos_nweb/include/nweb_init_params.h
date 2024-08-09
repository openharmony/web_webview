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
#ifndef NWEB_INIT_PARAMS_H
#define NWEB_INIT_PARAMS_H

#include "nweb.h"
#include "nweb_download_manager.h"

namespace OHOS::NWeb {

class NWebDOHConfigImpl : public NWebDOHConfig {
public:
    NWebDOHConfigImpl() = default;
    ~NWebDOHConfigImpl() = default;

    void SetMode(int mode)
    {
        mode_ = mode;
    }

    int GetMode() override
    {
        return mode_;
    }

    void SetConfig(const std::string& config)
    {
        config_ = config;
    }

    std::string GetConfig() override
    {
        return config_;
    }

private:
    int mode_ = -1;
    std::string config_;
};

class NWebCreateInfoImpl : public NWebCreateInfo {
public:
    NWebCreateInfoImpl() = default;
    ~NWebCreateInfoImpl() = default;

    void SetWidth(uint32_t width)
    {
        width_ = width;
    }

    uint32_t GetWidth() override
    {
        return width_;
    }

    void SetHeight(uint32_t height)
    {
        height_ = height;
    }

    uint32_t GetHeight() override
    {
        return height_;
    }

    void SetIsIncognitoMode(bool isIncognitoMode)
    {
        isIncognitoMode_ = isIncognitoMode;
    }

    bool GetIsIncognitoMode() override
    {
        return isIncognitoMode_;
    }

    void SetProducerSurface(void* producerSurface)
    {
        producerSurface_ = producerSurface;
    }

    void* GetProducerSurface() override
    {
        return producerSurface_;
    }

    void SetEnhanceSurfaceInfo(void* enhanceSurfaceInfo)
    {
        enhanceSurfaceInfo_ = enhanceSurfaceInfo;
    }

    void* GetEnhanceSurfaceInfo() override
    {
        return enhanceSurfaceInfo_;
    }

    void SetEngineInitArgs(std::shared_ptr<NWebEngineInitArgs> initArgs)
    {
        initArgs_ = initArgs;
    }

    std::shared_ptr<NWebEngineInitArgs> GetEngineInitArgs() override
    {
        return initArgs_;
    }

    void SetOutputFrameCallback(std::shared_ptr<NWebOutputFrameCallback> outputFrameCallback)
    {
        outputFrameCallback_ = outputFrameCallback;
    }

    std::shared_ptr<NWebOutputFrameCallback> GetOutputFrameCallback() override
    {
        return outputFrameCallback_;
    }

private:
    uint32_t width_ = 0;
    uint32_t height_ = 0;

    bool isIncognitoMode_ = false;

    void* producerSurface_ = nullptr;
    void* enhanceSurfaceInfo_ = nullptr;

    std::shared_ptr<NWebEngineInitArgs> initArgs_ = nullptr;
    std::shared_ptr<NWebOutputFrameCallback> outputFrameCallback_ = nullptr;
};

class NWebEngineInitArgsImpl : public NWebEngineInitArgs {
public:
    NWebEngineInitArgsImpl() = default;
    ~NWebEngineInitArgsImpl() = default;

    void AddArg(const std::string& arg)
    {
        argsToAdd_.emplace_back(arg);
    }

    void AddDeleteArg(const std::string& arg)
    {
        argsToDelete_.emplace_back(arg);
    }

    void SetDumpPath(const std::string& dumpPath)
    {
        dumpPath_ = dumpPath;
    }

    std::string GetDumpPath() override
    {
        return dumpPath_;
    }

    void SetIsPopup(bool isPopup)
    {
        isPopup_ = isPopup;
    }

    bool GetIsPopup() override
    {
        return isPopup_;
    }

    void SetIsFrameInfoDump(bool isFrameInfoDump)
    {
        isFrameInfoDump_ = isFrameInfoDump;
    }

    bool GetIsFrameInfoDump() override
    {
        return isFrameInfoDump_;
    }

    void SetIsEnhanceSurface(bool isEnhanceSurface)
    {
        isEnhanceSurface_ = isEnhanceSurface;
    }

    bool GetIsEnhanceSurface() override
    {
        return isEnhanceSurface_;
    }

    void SetIsMultiRendererProcess(bool isMultiRendererProcess)
    {
        isMultiRendererProcess_ = isMultiRendererProcess;
    }

    bool GetIsMultiRendererProcess() override
    {
        return isMultiRendererProcess_;
    }

    void SetArgsToAdd(const std::list<std::string>& argsToAdd)
    {
        argsToAdd_ = argsToAdd;
    }

    std::list<std::string> GetArgsToAdd() override
    {
        return argsToAdd_;
    }

    void SetArgsToDelete(const std::list<std::string>& argsToDelete)
    {
        argsToDelete_ = argsToDelete;
    }

    std::list<std::string> GetArgsToDelete() override
    {
        return argsToDelete_;
    }

    void SetSharedRenderProcessToken(const std::string& sharedRenderProcessToken)
    {
        sharedRenderProcessToken_ = sharedRenderProcessToken;
    }

    std::string GetSharedRenderProcessToken() override
    {
        return sharedRenderProcessToken_;
    }

private:
    std::string dumpPath_;

    bool isPopup_ = false;
    bool isFrameInfoDump_ = false;
    bool isEnhanceSurface_ = false;
    bool isMultiRendererProcess_ = false;

    std::list<std::string> argsToAdd_;
    std::list<std::string> argsToDelete_;
    std::string sharedRenderProcessToken_;
};

class NWebEnginePrefetchArgsImpl : public NWebEnginePrefetchArgs {
public:
    NWebEnginePrefetchArgsImpl(const std::string& url, const std::string& method, const std::string& formData)
        : url_(url), method_(method), form_data_(formData)
    {}

    ~NWebEnginePrefetchArgsImpl() = default;

    std::string GetUrl() override
    {
        return url_;
    }

    std::string GetMethod() override
    {
        return method_;
    }

    std::string GetFormData() override
    {
        return form_data_;
    }

private:
    std::string url_;
    std::string method_;
    std::string form_data_;
};

class NWebPDFConfigArgsImpl : public NWebPDFConfigArgs {
public:
    NWebPDFConfigArgsImpl(const double width, const double height, const double scale, const double marginTop,
        const double marginBottom, const double marginRight, const double marginLeft, const bool shouldPrintBackground)
        : width_(width), height_(height), scale_(scale), marginTop_(marginTop), marginBottom_(marginBottom),
          marginRight_(marginRight), marginLeft_(marginLeft), shouldPrintBackground_(shouldPrintBackground)
    {}

    ~NWebPDFConfigArgsImpl() = default;

    double GetWidth() override
    {
        return width_;
    }
    double GetHeight() override
    {
        return height_;
    }
    double GetScale() override
    {
        return scale_;
    }
    double GetMarginTop() override
    {
        return marginTop_;
    }
    double GetMarginBottom() override
    {
        return marginBottom_;
    }
    double GetMarginRight() override
    {
        return marginRight_;
    }
    double GetMarginLeft() override
    {
        return marginLeft_;
    }
    bool GetShouldPrintBackground() override
    {
        return shouldPrintBackground_;
    }

private:
    double width_;
    double height_;
    double scale_;
    double marginTop_;
    double marginBottom_;
    double marginRight_;
    double marginLeft_;
    bool shouldPrintBackground_;
};
} // namespace OHOS::NWeb

#endif // NWEB_INIT_PARAMS_H
