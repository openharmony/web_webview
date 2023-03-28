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

#include "nweb_handler_impl_test.h"

#include <cstring>
#include <fstream>
#include "nweb_test_log.h"
#include "nweb_url_resource_request.h"
#include "nweb_url_resource_response.h"
#include "securec.h"

namespace {
    const std::string EXECUTE_JAVASCRIPT_CALLBACK_HTML = "execute_javaScript_test.html";

    constexpr uint8_t BITS_PER_PIXEL = 4; /* 4 bits per pixel */
    constexpr uint8_t LAST_SECOND_CHANNEL_OF_PIXEL = 2;
    constexpr uint8_t LAST_THIRD_CHANNEL_OF_PIXEL = 3;
    struct BmpFileHeader {
        unsigned int bf_size;             /* Size of file */
        unsigned short bf_reserved_1 = 0; /* Reserved */
        unsigned short bf_reserved_2 = 0; /* ... */
        unsigned int bf_offbits = 0x36;   /* Offset to bitmap data */
    };
    /**** BMP file info structure ****/
    struct BmpInfoHeader {
        unsigned int bi_size;                 /* Size of info header */
        int bi_width;                         /* Width of image */
        int bi_height;                        /* Height of image */
        unsigned short bi_planes = 1;         /* Number of color planes */
        unsigned short bi_bit_count = 24;     /* 24 Number of bits per pixel */
        unsigned int bi_compression = 0;      /* Type of compression to use */
        unsigned int bi_size_image = 0;       /* Size of image data */
        int bi_x_pixels_per_meter = 5000;     /* 5000 X pixels per meter */
        int bi_y_pixels_per_meter = 5000;     /* 5000 Y pixels per meter */
        unsigned int bi_colors_used = 0;      /* Number of colors used */
        unsigned int bi_colors_important = 0; /* Number of important colors */
    };

    void RgbaToRgb(char* buf, int width, int height)
    {
        char* p_rgba = buf;
        char* p_rgb = buf;
        uint64_t len = width * height * BITS_PER_PIXEL;
        for (uint64_t i = 0; i < len; i++, p_rgba++) {
            if (i % BITS_PER_PIXEL == LAST_THIRD_CHANNEL_OF_PIXEL) {
                // check alpha value, if 0, set related color to white
                if (buf[i] == 0) {
                    *(p_rgb - LAST_THIRD_CHANNEL_OF_PIXEL) = 0xff;
                    *(p_rgb - LAST_SECOND_CHANNEL_OF_PIXEL) = 0xff;
                    *(p_rgb - 1) = 0xff;
                }
                continue;
            }
            *p_rgb++ = *p_rgba;
            if (i % BITS_PER_PIXEL == LAST_SECOND_CHANNEL_OF_PIXEL) {
                char tmp = *(p_rgb - 1);
                *(p_rgb - 1) = *(p_rgb - LAST_THIRD_CHANNEL_OF_PIXEL);
                *(p_rgb - LAST_THIRD_CHANNEL_OF_PIXEL) = tmp;
            }
        }
    }

    void WriteToBmp(const std::string& filename, char* buf, int width, int height)
    {
        BmpFileHeader bmp_file_header;
        BmpInfoHeader bmp_info_header;
        // Magic number for file. It does not fit in the header structure
        // due to alignment requirements, so put it outside
        unsigned short bmp_file_type = 0x4d42;
        constexpr int rgb_bits_per_pixel = LAST_THIRD_CHANNEL_OF_PIXEL;
        int rgb_buf_size = width * height * rgb_bits_per_pixel;
        bmp_file_header.bf_size =
            sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + rgb_buf_size;
        bmp_info_header.bi_size = sizeof(BmpInfoHeader);
        bmp_info_header.bi_width = width;
        bmp_info_header.bi_height = -height;
        FILE* file = fopen(filename.c_str(), "wb");
        if (!file) {
            return;
        }
        // Write headers
        (void)fwrite(&bmp_file_type, sizeof(bmp_file_type), 1, file);
        (void)fwrite(&bmp_file_header, sizeof(bmp_file_header), 1, file);
        (void)fwrite(&bmp_info_header, sizeof(bmp_info_header), 1, file);
        (void)fwrite(buf, rgb_buf_size, 1, file);
        (void)fclose(file);
    }

    void DumpToBmp(const std::string& filename, char* buf, int width, int height)
    {
        RgbaToRgb(buf, width, height);
        WriteToBmp(filename, buf, width, height);
    }
}

