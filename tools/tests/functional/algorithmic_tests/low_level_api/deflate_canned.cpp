/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include <array>

#include "operation_test.hpp"
#include "source_provider.hpp"
#include "ta_ll_common.hpp"

namespace qpl::test
{
    template<class Iterator>
    auto buildHuffmanTable(qpl_compression_huffman_table *huffman_table_ptr,
                           Iterator begin,
                           Iterator end,
                           qpl_compression_levels level,
                           qpl_path_t path) -> void
    {
        auto           *source_ptr = &*begin;
        const uint32_t source_size = std::distance(begin, end);

        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;

        qpl_histogram deflate_histogram{};

        auto status = qpl_gather_deflate_statistics(source_ptr,
                                                    source_size,
                                                    &deflate_histogram,
                                                    level,
                                                    path);

        ASSERT_EQ(status, QPL_STS_OK) << "Failed to gather statistics";

        status = qpl_build_compression_table(&deflate_histogram,
                                             huffman_table_ptr,
                                             QPL_DEFLATE_REPRESENTATION | table_representation);


        ASSERT_EQ(status, QPL_STS_OK) << "Failed to build compression table";
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned, default_level, JobFixture)
    {
        auto path = GetExecutionPath();
        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;

        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            source = dataset.second;

            destination.resize(source.size() * 2);
            std::vector<uint8_t> reference_buffer(destination.size(), 0u);

            const uint32_t file_size = (uint32_t) source.size();
            ASSERT_NE(0u, file_size) << "Couldn't open file: "
                                     << dataset.first;

            auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
            auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

            // Init job for a file
            uint32_t job_size = 0u;
            auto     status   = qpl_get_job_size(path, &job_size);
            ASSERT_EQ(QPL_STS_OK, status);

            auto job_buffer = std::make_unique<uint8_t[]>(job_size);
            auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

            buildHuffmanTable(huffman_table_ptr,
                              source.data(),
                              source.data() + file_size,
                              qpl_default_level,
                              path);

            ASSERT_EQ(QPL_STS_OK, status) << "Failed to build huffman table";

            status = qpl_init_job(path, decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

            // Configure compression job fields
            job_ptr->op                         = qpl_op_compress;
            job_ptr->level                      = qpl_default_level;
            job_ptr->next_in_ptr                = source.data();
            job_ptr->available_in               = file_size;
            job_ptr->next_out_ptr               = destination.data();
            job_ptr->available_out              = static_cast<uint32_t>(destination.size());
            job_ptr->compression_huffman_table  = huffman_table_ptr;
            job_ptr->flags                      = QPL_FLAG_FIRST |
                                                  QPL_FLAG_LAST |
                                                  QPL_FLAG_OMIT_VERIFY |
                                                  QPL_FLAG_CANNED_MODE;

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed";

#ifdef DEBUG_RATIO
            g_ostrm << "Ratio: "
                    << ((float)file_size / (float)job_ptr->total_out)
                    << "\n";
#endif

            // Create decompression table
            auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
            auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

            // Create compression table from decompression one
            status = qpl_comp_to_decompression_table(huffman_table_ptr,
                                                     decompression_table_ptr,
                                                     table_representation | QPL_DEFLATE_REPRESENTATION);

            ASSERT_EQ(QPL_STS_OK, status) << "decompression table creation failed";

            // Configure decompression job fields
            decompression_job_ptr->op            = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = destination.data();
            decompression_job_ptr->available_in  = job_ptr->total_out;
            decompression_job_ptr->next_out_ptr  = reference_buffer.data();
            decompression_job_ptr->available_out = static_cast<uint32_t>(reference_buffer.size());

            decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE;
            decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;

            status = run_job_api(decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "decompression failed";

            ASSERT_TRUE(CompareVectors(reference_buffer,
                                       source,
                                       file_size,
                                       "File: " + dataset.first));

            qpl_fini_job(job_ptr);
            qpl_fini_job(decompression_job_ptr);
        }
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned_indexing, default_level, JobFixture)
    {
        auto path = GetExecutionPath();
        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;
        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            for (uint32_t mini_block_size = qpl_mblk_size_512; mini_block_size < qpl_mblk_size_32k; mini_block_size++) {
            source = dataset.second;

            destination.resize(source.size() * 2);
            std::vector<uint8_t> reference_buffer(destination.size(), 0u);

            const uint32_t file_size = (uint32_t) source.size();
            ASSERT_NE(0u, file_size) << "Couldn't open file: "
                                     << dataset.first;

            std::vector<uint64_t> indices_array(source.size());

            auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
            auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

            // Init job for a file
            uint32_t job_size = 0u;
            auto     status   = qpl_get_job_size(path, &job_size);
            ASSERT_EQ(QPL_STS_OK, status);

            auto job_buffer = std::make_unique<uint8_t[]>(job_size);
            auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

            buildHuffmanTable(huffman_table_ptr,
                              source.data(),
                              source.data() + file_size,
                              qpl_default_level,
                              path);

            ASSERT_EQ(QPL_STS_OK, status) << "Failed to build huffman table";

            status = qpl_init_job(path, decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

            status = qpl_init_job(path, job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init compression job";

            // Configure compression job fields
            job_ptr->op                         = qpl_op_compress;
            job_ptr->level                      = qpl_default_level;
            job_ptr->next_in_ptr                = source.data();
            job_ptr->available_in               = file_size;
            job_ptr->next_out_ptr               = destination.data();
            job_ptr->available_out              = static_cast<uint32_t>(destination.size());
            job_ptr->compression_huffman_table  = huffman_table_ptr;
            job_ptr->flags                      = QPL_FLAG_FIRST |
                                                  QPL_FLAG_LAST |
                                                  QPL_FLAG_CANNED_MODE;
            job_ptr->idx_array = indices_array.data();
            job_ptr->mini_block_size = static_cast<qpl_mini_block_size>(mini_block_size);
            job_ptr->idx_max_size = static_cast<uint32_t>(indices_array.size());

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed\n File index: " << dataset.first;

#ifdef DEBUG_RATIO
            g_ostrm << "Ratio: "
                    << ((float)file_size / (float)job_ptr->total_out)
                    << "\n";
#endif

            // Create decompression table
            auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
            auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

            // Create compression table from decompression one
            status = qpl_comp_to_decompression_table(huffman_table_ptr,
                                                     decompression_table_ptr,
                                                     table_representation | QPL_DEFLATE_REPRESENTATION);

            ASSERT_EQ(QPL_STS_OK, status) << "decompression table creation failed";

            // Configure decompression job fields
            decompression_job_ptr->op            = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = destination.data();
            decompression_job_ptr->available_in  = job_ptr->total_out;
            decompression_job_ptr->next_out_ptr  = reference_buffer.data();
            decompression_job_ptr->available_out = static_cast<uint32_t>(reference_buffer.size());
            decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;
            decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE | QPL_FLAG_FIRST;

            // Decompress by miniblocks
            for (uint32_t i = 0; i < job_ptr->idx_num_written - 1; i++) {
                auto bit_start = (uint32_t) indices_array[i];
                auto bit_end   = (uint32_t) indices_array[i + 1];
                decompression_job_ptr->ignore_start_bits = bit_start & 7;
                decompression_job_ptr->ignore_end_bits   = 7 & (0 - bit_end);
                decompression_job_ptr->available_in      = ((bit_end + 7) / 8) - (bit_start / 8);
                decompression_job_ptr->next_in_ptr       = destination.data() + bit_start / 8;


                status = run_job_api(decompression_job_ptr);
                ASSERT_EQ(QPL_STS_OK, status) << "decompression failed";

                decompression_job_ptr->flags &= ~QPL_FLAG_FIRST;

                // Check for crc correctness
                uint32_t required_crc = static_cast<uint32_t>(indices_array[i + 1] >> 32u);
                ASSERT_EQ(decompression_job_ptr->crc, required_crc);
            }


            ASSERT_TRUE(CompareVectors(reference_buffer,
                                       source,
                                       file_size,
                                       "File: " + dataset.first));

            qpl_fini_job(job_ptr);
            qpl_fini_job(decompression_job_ptr);
        }
        }
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned, high_level, JobFixture)
    {
        auto path = GetExecutionPath();
        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;

        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            source = dataset.second;

            destination.resize(source.size() * 2);
            std::vector<uint8_t> reference_buffer(destination.size(), 0u);

            const uint32_t file_size = (uint32_t) source.size();
            ASSERT_NE(0u, file_size) << "Couldn't open file: "
                                     << dataset.first;

            auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
            auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

            // Init job for a file
            uint32_t job_size = 0u;
            auto     status   = qpl_get_job_size(path, &job_size);
            ASSERT_EQ(QPL_STS_OK, status);

            auto job_buffer = std::make_unique<uint8_t[]>(job_size);
            auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

            buildHuffmanTable(huffman_table_ptr,
                              source.data(),
                              source.data() + file_size,
                              qpl_high_level,
                              path);

            ASSERT_EQ(QPL_STS_OK, status) << "Failed to build huffman table";

            status = qpl_init_job(path, decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

            status = qpl_init_job(path, job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init compression job";

            // Configure compression job fields
            job_ptr->op                                = qpl_op_compress;
            job_ptr->next_in_ptr                            = source.data();
            job_ptr->available_in                           = file_size;
            job_ptr->next_out_ptr                           = destination.data();
            job_ptr->available_out                          = static_cast<uint32_t>(destination.size());
            job_ptr->compression_huffman_table           = huffman_table_ptr;
            job_ptr->level                             = qpl_high_level;
            job_ptr->flags                             = QPL_FLAG_FIRST |
                                                         QPL_FLAG_LAST |
                                                         QPL_FLAG_OMIT_VERIFY |
                                                         QPL_FLAG_CANNED_MODE;

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed";

#ifdef DEBUG_RATIO
            std::cout << "Ratio: "
                      << ((float)file_size / (float)job_ptr->total_out)
                      << "\n";
#endif

            // Create decompresssion table
            auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
            auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

            qpl_comp_to_decompression_table(huffman_table_ptr,
                                            decompression_table_ptr,
                                            table_representation | QPL_DEFLATE_REPRESENTATION);

            // Configure decompression job fields
            decompression_job_ptr->op       = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = destination.data();
            decompression_job_ptr->available_in  = job_ptr->total_out;
            decompression_job_ptr->next_out_ptr  = reference_buffer.data();
            decompression_job_ptr->available_out = static_cast<uint32_t>(reference_buffer.size());
            decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE;
            decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;

            status = run_job_api(decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "decompression failed";

            ASSERT_TRUE(CompareVectors(reference_buffer,
                                       source,
                                       file_size,
                                       "File: " + dataset.first));

            qpl_fini_job(job_ptr);
            qpl_fini_job(decompression_job_ptr);
        }
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned_indexing, high_level, JobFixture)
    {
        auto path = GetExecutionPath();

        if (qpl_path_t::qpl_path_hardware == path) {
            GTEST_SKIP_("High level compression is not supported on hardware path");
        }

        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;
        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            for (uint32_t mini_block_size = qpl_mblk_size_512; mini_block_size < qpl_mblk_size_32k; mini_block_size++) {
            source = dataset.second;

            destination.resize(source.size() * 2);
            std::vector<uint8_t> reference_buffer(destination.size(), 0u);

            const uint32_t file_size = (uint32_t) source.size();
            ASSERT_NE(0u, file_size) << "Couldn't open file: "
                                     << dataset.first;

            std::vector<uint64_t> indices_array(source.size());

            auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
            auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

            // Init job for a file
            uint32_t job_size = 0u;
            auto     status   = qpl_get_job_size(path, &job_size);
            ASSERT_EQ(QPL_STS_OK, status);

            auto job_buffer = std::make_unique<uint8_t[]>(job_size);
            auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

            buildHuffmanTable(huffman_table_ptr,
                              source.data(),
                              source.data() + file_size,
                              qpl_default_level,
                              path);

            ASSERT_EQ(QPL_STS_OK, status) << "Failed to build huffman table";

            status = qpl_init_job(path, decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

            status = qpl_init_job(path, job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "Failed to init compression job";

            // Configure compression job fields
            job_ptr->crc = 0;
            job_ptr->op                         = qpl_op_compress;
            job_ptr->next_in_ptr                = source.data();
            job_ptr->available_in               = file_size;
            job_ptr->next_out_ptr               = destination.data();
            job_ptr->available_out              = static_cast<uint32_t>(destination.size());
            job_ptr->compression_huffman_table  = huffman_table_ptr;
            job_ptr->level                      = qpl_high_level;
            job_ptr->flags                      = QPL_FLAG_FIRST |
                                                  QPL_FLAG_LAST |
                                                  QPL_FLAG_CANNED_MODE;
            job_ptr->idx_array = indices_array.data();
            job_ptr->mini_block_size = static_cast<qpl_mini_block_size>(mini_block_size);
            job_ptr->idx_max_size = static_cast<uint32_t>(indices_array.size());

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed";

#ifdef DEBUG_RATIO
            g_ostrm << "Ratio: "
                    << ((float)file_size / (float)job_ptr->total_out)
                    << "\n";
#endif

            // Create decompression table
            auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
            auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

            // Create compression table from decompression one
            status = qpl_comp_to_decompression_table(huffman_table_ptr,
                                                     decompression_table_ptr,
                                                     table_representation | QPL_DEFLATE_REPRESENTATION);

            ASSERT_EQ(QPL_STS_OK, status) << "decompression table creation failed";

            // Configure decompression job fields
            decompression_job_ptr->op            = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = destination.data();
            decompression_job_ptr->available_in  = job_ptr->total_out;
            decompression_job_ptr->next_out_ptr  = reference_buffer.data();
            decompression_job_ptr->available_out = static_cast<uint32_t>(reference_buffer.size());
            decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;
            decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE | QPL_FLAG_FIRST;

            // Decompress by miniblocks
            for (uint32_t i = 0; i < job_ptr->idx_num_written - 1; i++) {
                auto bit_start = (uint32_t) indices_array[i];
                auto bit_end   = (uint32_t) indices_array[i + 1];
                decompression_job_ptr->ignore_start_bits = bit_start & 7;
                decompression_job_ptr->ignore_end_bits   = 7 & (0 - bit_end);
                decompression_job_ptr->available_in      = ((bit_end + 7) / 8) - (bit_start / 8);
                decompression_job_ptr->next_in_ptr       = destination.data() + bit_start / 8;


                status = run_job_api(decompression_job_ptr);
                ASSERT_EQ(QPL_STS_OK, status) << "decompression failed";

                decompression_job_ptr->flags &= ~QPL_FLAG_FIRST;

                // Check for crc correctness
                uint32_t required_crc = static_cast<uint32_t>(indices_array[i + 1] >> 32u);
                ASSERT_EQ(decompression_job_ptr->crc, required_crc);
            }


            ASSERT_TRUE(CompareVectors(reference_buffer,
                                       source,
                                       file_size,
                                       "File: " + dataset.first));

            qpl_fini_job(job_ptr);
            qpl_fini_job(decompression_job_ptr);
        }
        }
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned, complex_high_level, JobFixture)
    {
        uint32_t              required_output_size = 0;
        uint32_t              job_size             = 0u;
        std::vector<uint32_t> file_sizes;
        std::vector<uint32_t> compressed_file_sizes;

        auto path = GetExecutionPath();

        if (qpl_path_t::qpl_path_hardware == path) {
            GTEST_SKIP_("High level compression is not supported on hardware path");
        }

        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;

        auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
        auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            std::vector<uint8_t> file_content;

            file_content = dataset.second;

            source.insert(source.end(), file_content.begin(), file_content.end());
            required_output_size += (uint32_t) file_content.size();
            file_sizes.push_back((uint32_t) file_content.size());
        }

        destination.resize(required_output_size * 2);
        std::vector<uint8_t> reference_buffer(destination.size(), 0u);


        // Init job for a file
        auto status = qpl_get_job_size(path, &job_size);
        ASSERT_EQ(QPL_STS_OK, status);

        auto job_buffer = std::make_unique<uint8_t[]>(job_size);
        auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

        buildHuffmanTable(huffman_table_ptr,
                          source.begin(),
                          source.end(),
                          qpl_high_level,
                          path);

        ASSERT_EQ(QPL_STS_OK, status) << "Failed to build huffman table";

        status = qpl_init_job(path, decompression_job_ptr);
        ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

        job_ptr->compression_huffman_table           = huffman_table_ptr;

        uint8_t *current_source_ptr      = &(*source.begin());
        uint8_t *current_destination_ptr = &(*destination.begin());


        for (auto file_size : file_sizes)
        {
            status = qpl_init_job(path, job_ptr);
            ASSERT_EQ(QPL_STS_OK, status);

            // Configure compression job fields
            job_ptr->op                        = qpl_op_compress;
            job_ptr->next_in_ptr               = current_source_ptr;
            job_ptr->available_in              = file_size;
            job_ptr->next_out_ptr              = current_destination_ptr;
            job_ptr->available_out             = file_size * 2;
            job_ptr->compression_huffman_table = huffman_table_ptr;
            job_ptr->level                     = qpl_high_level;
            job_ptr->flags                     = QPL_FLAG_FIRST |
                                                 QPL_FLAG_LAST |
                                                 QPL_FLAG_OMIT_VERIFY |
                                                 QPL_FLAG_CANNED_MODE;

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed";

            compressed_file_sizes.push_back(job_ptr->total_out);
            current_destination_ptr += job_ptr->total_out;
            current_source_ptr += file_size;

#ifdef DEBUG_RATIO
            std::cout << "Ratio: "
                      << ((float) file_size / (float) job_ptr->total_out)
                      << "\n";
#endif

            qpl_fini_job(job_ptr);
        }

        uint8_t  *current_reference_buffer_ptr  = &(*reference_buffer.begin());
        uint8_t  *current_compressed_source_ptr = &(*destination.begin());
        uint32_t output_bytes_available         = (uint32_t) reference_buffer.size();

        // Create decompression table
        auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
        auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

        qpl_comp_to_decompression_table(huffman_table_ptr,
                                        decompression_table_ptr,
                                        table_representation | QPL_DEFLATE_REPRESENTATION);

        decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE;
        decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;
        decompression_job_ptr->next_out_ptr  = current_reference_buffer_ptr;
        decompression_job_ptr->available_out = output_bytes_available;

        for (auto compressed_size : compressed_file_sizes)
        {
            decompression_job_ptr->op       = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = current_compressed_source_ptr;
            decompression_job_ptr->available_in  = compressed_size;

            status                 = run_job_api(decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status);

            current_compressed_source_ptr += compressed_size;
        }

        qpl_fini_job(decompression_job_ptr);

        ASSERT_TRUE(CompareVectors(reference_buffer, source, (uint32_t) source.size()));
    }

    QPL_LOW_LEVEL_API_ALGORITHMIC_TEST_F(deflate_canned, complex_default_level, JobFixture)
    {
        uint32_t              required_output_size = 0;
        uint32_t              job_size             = 0u;
        std::vector<uint32_t> file_sizes;
        std::vector<uint32_t> compressed_file_sizes;

        auto path = GetExecutionPath();
        auto table_representation = (path != qpl_path_hardware) ? QPL_SW_REPRESENTATION : QPL_HW_REPRESENTATION;

        auto table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_COMPRESSION_TABLE_SIZE));
        auto huffman_table_ptr = reinterpret_cast<qpl_compression_huffman_table *>(table_buffer.get());

        for (auto &dataset: util::TestEnvironment::GetInstance().GetAlgorithmicDataset().get_data())
        {
            std::vector<uint8_t> file_content = dataset.second;

            source.insert(source.end(), file_content.begin(), file_content.end());
            required_output_size += (uint32_t) file_content.size();
            file_sizes.push_back((uint32_t) file_content.size());
        }

        destination.resize(required_output_size * 2);
        std::vector<uint8_t> reference_buffer(destination.size(), 0u);


        // Init job for a file
        auto status = qpl_get_job_size(path, &job_size);
        ASSERT_EQ(QPL_STS_OK, status);

        auto job_buffer = std::make_unique<uint8_t[]>(job_size);
        auto *const decompression_job_ptr = reinterpret_cast<qpl_job *const>(job_buffer.get());

        buildHuffmanTable(huffman_table_ptr,
                          source.begin(),
                          source.end(),
                          qpl_default_level,
                          path);

        ASSERT_EQ(QPL_STS_OK, status);

        status = qpl_init_job(path, decompression_job_ptr);
        ASSERT_EQ(QPL_STS_OK, status) << "Failed to init decompression job";

        job_ptr->compression_huffman_table           = huffman_table_ptr;

        uint8_t *current_source_ptr      = &(*source.begin());
        uint8_t *current_destination_ptr = &(*destination.begin());


        for (auto file_size : file_sizes)
        {
            status = qpl_init_job(path, job_ptr);
            ASSERT_EQ(QPL_STS_OK, status);

            // Configure compression job fields
            job_ptr->op                         = qpl_op_compress;
            job_ptr->level                      = qpl_default_level;
            job_ptr->next_in_ptr                = current_source_ptr;
            job_ptr->available_in               = file_size;
            job_ptr->next_out_ptr               = current_destination_ptr;
            job_ptr->available_out              = file_size * 2;
            job_ptr->compression_huffman_table  = huffman_table_ptr;
            job_ptr->flags                      = QPL_FLAG_FIRST |
                                                  QPL_FLAG_LAST |
                                                  QPL_FLAG_OMIT_VERIFY |
                                                  QPL_FLAG_CANNED_MODE;

            status = run_job_api(job_ptr);
            ASSERT_EQ(QPL_STS_OK, status) << "compression failed";

            compressed_file_sizes.push_back(job_ptr->total_out);
            current_destination_ptr += job_ptr->total_out;
            current_source_ptr += file_size;

#ifdef DEBUG_RATIO
            std::cout << "Ratio: "
                      << ((float) file_size / (float) job_ptr->total_out)
                      << "\n";
#endif

            qpl_fini_job(job_ptr);
        }

        uint8_t  *current_reference_buffer_ptr  = &(*reference_buffer.begin());
        uint8_t  *current_compressed_source_ptr = &(*destination.begin());
        uint32_t output_bytes_available         = (uint32_t) reference_buffer.size();


        // Create decompresssion table
        auto decompression_table_buffer = std::make_unique<uint8_t[]>(static_cast<uint32_t>(QPL_DECOMPRESSION_TABLE_SIZE));
        auto decompression_table_ptr = reinterpret_cast<qpl_decompression_huffman_table*>(decompression_table_buffer.get());

        qpl_comp_to_decompression_table(huffman_table_ptr,
                                        decompression_table_ptr,
                                        table_representation | QPL_DEFLATE_REPRESENTATION);

        decompression_job_ptr->flags = QPL_FLAG_NO_BUFFERING | QPL_FLAG_RND_ACCESS | QPL_FLAG_CANNED_MODE;
        decompression_job_ptr->decompression_huffman_table = decompression_table_ptr;
        decompression_job_ptr->next_out_ptr  = current_reference_buffer_ptr;
        decompression_job_ptr->available_out = output_bytes_available;

        for (auto compressed_size : compressed_file_sizes)
        {
            decompression_job_ptr->op            = qpl_op_decompress;
            decompression_job_ptr->next_in_ptr   = current_compressed_source_ptr;
            decompression_job_ptr->available_in  = compressed_size;

            status                 = run_job_api(decompression_job_ptr);
            ASSERT_EQ(QPL_STS_OK, status);

            current_compressed_source_ptr += compressed_size;
        }

        qpl_fini_job(decompression_job_ptr);

        ASSERT_TRUE(CompareVectors(reference_buffer, source, (uint32_t) source.size()));
    }
}
