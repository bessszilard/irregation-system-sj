import argparse
import csv
import json
import sys
from datetime import datetime, timedelta

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.widgets import CheckButtons


def parse_time_range(time_str):
    """Parse time range string like '06:00->09:00' to start and end times in seconds"""
    start_str, end_str = time_str.split('->')
    start_h, start_m = map(int, start_str.split(':'))
    end_h, end_m = map(int, end_str.split(':'))
    
    start_seconds = start_h * 3600 + start_m * 60
    end_seconds = end_h * 3600 + end_m * 60
    
    return start_seconds, end_seconds

def get_weather_condition(current_seconds, weather_schedule):
    """Get current weather condition based on time"""
    for time_range, condition in weather_schedule.items():
        start_seconds, end_seconds = parse_time_range(time_range)
        if start_seconds <= current_seconds < end_seconds:
            return condition
    return "sunny"  # default

def calculate_sun_angle(hour):
    """Calculate sun angle for realistic temperature and light curves"""
    # Sun rises around 5:30, peaks at 12:00, sets around 20:30 in Hungarian summer
    if hour < 5.5:
        return 0
    elif hour > 20.5:
        return 0
    else:
        # Simple sine curve for sun angle
        progress = (hour - 5.5) / 15.0  # 15 hours of daylight
        return np.sin(progress * np.pi)

def generate_base_temperature(hour, condition):
    """Generate base temperature based on time and weather"""
    # Hungarian summer temperatures: night ~15-18°C, day ~25-35°C
    sun_angle = calculate_sun_angle(hour)
    base_temp = 16 + sun_angle * 19  # 16°C at night, up to 35°C peak
    
    # Weather condition modifiers
    modifiers = {
        "sunny": 0,
        "scattered_cloud": -2,
        "cloudy": -4,
        "light_rain": -6,
        "heavy_rain": -8,
        "overcast": -5
    }
    
    return base_temp + modifiers.get(condition, 0)

def generate_humidity(temp_sun, condition):
    """Generate humidity based on temperature and weather condition"""
    # Base humidity inversely related to temperature
    base_humidity = max(30, 90 - (temp_sun - 15) * 2)
    
    # Weather condition modifiers
    modifiers = {
        "sunny": 0,
        "scattered_cloud": 5,
        "cloudy": 10,
        "light_rain": 20,
        "heavy_rain": 30,
        "overcast": 15
    }
    
    humidity = base_humidity + modifiers.get(condition, 0)
    return min(95, max(25, humidity))

def generate_light_level(hour, condition):
    """Generate light level (0-99) based on time and weather"""
    sun_angle = calculate_sun_angle(hour)
    base_light = sun_angle * 99
    
    # Weather condition modifiers
    multipliers = {
        "sunny": 1.0,
        "scattered_cloud": 0.8,
        "cloudy": 0.4,
        "light_rain": 0.3,
        "heavy_rain": 0.2,
        "overcast": 0.3
    }
    
    light = base_light * multipliers.get(condition, 1.0)
    return max(0, min(99, int(light)))

