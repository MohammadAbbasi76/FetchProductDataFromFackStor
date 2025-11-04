#include "DataProcessing.hpp"
#include <stdexcept>
#include <iostream>

size_t DataProcessing::getProductCount() const {
    return Products.size();
}

std::vector<Product> DataProcessing::parseJsonData() {
    Products.clear(); 
    try {
        json jsonData = json::parse(JsonString);
        if (!jsonData.is_array()) {
            throw std::runtime_error("JSON data must be an array of products");
        }
        size_t productCount = jsonData.size();
        Products.reserve(productCount);

        for (const auto &item : jsonData) {
            Product product{};  

            if (!item.contains("id") || !item["id"].is_number()) {
                std::cerr << "Warning: Skipping product with invalid or missing ID\n";
                continue;
            }
            if (!item.contains("title") || !item["title"].is_string()) {
                std::cerr << "Warning: Skipping product with invalid or missing title\n";
                continue;
            }
            if (!item.contains("price") || !item["price"].is_number()) {
                std::cerr << "Warning: Skipping product with invalid or missing price\n";
                continue;
            }

            product.id = item["id"].get<int>();
            product.title = item["title"].get<std::string>();
            product.price = item["price"].get<double>();
            product.category = item.value("category", "Uncategorized");
            Products.push_back(product);
        }

    } catch (const json::parse_error &e) {
        throw std::runtime_error("Failed to parse JSON data: " + std::string(e.what()));
    } catch (const std::exception &e) {
        throw std::runtime_error("Error processing JSON data: " + std::string(e.what()));
    }

    return Products;
}
