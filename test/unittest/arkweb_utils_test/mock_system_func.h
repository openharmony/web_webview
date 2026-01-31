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
#ifndef MOCK_SYSTEM_FUNC_TEST
#define MOCK_SYSTEM_FUNC_TEST

#include <stdint.h>
#include <sys/types.h>
#include <dlfcn_ext.h>
#ifdef __cplusplus
extern "C" {
#endif

void SetMmapMockValue(void* mockValue);
void *MmapMock(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

void SetUnlinkMockValue(int mockValue);
int UnlinkMock(const char *pathName);

void SetOpenMockValue(int mockValue);
int OpenMock(const char *pathName, int flags, ...);

void SetDlopenNsExtMockValue(void *mockValue);
void *DlopenNsExtMock(Dl_namespace *dlns, const char *libName, int flag, const dl_extinfo *extinfo);

void SetForkMockValue(pid_t mockValue);
pid_t ForkMock();

void SetPrctlMockValue(int mockValue);
int PrctlMock(int option, ...);

void SetUidMockValue(int mockValue);
int SetUidMock(uid_t uid);

void SetGidMockValue(int mockValue);
int SetGidMock(gid_t gid);

void SetRestoreconRecurseMockValue(int mockValue);
int RestoreconRecurseMock(const char *path);

void SetMountMockValue(int mockValue);
int MountMock(const char *source, const char *target, const char *type, unsigned long flag, const void *data);
#ifdef __cplusplus
}
#endif

#endif // MOCK_SYSTEM_FUNC_TEST