def create_smooth_transitions(weather_schedule, total_points):
    """Create smooth transitions between weather conditions"""
    # Create arrays to store the raw weather intensities
    rain_intensity = np.zeros(total_points)
    cloud_intensity = np.zeros(total_points)
    
    interval = 10  # 10 seconds
    
    # Define weather condition intensities
    weather_intensities = {
        "sunny": {"rain": 0, "cloud": 0},
        "scattered_cloud": {"rain": 0, "cloud": 0.3},
        "cloudy": {"rain": 0, "cloud": 0.7},
        "overcast": {"rain": 0, "cloud": 0.9},
        "light_rain": {"rain": 0.3, "cloud": 0.8},
        "heavy_rain": {"rain": 0.8, "cloud": 0.9}
    }
    
    # First pass: set base intensities
    for i in range(total_points):
        current_seconds = i * interval
        condition = get_weather_condition(current_seconds, weather_schedule)
        
        intensities = weather_intensities.get(condition, {"rain": 0, "cloud": 0})
        rain_intensity[i] = intensities["rain"]
        cloud_intensity[i] = intensities["cloud"]
    
    # Apply smoothing with different kernels
    # Rain changes more gradually
    rain_kernel_size = min(600, total_points // 20)  # 100 minutes smoothing for rain
    rain_kernel = np.ones(rain_kernel_size) / rain_kernel_size
    rain_intensity = np.convolve(rain_intensity, rain_kernel, mode='same')
    
    # Cloud changes moderately
    cloud_kernel_size = min(360, total_points // 30)  # 60 minutes smoothing for clouds
    cloud_kernel = np.ones(cloud_kernel_size) / cloud_kernel_size
    cloud_intensity = np.convolve(cloud_intensity, cloud_kernel, mode='same')
    
    return rain_intensity, cloud_intensity

def calculate_sensor_values(hour, rain_intensity, cloud_intensity, i, total_points):
    """Calculate all sensor values based on smooth weather intensities"""
    
    # Temperature calculation
    sun_angle = calculate_sun_angle(hour)
    base_temp = 16 + sun_angle * 19
    
    # Temperature affected by clouds and rain
    temp_reduction = cloud_intensity * 4 + rain_intensity * 8
    temp_sun = base_temp - temp_reduction
    temp_shadow = temp_sun - np.random.uniform(2, 4)
    
    # Humidity calculation
    base_humidity = max(30, 90 - (temp_sun - 15) * 2)
    humidity_increase = cloud_intensity * 10 + rain_intensity * 25
    humidity = base_humidity + humidity_increase
    humidity = min(95, max(25, humidity))
    
    # Light calculation
    base_light = sun_angle * 99
    light_reduction = cloud_intensity * 0.6 + rain_intensity * 0.4
    light = base_light * (1 - light_reduction)
    light = max(0, min(99, int(light)))
    
    # Rain calculation - convert intensity to 0-99 scale
    rain = int(rain_intensity * 99)
    
    # Add some realistic noise for rain (creates peaks and valleys)
    if rain_intensity > 0.1:
        noise = np.sin(i * 0.01) * 5 + np.random.normal(0, 3)
        rain = max(0, min(99, rain + int(noise)))
    
    return temp_sun, temp_shadow, humidity, light, rain

def generate_soil_moisture(rain, current_moisture, dt_hours):
    """Update soil moisture based on rain and evaporation"""
    # Rain increases moisture
    if rain > 0:
        moisture_increase = rain * 0.5 * dt_hours  # Rain effect
        current_moisture = min(99, current_moisture + moisture_increase)
    
    # Evaporation decreases moisture (slower process)
    evaporation_rate = 2.0 * dt_hours  # moisture points per hour
    current_moisture = max(10, current_moisture - evaporation_rate)
    
    return current_moisture

def generate_water_pressure():
    """Generate constant water pressure at 10 bar"""
    return 10.0

def simulate_weather_day(weather_schedule, output_filename="weather_data.csv"):
    """Generate weather simulation data for 24 hours"""
    
    # Initialize data storage
    data = []
    
    # Time parameters
    total_seconds = 24 * 3600  # 24 hours in seconds
    interval = 10  # 10 seconds
    num_points = total_seconds // interval
    
    print(f"Generating {num_points} data points with smooth transitions...")
    
    # Generate smooth weather intensities
    rain_intensity, cloud_intensity = create_smooth_transitions(weather_schedule, num_points)
    
    # Initialize soil moisture
    soil_moisture = 40.0  # Starting soil moisture
    
    for i in range(num_points):
        current_seconds = i * interval
        current_time = timedelta(seconds=current_seconds)
        hour = current_seconds / 3600.0
        
        # Format time as HH:MM:SS
        hours = int(current_seconds // 3600)
        minutes = int((current_seconds % 3600) // 60)
        seconds = int(current_seconds % 60)
        time_str = f"{hours:02d}:{minutes:02d}:{seconds:02d}"
        
        # Calculate sensor values using smooth intensities
        temp_sun, temp_shadow, humidity, light, rain = calculate_sensor_values(
            hour, rain_intensity[i], cloud_intensity[i], i, num_points
        )
        
        # Update soil moisture
        dt_hours = interval / 3600.0
        soil_moisture = generate_soil_moisture(rain, soil_moisture, dt_hours)
        
        # Water pressure is constant at 10 bar
        water_pressure = generate_water_pressure()
        
        # Add minimal noise to temperatures for realism
        temp_sun += np.random.normal(0, 0.2)
        temp_shadow += np.random.normal(0, 0.2)
        humidity += np.random.normal(0, 0.5)
        humidity = max(20, min(100, humidity))
        
        # Store data point
        data_point = [
            time_str,
            round(temp_sun, 1),
            round(temp_shadow, 1),
            round(humidity, 1),
            0,  # FlowRateDailySum (kept as 0 as requested)
            0,  # FlowRateLitPerMin (kept as 0 as requested)
            rain,
            light,
            round(water_pressure, 1),
            round(soil_moisture, 1)
        ]
        
        data.append(data_point)
    
    # Write to CSV
    headers = [
        "Time", "TempOnSun_C", "TempInShadow_C", "Humidity_RH", 
        "FlowRateDailySum_L", "FlowRateLitPerMin", "Rain_0-99", 
        "Light_0-99", "WaterPressure_bar", "SoilMoistureLocal_0-99"
    ]
    
    with open(output_filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(headers)
        writer.writerows(data)
    
    print(f"Data saved to {output_filename}")
    return data, headers

def normalize_sensor_data(df):
    """Normalize all sensor data to 0-99 scale for unified plotting"""
    normalized_df = df.copy()
    
    # Temperature: normalize to 0-99 (assuming range 10-40°C)
    temp_min, temp_max = 10, 40
    normalized_df['TempOnSun_Normalized'] = ((df['TempOnSun_C'] - temp_min) / (temp_max - temp_min)) * 99
    normalized_df['TempInShadow_Normalized'] = ((df['TempInShadow_C'] - temp_min) / (temp_max - temp_min)) * 99
    
    # Humidity: already 0-100, scale to 0-99
    normalized_df['Humidity_Normalized'] = (df['Humidity_RH'] / 100) * 99
    
    # Water Pressure: normalize 0-10 bar to 0-99 scale
    normalized_df['WaterPressure_Normalized'] = (df['WaterPressure_bar'] / 10) * 99
    
    # Rain, Light, Soil Moisture are already 0-99
    normalized_df['Rain_Normalized'] = df['Rain_0-99']
    normalized_df['Light_Normalized'] = df['Light_0-99']
    normalized_df['SoilMoisture_Normalized'] = df['SoilMoistureLocal_0-99']
    
    return normalized_df

def plot_weather_data(filename="weather_data.csv"):
    """Plot all sensor data on a single 0-99 scale with interactive checkboxes"""
    df = pd.read_csv(filename)
    
    # Convert time to hours for plotting
    df['Hours'] = df.index * 10 / 3600  # 10 second intervals to hours
    
    # Normalize all data to 0-99 scale
    norm_df = normalize_sensor_data(df)
    
    # Create figure with space for checkboxes
    fig = plt.figure(figsize=(18, 10))
    
    # Main plot area
    ax = plt.subplot2grid((1, 10), (0, 0), colspan=8)
    
    # Checkbox area
    checkbox_ax = plt.subplot2grid((1, 10), (0, 8), colspan=2)
    
    # Define sensor data and properties
    sensors = {
        'TempOnSun_Normalized': {
            'data': norm_df['TempOnSun_Normalized'],
            'label': 'Temp in Sun',
            'unit': '°C',
            'color': 'red',
            'linewidth': 2,
            'alpha': 0.8
        },
        'TempInShadow_Normalized': {
            'data': norm_df['TempInShadow_Normalized'],
            'label': 'Temp in Shadow',
            'unit': '°C',
            'color': 'darkred',
            'linewidth': 2,
            'alpha': 0.8
        },
        'Humidity_Normalized': {
            'data': norm_df['Humidity_Normalized'],
            'label': 'Humidity',
            'unit': '%RH',
            'color': 'green',
            'linewidth': 2,
            'alpha': 0.8
        },
        'WaterPressure_Normalized': {
            'data': norm_df['WaterPressure_Normalized'],
            'label': 'Water Pressure',
            'unit': 'bar',
            'color': 'purple',
            'linewidth': 2,
            'alpha': 0.8
        },
        'Light_Normalized': {
            'data': norm_df['Light_Normalized'],
            'label': 'Light Level',
            'unit': '0-99',
            'color': 'gold',
            'linewidth': 2,
            'alpha': 0.8
        },
        'Rain_Normalized': {
            'data': norm_df['Rain_Normalized'],
            'label': 'Rain Level',
            'unit': '0-99',
            'color': 'blue',
            'linewidth': 3,
            'alpha': 0.7
        },
        'SoilMoisture_Normalized': {
            'data': norm_df['SoilMoisture_Normalized'],
            'label': 'Soil Moisture',
            'unit': '0-99',
            'color': 'brown',
            'linewidth': 2,
            'alpha': 0.8
        }
    }
    
    # Plot all sensors initially
    lines = {}
    for sensor_key, props in sensors.items():
        line, = ax.plot(norm_df['Hours'], props['data'], 
                       color=props['color'],
                       linewidth=props['linewidth'],
                       alpha=props['alpha'],
                       label=f"{props['label']} ({props['unit']})")
        lines[sensor_key] = line
    
    # Add rain fill area
    rain_fill = ax.fill_between(norm_df['Hours'], norm_df['Rain_Normalized'], 
                               alpha=0.3, color='blue', label='_nolegend_')
    
    # Configure main plot
    ax.set_xlabel('Hour of Day', fontsize=12)
    ax.set_ylabel('Normalized Sensor Values (0-99 Scale)', fontsize=12)
    ax.set_title('Hungarian Summer Day - All Weather Sensors (Normalized Scale)', fontsize=14, pad=20)
    ax.set_xlim(0, 24)
    ax.set_ylim(0, 100)
    ax.set_xticks(range(0, 25, 2))
    ax.grid(True, alpha=0.3)
    
    # Add original value annotations on y-axis
    ax2 = ax.twinx()
    ax2.set_ylabel('Original Value Ranges', fontsize=10, rotation=270, labelpad=20)
    ax2.set_ylim(0, 100)
    ax2.set_yticks([0, 25, 50, 75, 99])
    ax2.set_yticklabels(['Min', '25%', '50%', '75%', 'Max'], fontsize=9)
    
    # Add reference text for original ranges
    range_text = (
        "Original Ranges:\n"
        "Temp: 10-40°C\n"
        "Humidity: 0-100%\n" 
        "Pressure: 0-10 bar\n"
        "Others: 0-99"
    )
    ax.text(0.02, 0.98, range_text, transform=ax.transAxes, fontsize=9,
            verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
    
    # Create checkboxes
    sensor_labels = [f"{props['label']} ({props['unit']})" for props in sensors.values()]
    visibility = [True] * len(sensor_labels)  # All visible initially
    
    checkbox_ax.set_xlim(0, 1)
    checkbox_ax.set_ylim(0, 1)
    checkbox_ax.axis('off')
    
    # Create CheckButtons
    rax = plt.axes([0.82, 0.3, 0.15, 0.4])  # [left, bottom, width, height]
    check = CheckButtons(rax, sensor_labels, visibility)
    
    # Style the checkboxes (compatible with different matplotlib versions)
    try:
        # Try newer matplotlib API first
        for i, rect in enumerate(check.rectangles):
            rect.set_facecolor(list(sensors.values())[i]['color'])
            rect.set_alpha(0.3)
        
        for i, line in enumerate(check.lines):
            for l in line:
                l.set_color('black')
                l.set_linewidth(2)
    except AttributeError:
        # Fallback for older matplotlib versions
        try:
            # Alternative styling approach
            for i in range(len(sensor_labels)):
                color = list(sensors.values())[i]['color']
                # Set color for checkbox background if possible
                if hasattr(check, 'labels'):
                    check.labels[i].set_color(color)
        except:
            # If styling fails, just continue without custom colors
            pass
    
    # Callback function for checkboxes
    def toggle_sensor(label):
        # Find the sensor key by label
        sensor_key = None
        for key, props in sensors.items():
            if f"{props['label']} ({props['unit']})" == label:
                sensor_key = key
                break
        
        if sensor_key and sensor_key in lines:
            line = lines[sensor_key]
            line.set_visible(not line.get_visible())
            
            # Handle rain fill separately
            if sensor_key == 'Rain_Normalized':
                rain_fill.set_visible(line.get_visible())
        
        plt.draw()
    
    check.on_clicked(toggle_sensor)
    
    # Add legend
    ax.legend(loc='upper left', bbox_to_anchor=(0, 1), fontsize=10)
    
    # Add weather condition background (optional)
    weather_colors = {
        'sunny': 'yellow',
        'scattered_cloud': 'orange', 
        'cloudy': 'gray',
        'overcast': 'darkgray',
        'light_rain': 'lightblue',
        'heavy_rain': 'darkblue'
    }
    
    plt.tight_layout()
    plt.subplots_adjust(right=0.8)  # Make room for checkboxes
    plt.show()
    
    # Print statistics with original values
    print("\nData Statistics (Original Values):")
    print(f"Temperature (Sun): {df['TempOnSun_C'].min():.1f}°C to {df['TempOnSun_C'].max():.1f}°C")
    print(f"Temperature (Shadow): {df['TempInShadow_C'].min():.1f}°C to {df['TempInShadow_C'].max():.1f}°C")
    print(f"Humidity: {df['Humidity_RH'].min():.1f}% to {df['Humidity_RH'].max():.1f}%")
    print(f"Water Pressure: {df['WaterPressure_bar'].iloc[0]} bar (constant)")
    print(f"Rain Level: 0 to {df['Rain_0-99'].max()}")
    print(f"Light Level: 0 to {df['Light_0-99'].max()}")
    print(f"Soil Moisture: {df['SoilMoistureLocal_0-99'].min():.1f} to {df['SoilMoistureLocal_0-99'].max():.1f}")
    
    print("\nNormalized Statistics (0-99 Scale):")
    print(f"Temperature (Sun): {norm_df['TempOnSun_Normalized'].min():.1f} to {norm_df['TempOnSun_Normalized'].max():.1f}")
    print(f"Temperature (Shadow): {norm_df['TempInShadow_Normalized'].min():.1f} to {norm_df['TempInShadow_Normalized'].max():.1f}")
    print(f"Humidity: {norm_df['Humidity_Normalized'].min():.1f} to {norm_df['Humidity_Normalized'].max():.1f}")
    print(f"Water Pressure: {norm_df['WaterPressure_Normalized'].min():.1f} to {norm_df['WaterPressure_Normalized'].max():.1f}")

def main():
    """Main function to run the weather simulation"""
    
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Hungarian Summer Weather Sensor Simulator')
    parser.add_argument('json_file', help='JSON file containing weather schedule')
    parser.add_argument('-o', '--output', default='weather_data.csv', help='Output CSV filename (default: weather_data.csv)')
    
    args = parser.parse_args()
    
    # Load weather schedule from JSON file
    try:
        with open(args.json_file, 'r') as f:
            weather_schedule = json.load(f)
        print(f"Loaded weather schedule from {args.json_file}")
    except FileNotFoundError:
        print(f"Error: File '{args.json_file}' not found!")
        print("\nExample weather_schedule.json format:")
        example = {
            "00:00->06:00": "cloudy",
            "06:00->09:00": "light_rain", 
            "09:00->11:00": "cloudy",
            "11:00->15:00": "sunny",
            "15:00->17:00": "scattered_cloud",
            "17:00->19:00": "light_rain",
            "19:00->24:00": "cloudy"
        }
        print(json.dumps(example, indent=2))
        sys.exit(1)
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON format in '{args.json_file}'!")
        sys.exit(1)
    
    print("Weather Schedule:")
    for time_range, condition in weather_schedule.items():
        print(f"  {time_range}: {condition}")
    
    # Validate weather conditions
    valid_conditions = {"sunny", "cloudy", "light_rain", "heavy_rain", "scattered_cloud", "overcast"}
    for condition in weather_schedule.values():
        if condition not in valid_conditions:
            print(f"Warning: Unknown weather condition '{condition}'")
            print(f"Valid conditions: {', '.join(valid_conditions)}")
    
    # Generate simulation data
    data, headers = simulate_weather_day(weather_schedule, args.output)
    
    # Plot the results
    plot_weather_data(args.output)
    
    print(f"\nSimulation complete! Data saved to {args.output}")
    print("Available weather conditions: sunny, cloudy, light_rain, heavy_rain, scattered_cloud, overcast")

if __name__ == "__main__":
    main()