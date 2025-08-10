#include <nlohmann/json.hpp>
#include "Utility.hpp"
#include "../embedded/main/SolenoidManager.hpp"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <csv_file_path> <command_path>" << std::endl;
        return 1;
    }

    std::string csv_path      = argv[1];
    std::string commands_path = argv[2];

    // Initialize CSV reader
    CSVReader csvReader;
    if (!csvReader.readFile(csv_path))
    {
        return 1;
    }
    std::string mergedCmdStr = convertJsonToString(commands_path);

    std::cout << "My comands are: " << mergedCmdStr << std::endl;

    // Initialize SolenoidManager
    SolenoidManager sm;
    sm.loadCmdsFromString(mergedCmdStr);

    // Prepare output CSV writer
    CSVWriter csvWriter;

    // Create output headers (original headers + relay states)
    std::vector<std::string> outputHeaders = csvReader.getHeaders();
    for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
    {
        outputHeaders.push_back(ToString(relayId));
    }
    csvWriter.setHeaders(outputHeaders);

    std::cout << "Processing " << csvReader.size() << " rows..." << std::endl;

    // Process each line
    for (const auto& line : csvReader.getData())
    {
        // Create output line starting with input data
        std::map<std::string, std::string> csv_output_line = line;

        // Set sensor values in SolenoidManager
        try
        {
            sm.sensors().set("TESU", std::stod(line.at("TempOnSun_C")));
            sm.sensors().set("TESH", std::stod(line.at("TempInShadow_C")));
            sm.sensors().set("HUMI", std::stod(line.at("Humidity_RH")));
            sm.sensors().set("FRDS", std::stod(line.at("FlowRateDailySum_L")));
            sm.sensors().set("FRLM", std::stod(line.at("FlowRateLitPerMin")));
            sm.sensors().set("RAIN", std::stod(line.at("Rain_0-99")));
            sm.sensors().set("LIGH", std::stod(line.at("Light_0-99")));
            sm.sensors().set("WAPR", std::stod(line.at("WaterPressure_bar")));
            sm.sensors().set("SMLC", std::stod(line.at("SoilMoistureLocal_0-99")));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing sensor values: " << e.what() << std::endl;
            continue;
        }

        // Parse and set time
        try
        {
            auto [hours, minutes, seconds] = splitTimeToHMS(line.at("Time"));
            sm.localTime() = LocalTime::Build(hours, minutes, seconds); // Replace with actual LocalTime::Build
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing time: " << e.what() << std::endl;
            continue;
        }

        // Update relay states based on sensor data
        sm.updateRelayStates();

        // Add relay states to output line
        for (RelayIds relayId = RelayIds::Relay1; relayId < RelayIds::NumberOfRelays; relayId = incRelayId(relayId))
        {
            csv_output_line[ToString(relayId)] = sm.getRelayState(relayId) == RelayState::Opened ? "1" : "0";
        }

        // Add processed line to output
        csvWriter.addRow(csv_output_line);
    }

    // Generate output filename
    std::string output_path = replaceSubstring(csv_path, ".csv", "_proc.csv");

    // Write output file
    if (!csvWriter.writeFile(output_path))
    {
        return 1;
    }

    std::cout << "Processing complete! Output saved to: " << output_path << std::endl;
    return 0;
}