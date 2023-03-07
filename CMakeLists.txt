######################################################################
# @author      : momo
# @file        : CMakeLists
# @created     : Saturday Aug 13, 2022 23:14:15 CET
######################################################################

cmake_minimum_required(VERSION 3.22.2)

project(tsp_ga VERSION 0.1.1 LANGUAGES CXX)
include(cmake/general.cmake)
include(cmake/functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                                Conan                                 #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#include(cmake/conan.cmake)
#conan_cmake_configure(
#    BASIC_SETUP
#    CMAKE_TARGETS
#    # REQUIRES fmt/9.1.0
#    GENERATORS cmake_find_package
#)
#conan_cmake_autodetect(settings)
#conan_cmake_install(
#    PATH_OR_REFERENCE ${CMAKE_BINARY_DIR}
#    GENERATOR cmake_find_package
#    BUILD missing
#    SETTINGS ${settings}
#)
#
#set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
#set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
#
#find_package(fmt REQUIRED)
# find_package(Eigen3 REQUIRED)
# find_package(wg21_linear_algebra REQUIRED)
# find_package(scope-lite REQUIRED)
# find_package(scn REQUIRED)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                                 SPL                                  #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#find_package(spl REQUIRED)
add_library(spl SHARED IMPORTED GLOBAL)
set_target_properties(spl PROPERTIES
        IMPORTED_LOCATION /home/momo/git/spl/build/libspl.a
        INTERFACE_INCLUDE_DIRECTORIES /home/momo/git/spl/include/
        )

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                           Enable warnings                            #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
add_library(project_warnings INTERFACE)
include(cmake/compiler_warnings.cmake)
set_project_warnings(project_warnings)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#                               tsp_ga                                 #
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
add_executable(tsp_ga)
target_sources(tsp_ga PRIVATE src/main.cpp src/city.cpp src/genome.cpp src/population.cpp)
target_compile_features(tsp_ga PUBLIC cxx_std_20)
target_compile_options(tsp_ga PRIVATE -ffast-math)
target_link_libraries(tsp_ga
    PRIVATE
        fmt
        spl
        project_warnings
)
target_include_directories(tsp_ga PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
enable_lto(tsp_ga)
