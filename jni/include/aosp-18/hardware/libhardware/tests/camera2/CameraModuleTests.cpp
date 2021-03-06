/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#define LOG_TAG "CameraModuleTest"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include "hardware/hardware.h"
#include "hardware/camera2.h"

#include "Camera2Device.h"
#include "utils/StrongPointer.h"
#include "CameraModuleFixture.h"

namespace android {
namespace camera2 {
namespace tests {

class CameraModuleTest : public ::testing::Test,
                                  public CameraModuleFixture<> {

public:
    CameraModuleTest() {
        CameraModuleFixture::SetUp();
    }

    ~CameraModuleTest() {
        CameraModuleFixture::TearDown();
    }

    status_t initializeDevice(int cameraId) {

        // ignore HAL1s. count as test pass
        status_t stat;
        if (isDeviceVersionHal2(cameraId, &stat) && stat == OK) {
            stat = mDevice->initialize(mModule);
        }

        return stat;
    }

    int getDeviceVersion(int cameraId, status_t* status) {
        camera_info info;
        *status = mModule->get_camera_info(cameraId, &info);

        return info.device_version;
    }

    bool isDeviceVersionHal2(int cameraId, status_t* status) {
        return getDeviceVersion(cameraId, status)
               >= CAMERA_DEVICE_API_VERSION_2_0;
    }
};

TEST_F(CameraModuleTest, LoadModule) {

    TEST_EXTENSION_FORKING_INIT;

    for (int i = 0; i < mNumberOfCameras; ++i) {
        mDevice = new Camera2Device(i);

        ASSERT_EQ(OK, initializeDevice(i))
            << "Failed to initialize device " << i;
        mDevice.clear();
    }

}

TEST_F(CameraModuleTest, LoadModuleBadIndices) {

    TEST_EXTENSION_FORKING_INIT;

    int idx[] = { -1, mNumberOfCameras, mNumberOfCameras + 1 };

    for (unsigned i = 0; i < sizeof(idx)/sizeof(idx[0]); ++i) {
        mDevice = new Camera2Device(idx[i]);
        status_t deviceInitializeCode = initializeDevice(idx[i]);
        EXPECT_NE(OK, deviceInitializeCode);
        EXPECT_EQ(-ENODEV, deviceInitializeCode)
            << "Incorrect error code when trying to initialize invalid index "
            << idx[i];
        mDevice.clear();
    }
}

TEST_F(CameraModuleTest, GetCameraInfo) {

    TEST_EXTENSION_FORKING_INIT;

    for (int i = 0; i < mNumberOfCameras; ++i) {
        struct camera_info info;
        ASSERT_EQ(OK, mModule->get_camera_info(i, &info));
    }

}

TEST_F(CameraModuleTest, GetCameraInfoBadIndices) {

    TEST_EXTENSION_FORKING_INIT;

    int idx[] = { -1, mNumberOfCameras, mNumberOfCameras + 1 };
    for (unsigned i = 0; i < sizeof(idx)/sizeof(idx[0]); ++i) {
        struct camera_info info;
        EXPECT_NE(OK, mModule->get_camera_info(idx[i], &info));
        EXPECT_EQ(-ENODEV, mModule->get_camera_info(idx[i], &info))
            << "Incorrect error code for get_camera_info idx= "
            << idx[i];
    }
}

/**
 * TODO: Additional test to add: open two cameras at once.
 *       (is allowed to fail, at least for now, but should not blow up)
 *     - open same device multiple times
 *     - close same device multiple times
 */




}
}
}

