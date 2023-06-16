#include <iostream>
#include "csv.hpp"


int main(int argc, char* argv[]) 
{
    if (argc < 2) {
        std::cerr << "Expected 1 argument: path to csv file\n";
        return 0;
    } else if (argc > 2) {
        std::cerr << "Too few arguments: expected 1, " + std::to_string(argc - 1) + " provided\n";
        return 0;
    }

    CSV csv;
    try {
        csv.load_from_file(argv[1]);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    std::cout << "Well done\n";

    return 0;
}