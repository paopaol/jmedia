cmake_minimum_required(VERSION 3.6)
project(decode_audio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)

set(SOURCE_FILES
        decode_audio.c.cpp
        LibAVDecoder.cpp
        LibAVDecoder.h
        jmedia/FormatReader.cpp
        jmedia/FormatReader.h
        jmedia/Decoder.cpp
        jmedia/Decoder.h
        jmedia/Error.h
        jmedia/Filter/FilterGraph.cpp
        jmedia/Filter/FilterGraph.h
        jmedia/Filter/FilterConfig.h
        jmedia/Filter/FilterConfig_abuffer.cpp
        jmedia/Filter/FilterConfig_abuffer.h
        jmedia/Filter/FilterConfig_abuffersink.cpp
        jmedia/Filter/FilterConfig_abuffersink.h
        jmedia/Filter/FilterConfig.cpp
        jmedia/Filter/FilterConfig_aformat.cpp
        jmedia/Filter/FilterConfig_aformat.h
        jmedia/base.h
        jmedia/Reader.h
        jmedia/tools/Resampler.cpp
        jmedia/tools/Resampler.h jmedia/FormatWriter.cpp jmedia/FormatWriter.h)
add_executable(decode_audio ${SOURCE_FILES})
target_link_libraries(decode_audio avformat avdevice avfilter avcodec  avutil swscale swresample pthread z dl)
