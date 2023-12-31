#include "csv.hpp"
#include <sstream>
#include <queue>
#include <tuple>

namespace
{

    bool isCorrectSymbForVarName(char c)
    {
        return std::isalpha(c) || std::isdigit(c) || c == '_';
    }

    std::string getVarName(const std::string &str, int &pos)
    {
        std::string name = "";
        while (isCorrectSymbForVarName(str[pos]))
        {
            name += str[pos++];
        }
        --pos;
        return name;
    }

    int getNumber(const std::string &str, int &pos)
    {
        auto substr = str.substr(pos, str.size() - pos);
        size_t idx;
        auto num = std::stoi(substr, &idx);
        pos += idx - 1;
        return num;
    }

    Node *getLexem(const std::string &str, int &pos)
    {
        while (pos < str.size() && std::isspace(str[pos]))
        {
            ++pos;
        }
        if (pos == str.size())
        {
            auto res = new End();
            return res;
        }
        if (std::isalpha(str[pos]))
        {
            auto res = new Variable(getVarName(str, pos));
            return res;
        }
        if (std::isdigit(str[pos]))
        {
            auto res = new Number(getNumber(str, pos));
            return res;
        }

        switch (str[pos])
        {
        case '+':
        {
            auto res = new BinOp{BinOp_t::ADD};
            return res;
        }
        case '-':
        {
            auto res = new BinOp{BinOp_t::SUB};
            return res;
        }
        case '*':
        {
            auto res = new BinOp{BinOp_t::MUL};
            return res;
        }
        case '/':
        {
            auto res = new BinOp{BinOp_t::DIV};
            return res;
        }
        case '=':
        {
            auto res = new BinOp{BinOp_t::ASSIGN};
            return res;
        }
        default:
            throw std::invalid_argument(std::string{"getLexem: unknown symbol: \'"} + std::string{str[pos]} + "\'");
        }
        return nullptr;
    }

    std::vector<Node *> lexer(const std::string &str)
    {
        std::vector<Node *> lexems;
        for (int cur_pos = 0; cur_pos <= str.size(); ++cur_pos)
        {
            Node *lexem = getLexem(str, cur_pos);
            if (lexem == nullptr)
            {
                throw std::invalid_argument(std::string{"lexer: unknown lexem in ["} + str + "]");
            }
            lexems.push_back(lexem);
        }
        return lexems;
    }

    // get unique variable names in the semantic
    void getVariablesFromExpression(const Node *top, std::set<std::string> &variables)
    {

        if (top == nullptr)
            return;

        switch (top->getType())
        {
        case Node_t::BINOP:
        {
            auto op = static_cast<const BinOp *>(top);
            getVariablesFromExpression(op->getLhs(), variables);
            getVariablesFromExpression(op->getRhs(), variables);
            return;
        }
        case Node_t::EXPR:
        {
            auto expr = static_cast<const Expression *>(top);
            getVariablesFromExpression(expr->getTop(), variables);
            return;
        }
        case Node_t::VARNAME:
        {
            auto var = static_cast<const Variable *>(top);
            variables.insert(var->getName());
            return;
        }
        case Node_t::NUM:
            return;
        case Node_t::END:
            return;
        default:
            throw std::invalid_argument("getVariablesFromExpression: unknown type of node");
        }
    }

} // anonymus namespace

void CSV::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string word;
    std::getline(ss, word, ','); // skip first empty column
    while (std::getline(ss, word, ','))
    {
        columns[word] = std::vector<Node *>();
        col_names.push_back(word);
    }
    // content
    int row_count = 0;
    while (std::getline(file, line))
    {
        ss = std::stringstream(line);
        word.clear();
        std::getline(ss, word, ',');
        if (word.empty())
        {
            throw std::invalid_argument("CSV::load_from_file: empty row names are not supported");
        }
        rows[word] = row_count++;
        row_names.push_back(word);
        for (auto &&key : col_names)
        {
            word.clear();
            std::getline(ss, word, ',');
            try
            {
                auto lexems = lexer(word);
                columns[key].push_back(new Expression(lexems.begin()));
            }
            catch (std::exception &e)
            {
                throw std::runtime_error(std::string{} + e.what() + " in cell " + key + row_names[row_names.size() - 1]);
            }
        }
    }
}

// calculate all the cells in csv table, throws CalculationFail exception, if there are some unsolvable cells
void CSV::process()
{
    VarValues values;

    for (auto &&col : col_names)
    {
        for (auto &&row : row_names)
        {
            values[col + row] = nullptr;
        }
    }

    // check cycles in dependecies
    auto graph = makeDependencyGraph();
    if (graph.isCyclic())
    {
        throw CalculationFail("CSV::process: There is an infinity recursion in table");
    }

    // process all the cells
    std::queue<std::tuple<std::string, std::string, Node *>> queue; // element = {col_name, row_name, ptr to expression}
    for (auto &&col : col_names)
    {
        for (auto &&row : row_names)
        {
            queue.push({col, row, columns[col][rows[row]]});
        }
    }

    while (!queue.empty())
    {
        auto tuple = queue.front();
        queue.pop();
        std::string col, row;
        Node *node;
        std::tie(col, row, node) = tuple;

        if (node->getType() != Node_t::EXPR)
        {
            throw CalculationFail("CSV::process: non EXPR type in queue");
        }
        auto expr = static_cast<Expression *>(node);
        int res = 0;
        try
        {
            res = expr->calculate(values);
            delete expr;
            auto number = new Number(res);
            values[col + row] = number;
            columns[col][rows[row]] = number;
        }
        catch (UnknownValue &e)
        {
            queue.push({col, row, node});
        }
        catch (std::exception &e)
        {
            throw std::runtime_error(std::string{} + e.what() + " in cell " + col + row);
        }
    }

    is_complete = true; // now CSV contains only numbers
}

DirectedGraph<std::string> CSV::makeDependencyGraph()
{
    DirectedGraph<std::string> graph;
    for (auto &&col : col_names)
    {
        for (auto &&row : row_names)
        {
            std::set<std::string> dependencies;
            getVariablesFromExpression(columns[col][rows[row]], dependencies);
            for (auto &&d : dependencies)
            {
                graph.addEdge(col + row, d);
            }
        }
    }
    return graph;
}

void CSV::save(const std::string &filename)
{
    std::ofstream file(filename);
    save(file);
}

void CSV::save(std::ostream &os)
{
    if (!is_complete)
    {
        throw std::runtime_error("CSV::save: cannot save incomplete CSV, make sure CSV::process was invoked");
    }
    for (auto &&col : col_names)
    {
        os << "," << col;
    }
    os << std::endl;
    for (auto &&row : row_names)
    {
        os << row;
        for (auto &&col : col_names)
        {
            auto num = static_cast<Number *>(columns[col][rows[row]]);
            os << "," << num->getNum();
        }
        os << std::endl;
    }
}

CSV::~CSV()
{
    for (auto &&kv : columns)
    {
        auto vec = kv.second;
        for (auto &&node : vec)
        {
            delete node;
        }
    }
}