namespace OHOS::NWeb {
class JavaScriptResultCb : public NWebValueCallback<std::string> {
    void OnReceiveValue(std::string result) override
    {
        TESTLOG_I("JavaScript execute result = %{public}s", result.c_str());
    }
};
void NWebHandlerImplTest::OnProxyDied()
{
    TESTLOG_I("NWebHandlerImplTest::OnProxyDied called");
}

void NWebHandlerImplTest::SetNWeb(std::shared_ptr<NWeb> nweb)
{
    nwebweak_ = nweb;
}

void NWebHandlerImplTest::OnPageLoadEnd(int httpStatusCode, const std::string& url)
{
    TESTLOG_I("NWebHandlerImplTest::OnPageLoadend called, url=%{public}s", url.c_str());
    auto nwebShared = nwebweak_.lock();
    if (nwebShared == nullptr) {
        TESTLOG_E("nwebShared is nullptr");
        return;
    }

    if (url.find(EXECUTE_JAVASCRIPT_CALLBACK_HTML) != std::string::npos) {
        // invoke js function which is defined in html, test case 106
        std::string ss = "javascript:ExecuteJavaScriptTest()";
        std::shared_ptr<NWebValueCallback<std::string>> callback = std::make_shared<JavaScriptResultCb>();
        nwebShared->ExecuteJavaScript(ss, callback);
    }
}

void NWebHandlerImplTest::OnPageLoadBegin(const std::string& url)
{
    TESTLOG_I("NWebHandlerImplTest::OnPageLoadBegin called, url=%{public}s", url.c_str());
}

void NWebHandlerImplTest::OnPageLoadError(int errorCode,
                                          const std::string& description,
                                          const std::string& failingUrl)
{
    TESTLOG_I("NWebHandlerImplTest::OnPageLoadError called, url=%{public}s", failingUrl.c_str());
}

bool NWebHandlerImplTest::OnHandleInterceptUrlLoading(const std::string& url)
{
    TESTLOG_I("NWebHandlerImplTest::OnHandleInterceptUrlLoading called, url=%{public}s", url.c_str());
    return false;
}

void NWebHandlerImplTest::OnRouterPush(const std::string& param)
{
    TESTLOG_I("NWebHandlerImplTest::OnRouterPush called, url=%{public}s", param.c_str());
}

void NWebHandlerImplTest::OnMessage(const std::string& param)
{
    TESTLOG_I("NWebHandlerImplTest::OnMessage called, message=%{public}s", param.c_str());
}

const std::vector<std::string> NWebHandlerImplTest::VisitedUrlHistory()
{
    std::vector<std::string> vector_string;
    vector_string.push_back("https://www.qq.com");
    vector_string.push_back("file:///data/local/cef/cef_user_data/jingtai.html");
    return vector_string;
}

void NWebHandlerImplTest::OnResourceLoadError(std::shared_ptr<NWebUrlResourceRequest> request,
                                              std::shared_ptr<NWebUrlResourceError> error)
{
    TESTLOG_I("OnPageLoadError, url=%{public}s, errorCode=%{public}d, desc=%{public}s", request->Url().c_str(),
        error->ErrorCode(), error->ErrorInfo().c_str());
}
void NWebHandlerImplTest::OnHttpError(std::shared_ptr<NWebUrlResourceRequest> request,
                                      std::shared_ptr<NWebUrlResourceResponse> errorResponse)
{
    TESTLOG_I("OnHttpError, url=%{public}s, mimeType=%{public}s, is_main_frame=%{public}d," \
        "has_user_gesture=%{public}d", request->Url().c_str(),
        errorResponse->ResponseMimeType().c_str(), request->IsAboutMainFrame(), request->FromGesture());
}
void NWebHandlerImplTest::OnPageIcon(const void* data,
                                     size_t width,
                                     size_t height,
                                     ImageColorType colorType,
                                     ImageAlphaType alphaType)
{
    TESTLOG_I("OnPageIcon, width=%{public}zu, height=%{public}zu", width, height);
    size_t len = width * height * BITS_PER_PIXEL;
    char* data_temp = new char[len];
    if (memcpy_s(data_temp, len, data, len) == 0) {
        DumpToBmp("/system/etc/webview/icon.bmp", data_temp, width, height);
    }
    delete[] data_temp;
    data_temp = nullptr;
}

void NWebHandlerImplTest::OnDesktopIconUrl(const std::string& iconUrl, bool precomposed)
{
    TESTLOG_I("OnDesktopIconUrl, iconUrl=%{public}s, precomposed=%{public}d", iconUrl.c_str(), precomposed);
}

void NWebHandlerImplTest::OnFocus()
{
    TESTLOG_I("NWebHandlerImplTest::OnFocus");
}

void NWebHandlerImplTest::OnLoadingProgress(int newProgress)
{
    TESTLOG_I("NWebHandlerImplTest::OnLoadingProgress progress=%{public}d", newProgress);
}

void NWebHandlerImplTest::OnPageTitle(const std::string &title)
{
    TESTLOG_I("NWebHandlerImplTest::OnTitle title=%{public}s", title.c_str());
}

void NWebHandlerImplTest::OnResource(const std::string &url)
{
    TESTLOG_I("NWebHandlerImplTest::OnResource url=%{public}s", url.c_str());
}

void NWebHandlerImplTest::OnGeolocationShow(const std::string& origin,
                                            std::shared_ptr<NWebGeolocationCallbackInterface> callback)
{
    TESTLOG_I("NWebHandlerImplTest::OnGeolocationShow called, origin=%{public}s", origin.c_str());
    callback->GeolocationCallbackInvoke(origin, true, true);
}

void NWebHandlerImplTest::OnGeolocationHide()
{
    TESTLOG_I("NWebHandlerImplTest::OnGeolocationHide called");
}

void NWebHandlerImplTest::OnPermissionRequest(std::shared_ptr<NWebAccessRequest> request)
{
    int id = request->ResourceAcessId();
    TESTLOG_I("NWebHandlerImplTest::OnPermissionRequest called, origin=%{public}s, resourceId=%{public}d",
              request->Origin().c_str(), id);
    request->Agree(id);
}

void NWebHandlerImplTest::OnPermissionRequestCanceled(std::shared_ptr<NWebAccessRequest> request)
{
    int id = request->ResourceAcessId();
    TESTLOG_I("NWebHandlerImplTest::OnPermissionRequestCanceled called, origin=%{public}s, resourceId=%{public}d",
              request->Origin().c_str(), id);
    request->Refuse();
}
} // namespace OHOS::NWeb
