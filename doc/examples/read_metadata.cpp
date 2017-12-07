#include <format/format_reader.h>

#include <memory>
#include <map>
#include <functional>



int main(int argc, char *argv[])
{
	av_register_all();
	avformat_network_init();

	if (argc != 2) {
		puts("useage:read_meta.exe inputfile");
		return 1;
	}
	std::string fileName = argv[1];

	JMedia::FormatReader file(fileName);

	int error = file.open();
	if (error < 0) {
		puts(file.errors());
		return 1;
	}
	std::shared_ptr<void> deferFileClose(nullptr, std::bind(&JMedia::FormatReader::close, &file));

	std::map<string, string> metadata = file.metadata();
	printf("filename:%-20s\n\n", fileName.c_str());
	for (auto tag = metadata.begin(); tag != metadata.end(); tag++) {
		printf("%-20s:%-20s\n", tag->first.c_str(), tag->second.c_str());
	}

	return 0;
}