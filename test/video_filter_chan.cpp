#include <filter/video/filter_ascii_chan.h>
#include <filter/video/filter_ascii_graph.h>



int main(int argc, char *argv[])
{
	JMedia::VideoFilterAsciiChan chan;

	auto chans = chan.in("temp").scale(100, 100).output("out").string();
	puts(chans.c_str());
	chan.reset();

	chans = chan.in("temp").output("out").string();
	puts(chans.c_str());
	chan.reset();

	chans = chan.zoomX(2, 2).string();
	puts(chans.c_str());
	chan.reset();

	chans = chan.zoomX(2, 2).output("out").string();
	puts(chans.c_str());
	chan.reset();

	chans = chan.reduceX(2, 2).output("out").string();
	puts(chans.c_str());
	chan.reset();

	chans = chan.movie("123.mp4").string();
	puts(chans.c_str());
	chan.reset();


	JMedia::VideoFilterAsciiGraph graph;

	graph.push_chan(
		chan.reset().movie("1.mp4").scale(180, -1).output("over")
	);
	auto graphs = graph.string();
	puts(graphs.c_str());

	puts("\n\n");

	graph.reset().push_chan(
		chan.reset().movie("1.mp4").output("wm")
	).push_chan(
		chan.reset().in("in").in("wm").overlay(5, 5).reduceX(5, 5).output("out")
	);
	graphs = graph.string();
	puts(graphs.c_str());
	return 0;
}