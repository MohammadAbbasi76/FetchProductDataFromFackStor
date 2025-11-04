#include "ReceiveData.hpp"
#include <iostream>
#include <string>
#include <vector>
int main() {

  std::string url = "https://fakestoreapi.com/products";
  ReceiveData receiver(url);
  int result = receiver.SendRquestAndHandleIt();
  return 0;
}