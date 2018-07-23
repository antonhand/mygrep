#include "syntax.h"
#include <stdexcept>
/*!
*\file
*\brief Файл с реализацией синтаксического анализатора
*
* Данный файл содержит в себе реализацию всех методов классов Syntax и Syn_lexem
*/

Syn_lexem::Syn_lexem(Lexem &lexem) : type(lexem.type), terminal(lexem.terminal), param1(lexem.param1), param2(lexem.param2) {}

Syn_lexem::Syn_lexem(int param2, vector<Syn_lexem> operand) : type(Lexem::ITER_N_M), param2(param2), operand1(operand) {}

void Syn_lexem::print()
{
    switch (type) {
    case Lexem::ITER:
        cout << "*(";
        for (auto it = operand1.begin(); it != operand1.end(); it++) {
            it->print();
        }
        cout << ')';
        break;
    case Lexem::ITER_N_M:
        cout << '{' << param1 << ',' << param2 << "}(";
        for (auto it = operand1.begin(); it != operand1.end(); it++) {
            it->print();
        }
        cout << ')';
        break;
    case Lexem::LAZY_ITER:
        cout << "*?(";
        for (auto it = operand1.begin(); it != operand1.end(); it++) {
            it->print();
        }
        cout << ')';
        break;
    case Lexem::TERMINAL:
        cout << terminal;
        break;
    case Lexem::OR:
        cout << '(';
        for (auto it = operand1.begin(); it != operand1.end(); it++) {
            it->print();
        }
        cout << '|';
        for (auto it = operand2.begin(); it != operand2.end(); it++) {
            it->print();
        }
        cout << ')';
        break;
    default:
        cout << endl;
        break;
    }
}

void Syntax::gl()
{
    cur = scan.get_lex();
}

void Syntax::S()
{
    if (cur.type == Lexem::END) {
        return;
    }
    try {
        internal = S1();
        /*for (auto it = internal.begin(); it != internal.end(); it++) {
            it->print();
        }
        cout << endl;*/
    } catch (const invalid_argument &err) {
        cout << "Неверное регулярное выражение: " << err.what() << endl;
        throw;
    }
}

vector<Syn_lexem> Syntax::S1(bool concat)
{
    vector<Syn_lexem> v1, v2;
    switch (cur.type) {
    case Lexem::OPEN_BRACE:
        gl();
        countbrace++;
        v1 = S1();
        if (cur.type != Lexem::CLOSE_BRACE) {
            throw invalid_argument("неверное скобочное выражение.");
        }
        if (!v1.size()) {
            throw invalid_argument("пустое выражение в скобках.");
        }
        countbrace--;
        gl();
        v1 = O(v1);
        break;
    case Lexem::TERMINAL:
        v1.push_back(cur);
        gl();
        v1 = O(v1);
        break;
    case Lexem::END:
        throw invalid_argument("операция '|' должна быть бинарной.");
    case Lexem::CLOSE_BRACE:
        if (countbrace > 0) {
            return v1;
        }
        throw invalid_argument("неожиданный символ: ')'.");
    default:
        throw invalid_argument("операция не может находиться до операндов.");
    }
    if (cur.type == Lexem::TERMINAL || cur.type == Lexem::OPEN_BRACE) {
        v2 = S1(true);
        v1.insert(v1.end(), v2.begin(), v2.end());
    }
    if (!concat && cur.type == Lexem::OR) {
        gl();
        v2 = S1();
        Lexem tmp(Lexem::OR);
        Syn_lexem alter(tmp);
        alter.operand1 = v1;
        alter.operand2 = v2;
        v1.clear();
        v1.push_back(alter);
    }
    if(cur.type == Lexem::CLOSE_BRACE && countbrace <= 0){
        throw invalid_argument("неверное скобочное выражение.");
    }
    return v1;
}

vector<Syn_lexem> Syntax::O(vector<Syn_lexem> &operand)
{
    vector<Syn_lexem> v;
    if (cur.type >= Lexem::ITER && cur.type <= Lexem::LAZY_ITER) {
        if (operand.size() == 1 && operand[0].type == Lexem::ITER) {
            gl();
            return O(operand);
        }
        Syn_lexem oper(cur);
        oper.operand1 = operand;
        v.push_back(oper);
        gl();
        return O(v);
    }
    return operand;
}

Syntax::Syntax(const string &reg) : scan(reg)
{
    gl();
    S();
}


vector<Syn_lexem> Syntax::get_internal()
{
    return internal;
}
