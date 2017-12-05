add_executable(filter_buffer_test filter_buffer_test.cpp)
target_link_libraries(filter_buffer_test avformat avdevice avfilter avcodec  avutil swscale swresample jmedia)
set_target_properties(filter_buffer_test PROPERTIES FOLDER "test")