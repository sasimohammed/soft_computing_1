cmake_minimum_required(VERSION 3.16)
project(GAToolboxAssignment1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# CVRP sources. Add every new .cpp file here.
add_library(cvrp_lib STATIC
    src/cvrp/CVRPProblem.cpp
    src/cvrp/CVRPParser.cpp
    src/cvrp/CVRPDecoder.cpp
    src/cvrp/CVRPEvaluator.cpp
    src/cvrp/CVRPSolver.cpp
    src/cvrp/CVRPPrinter.cpp
)
target_include_directories(cvrp_lib PUBLIC include)
if(MSVC)
    target_compile_options(cvrp_lib PUBLIC /W4)
else()
    target_compile_options(cvrp_lib PUBLIC -Wall -Wextra)
endif()

add_executable(ga_toolbox src/main.cpp)
target_link_libraries(ga_toolbox PRIVATE cvrp_lib)

enable_testing()
foreach(test_name
        chromosome_tests
        selection_tests
        crossover_tests
        mutation_tests
        replacement_tests
        ga_core_tests
        cvrp_tests)
    add_executable(${test_name} tests/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE cvrp_lib)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
