#include "Token.h"

Token::Token(char chr)
{
    switch (chr) {
    case '+':
        m_type = TokenType::Plus;
        m_precedence = 2;
        m_associativity = Associativity::Left;
        break;
    case '-':
        m_type = TokenType::Minus;
        m_precedence = 2;
        m_associativity = Associativity::Left;
        break;
    case '*':
        m_type = TokenType::Multiply;
        m_precedence = 3;
        m_associativity = Associativity::Left;
        break;
    case '/':
        m_type = TokenType::Divide;
        m_precedence = 3;
        m_associativity = Associativity::Left;
        break;
    case '^':
        m_type = TokenType::Power;
        m_precedence = 4;
        m_associativity = Associativity::Right;
        break;
    case '(':
        m_type = TokenType::OpeningParenthesis;
        break;
    case ')':
        m_type = TokenType::ClosingParethensis;
        break;
    default:
        VERIFY_NOT_REACHED();
    }
}
