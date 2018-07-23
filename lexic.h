#ifndef LEXIC_H_INCLUDED
#define LEXIC_H_INCLUDED
/*!
*\file
*\brief Заголовочный файл лексического анализатора
*
* Данный файл содержит в себе определения структуры лексем и класса лексического анализатора
*/


#include <iostream>

#include <vector>
#include <string>

using namespace std;

/// Класс лексем
class Lexem
{
    friend class Mygrep;
    friend class Syntax;
    friend class Lex;
    friend class Syn_lexem;

    string terminal = "";   ///< Терминальная цепочка
    int param1 = 0;         ///< Первый параметр лексемы
    int param2 = 0;         ///< Второй параметр лексемы

    ///Возможные типы лексем
    enum lexem_types
    {
        OPEN_BRACE,     ///< Открывающая скобка
        CLOSE_BRACE,    ///< Закрывающая скобка
        ITER,           ///< Итерация
        ITER_N_M,       ///< Итерация от n до m раз
        LAZY_ITER,      ///< Ленивая итерация
        TERMINAL,       ///< Терминальная цепочка
        OR,             ///< Перечисление
        END             ///< Признак конца
    };

    lexem_types type;       ///< Тип лексемы

    /*!
    * Конструктор для нетерминальной лексемы
    * \param[in] type Тип лексемы. По умолчанию END
    * \param[in] param1 Первый параметр лексемы. По умолчанию 0
    * \param[in] param1 Второй параметр лексемы. По умолчанию 0
    */
    Lexem(lexem_types type = END, int param1 = 0, int param2 = 0) : param1(param1), param2(param2), type(type)
    {
    }

    /*!
    * Конструктор для терминальной лексемы
    * \param[in] terminal Терминальная цепочка
    */
    Lexem(string &terminal) : terminal(terminal), type(TERMINAL)
    {
    }

    ///Функция распечатывает лексемы в исходном виде
    void print();
};

/*! \brief Лексический анализатор
 *
 * Лексический анализатор преобразует исходную строку в последовательность типизированных лексем и проверяет правильность экранирования символов и написания многосимвольных операторов
 */
class Lex
{
    friend class Syntax;
    vector<Lexem> lexems;       ///< Вектор, в который записывается последовательность лексем, эквивалентная исходному регулярному выражению

    static char possible_op[];  ///< служебный массив символов, имеющих значение операций
    unsigned cur = 0;           ///< Текущая позиция в векторе lexems (для функции get_lex())

    /*! \brief Функция, проверяющая является ли символ оператором
     *
     * \param[in] ch Символ для  проверки
     * \return true, если символ является оператором, false — иначе
     *
     */
    bool isoper(char ch);

    /*! \brief Функция, проверяющая является ли символ терминальным
     *
     * \param[in] ch Символ для  проверки
     * \return <b>true</b>, если символ является терминальным, false — иначе
     *
     */
    bool isterm(char ch);

    /*! \brief Конструктор от строки с регулярным выражением.
     *
     * Сделан явным, поскольку неясен смысл выражения вида Lex c = "a*";
     */
    explicit Lex(const string &reg);

    /*! \brief Функция, последовательно возвращающая лексемы
     *
     * \return Возвращает очередную лексему
     *
     */
    Lexem get_lex();
};

#endif // LEXIC_H_INCLUDED
