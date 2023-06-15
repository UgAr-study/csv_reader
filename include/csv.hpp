#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "expression.hpp"

class CSV
{
    public:
    CSV() {}
    void load_from_file(std::string filename);

    private:
    void process();

    private:
    Expression exp;
};

std::ostream& operator<<(std::ostream & os, const CSV& csv);