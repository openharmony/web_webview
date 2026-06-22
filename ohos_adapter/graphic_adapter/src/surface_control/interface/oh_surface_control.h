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

#ifndef OH_SURFACE_CONTROL_H_
#define OH_SURFACE_CONTROL_H_

#include <cstddef>
#include <cstdint>
#include <functional>

#include "common/rs_common_def.h"

typedef struct NativeWindow OHNativeWindow;
typedef struct OH_NativeBuffer OH_NativeBuffer;

struct OH_SurfaceControl;
typedef struct OH_SurfaceControl OH_SurfaceControl;

namespace OHOS {
namespace NWeb {
struct OH_Rect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
};
typedef struct OH_Rect OH_Rect;
class OH_SurfaceTransaction;
class OH_SurfaceTransactionStats;
struct OH_SurfaceControl;
typedef void (*OH_SurfaceTransaction_OnComplete)(void* context, uint64_t timestamp);
typedef void (*OH_SurfaceTransaction_OnCommit)(void* context, OH_SurfaceTransactionStats* stats);
typedef void (*OH_SurfaceTransaction_OnBufferRelease)(void* context, int releaseFenceFd);

class RSC_EXPORT SurfaceControlUtils {
public:
    // 禁止实例化，仅使用静态方法
    SurfaceControlUtils() = delete;
    ~SurfaceControlUtils() = delete;

    static OH_SurfaceControl* CreateSurfaceControlFromNativeWindow(NativeWindow* parent, const char* name);
    static OH_SurfaceControl* Create(const char* name);

    // 引用计数管理
    static void Release(OH_SurfaceControl* surfaceControl);

    // 嵌套类：SurfaceTransaction
    class Transaction {
    public:
        // 禁止实例化，仅使用静态方法
        Transaction() = delete;
        ~Transaction() = delete;

        // 事务管理
        static OH_SurfaceTransaction* CreateSurfaceTransaction(OHNativeWindow* nativeWindow);
        static void Delete(OH_SurfaceTransaction* transaction);
        static void Commit(OH_SurfaceTransaction* transaction);

        // 回调设置
        // Transaction has been accepted by RS and presented.
        static void SetOnComplete(
            OH_SurfaceTransaction* transaction, void* context, OH_SurfaceTransaction_OnComplete func);

        // 事务操作
        static void Reparent(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, OH_SurfaceControl* newParent);

        // 枚举类型
        enum class Visibility : int8_t {
            HIDE = 0,
            SHOW = 1
        };

        static void SetVisibility(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, Visibility visibility);

        static void SetZOrder(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t zOrder);

        static void SetBuffer(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            OH_NativeBuffer* buffer, int acquireFenceFd, void* context, OH_SurfaceTransaction_OnBufferRelease func);

        static void SetBounds(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h);

        static void SetPivot(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y);

        enum Transform : int32_t {
            OH_TRANSFORM_ROTATE_NONE = 0, /**< No rotation */
            OH_TRANSFORM_ROTATE_90,       /**< Rotation by 90 degrees */
            OH_TRANSFORM_ROTATE_180,      /**< Rotation by 180 degrees */
            OH_TRANSFORM_ROTATE_270,      /**< Rotation by 270 degrees */
            OH_TRANSFORM_FLIP_H,          /**< Flip horizontally */
            OH_TRANSFORM_FLIP_V,          /**< Flip vertically */
            OH_TRANSFORM_FLIP_H_ROT90,    /**< Flip horizontally and rotate 90 degrees */
            OH_TRANSFORM_FLIP_V_ROT90,    /**< Flip vertically and rotate 90 degrees */
            OH_TRANSFORM_FLIP_H_ROT180,   /**< Flip horizontally and rotate 180 degrees */
            OH_TRANSFORM_FLIP_V_ROT180,   /**< Flip vertically and rotate 180 degrees */
            OH_TRANSFORM_FLIP_H_ROT270,   /**< Flip horizontally and rotate 270 degrees */
            OH_TRANSFORM_FLIP_V_ROT270,   /**< Flip vertically and rotate 270 degrees */
            OH_TRANSFORM_ROTATE_LAST = OH_TRANSFORM_FLIP_V_ROT270,
        };

        static void SetBufferTransform(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t transform);

        static void SetTranslate(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            float translateX, float translateY, float translateZ);

        static void SetDamageRegion(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            const OH_Rect* rects, uint32_t count);

        static void SetBufferAlpha(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float alpha);

        static void SetForegroundColor(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            float red, float green, float blue, float alpha);

        static void SetBackgroundColor(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            float red, float green, float blue, float alpha);

        static void SetBorderColor(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            float red, float green, float blue, float alpha);

        static void SetBorderWidth(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            float left, float top, float right, float bottom);

        static void SetBorderStyle(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

        static void SetHardwareEnableHint(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
            bool enable);

        static void SetName(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, const char* name);

        static void SetFrameGravity(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t gravity);

        static void SetSrcRect(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h);

        static void SetDisplayRect(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h);

        static void ClearBufferQueueCache(
            OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, bool cleanAll);
    };
};
} // namespace NWeb
} // namespace OHOS

#endif // OH_SURFACE_CONTROL_H_