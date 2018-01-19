add_executable(jmedia_test reader_test.cpp)
target_link_libraries(jmedia_test 
	avformat 
	avdevice 
	avfilter 
	avcodec  
	avutil 
	swscale 
	swresample 
	jmedia 
	gtest
)

set_target_properties(jmedia_test PROPERTIES FOLDER "test")


add_executable(
	video_filter_chan
	video_filter_chan.cpp
	)
target_link_libraries(video_filter_chan
	avformat 
	avdevice 
	avfilter 
	avcodec  
	avutil 
	swscale 
	swresample 
	jmedia 
	gtest
)
set_target_properties(video_filter_chan PROPERTIES FOLDER "test")

add_executable(video_filter_ascii_test video_filter_ascii_test.cpp)
target_link_libraries(video_filter_ascii_test 
	avformat 
	avdevice 
	avfilter 
	avcodec  
	avutil 
	swscale 
	swresample 
	jmedia 
	gtest
)

set_target_properties(video_filter_ascii_test PROPERTIES FOLDER "test")


add_custom_command(
	TARGET
	video_filter_chan
	jmedia_test
	video_filter_ascii_test
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/avcodec-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/avdevice-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/avfilter-6.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/avformat-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/avutil-55.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/postproc-54.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/swresample-2.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/thrd/ffmpeg/bin/swscale-4.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/logo.png ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/test.jpg ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/video.mov ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	VERBATIM	
)

include(GoogleTest)
gtest_add_tests(
	TARGET      jmedia_test video_filter_ascii_test
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
)
gtest_add_tests(
	TARGET      video_filter_ascii_test
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
)

