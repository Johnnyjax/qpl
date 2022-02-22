/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "analytics_state_t.h"
#include "filter_operations.hpp"
#include "arguments_check.hpp"
#include "analytics/set_membership.hpp"

uint32_t perform_set_membership(qpl_job *job_ptr,
                                uint8_t *unpack_buffer_ptr,
                                uint32_t unpack_buffer_size,
                                uint8_t *set_buffer_ptr,
                                uint32_t set_buffer_size) {
    using namespace qpl::ml;
    using namespace qpl::ml::analytics;

    OWN_QPL_CHECK_STATUS(qpl::job::validate_operation<qpl_op_set_membership>(job_ptr))

    const auto input_stream_format  = get_stream_format(job_ptr->parser);
    const auto out_bit_width_format = static_cast<analytics::output_bit_width_format_t>(job_ptr->out_bit_width);
    const auto set_stream_format    = job_ptr->flags & QPL_FLAG_SRC2_BE ? stream_format_t::be_format
                                                                        : stream_format_t::le_format;
    const auto output_stream_format = (job_ptr->flags & QPL_FLAG_OUT_BE) ? stream_format_t::be_format
                                                                         : stream_format_t::le_format;
    const auto crc_type             = job_ptr->flags & QPL_FLAG_CRC32C ? analytics::input_stream_t::crc_t::iscsi
                                                                       : analytics::input_stream_t::crc_t::gzip;

    auto *src_begin = const_cast<uint8_t *>(job_ptr->next_in_ptr);
    auto *src_end   = const_cast<uint8_t *>(job_ptr->next_in_ptr + job_ptr->available_in);
    auto *dst_begin = const_cast<uint8_t *>(job_ptr->next_out_ptr);
    auto *dst_end   = const_cast<uint8_t *>(job_ptr->next_out_ptr + job_ptr->available_out);
    auto *set_begin = const_cast<uint8_t *>(job_ptr->next_src2_ptr);
    auto *set_end   = const_cast<uint8_t *>(job_ptr->next_src2_ptr + job_ptr->available_src2);

    auto *analytics_state_ptr     = reinterpret_cast<own_analytics_state_t *>( job_ptr->data_ptr.analytics_state_ptr);
    auto *decompress_buffer_begin = analytics_state_ptr->inflate_buf_ptr;
    auto *decompress_buffer_end   = decompress_buffer_begin + analytics_state_ptr->inflate_buf_size;

    allocation_buffer_t state_buffer(job_ptr->data_ptr.middle_layer_buffer_ptr, job_ptr->data_ptr.hw_state_ptr);

    auto input_stream = analytics::input_stream_t::builder(src_begin, src_end)
            .element_count(job_ptr->num_input_elements)
            .omit_checksums(job_ptr->flags & QPL_FLAG_OMIT_CHECKSUMS)
            .omit_aggregates(job_ptr->flags & QPL_FLAG_OMIT_AGGREGATES)
            .ignore_bytes(job_ptr->drop_initial_bytes)
            .crc_type(crc_type)
            .compressed(job_ptr->flags & QPL_FLAG_DECOMPRESS_ENABLE,
                        static_cast<qpl_decomp_end_proc>(job_ptr->decomp_end_processing),
                        job_ptr->ignore_end_bits)
            .decompress_buffer<execution_path_t::auto_detect>(decompress_buffer_begin, decompress_buffer_end)
            .stream_format(input_stream_format, job_ptr->src1_bit_width)
            .build<execution_path_t::auto_detect>(state_buffer);

    const uint32_t set_size = 1u << (input_stream.bit_width() - job_ptr->param_low - job_ptr->param_high);

    auto set_stream = analytics::input_stream_t::builder(set_begin, set_end)
            .element_count(set_size)
            .stream_format(set_stream_format, job_ptr->src2_bit_width)
            .build<execution_path_t::auto_detect>();

    auto output_stream = analytics::output_stream_t<analytics::bit_stream>::builder(dst_begin, dst_end)
            .stream_format(output_stream_format)
            .bit_format(out_bit_width_format, bit_bits_size)
            .nominal(true)
            .initial_output_index(job_ptr->initial_output_index)
            .build<execution_path_t::auto_detect>();

    auto bad_arg_status = validate_input_stream(input_stream);

    if (bad_arg_status != status_list::ok) {
        return bad_arg_status;
    }

    // Configure buffers
    limited_buffer_t unpack_buffer(unpack_buffer_ptr, unpack_buffer_ptr + unpack_buffer_size, input_stream.bit_width());
    limited_buffer_t set_buffer(set_buffer_ptr, set_buffer_ptr + set_buffer_size, 1u);

    analytic_operation_result_t result{};

    switch (job_ptr->data_ptr.path) {
        case qpl_path_hardware:
            result = call_set_membership<execution_path_t::hardware>(input_stream,
                                                                     set_stream,
                                                                     output_stream,
                                                                     job_ptr->param_low,
                                                                     job_ptr->param_high,
                                                                     unpack_buffer,
                                                                     set_buffer,
                                                                     job_ptr->numa_id);
            break;
        case qpl_path_auto:
            result = call_set_membership<execution_path_t::auto_detect>(input_stream,
                                                                        set_stream,
                                                                        output_stream,
                                                                        job_ptr->param_low,
                                                                        job_ptr->param_high,
                                                                        unpack_buffer,
                                                                        set_buffer,
                                                                        job_ptr->numa_id);
            break;
        case qpl_path_software:
            result = call_set_membership<execution_path_t::software>(input_stream,
                                                                     set_stream,
                                                                     output_stream,
                                                                     job_ptr->param_low,
                                                                     job_ptr->param_high,
                                                                     unpack_buffer,
                                                                     set_buffer);
    }

    job_ptr->total_out = result.output_bytes_;

    if (result.status_code_ == QPL_STS_OK || result.status_code_ == QPL_STS_DST_IS_SHORT_ERR) {
        update_job(job_ptr, result);
    }

    return result.status_code_;
}
