#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "expression.hpp"
#include "graph.hpp"

class CalculationFail: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class CSV
{
    bool is_complete = false;
    std::vector<std::string> col_names;
    std::vector<std::string> row_names;
    std::unordered_map<std::string, std::vector<Node*>> columns; // name of column: column values
    std::unordered_map<std::string, int> rows; // name of row: idx of element in columns
public:
    CSV() {}
    CSV(const std::string& filename) { load_from_file(filename); }
    void load_from_file(const std::string& filename);
    void process();
    void save(const std::string& filename);
    void save(std::ostream& os);
private:
    DirectedGraph<std::string> makeDependencyGraph();
};