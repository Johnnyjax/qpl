/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <vector>
#include <string>
#include "gtest/gtest.h"
#include "qpl/qpl.h"
#include "../../../common/analytic_mask_fixture.hpp"
#include "util.hpp"
#include "source_provider.hpp"
#include "qpl_api_ref.h"
#include "ta_ll_common.hpp"
#include "check_result.hpp"

namespace qpl::test {
class RleBurstTest : public AnalyticMaskFixture {
public:
    void InitializeTestCases() {
        std::vector<uint32_t> lengths = GenerateNumberOfElementsVector();

        for (uint32_t length : lengths) {
            for (uint32_t bit_width : {8, 16, 32}) {
                for (uint32_t second_bit_width = 1; second_bit_width <= 32; second_bit_width++) {
                    for (auto parser : {qpl_p_le_packed_array, qpl_p_be_packed_array, qpl_p_parquet_rle}) {
                        if (32u == bit_width && 1u == length) {
                            continue;
                        }

                        AnalyticTestCase test_case;
                        test_case.operation                 = qpl_op_rle_burst;
                        test_case.number_of_elements        = length;
                        test_case.source_bit_width          = bit_width;
                        test_case.destination_bit_width     = 1;
                        test_case.parser                    = parser;
                        test_case.flags                     = 0;
                        test_case.second_input_bit_width    = second_bit_width;
                        test_case.second_input_num_elements = (bit_width != 32) ? length : length - 1;

                        AddNewTestCase(test_case);

                        test_case.flags = QPL_FLAG_SRC2_BE;
                        AddNewTestCase(test_case);

                        test_case.flags = QPL_FLAG_OUT_BE;
                        AddNewTestCase(test_case);
                    }
                }
            }
        }
    }

    void SetBuffers() override {
        source_provider source_gen(current_test_case.number_of_elements,
                                   current_test_case.source_bit_width,
                                   GetSeed(),
                                   current_test_case.parser);

        source = source_gen.get_counter_source_expand_rle();
        uint32_t destination_count = source_gen.get_count_expand_rle_value();

        source_provider mask_gen(current_test_case.second_input_num_elements,
                                 current_test_case.second_input_bit_width,
                                 GetSeed());

        mask = mask_gen.get_source();

        uint32_t destination_bit_width =
                         (1u == current_test_case.destination_bit_width) ? current_test_case.second_input_bit_width
                                                                         : current_test_case.destination_bit_width;

        uint32_t dest_size = ((destination_count * destination_bit_width) + 7u) >> 3u;
        destination.resize(dest_size);
        reference_destination.resize(dest_size);

        job_ptr->available_in             = static_cast<uint32_t>(source.size());
        job_ptr->next_in_ptr              = source.data();
        job_ptr->available_src2           = static_cast<uint32_t>(mask.size());
        job_ptr->next_src2_ptr            = mask.data();
        reference_job_ptr->available_in   = static_cast<uint32_t>(source.size());
        reference_job_ptr->next_in_ptr    = source.data();
        reference_job_ptr->available_src2 = static_cast<uint32_t>(mask.size());
        reference_job_ptr->next_src2_ptr  = mask.data();

        job_ptr->available_out           = static_cast<uint32_t>(destination.size());
        job_ptr->next_out_ptr            = destination.data();
        reference_job_ptr->available_out = static_cast<uint32_t>(reference_destination.size());
        reference_job_ptr->next_out_ptr  = reference_destination.data();
    }

    void SetUp() override {
        AnalyticFixture::SetUp();
        InitializeTestCases();
    }
};

QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_TC(rle_burst, analytic_only, RleBurstTest) {
    auto status = run_job_api(job_ptr);

    auto reference_status = ref_expand_rle(reference_job_ptr);

    ASSERT_EQ(QPL_STS_OK, status);
    ASSERT_EQ(QPL_STS_OK, reference_status);

    // Temporary reference does not update the job fields right.
    // TODO: fix reference code behaviour
    // EXPECT_TRUE(CompareTotalInOutWithReference());

    EXPECT_TRUE(compare_checksum_fields(job_ptr, reference_job_ptr));
    EXPECT_TRUE(CompareVectors(destination, reference_destination, job_ptr->total_out));
}

QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_TC(rle_burst, analytic_with_decompress, RleBurstTest) {
    std::vector<uint8_t> compressed_source;
    ASSERT_NO_THROW(compressed_source = GetCompressedSource());
    job_ptr->available_in = static_cast<uint32_t>(compressed_source.size());
    job_ptr->next_in_ptr  = compressed_source.data();
    job_ptr->flags |= QPL_FLAG_DECOMPRESS_ENABLE;

    if (GetExecutionPath() == qpl_path_software && current_test_case.parser == qpl_p_parquet_rle) {
        job_ptr->src1_bit_width = 0u;
    }

    auto status = run_job_api(job_ptr);
    EXPECT_EQ(QPL_STS_OK, status);

    auto reference_status = ref_expand_rle(reference_job_ptr);
    EXPECT_EQ(QPL_STS_OK, reference_status);

    EXPECT_TRUE(CompareVectors(destination, reference_destination));
}
}
