cmake_minimum_required(VERSION 3.6)
project(GetMove)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        include/Config.h
        include/Database.h
        include/Lcm.h
        include/OccurenceDeriver.h
        src/Lcm/Config.cpp
        src/Lcm/Database.cpp
        src/Lcm/Lcm.cpp
        src/Lcm/main.cpp
        src/Lcm/OccurenceDeriver.cpp)

add_executable(GetMove ${SOURCE_FILES})
add_executable(GetMax src/getmax/getmax.cpp)
add_executable(GetFullJson src/generateJson/generateFullJson.cpp)
add_executable(Generator src/generator/generator.cpp)