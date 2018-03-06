cmake_minimum_required(VERSION 2.8.7)
project (HybridControllBoard)


enable_language(C)
enable_language(CXX)


add_subdirectory(src)
add_subdirectory(tests)
add_library(main main.c)
