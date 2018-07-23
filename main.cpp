#include <iostream>
#include <fstream>
#include <dirent.h>
#include "mygrep.h"

using namespace std;

void test(){
    DIR *d = opendir("./tests");
    dirent *file;
    if(d){
        file = readdir(d);
    } else {
        return;
    }
    while(file){
        string name(file->d_name);
        if(name.length() >= 3 && name.substr(name.length() - 3) == ".re"){
            ifstream fin;
            fin.open("./tests/" + name);
            string s, s2;
            fin >> s;
            bool issearch = false;
            if (fin >> s2 && s2 == "search") {
                issearch = true;
                cout << "Режим поиска" << endl;
            }
            fin.close();
            cout << "Регулярное выражение: '" << s << '\'' << endl;
            try {
                Mygrep mygrep(s);
                fin.open("./tests/" + name.substr(0, name.length() - 3) + ".dat");
                cout << "Входные данные:" << endl;
                while (getline(fin, s)) {
                    cout << s << endl;
                }
                fin.close();
                fin.open("./tests/" + name.substr(0, name.length() - 3) + ".dat");
                cout << "Выходные данные:" << endl;
                ifstream ans;
                ans.open("./tests/" + name.substr(0, name.length() - 3) + ".ans");
                bool good = true;
                while (getline(fin, s)) {
                    string s1;
                    if ((!issearch && mygrep.check(s)) || (issearch && (s = mygrep.search(s)) != " ")) {
                        cout << s << endl;
                        if (ans >> s1) {
                            if (s != s1) {
                                good = false;
                            }
                        } else {
                            good = false;
                        }
                    }
                }
                if (ans >> s) {
                    good = false;
                }
                cout << "Вердикт:" << endl;
                if (good) {
                    cout << "Пройдено";
                } else {
                    cout << "Неправильный ответ";
                }
                cout << endl << endl;
                fin.close();
                ans.close();
            } catch (...) {}
        }
        file = readdir(d);
    }
    closedir(d);
}

int main(int argc, char *argv[])
{
    if(argc == 2 && string(argv[1]) == "-test"){
        test();
        return 0;
    }
    try{
        Mygrep mygrep(argv[1]);
        string inp;
        if (argc == 2) {
            while (getline(cin, inp)) {
                if (mygrep.check(inp)) {
                    cout << inp << endl;
                }
            }
        } else if (argc > 2 && string(argv[2]) == "search") {
            while (getline(cin, inp)) {
                string res = mygrep.search(inp);
                if (res == " ") {
                    continue;
                }
                cout << res << endl;
            }
        }
    } catch (...){}
    return 0;
}
