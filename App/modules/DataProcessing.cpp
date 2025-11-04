#include "DataProcessing.hpp"

std::vector<Product> DataProcessing::parseJsonData() {
  std::vector<Product> products;
  try {
    json jsonData = json::parse(&jsonString);

    // Check if the JSON is an array
    if (jsonData.is_array()) {
      for (const auto &item : jsonData) {
        Product product;

        // Extract product information with validation
        if (item.contains("id") && item["id"].is_number()) {
          product.id = item["id"];
        }

        if (item.contains("title") && item["title"].is_string()) {
          product.title = item["title"];
        }

        if (item.contains("price") && item["price"].is_number()) {
          product.price = item["price"];
        }

        if (item.contains("category") && item["category"].is_string()) {
          product.category = item["category"];
        }

        products.push_back(product);
      }
    }
  } catch (const json::parse_error &e) {
    // Handle JSON parsing errors
    throw std::runtime_error("Failed to parse JSON data: " +
                             std::string(e.what()));
  }

  return products;
}