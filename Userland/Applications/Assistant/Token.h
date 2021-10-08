/*
 * Copyright (c) 2021, Fabian Blatz
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Assertions.h>
#include <AK/StringView.h>
#include <AK/Types.h>

enum class TokenType {
    Unknown,
    Plus,
    Minus,
    Multiply,
    Divide,
    Power,
    Number,
    OpeningParenthesis,
    ClosingParethensis
};

enum class Associativity {
    Left,
    Right
};

class Token {
public:
    Token(char);
    Token(double value)
        : m_type(TokenType::Number)
        , m_value(value)

    {
    }

    TokenType type() const { return m_type; }
    bool is_operator() const
    {
        return m_type == TokenType::Plus || m_type == TokenType::Minus || m_type == TokenType::Divide || m_type == TokenType::Multiply || m_type == TokenType::Power;
    }
    Associativity associativity() const
    {
        VERIFY(is_operator());
        return m_associativity;
    }
    u8 precedence() const
    {
        VERIFY(is_operator());
        return m_precedence;
    }
    double value() const
    {
        VERIFY(m_type == TokenType::Number);
        return m_value;
    }

private:
    TokenType m_type { TokenType::Unknown };
    Associativity m_associativity { Associativity::Left };
    u8 m_precedence { 0 };
    double m_value { 0 };
};
