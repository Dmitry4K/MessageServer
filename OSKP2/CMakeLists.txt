  
cmake_minimum_required(VERSION 3.0)

project(Server)

set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD 17)

add_executable(Server
  Source.cpp
  ServerClass.cpp
)