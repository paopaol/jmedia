#include <gtest/gtest.h>
#include <format/format_reader.h>
#include <memory>
#include <functional>
#include <filesystem>
#include <string>
#include <iostream>

#include <Windows.h>

static std::tr2::sys::path current_path;

static std::tr2::sys::path exePath()
{
	char	exe[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, exe, MAX_PATH);

	std::tr2::sys::path p(exe);

	return p.parent_path();
}

TEST(format_reader, open_not_found_file)
{
	JMedia::FormatReader file("not_found.mp4");
	EXPECT_NE(0, file.open());
}

TEST(format_reader, open_picture)
{
	auto path = current_path;
	path.append("logo.png");

	JMedia::FormatReader file(path.string());

	EXPECT_EQ(0, file.open());
	std::shared_ptr<void> deferClose(nullptr, std::bind(&JMedia::FormatReader::close, &file));
}

TEST(format_reader, open_video)
{
	auto path = current_path;
	path.append("video.mov");

	JMedia::FormatReader file(path.string());

	EXPECT_EQ(0, file.open());
	std::shared_ptr<void> deferClose(nullptr, std::bind(&JMedia::FormatReader::close, &file));
}

TEST(format_reader, open_badfile)
{
	auto path = current_path;
	path.append("avutil-55.dll");

	JMedia::FormatReader file(path.string());

	EXPECT_NE(0, file.open());
	std::shared_ptr<void> deferClose(nullptr, std::bind(&JMedia::FormatReader::close, &file));
}

TEST(format_reader, open_empty)
{
	auto path = current_path;
	path.append("avutil-55.dll");

	JMedia::FormatReader file("");

	EXPECT_NE(0, file.open());
	std::shared_ptr<void> deferClose(nullptr, std::bind(&JMedia::FormatReader::close, &file));
}


GTEST_API_ int main(int argc, const char *argv[])
{
	av_register_all();
    avformat_network_init();

	current_path = exePath();

	testing::InitGoogleTest(&argc, (char **)argv);
	return RUN_ALL_TESTS();

}