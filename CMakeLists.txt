cmake_minimum_required(VERSION 3.6)
project(jmedia)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})


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
  )



set(source_base
  jmedia/Error.h
  jmedia/base.h
  )
source_group(base FILES ${source_base})

set(
  source_format
  jmedia/format/format_reader.cpp
  jmedia/format/format_reader.h
  jmedia/format/reader.h
  #jmedia/format/format_writer.h
  #jmedia/format/format_writer.cpp

  )
source_group(format FILES ${source_format})

set(
  source_codec
  jmedia/codec/decoder.cpp
  jmedia/codec/decoder.h
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
target_link_libraries(jmedia avformat avdevice avfilter avcodec  avutil swscale swresample)

if (NOT DISABLE_JMEDIA_TEST)
  add_subdirectory(test)
endif()
if (BUILD_JMEDIA_EXAMPLE)
  add_subdirectory(doc/examples)
endif()
