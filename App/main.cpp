// #include "modules/ReceiveData.hpp"
// #include "modules/DataProcessing.hpp"
// #include <iostream>
// #include <string>
// #include <vector>

// int main() {
//     // Example with live data from API
//     std::string url = "https://fakestoreapi.com/products";
//     ReceiveData receiver(url);
    
//     try {
//         // Fetch the data from the API
//         std::string jsonData = receiver.SendRquestAndHandleIt();
        
//         // Process the received JSON data
//         DataProcessing processor(jsonData);
//         std::vector<Product> products = processor.parseJsonData();
        
//         // Print the results
//         std::cout << "\nSuccessfully processed " << processor.getProductCount() << " products:\n\n";
        
//         for (const auto& product : products) {
//             std::cout << "ID: " << product.id << "\n"
//                      << "Title: " << product.title << "\n"
//                      << "Price: $" << product.price << "\n"
//                      << "Category: " << product.category << "\n"
//                      << "------------------------\n";
//         }
        
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }
    
//     return 0;
// }

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;
using namespace std;

int main() {
    // Create some JSON items (like products)
    vector<json> products = {
        {
            {"ID", 1},
            {"Title", "Mens Casual Premium Slim Fit T-Shirts"},
            {"Price", 22.3},
            {"Category", "men's clothing"}
        },
        {
            {"ID", 2},
            {"Title", "Women's Casual Top"},
            {"Price", 18.5},
            {"Category", "women's clothing"}
        },
        {
            {"ID", 3},
            {"Title", "Smartphone 128GB"},
            {"Price", 499.99},
            {"Category", "electronics"}
        }
    };

    // Open CSV file for writing
    ofstream file("products.csv");
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing.\n";
        return 1;
    }

    // Write CSV header
    file << "ID,Title,Price,Category\n";

    // Write each JSON item as a CSV row
    for (const auto& item : products) {
        file << item["ID"] << ","
             << "\"" << item["Title"] << "\"" << ","  // quote to handle commas
             << item["Price"] << ","
             << "\"" << item["Category"] << "\"" << "\n";
    }

    file.close();
    cout << "CSV file 'products.csv' created successfully.\n";
    return 0;
}
