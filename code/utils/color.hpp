#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>

/**
 * necessario para colocar cores nas mensagens de maneira fácil
 *
 * cada função retorna uma tag de cor do shell, exemplo:
 * tenho a frase "o dia está lindo", gostaria que essa frase fosse vermelha
 * eu teria que escrever "\033[31mo dia está lindo"
 *
 * "\033[31m" é uma tag de cor do shell
 *
 * crio uma flag para retorna a saida de uma respectiva cor
 */
namespace color
{
    inline std::ostream &red(std::ostream &os) { return os << "\033[31m"; }
    inline std::ostream &green(std::ostream &os) { return os << "\033[32m"; }
    inline std::ostream &yellow(std::ostream &os) { return os << "\033[33m"; }
    inline std::ostream &blue(std::ostream &os) { return os << "\033[34m"; }
    inline std::ostream &reset(std::ostream &os) { return os << "\033[0m"; }
}

#endif
