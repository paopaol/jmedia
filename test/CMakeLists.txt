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

add_custom_command(
	TARGET
	filter_buffer_test
	jmedia_test
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/avcodec-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/avdevice-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/avfilter-6.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/avformat-57.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/avutil-55.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/postproc-54.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/swresample-2.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thrd/ffmpeg/bin/swscale-4.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/doc/logo.png ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/doc/test.jpg ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/doc/video.mov ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	VERBATIM	
)

include(GoogleTest)
gtest_add_tests(
	TARGET      jmedia_test
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}
	#TEST_SUFfIX .noArgs
	#TEST_LIST noArgsTests
)