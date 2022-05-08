/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>

class PasswordGenerator {
public:
    PasswordGenerator() = delete;
    enum class PasswordGenerationPolicy {
        LetterAndDigits,
        LetterAndDigitsAndSpecialCharacters,
        ExcludeSimilarLookingCharacters
    };

    static String generate(size_t, PasswordGenerationPolicy policy = PasswordGenerationPolicy::LetterAndDigitsAndSpecialCharacters);
};
