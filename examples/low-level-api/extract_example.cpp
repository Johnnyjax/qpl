/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

//* [QPL_LOW_LEVEL_EXTRACT_EXAMPLE] */

#include <iostream>
#include <vector>
#include <numeric>
#include <memory>

#include "qpl/qpl.h"
#include "examples_utils.hpp" // for argument parsing function
#include "prle_generator.hpp"

using namespace qpl::test;

constexpr uint32_t rle_element_bit_width = 7U;
constexpr uint32_t rle_element_value = 113U;

constexpr uint32_t rle_burst_counter_element_value = 3U;
constexpr uint32_t rle_burst_counter_bit_width = 8U;

constexpr uint32_t extract_lower_index = 100U;
constexpr uint32_t extract_upper_index = 1000U;

constexpr uint32_t rle_burst_src2_bit_width = 11U;

constexpr uint32_t max_bit_index = 7;
constexpr uint32_t bit_to_byte_shift_offset = 3;

/**
 * @brief This example requires a command line argument to set the execution path. Valid values are `software_path`
 * and `hardware_path`.
 * In QPL, @ref qpl_path_software (`Software Path`) means that computations will be done with CPU.
 * Accelerator can be used instead of CPU. In this case, @ref qpl_path_hardware (`Hardware Path`) must be specified.
 * If there is no difference where calculations should be done, @ref qpl_path_auto (`Auto Path`) can be used to allow
 * the library to chose the path to execute. The Auto Path usage is not demonstrated by this example.
 *
 * @warning ---! Important !---
 * `Hardware Path` doesn't support all features declared for `Software Path`
 *
 */

static uint32_t bits_to_bytes(uint32_t bits_count) {
    uint32_t bytes_count = (bits_count + max_bit_index) >> bit_to_byte_shift_offset;
    return bytes_count;
}

auto main(int argc, char** argv) -> int {
    std::cout << "Intel(R) Query Processing Library version is " << qpl_get_library_version() << ".\n";

    // Default to Software Path
    qpl_path_t execution_path = qpl_path_software;

    // Get path from input argument
    int parse_ret = parse_execution_path(argc, argv, &execution_path);
    if (parse_ret != 0) {
        return 1;
    }

    std::unique_ptr<uint8_t[]> job_buffer;
    uint32_t   size = 0;

        // Job initialization
    qpl_status status = qpl_get_job_size(execution_path, &size);
    if (status != QPL_STS_OK) {
        std::cout << "An error " << status << " acquired during job size getting.\n";
        return 1;
    }

    job_buffer = std::make_unique<uint8_t[]>(size);
    qpl_job *job_ptr = reinterpret_cast<qpl_job *>(job_buffer.get());
    status = qpl_init_job(execution_path, job_ptr);
    if (status != QPL_STS_OK) {
        std::cout << "An error " << status << " acquired during job initializing.\n";
        return 1;
    }
    std::vector<uint8_t> source;
    std::vector<uint8_t> destination;

    rle_element_t rle_element;
    rle_element.bit_width = rle_element_bit_width;
    rle_element.element_value = rle_element_value;
    rle_element.repeat_count = 4000U;

    auto parquet_group = create_rle_group(rle_element);
    // Source should contain single rle group + 1st byte as prle stream bit width
    source.resize(parquet_group.size() + 1);
    source[0] = static_cast<uint32_t>(rle_element.bit_width);

    std::copy(parquet_group.begin(), parquet_group.end(), source.begin() + 1);

    uint32_t destination_size = rle_element.repeat_count * (bits_to_bytes(rle_element.bit_width));

    destination.resize(destination_size);

    job_ptr->op = qpl_op_extract;
    job_ptr->param_low = extract_lower_index;
    job_ptr->param_high = extract_upper_index;
    job_ptr->next_in_ptr = source.data();
    job_ptr->available_in = static_cast<uint32_t>(source.size());
    job_ptr->next_out_ptr = destination.data();
    job_ptr->available_out = static_cast<uint32_t>(destination.size());
    job_ptr->num_input_elements = rle_element.repeat_count;
    job_ptr->parser = qpl_p_parquet_rle;

    // TODO: REMOVE
    job_ptr->src1_bit_width = rle_element.bit_width;

    status = qpl_execute_job(job_ptr);
    if (status != QPL_STS_OK) {
        std::cout << "An error " << status << " acquired during performing extract.\n";
        return 1;
    }

    const auto extract_size = job_ptr->total_out;

    // Freeing resources
    status = qpl_fini_job(job_ptr);
    if (status != QPL_STS_OK) {
        std::cout << "An error " << status << " acquired during job finalization.\n";
        return 1;
    }

    // // Compare with reference
    // for (size_t i = 0; i < extract_size; i++) {
    //     if (destination[i] != source[i + extract_lower_index]) {
    //         std::cout << "Extract was done incorrectly.\n";
    //         return 1;
    //     }
    // }
    std::cout << "Extract was performed successfully." << std::endl;

    return 0;
}

//* [QPL_LOW_LEVEL_EXTRACT_EXAMPLE] */
