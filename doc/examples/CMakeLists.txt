add_executable(video_watermark watermark.cpp)
target_link_libraries(video_watermark avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

add_executable(decode_media decode_media.cpp)
target_link_libraries(decode_media avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

add_executable(scale_video scale_video.cpp)
target_link_libraries(scale_video avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

add_executable(read_metadata read_metadata.cpp)
target_link_libraries(read_metadata avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

add_executable(video_in_video video_in_video.cpp)
target_link_libraries(video_in_video avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

#add_executable(example_muxer example_muxer.cpp)
#target_link_libraries(example_muxer avformat avdevice avfilter avcodec  avutil swscale swresample jmedia Winmm)

set_target_properties(
	video_watermark
	decode_media
	scale_video
	read_metadata
	video_in_video
  #example_muxer
	PROPERTIES
	FOLDER
	"jmedia/example"
)


add_custom_command(
	TARGET
	video_watermark
	decode_media
	scale_video
	read_metadata
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/avcodec-57.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/avdevice-57.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/avfilter-6.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/avformat-57.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/avutil-55.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/postproc-54.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/swresample-2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_BINS_DIR}/swscale-4.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/logo.png ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/test.jpg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_PATH}/doc/video.mov ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
	VERBATIM	
)

#decode
add_test(
	test_decode_picture
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/decode_media.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/test.jpg
)
add_test(
	test_decode_video
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/decode_media.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/video.mov
)
add_test(
	test_decode_badfile
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/decode_media.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/avdevice-57.dll
)
set_tests_properties(
	test_decode_badfile
	PROPERTIES
	WILL_FAIL true
)

#watermask
add_test(
	test_jpg_add_watermark
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/video_watermark.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/test.jpg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/logo.png
)
add_test(
	test_video_add_watermark
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/video_watermark.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/video.mov ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/logo.png
)

#scale
add_test(
	test_video_scale_picture
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scale_video.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/test.jpg 500 700 yuv
)
add_test(
	test_scale_picture
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scale_video.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/test.jpg 555 333 bmp
)
add_test(
	test_scale_picture2
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scale_video.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/test.jpg 555 333 yuv
)
add_test(
	test_scale_video
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scale_video.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/video.mov 500 700 yuv
)
