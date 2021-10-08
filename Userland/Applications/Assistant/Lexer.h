/*
 * Copyright (c) 2021, Fabian Blatz
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once
#include "Token.h"
#include <AK/GenericLexer.h>
#include <AK/StringView.h>
#include <AK/Vector.h>

class Lexer : private GenericLexer {
public:
    Lexer(StringView& input)
        : GenericLexer(input)
    {
    }

private:
};
