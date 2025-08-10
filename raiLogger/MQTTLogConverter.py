#!/usr/bin/env python3
"""
Convert MQTT log CSV to sensor data CSV format
Converts from timestamped topic/payload format to structured sensor columns
"""

import json
import re
import sys
from datetime import datetime

import pandas as pd


class MQTTLogConverter:
    def __init__(self):
        # Define the output column mapping
        self.output_columns = [
            'Time', 'TempOnSun_C', 'TempInShadow_C', 'Humidity_RH', 
            'FlowRateDailySum_L', 'FlowRateLitPerMin', 'Rain_0-99', 
            'Light_0-99', 'WaterPressure_bar', 'SoilMoistureLocal_0-99',
            'R01', 'R02', 'R03', 'R04', 'R05', 'R06', 'R07', 'R08',
            'R09', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15', 'R16'
        ]
        
        # Store the last known values for each column (except Time)
        self.last_values = {col: 0 for col in self.output_columns[1:]}
        
        # Mapping from JSON keys to output columns
        self.sensor_mapping = {
            'tempOnSun_C': 'TempOnSun_C',
            'tempInShadow_C': 'TempInShadow_C', 
            'humidity_%RH': 'Humidity_RH',
            'flowDaySum_Min': 'FlowRateDailySum_L',
            'flowRate_LitMin': 'FlowRateLitPerMin',
            'rainSensor_0-99': 'Rain_0-99',
            'light_0-99': 'Light_0-99',
            'waterPressure_bar': 'WaterPressure_bar',
            'soilMoistureLocal_0-99': 'SoilMoistureLocal_0-99'
        }
    
    def parse_json_payload(self, payload_str):
        """Parse JSON payload string, handling malformed JSON"""
        try:
            # Clean up the payload string
            payload_str = payload_str.strip().strip('"')
            # Fix escaped quotes
            payload_str = payload_str.replace('""', '"')
            return json.loads(payload_str)
        except json.JSONDecodeError as e:
            print(f"Warning: Failed to parse JSON payload: {payload_str[:100]}...")
            return None
    
    def extract_time_from_timestamp(self, timestamp_str):
        """Extract HH:MM:SS from full timestamp"""
        try:
            dt = datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S.%f")
            return dt.strftime("%H:%M:%S")
        except ValueError:
            try:
                # Try without microseconds
                dt = datetime.strptime(timestamp_str, "%Y-%m-%d %H:%M:%S")
                return dt.strftime("%H:%M:%S")
            except ValueError:
                print(f"Warning: Could not parse timestamp: {timestamp_str}")
                return "00:00:00"
    
    def process_sensors_data(self, data):
        """Process sensors topic data"""
        updates = {}
        
        for json_key, output_col in self.sensor_mapping.items():
            if json_key in data:
                value = data[json_key]
                # Handle null values
                if value is None:
                    value = 0
                updates[output_col] = float(value)
        
        return updates
    
    def process_relays_data(self, data):
        """Process relays topic data"""
        updates = {}
        
        for relay_id in ['R01', 'R02', 'R03', 'R04', 'R05', 'R06', 'R07', 'R08',
                        'R09', 'R10', 'R11', 'R12', 'R13', 'R14', 'R15', 'R16']:
            if relay_id in data:
                relay_info = data[relay_id]
                if isinstance(relay_info, dict) and 'state' in relay_info:
                    # Convert "Opened" to 1, "Closed" to 0
                    state = 1 if relay_info['state'].lower() == 'opened' else 0
                    updates[relay_id] = state
        
        return updates
    
    def process_row(self, row):
        """Process a single row from the input CSV"""
        timestamp = row['Timestamp']
        topic = row['Topic']
        payload = row['Payload']
        
        # Extract time
        time_str = self.extract_time_from_timestamp(timestamp)
        
        # Parse payload
        data = self.parse_json_payload(payload)
        if data is None:
            return None
        
        # Process based on topic
        updates = {}
        if topic == 'rai/jdm7/sensors':
            updates = self.process_sensors_data(data)
        elif topic == 'rai/jdm7/relays':
            updates = self.process_relays_data(data)
        elif topic == 'rai/jdm7/localTime':
            # We only need the timestamp, no data updates
            pass
        
        # Update last known values
        self.last_values.update(updates)
        
        # Create output row with current time and last known values
        output_row = {'Time': time_str}
        output_row.update(self.last_values.copy())
        
        return output_row
    
    def convert_log(self, input_file, output_file):
        """Convert the entire log file"""
        print(f"Converting {input_file} to {output_file}...")
        
        try:
            # Read input CSV
            df_input = pd.read_csv(input_file, delimiter=';')
            print(f"Loaded {len(df_input)} rows from input file")
            
            # Process each row
            output_rows = []
            processed_timestamps = set()
            
            for idx, row in df_input.iterrows():
                try:
                    output_row = self.process_row(row)
                    if output_row:
                        # Only add unique timestamps to avoid duplicates
                        timestamp_key = (row['Timestamp'], row['Topic'])
                        if timestamp_key not in processed_timestamps:
                            output_rows.append(output_row)
                            processed_timestamps.add(timestamp_key)
                            
                            # Print progress
                            if len(output_rows) % 100 == 0:
                                print(f"Processed {len(output_rows)} unique entries...")
                                
                except Exception as e:
                    print(f"Error processing row {idx}: {e}")
                    continue
            
            # Create output DataFrame
            df_output = pd.DataFrame(output_rows, columns=self.output_columns)
            
            # Remove duplicate times, keeping the last occurrence (most complete data)
            df_output = df_output.drop_duplicates(subset=['Time'], keep='last')
            
            # Sort by time
            df_output = df_output.sort_values('Time')
            
            # Save to CSV
            df_output.to_csv(output_file, index=False)
            print(f"Successfully converted to {output_file}")
            print(f"Output contains {len(df_output)} unique time entries")
            
            # Show first few rows as preview
            print("\nFirst 5 rows of output:")
            print(df_output.head().to_string(index=False))
            
        except FileNotFoundError:
            print(f"Error: Input file '{input_file}' not found")
        except Exception as e:
            print(f"Error during conversion: {e}")

def main():
    """Main function"""
    if len(sys.argv) != 3:
        print("Usage: python mqtt_log_converter.py input.csv output.csv")
        print("Example: python mqtt_log_converter.py mqtt_log_2025-08-10.csv sensor_data.csv")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    # Create converter and process
    converter = MQTTLogConverter()
    converter.convert_log(input_file, output_file)

if __name__ == "__main__":
    main()