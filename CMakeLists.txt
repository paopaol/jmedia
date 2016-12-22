cmake_minimum_required(VERSION 3.6)
project(decode_audio)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)

set(SOURCE_FILES LibAVInputFileAudio.cpp LibAVInputFileAudio.h LibAVOutputFileAudio.cpp LibAVOutputFileAudio.h TransCodeAAC.cpp TransCodeAAC.h decode_audio.c.cpp LibAVDecoder.cpp LibAVDecoder.h)
add_executable(decode_audio ${SOURCE_FILES})
target_link_libraries(decode_audio avformat avcodec avutil avdevice avfilter avresample pthread)
