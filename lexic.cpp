#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include "lexic.h"

/*!
*\file
*\brief Файл с реализацией лексического анализатора
*
* Данный файл содержит в себе реализацию всех методов классов Lex и Lexem
*/

void Lexem::print()
{
    switch (type) {
    case ITER:
        cout << '*';
        break;
    case ITER_N_M:
        cout << '{' << param1 << ',' << param2 << '}';
        break;
    case LAZY_ITER:
        cout << "*?";
        break;
    case TERMINAL:
        cout << terminal;
        break;
    case OR:
        cout << '|';
        break;
    default:
        cout << endl;
        break;
    }
}

char Lex::possible_op[] =
{
    '*',
    '|',
    '(',
    ')',
    '+',
    '?',
    '{',
    '}',
    '[',
    ']',
    '-',
    '\\',
    0
};

bool Lex::isoper(char ch)
{
    char *c = possible_op;
    for (; *c != 0; c++) {
        if (ch == *c) {
            return true;
        }
    }
    return false;
}

bool Lex::isterm(char ch)
{
    if (isoper(ch)) {
        return false;
    }
    if (ch >= '!' && ch <= '~') {
        return true;
    }
    return false;
}

Lex::Lex(const string &reg)
{
    if (reg == "") {
        return;
    }
    string terminal;
    try {
        for (auto it = reg.begin(); it != reg.end(); it++) {
            if (*it == '\\') {
                it++;
                if (it == reg.end() || !isoper(*it)) {
                    string err = "ошибочное экранирование символа.";
                    throw invalid_argument(err);
                }
                terminal += *it;
            } else if (isoper(*it)) {
                if (terminal != "") {
                    if ((*it == '*' || *it == '{' || *it == '?') && terminal.length() > 1) {
                        char tmp = terminal[terminal.length() - 1];
                        terminal.pop_back();
                        lexems.push_back(Lexem(terminal));
                        terminal = tmp;
                    }
                    lexems.push_back(Lexem(terminal));
                    terminal = "";
                }
                switch (*it) {
                case '|':
                    lexems.push_back(Lexem(Lexem::OR));
                    break;
                case '*':
                    if (it + 1 != reg.end() && it[1] == '?') {
                        lexems.push_back(Lexem(Lexem::LAZY_ITER));
                        it++;
                    } else {
                        lexems.push_back(Lexem(Lexem::ITER));
                    }
                    break;
                case '(':
                    lexems.push_back(Lexem(Lexem::OPEN_BRACE));
                    break;
                case ')':
                    lexems.push_back(Lexem(Lexem::CLOSE_BRACE));
                    break;
                case '?':
                    lexems.push_back(Lexem(Lexem::ITER_N_M, 0, 1));
                    break;
                case '{':
                {
                    int par1 = 0;
                    int par2 = 0;
                    it++;
                    int count_dig = 0;
                    for (; isdigit(*it); count_dig++, it++) {
                        par1 *= 10;
                        par1 += *it - '0';
                    }
                    if (!count_dig || *it != ',') {
                        throw invalid_argument("неверное описание операции {m,n}.");
                    }
                    count_dig = 0;
                    it++;
                    for (; isdigit(*it); count_dig++, it++) {
                        par2 *= 10;
                        par2 += *it - '0';
                    }
                    if (!count_dig || *it != '}') {
                        throw invalid_argument("неверное описание операции {m,n}.");
                    } else {
                        lexems.push_back(Lexem(Lexem::ITER_N_M, par1, par2));
                    }
                    break;
                }
                default:
                    throw *it;
                }
            } else if (isterm(*it)) {
                terminal += *it;
            } else {
                throw *it;
            }
        }
    } catch (const invalid_argument &err) {
        cout << "Неверное регулярное выражение: " << err.what() << endl;
        throw;
    } catch (char ch) {
        cout << "Неверное регулярное выражение: символ '" << ch << "' не может быть в регулярном выражении.";
        throw;
    }
    if (terminal != "") {
        lexems.push_back(terminal);
    }
}

Lexem Lex::get_lex()
{
    if (cur < lexems.size()) {
        return lexems[cur++];
    }
    return Lexem(Lexem::END);
}
