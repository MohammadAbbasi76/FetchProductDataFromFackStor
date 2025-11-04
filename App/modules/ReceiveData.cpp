#include "ReceiveData.hpp"
#include "DataProcessing.hpp"

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

int ReceiveData::SendRquestAndHandleIt() {

  const int maxRetries = 3;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Failed to initialize CURL." << std::endl;
    return 1;
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
        std::cerr << "SSL/TLS error. Please verify certificates and system "
                     "time.\n";
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 2;

      default:
        std::cerr << "Unhandled CURL error.\n";
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 3;
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

  // Parsing and Logic Error Handling
  if (success) {
    //   try {
    //     auto jsonData = json::parse(response);

    //     std::cout << "\nProduct list:\n";
    //     for (const auto &item : jsonData) {
    //       std::cout << " - " << item["title"] << " ($" << item["price"] <<
    //       ")\n";
    //     }
    //   } catch (const std::exception &e) {
    //     std::cerr << "JSON parsing failed: " << e.what() << std::endl;
    //   }

    DataProcessing processor(response);
    auto products = processor.parseJsonData();

  } else {
    std::cerr << "Failed to fetch data after " << maxRetries << " attempts.\n";
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
