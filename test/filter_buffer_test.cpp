#include <filter/filter_buffer.h>
#include <filter/filter_buffersink.h>
#include <filter/filter_graph.h>


int main(int argc, char *argv[])
{
	JMedia::FilterGraph graph;

	JMedia::FilterBuffer buffer(&graph, "in");
	JMedia::FilterBuffersink buffersink(&graph, "out");

}