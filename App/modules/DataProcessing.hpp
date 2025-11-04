#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

struct Product {
  int id;
  std::string title;
  double price;
  std::string category;
};

class DataProcessing {
public:
  DataProcessing(const std::string &jsonStr) : jsonString(jsonStr){};
  ~DataProcessing() = default;

  std::vector<Product> parseJsonData();
  size_t getProductCount() const;

private:
  const std::string jsonString;
  std::vector<Product> products;
};
