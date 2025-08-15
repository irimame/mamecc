#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "The number of arguments must be 2." << std::endl;
        return 1;
    }

    std::cout << ".intel_syntax noprefix" << std::endl;
    std::cout << ".globl main" << std::endl;
    std::cout << "main: " << std::endl;
    std::cout << "  mov rax, " << argv[1] << std::endl;
    std::cout << "  ret" << std::endl;
    return 0;
}