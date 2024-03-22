#include "sys/print.hpp"

extern "C" void kernel_main(int argc, const char **argv) {
  hls::kprintln("Into kernel S-MODE!");
}