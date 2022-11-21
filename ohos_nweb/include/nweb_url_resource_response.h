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

#ifndef NWEB_URL_RESOURCE_RESPONSE_H
#define NWEB_URL_RESOURCE_RESPONSE_H

#include <map>
#include <string>

namespace OHOS::NWeb {

class NWebResourceReadyCallback {
public:
    virtual ~NWebResourceReadyCallback() {};
    virtual void Continue() = 0;
    virtual void Cancel() = 0;
};

class NWebUrlResourceResponse {
public:
    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param mime_type the resource response's MIME type, for example {
     * "text/html"}.
     * @param encoding the resource response's character encoding, for example
     * {"utf-8"}.
     * @param status_code the status code needs to be in the ranges [100, 299],
     * [400, 599]. Causing a redirect by specifying a 3xx code is not supported.
     * @param reason_phrase the phrase describing the status code, for example
     * "OK". Must be non-empty.
     * @param response_headers the resource response's headers represented as a
     * mapping of header name -> header value.
     * @param input_stream the input stream that provides the resource response's
     * data.
     */
    NWebUrlResourceResponse(const std::string& mime_type,
                            const std::string& encoding,
                            const int status_code,
                            const std::string& reason_phrase,
                            const std::map<std::string, std::string>& response_headers,
                            std::string& input_stream)
        : mime_type_(mime_type),
          encoding_(encoding),
          status_code_(status_code),
          reason_phrase_(reason_phrase),
          response_headers_(response_headers),
          input_stream_(input_stream) {}

    NWebUrlResourceResponse(const std::string& mime_type,
                            const std::string& encoding,
                            std::string& input_stream)
        : mime_type_(mime_type),
          encoding_(encoding),
          input_stream_(input_stream) {}

    ~NWebUrlResourceResponse() = default;

    /**
     * @brief get input stream
     *
     * @retval inputstream string
     */
    const std::string& ResponseData()
    {
        return input_stream_;
    }

    /**
     * @brief set input stream
     *
     * @param input_stream set inputstream for example: fread(buf, 1, sizeof(buf),
     * file)
     */
    void PutResponseData(std::string& input_stream)
    {
        input_stream_ = input_stream;
        fd_ = 0;
        isFileFd_ = false;
    }

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param encoding encoding { "utf-8" }
     */
    void PutResponseEncoding(const std::string& encoding)
    {
        encoding_ = encoding;
    }

    /**
     * @brief get encoding
     *
     * @retval encoding the resource response's encoding
     */
    std::string ResponseEncoding()
    {
        return encoding_;
    }

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param mime_type mime_type{ "text/html" }
     */
    void PutResponseMimeType(const std::string& mime_type)
    {
        mime_type_ = mime_type;
    }

    /**
     * @brief Get mimetype
     *
     * @retval mimetype The resource response's MIME type
     */
    std::string ResponseMimeType()
    {
        return mime_type_;
    }

    /**
     * @brief Set ResponseHeaders
     *
     * @param response_headers response header
     */
    void PutResponseHeaders(const std::map<std::string, std::string>& response_headers)
    {
        response_headers_ = response_headers;
    }

    /**
     * @brief Get ResponseHeaders
     *
     * @retval response headers
     */
    const std::map<std::string, std::string>& ResponseHeaders()
    {
        return response_headers_;
    }

    /**
     * @brief Set StatusCode And ReasonPhrase
     *
     * @param status_code status code
     * @param reasonphrase reason phrase
     */
    void PutResponseStateAndStatuscode(int status_code,
                                    std::string reason_phrase)
    {
        status_code_ = status_code;
        reason_phrase_ = reason_phrase;
    }

    /**
     * @brief Get status code
     *
     * @retval status code
    */
    int ResponseStatusCode()
    {
        return status_code_;
    }

    /**
     * @brief Get ReasonPhrase
     *
     * @retval errorcode reason
    */
    std::string ResponseStatus()
    {
        return reason_phrase_;
    }

    void PutResponseDataStatus(bool isDataReady)
    {
        isDataReady_ = isDataReady;
        if (isDataReady_ == true && readyCallback_ != nullptr) {
            readyCallback_->Continue();
            readyCallback_ = nullptr;
        }
    }

    bool ResponseDataStatus() const
    {
        return isDataReady_;
    }

    bool ResponseIsFileHandle() const
    {
        return isFileFd_;
    }

    void PutResponseFileHandle(int fd)
    {
        fd_ = fd;
        isFileFd_ = true;
        input_stream_.clear();
    }

    int ResponseFileHandle() const
    {
        return fd_;
    }

    void PutResponseReadyCallback(std::shared_ptr<NWebResourceReadyCallback> readyCallback)
    {
        readyCallback_ = readyCallback;
    }

private:
    std::string mime_type_;
    std::string encoding_;
    int status_code_ = 200;
    std::string reason_phrase_;
    std::map<std::string, std::string> response_headers_;
    std::string input_stream_;
    int fd_ = -1;
    bool isFileFd_ = false;
    bool isDataReady_ = true;
    std::shared_ptr<NWebResourceReadyCallback> readyCallback_;
};
} // namespace OHOS::NWeb

#endif // NWEB_URL_RESOURCE_RESPONSE_H
