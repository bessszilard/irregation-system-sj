#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <tuple>
// #include "../embedded/main/SolenoidManager.hpp"

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

// Mock RelayIds enum for demonstration (replace with your actual enum)
enum class RelayIds {
    Relay1 = 0,
    Relay2,
    Relay3,
    Relay4,
    Relay5,
    Relay6,
    Relay7,
    Relay8,
    NumberOfRelays
};

// Helper function to increment RelayIds
RelayIds incRelayId(RelayIds id) {
    return static_cast<RelayIds>(static_cast<int>(id) + 1);
}

// Helper function to convert RelayIds to string
std::string ToString(RelayIds id) {
    switch (id) {
        case RelayIds::Relay1: return "Relay1";
        case RelayIds::Relay2: return "Relay2";
        case RelayIds::Relay3: return "Relay3";
        case RelayIds::Relay4: return "Relay4";
        case RelayIds::Relay5: return "Relay5";
        case RelayIds::Relay6: return "Relay6";
        case RelayIds::Relay7: return "Relay7";
        case RelayIds::Relay8: return "Relay8";
        default: return "Unknown";
    }
}

// Mock SolenoidManager class for demonstration (replace with your actual implementation)
class MockSensorManager {
private:
    std::map<std::string, double> sensorValues;
    
public:
    void set(const std::string& sensorType, double value) {
        sensorValues[sensorType] = value;
    }
    
    double get(const std::string& sensorType) const {
        auto it = sensorValues.find(sensorType);
        return (it != sensorValues.end()) ? it->second : 0.0;
    }
};

class MockLocalTime {
public:
    int hours, minutes, seconds;
    
    static MockLocalTime Build(int h, int m, int s) {
        MockLocalTime time;
        time.hours = h;
        time.minutes = m;
        time.seconds = s;
        return time;
    }
};

class MockSolenoidManager {
private:
    MockSensorManager sensorManager;
    MockLocalTime currentTime;
    std::map<RelayIds, bool> relayStates;
    
public:
    MockSensorManager& sensors() { return sensorManager; }
    MockLocalTime& localTime() { return currentTime; }
    
    void updateRelayStates() {
        // Mock implementation - you would implement your actual logic here
        // Example: Turn on irrigation if soil moisture is low and it's daytime
        double soilMoisture = sensorManager.get("SMLC");
        double lightLevel = sensorManager.get("LIGH");
        int hour = currentTime.hours;
        
        // Example logic
        relayStates[RelayIds::Relay1] = (soilMoisture < 30.0 && lightLevel > 20.0 && hour >= 6 && hour <= 20);
        relayStates[RelayIds::Relay2] = (sensorManager.get("WAPR") > 5.0); // Water pressure relay
        relayStates[RelayIds::Relay3] = (sensorManager.get("TESU") > 30.0); // Temperature-based fan
        relayStates[RelayIds::Relay4] = (sensorManager.get("RAIN") > 50.0); // Rain protection
        
        // Set remaining relays to false for this example
        for (RelayIds id = RelayIds::Relay5; id < RelayIds::NumberOfRelays; id = incRelayId(id)) {
            relayStates[id] = false;
        }
    }
    
    bool getRelayState(RelayIds id) const {
        auto it = relayStates.find(id);
        return (it != relayStates.end()) ? it->second : false;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <csv_file_path>" << std::endl;
        return 1;
    }
    
    std::string csv_path = argv[1];
    
    // Initialize CSV reader
    CSVReader csvReader;
    if (!csvReader.readFile(csv_path)) {
        return 1;
    }
    
    // Initialize SolenoidManager
    MockSolenoidManager sm;  // Replace with: SolenoidManager sm;
    
    // Prepare output CSV writer
    CSVWriter csvWriter;
    
    // Create output headers (original headers + relay states)
    std::vector<std::string> outputHeaders = csvReader.getHeaders();
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId)) {
        outputHeaders.push_back(ToString(relayId));
    }
    csvWriter.setHeaders(outputHeaders);
    
    std::cout << "Processing " << csvReader.size() << " rows..." << std::endl;
    
    // Process each line
    for (const auto& line : csvReader.getData()) {
        // Create output line starting with input data
        std::map<std::string, std::string> csv_output_line = line;
        
        // Set sensor values in SolenoidManager
        try {
            sm.sensors().set("TESU", std::stod(line.at("TempOnSun_C")));
            sm.sensors().set("TESH", std::stod(line.at("TempInShadow_C")));
            sm.sensors().set("HUMI", std::stod(line.at("Humidity_RH")));
            sm.sensors().set("FRDS", std::stod(line.at("FlowRateDailySum_L")));
            sm.sensors().set("FRLM", std::stod(line.at("FlowRateLitPerMin")));
            sm.sensors().set("RAIN", std::stod(line.at("Rain_0-99")));
            sm.sensors().set("LIGH", std::stod(line.at("Light_0-99")));
            sm.sensors().set("WAPR", std::stod(line.at("WaterPressure_bar")));
            sm.sensors().set("SMLC", std::stod(line.at("SoilMoistureLocal_0-99")));
        } catch (const std::exception& e) {
            std::cerr << "Error parsing sensor values: " << e.what() << std::endl;
            continue;
        }
        
        // Parse and set time
        try {
            auto [hours, minutes, seconds] = splitTimeToHMS(line.at("Time"));
            sm.localTime() = MockLocalTime::Build(hours, minutes, seconds);  // Replace with actual LocalTime::Build
        } catch (const std::exception& e) {
            std::cerr << "Error parsing time: " << e.what() << std::endl;
            continue;
        }
        
        // Update relay states based on sensor data
        sm.updateRelayStates();
        
        // Add relay states to output line
        for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId)) {
            csv_output_line[ToString(relayId)] = sm.getRelayState(relayId) ? "1" : "0";
        }
        
        // Add processed line to output
        csvWriter.addRow(csv_output_line);
    }
    
    // Generate output filename
    std::string output_path = replaceSubstring(csv_path, ".csv", "_proc.csv");
    
    // Write output file
    if (!csvWriter.writeFile(output_path)) {
        return 1;
    }
    
    std::cout << "Processing complete! Output saved to: " << output_path << std::endl;
    return 0;
}

// Compilation command:
// g++ -std=c++17 -o csv_processor csv_processor.cpp

// Usage:
// ./csv_processor weather_data.csv