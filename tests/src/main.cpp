#include <aal/bars.h>
#include <iostream>

#include "utils/file_util.h"

int main(int argc, char** argv) {
  std::cout << "[c++] Init Testing" << std::endl;
  const auto data = file::util::ReadAllBytes(argv[1]);
  aal::AudioResources bars{{data.data(), data.size()}};
  std::cout << bars.GetResource(argv[2]).value().name << std::endl;
}