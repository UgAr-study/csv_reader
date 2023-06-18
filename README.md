# Build
In project directory:
```bash
$ mkdir build
$ cmake build
$ cmake --build build --target csv_manager
```
# Run
In project directory:
```bash
$ ./build/csv_manager <path to csv file>
```

# Testing
In directory `test` there is a python script for running all the .csv tests in this directory. You can run it from `test` directory:
```bash
$ python3 tester.py
```
# Program description
The main class of this program is `CSV`:
```Cpp
class CSV
{
public:
    CSV() {}
    CSV(const std::string& filename);
    void load_from_file(const std::string& filename);
    void process();
    void save(const std::string& filename);
    void save(std::ostream& os);
};
```
- **CSV::CSV(); CSV::CSV(filename)**  - constructs an empty `CSV` or filled with file content.
- **CSV::load_from_file(filename)** - fills an empty `CSV` with content from file.
- **CSV::process()** - calculates all the `CSV` cells and make it valid for saving.
- **CSV::save(filename); CSV::save(ostream)** - writes `CSV` content to the file or to the ostream directly. *NOTE*: works only after successfull `CSV::process`.

`CSV` can be constructed from file directly, or with help of `CSV::load_from_file` method. After loading from file `CSV` stores each cell as `Expression` to make it possible to calculate the cell's value from expression. This calculation is made in `CSV::process` method, which firstly checks the possibility of making such calculation and only after that computes all the expressions. It is important and convenient for further functionality extension, because the expressions can be modified before invoking the `CSV::process`.

To extent the functionality one can add new types in the enums: `Node_t` or `BinOp_t`, and describe its handling in the following functions: `lexer`, `BuildTree` and `TreeCalculation`.

For example if someone wants to introduce functions, such as MIN, MAX, etc., he can add new type `FUNC` in `Node_t` and create new enum `Func_t` for built-in functions. Then he should write the handling process of new type in `lexer`, `BuildTree` and `TreeCalculation`.