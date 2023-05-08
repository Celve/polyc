cmake_minimum_required(VERSION 3.26)
project(poly-compiler)

set(CMAKE_CXX_FLAGS "-g -lgmp -lcloog-isl -lclan -losl -DCLOOG_INT_GMP")

add_executable(poc src/main.cpp src/utils/osl_int.cpp src/utils/osl_statement.cpp src/utils/str.cpp src/matrix/matrix.cpp src/matrix/scattering.cpp)
target_include_directories(poc PUBLIC src/include)

