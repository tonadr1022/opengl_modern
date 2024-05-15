#include <cstdint>
#include <iostream>

int main() {
  // uint8_t bytes[] = {96, 246, 58, 173, 146, 97, 0, 0};  // Bytes from 0 to 7
  uint8_t bytes[] = {52, 10, 0, 0, 1, 0, 0, 0};  // Bytes from 0 to 7
  uint64_t result = 0;

  // Concatenate the bytes into a 64-bit integer
  for (int i = 7; i >= 0; --i) {
    result = (result << 8) | bytes[i];
  }

  std::cout << "Result: " << result << std::endl;

  return 0;
}
