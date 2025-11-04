#pragma once
#include <chrono>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

class ReceiveData {

public:
  ReceiveData(std::string urlAddress) : url(urlAddress){};
  ~ReceiveData() = default;
  int SendRquestAndHandleIt();

private:
  const std::string url;
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
  void PrintCurlError(CURLcode code);
  void WaitBeforeRetry(int attempt);
};