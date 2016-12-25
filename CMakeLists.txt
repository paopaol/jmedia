cmake_minimum_required(VERSION 3.6)
project(decode_audio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)

set(SOURCE_FILES
        decode_audio.c.cpp
        LibAVDecoder.cpp
        LibAVDecoder.h
        jmedia/Reader.cpp
        jmedia/Reader.h
        jmedia/Decoder.cpp
        jmedia/Decoder.h
        jmedia/Error.h jmedia/Filter/FilterGraph.cpp jmedia/Filter/FilterGraph.h jmedia/Filter/FilterConfig.h jmedia/Filter/FilterConfig_abuffer.cpp jmedia/Filter/FilterConfig_abuffer.h)
add_executable(decode_audio ${SOURCE_FILES})
target_link_libraries(decode_audio avformat avcodec avutil avdevice avfilter avresample pthread)
