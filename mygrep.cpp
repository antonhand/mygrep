#include "mygrep.h"
#include "syntax.h"
#include <string>

/*!
*\file
*\brief Файл с реализацией основного функционала программы
*
* Данный файл содержит в себе реализацию всех методов класса Mygrep
*/

Mygrep::Mygrep(const string& reg)
{
    Syntax syn(reg);
    internal = syn.get_internal();
    internal_it = internal.begin();
}

Mygrep::Mygrep(vector<Syn_lexem> &v)
{
    internal = v;
    internal_it = internal.begin();
}

bool Mygrep::selector()
{
    switch (internal_it->type) {
    case Lexem::ITER:
        iter();
        break;
    case Lexem::ITER_N_M:
        return iter_n_m(internal_it->param1, internal_it->param2);
    case Lexem::LAZY_ITER:
        iter(true);
        break;
    case Lexem::TERMINAL:
        return terminal();
    case Lexem::OR:
        return alternate();
    default:
        return false;
    }
    return true;
}

bool Mygrep::check(const string &s, bool forsearch)
{
    issearch = forsearch;
    inp = s;
    inp_pos = 0;
    for (internal_it = internal.begin(); internal_it != internal.end(); internal_it++) {
        bool ispush = false;
        if (internal_it + 1 != internal.end()) {
            v_next.insert(v_next.begin(), internal_it + 1, internal.end());
            ispush = true;
        }
        if (!selector()) {
            if (ispush) {
                v_next.clear();
            }
            return false;
        }
        if (ispush) {
            v_next.clear();
        }
    }
    return forsearch || inp_pos == inp.size();
}

bool Mygrep::check(vector<Syn_lexem> &v)
{
    auto tmp = internal_it;
    for (internal_it = v.begin(); internal_it != v.end(); internal_it++) {
        bool ispush = false;
        int count_ins = 0;
        if (internal_it + 1 != v.end()) {
            count_ins = distance(internal_it + 1, v.end());
            v_next.insert(v_next.begin(), internal_it + 1, v.end());
            ispush = true;
        }
        if (!selector()) {
            internal_it = tmp;
            if (ispush) {
                v_next.erase(v_next.begin(), v_next.begin() + count_ins);
            }
            return false;
        }
        if (ispush) {
            v_next.erase(v_next.begin(), v_next.begin() + count_ins);
        }
    }
    internal_it = tmp;
    return true;
}

string Mygrep::search(const string &s)
{
    issearch = true;
    for (unsigned beg_pos = 0; beg_pos < s.length(); beg_pos++) {
        if (check(s.substr(beg_pos), true) && inp_pos) {
            return s.substr(beg_pos, inp_pos);
        }
    }
    return " ";
}

bool Mygrep::check_next()
{
    Mygrep next(v_next);
    return next.check(inp.substr(inp_pos), issearch);
}

void Mygrep::iter(bool lazy)
{
    /*cout << "iter ";
    internal_it->print();
    cout << endl;*/
    if (inp_pos == inp.length()) {
        /*cout << "iter_out ";
        cout << endl;*/
        return;
    }
    bool ispush = false;
    if (internal_it->operand1[0].type != Lexem::TERMINAL) {
        v_next.insert(v_next.begin(), *internal_it);
        ispush = true;
    }
    size_t good_pos = inp_pos;
    size_t beg_pos = inp_pos;
    while (check(internal_it->operand1)) {
        if (inp_pos == beg_pos) {
            break;
        } else {
            beg_pos = inp_pos;
        }
        if (check_next()) {
            good_pos = inp_pos;
            if (issearch && lazy) {
                break;
            }
        }

    }
    inp_pos = good_pos;
    if (ispush) {
        v_next.erase(v_next.begin());
    }
   /* cout << "iter_out ";
    cout << endl;*/
}

bool Mygrep::iter_n_m(int n, int m)
{
    for (int i = 0; i < n; i++) {
        if (!check(internal_it->operand1)) {
            return false;
        }
    }
    size_t good_pos = inp_pos;
    for (int i = n; i < m; i++) {
        if (!check(internal_it->operand1)) {
            break;
        }
        v_next.insert(v_next.begin(), (Syn_lexem(m - n - 1, internal_it->operand1)));
        if (check_next()) {
            good_pos = inp_pos;
        }
        v_next.erase(v_next.begin());
    }
    inp_pos = good_pos;
    return true;
}

bool Mygrep::alternate()
{
    /*cout << "alter ";
    internal_it->print();
    cout << endl;*/
    auto tmp_pos = inp_pos;
    size_t new_pos = inp_pos;
    size_t beg_pos = inp_pos;
    bool good = false;
    if (check(internal_it->operand1)){
            new_pos = inp_pos;
            good = true;
    }
    inp_pos = tmp_pos;
    if (check(internal_it->operand2)) {
        if (new_pos == inp_pos && inp_pos == beg_pos) {
            return true;
        }
        if (good && new_pos >= inp_pos) {
            swap(inp_pos, new_pos);
            if (check_next()) {
                return true;
            } else {
                inp_pos = new_pos;
                return check_next();
            }
        } else if (check_next()) {
            return true;
        } else if (good) {
            inp_pos = new_pos;
            return check_next();
        }
    } else if (good) {
        inp_pos = new_pos;
        return check_next();
    }
    return false;
}

bool Mygrep::terminal()
{
    if (inp.find(internal_it->terminal, inp_pos) == inp_pos) {
        inp_pos += internal_it->terminal.size();
        return true;
    }
    return false;
}



