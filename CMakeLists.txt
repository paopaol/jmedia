cmake_minimum_required(VERSION 3.6)
project(jmedia)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)


set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE CACHE BOOL "export windows symbols")
#if(NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE Release)
#endif()
#set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ROOT_PATH})

# output dir
if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/BINS)
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/BINS)
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/LIBS)
endif()

set(FFMPEG_LIBS_DIR "" CACHE PATH "ffmpeg lib dir")
set(FFMPEG_INCLUDE_DIR "" CACHE PATH "ffmpeg include dir")
set(FFMPEG_BINS_DIR "" CACHE PATH "ffmpeg bin dir")

set(JMEDIA_DISABLE_JMEDIA_TEST TRUE CACHE BOOL "enable unit test or not")
set(JMEDIA_BUILD_JMEDIA_EXAMPLE FALSE CACHE BOOL "build example or not")
set(JMEDIA_BUILD_INSTALL_TARGETS FALSE CACHE BOOL "install jmedia or not")


## ffmpeg
if (NOT FFMPEG_LIBS_DIR)
  message(FATAL_ERROR "pls set FFMPEG_LIBS_DIR")
endif()
if (NOT FFMPEG_BINS_DIR)
  message(FATAL_ERROR "pls set FFMPEG_BINS_DIR")
endif()
if (NOT FFMPEG_INCLUDE_DIR)
  message(FATAL_ERROR "pls set FFMPEG_INCLUDE_DIR")
endif()


# for test switch
if (NOT DISABLE_JMEDIA_TEST)
  enable_testing()
endif()

# for example switch
if (NOT DISABLE_JMEDIA_EXAMPLE)
  set(BUILD_JMEDIA_EXAMPLE ON)
endif()



#ffmpeg
include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIBS_DIR})





include_directories(
  jmedia
  jmedia/format
  jmedia/codec
  jmedia/resampler
  jmedia/scaler
  jmedia/filter
  jmedia/writer
  )



set(source_base
  jmedia/error.h
  jmedia/base.h
  )
source_group(base FILES ${source_base})

set(
  source_format
  jmedia/format/format_reader.cpp
  jmedia/format/format_reader.h
  jmedia/format/reader.h
  jmedia/format/format_writer.h
  jmedia/format/format_writer.cpp

  )
source_group(format FILES ${source_format})

set(
  source_codec
  jmedia/codec/decoder.cpp
  jmedia/codec/decoder.h
  jmedia/codec/encoder.cpp
  jmedia/codec/encoder.h
  )
source_group(codec FILES ${source_codec})

set(
  source_filter_graph
  jmedia/filter/filter_graph.cpp
  jmedia/filter/filter_graph.h
  jmedia/filter/filter.cpp
  jmedia/filter/filter.h
  jmedia/filter/filter_buffersink.cpp
  jmedia/filter/filter_buffersink.h
  jmedia/filter/filter_buffer.cpp
  jmedia/filter/filter_buffer.h
  )
source_group(filter FILES ${source_filter_graph})

set(
  filters_video
  jmedia/filter/video/filter_ascii_chan.h
  jmedia/filter/video/filter_ascii_chan.cpp

  jmedia/filter/video/filter_ascii_graph.h
  jmedia/filter/video/filter_ascii_graph.cpp

  jmedia/filter/video/filter_ascii_scale.cpp
  jmedia/filter/video/filter_ascii_movie.cpp
  jmedia/filter/video/filter_ascii_overlay.cpp
  jmedia/filter/video/filter_ascii_drawbox.cpp
  jmedia/filter/video/filter_ascii_crop.cpp
  jmedia/filter/video/filter_ascii_drawtext.cpp


  )
source_group(filter\\\\video FILES ${filters_video})

set(
  source_filter
  ${source_filter_graph}
  ${filters_video}
  )



set(
  source_scaler
  jmedia/scaler/scaler.cpp
  jmedia/scaler/scaler.h
  )
source_group(scaler FILES ${source_scaler})

set(
  source_resampler
  jmedia/resampler/resampler.cpp
  jmedia/resampler/resampler.h
  )
source_group(resampler FILES ${source_resampler})



set(SOURCE_FILES
  ${source_base}
  ${source_format}
  ${source_codec}
  ${source_filter}
  ${source_scaler}
  ${source_resampler}
  )





SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

add_library(jmedia ${SOURCE_FILES})
target_compile_definitions(jmedia PUBLIC  "_CRT_SECURE_NO_WARNINGS" "__STDC_LIMIT_MACROS" "__STDC_CONSTANT_MACROS ")
target_link_libraries(jmedia PUBLIC
  avformat avdevice avfilter avcodec  avutil swscale swresample winmm)

set_target_properties(
  jmedia
  PROPERTIES
  FOLDER
  "jmedia"
  )

if (NOT JMEDIA_DISABLE_JMEDIA_TEST)
  add_subdirectory(test)
endif()

if (JMEDIA_BUILD_JMEDIA_EXAMPLE)
  add_subdirectory(doc/examples)
endif()

if (JMEDIA_BUILD_INSTALL_TARGETS)
    if(CMAKE_SIZEOF_VOID_P STREQUAL 4)
        set(platform x86)
    else()
        set(platform x64)
    endif()

    install(TARGETS jmedia
        CONFIGURATIONS RelWithDebInfo
        ARCHIVE DESTINATION lib/static.${platform}.RelWithDebInfo
        LIBRARY DESTINATION lib/${platform}.RelWithDebInfo
        RUNTIME DESTINATION bin/${platform}.RelWithDebInfo)

    install(TARGETS jmedia
        CONFIGURATIONS Debug
        ARCHIVE DESTINATION lib/static.${platform}.Debug
        LIBRARY DESTINATION lib/${platform}.Debug
        RUNTIME DESTINATION bin/${platform}.Debug)

    install(TARGETS jmedia
        CONFIGURATIONS Release
        ARCHIVE DESTINATION lib/static.${platform}.Release
        LIBRARY DESTINATION lib/${platform}.Release
        RUNTIME DESTINATION bin/${platform}.Release)

    install(DIRECTORY jmedia
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
endif()
