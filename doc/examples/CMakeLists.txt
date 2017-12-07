add_executable(video_watermark watermark.cpp)
target_link_libraries(video_watermark avformat avdevice avfilter avcodec  avutil swscale swresample jmedia)

add_executable(decode_media decode_media.cpp)
target_link_libraries(decode_media avformat avdevice avfilter avcodec  avutil swscale swresample jmedia)

add_executable(scale_video scale_video.cpp)
target_link_libraries(scale_video avformat avdevice avfilter avcodec  avutil swscale swresample jmedia)

set_target_properties(
	video_watermark
	decode_media
	scale_video
	PROPERTIES
	FOLDER
	"example"
)


add_custom_command(
	TARGET
	video_watermark
	decode_media
	scale_video
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

#decode
add_test(
	test_decode_picture
	decode_media.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test.jpg
)
add_test(
	test_decode_video
	decode_media.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/video.mov
)
add_test(
	test_decode_badfile
	decode_media.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/avdevice-57.dll
)
set_tests_properties(
	test_decode_badfile
	PROPERTIES
	WILL_FAIL true
)

#watermask
add_test(
	test_jpg_add_watermark
	video_watermark.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test.jpg ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/logo.png
)
add_test(
	test_video_add_watermark
	video_watermark.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/video.mov ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/logo.png
)

#scale
add_test(
	test_video_scale_picture
	scale_video.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test.jpg 500 700 yuv
)
add_test(
	test_scale_picture
	scale_video.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test.jpg 555 333 bmp
)
add_test(
	test_scale_picture2
	scale_video.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test.jpg 555 333 yuv
)
add_test(
	test_scale_video
	scale_video.exe ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/video.mov 500 700 yuv
)