cmake_minimum_required(VERSION 3.15)
project(PatternStreams CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(PatternStreams
        src/PatternStreams.cpp
        src/PatternStreamsPrivate.h src/PatternStreamsPrivate.cpp)
target_include_directories(PatternStreams PUBLIC include)

set_target_properties(PatternStreams PROPERTIES PUBLIC_HEADER include/PatternStreams.h)
install(TARGETS PatternStreams)
