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

#include "ashmem_adapter.h"

#include <cerrno>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <linux/ashmem.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>

#include "nweb_log.h"

namespace OHOS::NWeb {
static  pthread_mutex_t g_ashmemLock = PTHREAD_MUTEX_INITIALIZER;

static int ShmemAdapterOpenLocked()
{
    std::string ashmem_path = std::string("/") + std::string(ASHMEM_NAME_DEF);
    int fd = open(ashmem_path.c_str(), O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        WVLOG_E("%{public}s: fd is invalid, fd = %{public}d, errno = %{public}d",
            __func__, fd, errno);
        return fd;
    }

    struct stat st;
    int ret = fstat(fd, &st);
    if (ret < 0) {
        WVLOG_E("%{public}s: failed to exec fstat, ret = %{public}d, errno = %{public}d",
            __func__, ret, errno);
        close(fd);
        return ret;
    }

    if (!S_ISCHR(st.st_mode) || !st.st_rdev) {
        WVLOG_E("%{public}s: stat status is invalid, st_mode = %{public}u", __func__, st.st_mode);
        close(fd);
        return -1;
    }

    return fd;
}

static int ShmemAdapterOpen()
{
    pthread_mutex_lock(&g_ashmemLock);
    int fd = ShmemAdapterOpenLocked();
    pthread_mutex_unlock(&g_ashmemLock);
    return fd;
}

void ShmemAdapterClose(int fd)
{
    if (fd > 0) {
        ::close(fd);
    }
}

int ShmemAdapterCreate(const char *name, int size)
{
    int ret;

    if (size <= 0) {
        WVLOG_E("%{public}s: Parameter is invalid, size = %{public}d", __func__, size);
        return -1;
    }

    int fd = ShmemAdapterOpen();
    if (fd < 0) {
        WVLOG_E("%{public}s: failed to exec ShmemAdaterOpen, fd = %{public}d", __func__, fd);
        return fd;
    }

    if (name != nullptr) {
        if (strlen(name) >= ASHMEM_NAME_LEN) {
            close(fd);
            WVLOG_E("%{public}s: failed to exec strcpy_s, name = %{public}s", __func__, name);
            return -1;
        }

        ret = ioctl(fd, ASHMEM_SET_NAME, name);
        if (ret < 0) {
            close(fd);
            WVLOG_E("%{public}s: failed to set name, name = %{public}s, ret = %{public}d"
                "errno = %{public}d", __func__, name, ret, errno);
            return ret;
        }
    }

    ret = ioctl(fd, ASHMEM_SET_SIZE, size);
    if (ret < 0) {
        close(fd);
        WVLOG_E("%{public}s: failed to set size, size = %{public}d, ret = %{public}d"
            "errno = %{public}d", __func__, size, ret, errno);
        return ret;
    }

    return fd;
}

int ShmemAdapterSetProt(int fd, int prot)
{
    return ioctl(fd, ASHMEM_SET_PROT_MASK, prot);
}

int ShmemAdapterGetSize(int fd)
{
    return ioctl(fd, ASHMEM_GET_SIZE, NULL);
}

int ShmemAdapterGetProt(int fd)
{
    return ioctl(fd, ASHMEM_GET_PROT_MASK);
}

void *ShmemAdapterMap(int fd, int flag)
{
    void *startAddr = ::mmap(nullptr, ShmemAdapterGetSize(fd), flag, MAP_SHARED, fd, 0);
    if (startAddr == MAP_FAILED) {
        WVLOG_E("Failed to exec mmap, errno = %{public}d", errno);
        return startAddr;
    }

    return startAddr;
}

void ShmemAdapterUnmap(void *addr, int size)
{
    if (addr != nullptr) {
        ::munmap(addr, size);
    }
}

}
