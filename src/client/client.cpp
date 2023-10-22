#include <iostream>
#include <share-a-keyb/networking.hpp>

int main() {
  init_networking();
  std::cout << "Hello, I'm a client!\n";
  return 0;
}