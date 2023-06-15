cmake_minimum_required(VERSION 3.0.0)
project(csv_manager VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(csv_manager 
    main.cpp 
    src/csv.cpp
    src/expression.cpp)
target_include_directories(csv_manager PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
