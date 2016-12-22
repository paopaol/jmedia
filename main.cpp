#include <iostream>

using  namespace std;

extern "C" {
#include "libavformat/avformat.h"
}


int main(int argc, char *argv[]) {
    int                 error;


    av_register_all();
    return 0;
}