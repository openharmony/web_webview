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

#include <testing/gtest/include/gtest/gtest.h>
#include <testing/gmock/include/gmock/gmock.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>

#include "ashmem_adapter.h"

using namespace testing;

namespace OHOS::NWeb {
namespace {
const int MAX_PARCEL_SIZE = 1000;
char g_data[MAX_PARCEL_SIZE];
const int32_t MEMORY_SIZE = 1024;
const std::string MEMORY_CONTENT = "HelloWorld2020\0";
const std::string MEMORY_NAME = "Test SharedMemory\0";

class UtilsAshmemTest : public testing::Test {
public:
    static void TearDownTestCase(void);
};

void UtilsAshmemTest::TearDownTestCase(void) {
    for (int i = 0; i < MAX_PARCEL_SIZE; i++) {
        g_data[i] = 0;
    }
}

struct CheckParams {
    int fd;
    void *ptr;
    int total_size;
    int flag;
    const void *data;
    int32_t size;
    int32_t offset;
};

bool CheckValid(const struct CheckParams *param, int cmd) {
    if (param->ptr == nullptr) {
        return false;
    }

    if ((param->size < 0) || (param->size > param->total_size) ||
        (param->offset < 0) || (param->offset > param->total_size)) {
        return false;
    }

    if (param->offset + param->size > param->total_size) {
        return false;
    }

    if (!(static_cast<uint32_t>(ShmemAdapterGetProt(param->fd)) & static_cast<uint32_t>(cmd)) ||
        !(static_cast<uint32_t>(param->flag) & cmd)) {
        return false;
    }

    return true;
}

bool WriteToAshmem(struct CheckParams *param) {
    if (param->data == nullptr) {
        return false;
    }

    if (!CheckValid(param, PROT_WRITE)) {
        return false;
    }

    if (param->offset + param->size > param->total_size) {
        return false;
    }

    char *dst = reinterpret_cast<char *>(param->ptr) + param->offset;
    const char *src = reinterpret_cast<const char *>(param->data);
    for (int i = 0; i < param->size; i++) {
        dst[i] = src[i];
    }

    return true;
}

const char *ReadFromAshmem(struct CheckParams *param) {
    if (!CheckValid(param, PROT_READ)) {
        return nullptr;
    }

    return reinterpret_cast<const char *>(param->ptr) + param->offset;
}

TEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_001) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);
    ASSERT_TRUE(ShmemAdapterGetSize(fd) == MEMORY_SIZE);

    void *ptr = ShmemAdapterMap(fd, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ptr != MAP_FAILED);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_002) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    param.offset += sizeof(MEMORY_CONTENT);
    ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    param.data = nullptr;
    param.offset = 0;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData != nullptr);

    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    param.offset = sizeof(MEMORY_CONTENT);
    readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData != nullptr);

    readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_003) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    flag = PROT_READ;
    ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    param.flag = flag;
    param.ptr = ptr;
    param.offset = sizeof(MEMORY_CONTENT);
    ret = WriteToAshmem(&param);
    ASSERT_FALSE(ret);

    param.data = nullptr;
    param.offset = 0;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData != nullptr);
    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_004) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ptr = nullptr;

    ret = ShmemAdapterSetProt(fd, PROT_READ);
    ASSERT_TRUE(ret >= 0);

    ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr == MAP_FAILED);

    flag = PROT_READ;
    ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    param.flag = flag;
    param.ptr = ptr;
    param.data = nullptr;
    param.offset = 0;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData != nullptr);
    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_WriteAndRead_005) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    ret = ShmemAdapterSetProt(fd, PROT_READ);
    ASSERT_TRUE(ret >= 0);

    ret = WriteToAshmem(&param);
    ASSERT_FALSE(ret);

    param.data = nullptr;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData != nullptr);
    const char *readContent = reinterpret_cast<const char *>(readData);
    EXPECT_EQ(memcmp(MEMORY_CONTENT.c_str(), readContent, sizeof(MEMORY_CONTENT)), 0);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_001) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), -1);
    ASSERT_TRUE(fd < 0);

    fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    bool ret = ShmemAdapterSetProt(fd, -1);
    ASSERT_TRUE(ret >= 0);

    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_002) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    ShmemAdapterClose(fd);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr == MAP_FAILED);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_003) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    ShmemAdapterClose(fd);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_FALSE(ret);
    param.data = nullptr;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData == nullptr);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_004) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ptr = nullptr;

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_FALSE(ret);
    param.data = nullptr;
    auto readData = ReadFromAshmem(&param);
    ASSERT_TRUE(readData == nullptr);

    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_005) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int ret = ShmemAdapterSetProt(fd, PROT_WRITE);
    ASSERT_TRUE(ret >= 0);

    ret = ShmemAdapterSetProt(fd, PROT_READ);
    ASSERT_TRUE(ret < 0);

    ret = ShmemAdapterSetProt(fd, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ret < 0);

    ret = ShmemAdapterSetProt(fd, PROT_NONE);
    ASSERT_TRUE(ret >= 0);

    ret = ShmemAdapterSetProt(fd, PROT_READ);
    ASSERT_TRUE(ret < 0);

    ShmemAdapterClose(fd);
}

TEST_F(UtilsAshmemTest, test_ashmem_InvalidOperation_006) {
    int fd = ShmemAdapterCreate(MEMORY_NAME.c_str(), MEMORY_SIZE);
    ASSERT_TRUE(fd > 0);

    int flag = PROT_READ | PROT_WRITE;
    void *ptr = ShmemAdapterMap(fd, flag);
    ASSERT_TRUE(ptr != MAP_FAILED);

    struct CheckParams param = {0};
    param.fd = fd;
    param.ptr = ptr;
    param.total_size = ShmemAdapterGetSize(fd);
    param.flag = flag;
    param.data = MEMORY_CONTENT.c_str();
    param.size = sizeof(MEMORY_CONTENT);
    param.offset = 0;
    int ret = WriteToAshmem(&param);
    ASSERT_TRUE(ret);

    param.offset = MEMORY_SIZE + 1;
    ret = WriteToAshmem(&param);
    EXPECT_FALSE(ret);

    param.offset = -1;
    ret = WriteToAshmem(&param);
    EXPECT_FALSE(ret);

    param.size = MEMORY_SIZE + 1;
    param.offset = 0;
    ret = WriteToAshmem(&param);
    EXPECT_FALSE(ret);

    param.size = -1;
    ret = WriteToAshmem(&param);
    EXPECT_FALSE(ret);

    param.size = sizeof(MEMORY_CONTENT);
    param.offset = MEMORY_SIZE;
    ret = WriteToAshmem(&param);
    EXPECT_FALSE(ret);

    ShmemAdapterUnmap(ptr, MEMORY_SIZE);
    ShmemAdapterClose(fd);
}
}
} // namespace OHOS
