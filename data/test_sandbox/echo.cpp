
#include <iostream>
#include <string>

int main()
{

  std::string text;
  auto count = 3;
  while (count-- > 0) {
    std::cin >> text;

    std::cout << count << " \"???" << text << "???\"" << std::endl;
    std::cerr << count << " \"!!!" << text << "!!!\"" << std::endl;
  }
  return 0;
}
