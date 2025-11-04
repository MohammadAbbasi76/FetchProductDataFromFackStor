#ifndef FILEOPRATION_HPP
#define FILEOPRATION_HPP

// File: FileOpration.hpp
// Purpose: parse JSON (text or nlohmann::json) and write a CSV file containing extracted items.

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace FileOpration {
    using json = nlohmann::json;

    // Escape a single CSV cell
    inline std::string escapeCsvCell(const std::string& s) {
        bool needQuote = s.find_first_of(",\"\n\r") != std::string::npos;
        std::string out;
        out.reserve(s.size() + 4);
        if (needQuote) {
            out.push_back('"');
            for (char c : s) {
                if (c == '"') out.append("\"\""); else out.push_back(c);
            }
            out.push_back('"');
            return out;
        }
        return s;
    }

    // Try to locate an array of items inside the parsed JSON:
    // - if json is an array -> return it
    // - otherwise try common keys: "items", "data", "results"
    inline const json* locate_items_array(const json& j) {
        if (j.is_array()) return &j;
        static const std::vector<std::string> common_keys = {"items", "data", "results", "rows"};
        for (auto& k : common_keys) {
            if (j.contains(k) && j[k].is_array()) return &j[k];
        }
        return nullptr;
    }

    // Collect header fields (if provided, use them; otherwise union of object keys)
    inline std::vector<std::string> collect_fields(const json& arr, const std::vector<std::string>& provided) {
        if (!provided.empty()) return provided;
        std::set<std::string> setkeys;
        for (const auto& item : arr) {
            if (!item.is_object()) continue;
            for (auto it = item.begin(); it != item.end(); ++it) setkeys.insert(it.key());
        }
        return std::vector<std::string>(setkeys.begin(), setkeys.end());
    }

    // Convert a JSON value to a string suitable for CSV cell
    inline std::string jsonValueToString(const json& v) {
        if (v.is_string()) return v.get<std::string>();
        if (v.is_boolean()) return v.get<bool>() ? "true" : "false";
        if (v.is_number()) return v.dump(); // keeps numeric representation
        if (v.is_null()) return "";
        // for arrays or objects, dump compact JSON
        return v.dump();
    }

    // Main: write CSV from a parsed JSON object/array
    inline bool jsonToCsv(const json& parsed, const std::string& csvPath, const std::vector<std::string>& fields = {}) {
        const json* arr = locate_items_array(parsed);
        if (!arr) {
            // If top-level is an object but not containing array, but is an object we can treat single object as one row
            if (parsed.is_object()) {
                json temp = json::array();
                temp.push_back(parsed);
                arr = &temp;
            } else {
                return false;
            }
        }

        std::vector<std::string> header = collect_fields(*arr, fields);
        std::ofstream ofs(csvPath, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) return false;

        // Write header
        for (size_t i = 0; i < header.size(); ++i) {
            if (i) ofs << ',';
            ofs << escapeCsvCell(header[i]);
        }
        ofs << '\n';

        // Write rows
        for (const auto& item : *arr) {
            for (size_t i = 0; i < header.size(); ++i) {
                if (i) ofs << ',';
                const std::string& key = header[i];
                if (item.is_object() && item.contains(key)) {
                    ofs << escapeCsvCell(jsonValueToString(item.at(key)));
                } else {
                    ofs << ""; // empty cell
                }
            }
            ofs << '\n';
        }
        return true;
    }

    // Convenience: parse JSON text then call jsonToCsv. Returns true on success.
    inline bool jsonTextToCsv(const std::string& jsonText, const std::string& csvPath, const std::vector<std::string>& fields = {}) {
        try {
            json parsed = json::parse(jsonText);
            return jsonToCsv(parsed, csvPath, fields);
        } catch (const std::exception& ex) {
            // parsing failed
            std::cerr << "JSON parse error: " << ex.what() << '\n';
            return false;
        }
    }
} // namespace FileOpration

#endif // FILEOPRATION_HPP