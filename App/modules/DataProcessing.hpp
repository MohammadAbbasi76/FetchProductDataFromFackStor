#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Product {
    int id;
    std::string title;
    double price;
    std::string category;
};

class DataProcessing {
public:
    DataProcessing(const std::string& jsonStr) : jsonString(jsonStr) {};
    ~DataProcessing() = default;

    std::vector<Product> parseJsonData();
    private:
    const std::string jsonString;
};
