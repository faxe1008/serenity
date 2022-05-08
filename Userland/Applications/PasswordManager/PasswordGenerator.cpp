/*
 * Copyright (c) 2022, Fabian Blatz <fabianblatz@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "PasswordGenerator.h"
#include <AK/Random.h>
#include <AK/StringView.h>

String PasswordGenerator::generate(size_t length, PasswordGenerationPolicy policy)
{
    StringView character_pool;
    switch (policy) {
    case PasswordGenerationPolicy::LetterAndDigits:
        character_pool = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"sv;
        break;
    case PasswordGenerationPolicy::LetterAndDigitsAndSpecialCharacters:
        character_pool = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+-\\{}|~"sv;
        break;
    case PasswordGenerationPolicy::ExcludeSimilarLookingCharacters:
        character_pool = "abcdefghjkmnpqrstuvwxyzABCDEFGHJKMNPQRSTUVWXYZ123456789!\"#$%&'()*+-\\{}|~"sv;
        break;
    default:
        VERIFY_NOT_REACHED();
    }

    u8 randomized_character_pool_indicies[length];
    fill_with_random(static_cast<void*>(&randomized_character_pool_indicies), length);

    char password[length];
    for (size_t i = 0; i < length; i++) {
        password[i] = character_pool[randomized_character_pool_indicies[i] % character_pool.length()];
    }
    return String(password, length);
}
