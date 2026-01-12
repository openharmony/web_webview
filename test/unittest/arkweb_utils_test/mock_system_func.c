/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "mock_system_func.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNUSED(x) ((void)(x))
#define MAP_FAILED ((void *)-1)

void* g_mmapMockValue = MAP_FAILED;

void SetMmapMockValue(void* mockValue)
{
    g_mmapMockValue = mockValue;
}

void *MmapMock(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    UNUSED(addr);
    UNUSED(length);
    UNUSED(prot);
    UNUSED(flags);
    UNUSED(fd);
    UNUSED(offset);
    return g_mmapMockValue;
}

int g_unlinkMockValue = 0;

void SetUnlinkMockValue(int mockValue)
{
    g_unlinkMockValue = mockValue;
}

int UnlinkMock(const char *pathName)
{
    UNUSED(pathName);
    return g_unlinkMockValue;
}

int g_openMockValue = 0;

void SetOpenMockValue(int mockValue)
{
    g_openMockValue = mockValue;
}

int OpenMock(const char *pathName, int flags, ...)
{
    UNUSED(pathName);
    UNUSED(flags);
    return g_openMockValue;
}

void* g_dlopenNsExtMockValue = (void*)0;

void SetDlopenNsExtMockValue(void *mockValue)
{
    g_dlopenNsExtMockValue = mockValue;
}

void *DlopenNsExtMock(Dl_namespace *dlns, const char *libName, int flag, const dl_extinfo *extinfo)
{
    UNUSED(dlns);
    UNUSED(libName);
    UNUSED(flag);
    UNUSED(extinfo);
    return g_dlopenNsExtMockValue;
}

pid_t g_forkMockValue = 0;

void SetForkMockValue(pid_t mockValue)
{
    g_forkMockValue = mockValue;
}

pid_t ForkMock()
{
    return g_forkMockValue;
}

int g_prctlMockValue = 0;

void SetPrctlMockValue(int mockValue)
{
    g_prctlMockValue = mockValue;
}

int PrctlMock(int option, ...)
{
    UNUSED(option);
    return g_prctlMockValue;
}

int g_setUidMockValue = 0;

void SetUidMockValue(int mockValue)
{
    g_setUidMockValue = mockValue;
}

int SetUidMock(uid_t uid)
{
    UNUSED(uid);
    return g_setUidMockValue;
}

int g_setGidMockValue = 0;

void SetGidMockValue(int mockValue)
{
    g_setGidMockValue = mockValue;
}

int SetGidMock(gid_t gid)
{
    UNUSED(gid);
    return g_setGidMockValue;
}

int g_restoreconRecurseMockValue = 0;

void SetRestoreconRecurseMockValue(int mockValue)
{
    g_restoreconRecurseMockValue = mockValue;
}

int RestoreconRecurseMock(const char *path)
{
    UNUSED(path);
    return g_restoreconRecurseMockValue;
}

#ifdef __cplusplus
}
#endif