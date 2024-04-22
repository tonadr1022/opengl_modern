#include <glfw/glfw3.h>

#include <iostream>
#include <test.hpp>

#include "test/bar.hpp"

int main() {
  std::cout << "hello\n";
  Test::foo();
  Bar::bar();
  return 0;
}
