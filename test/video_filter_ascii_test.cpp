#include <gtest/gtest.h>
#include <filter/video/filter_ascii_chan.h>
#include <filter/video/filter_ascii_graph.h>
#include <format/format_reader.h>


TEST(VideoFilterAsciiChan, rest)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.reset();
	EXPECT_EQ(std::string(""), chan.string());
}

TEST(VideoFilterAsciiChan, scale)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.scale(1920, 1080);
	EXPECT_EQ(std::string("scale=w=1920:h=1080"), chan.string());
}

TEST(VideoFilterAsciiChan, zoomX)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.zoomX(3, 3);
	EXPECT_EQ(std::string("scale=w=3*iw:h=3*ih"), chan.string());
}

TEST(VideoFilterAsciiChan, reduceX)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.reduceX(3, 3);
	EXPECT_EQ(std::string("scale=w=iw/3:h=ih/3"), chan.string());
}

TEST(VideoFilterAsciiChan, movie)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.movie("1.mp4");
	EXPECT_EQ(std::string("movie=\\'1.mp4\\'"), chan.string());
}

TEST(VideoFilterAsciiChan, overlay)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.overlay(4, 4);
	EXPECT_EQ(std::string("overlay=x=4:y=4"), chan.string());
}

TEST(VideoFilterAsciiChan, overlay_top_left)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.overlay_top_left(5, 5);
	EXPECT_EQ(std::string("overlay=x=5:y=5"), chan.string());
}

TEST(VideoFilterAsciiChan, overlay_top_right)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.overlay_top_right(5, 5);
	EXPECT_EQ(std::string("overlay=x=main_w-overlay_w-5:y=5"), chan.string());
}
TEST(VideoFilterAsciiChan, overlay_bottom_left)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.overlay_bottom_left(5, 5);
	EXPECT_EQ(std::string("overlay=x=5:y=main_h-overlay_h-5"), chan.string());
}

TEST(VideoFilterAsciiChan, overlay_bottom_right)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.overlay_bottom_right(5, 5);
	EXPECT_EQ(std::string("overlay=x=main_w-overlay_w-5:y=main_h-overlay_h-5"), chan.string());
}

TEST(VideoFilterAsciiChan, long_filter_chan)
{
	JMedia::VideoFilterAsciiChan chan;
	chan.in("in").in("wm").overlay_bottom_right(10, 10).scale(1024, 768).output("out");
	EXPECT_EQ(std::string("[in] [wm] overlay=x=main_w-overlay_w-10:y=main_h-overlay_h-10,scale=w=1024:h=768 [out] "), chan.string());
}


GTEST_API_ int main(int argc, char **argv)
{
	av_register_all();
    avformat_network_init();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}