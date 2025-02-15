// Copyright (C) 2023 Intel Corporation
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
// See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "uur/known_failure.h"
#include <uur/fixtures.h>

struct urProgramGetBuildInfoTest : uur::urProgramTest {
    void SetUp() override {
        UUR_RETURN_ON_FATAL_FAILURE(urProgramTest::SetUp());
        ASSERT_SUCCESS(urProgramBuild(this->context, program, nullptr));
    }
};
UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(urProgramGetBuildInfoTest);

TEST_P(urProgramGetBuildInfoTest, SuccessStatus) {
    UUR_KNOWN_FAILURE_ON(uur::LevelZero{}, uur::LevelZeroV2{});

    size_t size = 0;
    auto info_type = UR_PROGRAM_BUILD_INFO_STATUS;
    ASSERT_SUCCESS(
        urProgramGetBuildInfo(program, device, info_type, 0, nullptr, &size));
    ASSERT_EQ(sizeof(ur_program_build_status_t), size);

    ur_program_build_status_t returned_status =
        UR_PROGRAM_BUILD_STATUS_FORCE_UINT32;
    ASSERT_SUCCESS(urProgramGetBuildInfo(program, device, info_type, size,
                                         &returned_status, nullptr));

    ASSERT_GE(returned_status, UR_PROGRAM_BUILD_STATUS_NONE);
    ASSERT_LE(returned_status, UR_PROGRAM_BUILD_STATUS_IN_PROGRESS);
}

TEST_P(urProgramGetBuildInfoTest, SuccessOptions) {
    size_t size = 0;
    auto info_type = UR_PROGRAM_BUILD_INFO_OPTIONS;
    ASSERT_SUCCESS(
        urProgramGetBuildInfo(program, device, info_type, 0, nullptr, &size));
    ASSERT_GT(size, 0);

    std::vector<char> returned_options(size);
    returned_options[size - 1] = 'x';
    ASSERT_SUCCESS(urProgramGetBuildInfo(program, device, info_type, size,
                                         returned_options.data(), nullptr));

    ASSERT_EQ(size, returned_options.size());
    ASSERT_EQ(returned_options[size - 1], '\0');
}

TEST_P(urProgramGetBuildInfoTest, SuccessLog) {
    UUR_KNOWN_FAILURE_ON(uur::CUDA{});

    size_t size = 0;
    auto info_type = UR_PROGRAM_BUILD_INFO_LOG;
    ASSERT_SUCCESS(
        urProgramGetBuildInfo(program, device, info_type, 0, nullptr, &size));
    ASSERT_GT(size, 0);

    std::vector<char> returned_log(size);
    returned_log[size - 1] = 'x';
    ASSERT_SUCCESS(urProgramGetBuildInfo(program, device, info_type, size,
                                         returned_log.data(), nullptr));

    ASSERT_EQ(size, returned_log.size());
    ASSERT_EQ(returned_log[size - 1], '\0');
}

TEST_P(urProgramGetBuildInfoTest, SuccessBinaryType) {
    size_t size = 0;
    auto info_type = UR_PROGRAM_BUILD_INFO_BINARY_TYPE;
    ASSERT_SUCCESS(
        urProgramGetBuildInfo(program, device, info_type, 0, nullptr, &size));
    ASSERT_EQ(sizeof(ur_program_binary_type_t), size);

    ur_program_binary_type_t returned_binary_type =
        UR_PROGRAM_BINARY_TYPE_FORCE_UINT32;
    ASSERT_SUCCESS(urProgramGetBuildInfo(program, device, info_type, size,
                                         &returned_binary_type, nullptr));

    ASSERT_GE(returned_binary_type, UR_PROGRAM_BINARY_TYPE_NONE);
    ASSERT_LE(returned_binary_type, UR_PROGRAM_BINARY_TYPE_EXECUTABLE);
}

TEST_P(urProgramGetBuildInfoTest, InvalidNullHandleProgram) {
    ur_program_build_status_t programBuildStatus =
        UR_PROGRAM_BUILD_STATUS_ERROR;
    ASSERT_EQ_RESULT(urProgramGetBuildInfo(nullptr, device,
                                           UR_PROGRAM_BUILD_INFO_STATUS,
                                           sizeof(programBuildStatus),
                                           &programBuildStatus, nullptr),
                     UR_RESULT_ERROR_INVALID_NULL_HANDLE);
}

TEST_P(urProgramGetBuildInfoTest, InvalidNullHandleDevice) {
    ur_program_build_status_t programBuildStatus =
        UR_PROGRAM_BUILD_STATUS_ERROR;
    ASSERT_EQ_RESULT(urProgramGetBuildInfo(program, nullptr,
                                           UR_PROGRAM_BUILD_INFO_STATUS,
                                           sizeof(programBuildStatus),
                                           &programBuildStatus, nullptr),
                     UR_RESULT_ERROR_INVALID_NULL_HANDLE);
}

TEST_P(urProgramGetBuildInfoTest, InvalidEnumeration) {
    size_t propSizeOut = UR_PROGRAM_BUILD_STATUS_ERROR;
    ASSERT_EQ_RESULT(UR_RESULT_ERROR_INVALID_ENUMERATION,
                     urProgramGetBuildInfo(program, device,
                                           UR_PROGRAM_BUILD_INFO_FORCE_UINT32,
                                           0, nullptr, &propSizeOut));
}
