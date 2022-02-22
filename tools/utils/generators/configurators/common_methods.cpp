/*******************************************************************************
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#include "common_methods.hpp"
#include "random_generator.h"

Gen8u gz_generator::CommonMethods::bsr_32(Gen32u number)
{
    Gen8u bitsCount = 0;

    if (number >= (1u << 16u))
    {
        number >>= 16u;
        bitsCount = 16;
    }

    if (number >= (1u << 8u))
    {
        number >>= 8u;
        bitsCount += 8;
    }

    if (number >= (1u << 4u))
    {
        number >>= 4u;
        bitsCount += 4;
    }

    if (number >= (1u << 2u))
    {
        number >>= 2u;
        bitsCount += 2;
    }

    if (number >= (1u << 1u))
    {
        number >>= 1u;
        bitsCount += 1;
    }

    return bitsCount;
}

void gz_generator::CommonMethods::shuffle_32u(Gen32u *vector_ptr, Gen32u vectorLength, Gen32u seed)
{
    Gen32u *current_ptr = vector_ptr;
    Gen32u *pVectorEnd = vector_ptr + vectorLength;
    Gen32u tmp = 0;

    if (vectorLength > 1u)
    {
        qpl::test::random rand(0u, vectorLength - 1u, seed);
        while (current_ptr < pVectorEnd)
        {
            Gen32u i = static_cast<Gen32u>(rand);
            tmp = vector_ptr[i];
            vector_ptr[i] = *current_ptr;
            *current_ptr = tmp;
            current_ptr++;
        }
    }
}

Gen32u gz_generator::CommonMethods::pick(Gen32u *vector_ptr, Gen32u vectorLength, Gen32u seed)
{
    qpl::test::random random(0, vectorLength, seed);

    return vector_ptr[static_cast<Gen32u>(random)];
}

Gen32u gz_generator::CommonMethods::code2Match(Gen32u code, Gen32u seed)
{
    Gen32u length = 0;
    Gen32u numberExtraBits = 0;
    Gen32u extra_bits = 0;

    Gen32u extraBitCountTable[] = {0, 0, 0, 0, 0, 0,
                                   0, 0, 1, 1, 1, 1,
                                   2, 2, 2, 2, 3, 3,
                                   3, 3, 4, 4, 4, 4,
                                   5, 5, 5, 5, 0};

    Gen32u distanceTable[] = {3, 4, 5, 6, 7,
                              8, 9, 10, 11, 13,
                              15, 17, 19, 23, 27,
                              31, 35, 43, 51, 59,
                              67, 83, 99, 115, 131,
                              163, 195, 227, 258};

    qpl::test::random random(0, 0, seed);
    code -= 257;

    numberExtraBits = extraBitCountTable[code];
    length = distanceTable[code];
    random.set_range(0, (1u << numberExtraBits));
    extra_bits = static_cast<Gen32u> (random);
    length += extra_bits;

    length = ((length == 258) && (code != 28)) ? 257 : length;

    return length;
}