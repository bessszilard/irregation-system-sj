#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>
#include <nlohmann/json.hpp>


// CSV Reader class
class CSVReader {
private:
    std::vector<std::map<std::string, std::string>> data;
    std::vector<std::string> headers;
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            // Trim whitespace
            token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
            token.erase(std::find_if(token.rbegin(), token.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), token.end());
            
            tokens.push_back(token);
        }
        return tokens;
    }
    
public:
    bool readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }
        
        std::string line;
        bool firstLine = true;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::vector<std::string> tokens = split(line, ',');
            
            if (firstLine) {
                headers = tokens;
                firstLine = false;
            } else {
                std::map<std::string, std::string> row;
                for (size_t i = 0; i < tokens.size() && i < headers.size(); ++i) {
                    row[headers[i]] = tokens[i];
                }
                data.push_back(row);
            }
        }
        
        file.close();
        std::cout << "Loaded " << data.size() << " rows from " << filename << std::endl;
        return true;
    }
    
    const std::vector<std::map<std::string, std::string>>& getData() const {
        return data;
    }
    
    const std::vector<std::string>& getHeaders() const {
        return headers;
    }
    
    size_t size() const {
        return data.size();
    }
};

// CSV Writer class
class CSVWriter {
private:
    std::vector<std::string> headers;
    std::vector<std::map<std::string, std::string>> data;
    
public:
    void setHeaders(const std::vector<std::string>& h) {
        headers = h;
    }
    
    void addRow(const std::map<std::string, std::string>& row) {
        data.push_back(row);
    }
    
    bool writeFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not create file " << filename << std::endl;
            return false;
        }
        
        // Write headers
        for (size_t i = 0; i < headers.size(); ++i) {
            file << headers[i];
            if (i < headers.size() - 1) file << ",";
        }
        file << std::endl;
        
        // Write data
        for (const auto& row : data) {
            for (size_t i = 0; i < headers.size(); ++i) {
                auto it = row.find(headers[i]);
                if (it != row.end()) {
                    file << it->second;
                } else {
                    file << ""; // Empty field
                }
                if (i < headers.size() - 1) file << ",";
            }
            file << std::endl;
        }
        
        file.close();
        std::cout << "Saved " << data.size() << " rows to " << filename << std::endl;
        return true;
    }
};

// Utility function to split time string into hours, minutes, seconds
std::tuple<int, int, int> splitTimeToHMS(const std::string& timeStr) {
    std::vector<std::string> parts;
    std::stringstream ss(timeStr);
    std::string part;
    
    while (std::getline(ss, part, ':')) {
        parts.push_back(part);
    }
    
    int hours = 0, minutes = 0, seconds = 0;
    
    if (parts.size() >= 1) hours = std::stoi(parts[0]);
    if (parts.size() >= 2) minutes = std::stoi(parts[1]);
    if (parts.size() >= 3) seconds = std::stoi(parts[2]);
    
    return std::make_tuple(hours, minutes, seconds);
}

// Utility function to convert double to string with precision
std::string doubleToString(double value, int precision = 1) {
    std::ostringstream oss;
    oss.precision(precision);
    oss << std::fixed << value;
    return oss.str();
}

// Utility function to replace substring in string
std::string replaceSubstring(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = result.find(from);
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    return result;
}

std::string convertJsonToString(const std::string& filepath) {
    try {
        // Read the JSON file
        std::ifstream input(filepath);
         nlohmann::json data = nlohmann::json::parse(input);
        
        // Get the cmdList array
        std::vector<std::string> cmdList = data["cmdList"];
        
        // Join the strings with '|' delimiter
        std::string result;
        for (size_t i = 0; i < cmdList.size(); ++i) {
            if (i != 0) {
                result += "|";
            }
            result += cmdList[i];
        }
        
        return result;
    } catch (const std::exception& e) {
        std::cerr << "Error processing JSON: " << e.what() << std::endl;
        return "";
    }
}

