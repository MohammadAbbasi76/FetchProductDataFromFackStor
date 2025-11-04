#include "ReceiveData.hpp"
#include <stdexcept>

size_t ReceiveData::WriteCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp) {
  size_t totalSize = size * nmemb;
  if (userp) {
    auto output = static_cast<std::string *>(userp);
    output->append(static_cast<char *>(contents), totalSize);
  }
  return totalSize;
}

// Print CURL error nicely
void ReceiveData::PrintCurlError(CURLcode code) {
  std::cerr << "CURL error: " << curl_easy_strerror(code) << std::endl;
}

// Helper: Sleep before retry
void ReceiveData::WaitBeforeRetry(int attempt) {
  int delay = 1000 * attempt; // Exponential backoff (1s, 2s, 3s…)
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

std::string ReceiveData::SendRquestAndHandleIt() {

  const int maxRetries = 3;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialize CURL." << std::endl;
    throw std::runtime_error("Failed to initialize CURL");
  }

  std::string response;
  CURLcode res;

  // Configure CURL
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

  int attempt = 0;
  bool success = false;

  while (attempt < maxRetries && !success) {
    attempt++;
    std::cout << "Attempt " << attempt << " to fetch data..." << std::endl;

    response.clear();
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      PrintCurlError(res);

      switch (res) {
      case CURLE_COULDNT_RESOLVE_HOST:
      case CURLE_COULDNT_CONNECT:
      case CURLE_OPERATION_TIMEDOUT:
        std::cerr << "Connection error. Retrying...\n";
        WaitBeforeRetry(attempt);
        continue;

      case CURLE_PEER_FAILED_VERIFICATION:
      case CURLE_SSL_CONNECT_ERROR:
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        throw std::runtime_error("SSL/TLS error. Please verify certificates and system time.");

      default:
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        throw std::runtime_error("Unhandled CURL error: " + std::string(curl_easy_strerror(res)));
      }
    }

    // HTTP-level handling
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    if (httpCode == 200) {
      std::cout << "HTTP 200 OK\n";
      success = true;
    } else if (httpCode == 404) {
      std::cerr << "Error 404: Endpoint not found.\n";
      break;
    } else if (httpCode == 500) {
      std::cerr << "Server error (500). Retrying...\n";
      WaitBeforeRetry(attempt);
      continue;
    } else if (httpCode == 403) {
      std::cerr << "Access denied (403). Check API permissions.\n";
      break;
    } else {
      std::cerr << "Unexpected HTTP code: " << httpCode << std::endl;
      break;
    }
  }

  if (!success) {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    throw std::runtime_error("Failed to fetch data after " + std::to_string(maxRetries) + " attempts");
  }

  // Clean up CURL
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  
  // Return the JSON response
  return response;
}
