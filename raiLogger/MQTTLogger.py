#!/usr/bin/env python3
"""
MQTT Logger for rai/jdm7/* topics
Logs all messages to daily CSV files with timestamp, topic, and payload
"""

import csv
import os
import signal
import sys
import threading
from datetime import datetime

import paho.mqtt.client as mqtt


class MQTTLogger:
    def __init__(self, broker_host="localhost", broker_port=1883, 
                 username=None, password=None, log_directory="logs"):
        self.broker_host = broker_host
        self.broker_port = broker_port
        self.username = username
        self.password = password
        self.log_directory = log_directory
        self.topic_filter = "rai/jdm7/+"  # Subscribe to all topics under rai/jdm7/
        self.client = None
        self.running = True
        self.lock = threading.Lock()  # Thread safety for file operations
        
        # Create log directory if it doesn't exist
        os.makedirs(self.log_directory, exist_ok=True)
        
        # Set up signal handlers for graceful shutdown
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        """Handle shutdown signals gracefully"""
        print(f"\nReceived signal {signum}. Shutting down gracefully...")
        self.running = False
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
        sys.exit(0)
    
    def get_log_filename(self):
        """Generate daily log filename"""
        today = datetime.now().strftime("%Y-%m-%d")
        return os.path.join(self.log_directory, f"mqtt_log_{today}.csv")
    
    def write_to_csv(self, timestamp, topic, payload):
        """Write message to CSV file (thread-safe)"""
        with self.lock:
            log_file = self.get_log_filename()
            file_exists = os.path.exists(log_file)
            
            try:
                with open(log_file, 'a', newline='', encoding='utf-8') as csvfile:
                    writer = csv.writer(csvfile, delimiter=";")
                    
                    # Write header if new file
                    if not file_exists:
                        writer.writerow(['Timestamp', 'Topic', 'Payload'])
                        print(f"Created new log file: {log_file}")
                    
                    # Write the message
                    writer.writerow([timestamp, topic, payload])
                    
            except Exception as e:
                print(f"Error writing to CSV: {e}")
    
    def on_connect(self, client, userdata, flags, rc):
        """Callback for when client connects to broker"""
        if rc == 0:
            print(f"Connected to MQTT broker at {self.broker_host}:{self.broker_port}")
            # Subscribe to all topics under rai/jdm7/
            client.subscribe(self.topic_filter)
            print(f"Subscribed to: {self.topic_filter}")
        else:
            print(f"Failed to connect to MQTT broker. Return code: {rc}")
    
    def on_disconnect(self, client, userdata, rc):
        """Callback for when client disconnects from broker"""
        if rc != 0:
            print("Unexpected disconnection from MQTT broker")
        else:
            print("Disconnected from MQTT broker")
    
    def on_message(self, client, userdata, msg):
        """Callback for when message is received"""
        try:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]  # Include milliseconds
            topic = msg.topic
            payload = msg.payload.decode('utf-8', errors='replace')  # Handle non-UTF8 payloads
            
            # Log to console
            print(f"[{timestamp}] {topic}: {payload}")
            
            # Write to CSV
            self.write_to_csv(timestamp, topic, payload)
            
        except Exception as e:
            print(f"Error processing message: {e}")
    
    def on_subscribe(self, client, userdata, mid, granted_qos):
        """Callback for when subscription is confirmed"""
        print(f"Subscription confirmed with QoS: {granted_qos}")
    
    def start_logging(self):
        """Start the MQTT client and begin logging"""
        try:
            # Create MQTT client
            self.client = mqtt.Client()
            
            # Set callbacks
            self.client.on_connect = self.on_connect
            self.client.on_message = self.on_message
            self.client.on_disconnect = self.on_disconnect
            self.client.on_subscribe = self.on_subscribe
            
            # Set credentials if provided
            if self.username and self.password:
                self.client.username_pw_set(self.username, self.password)
            
            print(f"Starting MQTT Logger...")
            print(f"Broker: {self.broker_host}:{self.broker_port}")
            print(f"Topic filter: {self.topic_filter}")
            print(f"Log directory: {self.log_directory}")
            print(f"Press Ctrl+C to stop")
            
            # Connect to broker
            self.client.connect(self.broker_host, self.broker_port, 60)
            
            # Start the loop
            self.client.loop_start()
            
            # Keep the main thread alive
            while self.running:
                threading.Event().wait(1)
                
        except Exception as e:
            print(f"Error starting MQTT logger: {e}")
        finally:
            if self.client:
                self.client.loop_stop()
                self.client.disconnect()

def main():
    """Main function with configuration"""
    # Configuration - modify these as needed
    BROKER_HOST = "test.mosquitto.org"     # Change to your MQTT broker IP/hostname
    BROKER_PORT = 1883           # Change to your MQTT broker port
    USERNAME = None              # Set to your MQTT username if required
    PASSWORD = None              # Set to your MQTT password if required
    LOG_DIRECTORY = "mqtt_logs"  # Directory to store log files
    
    # Create and start logger
    logger = MQTTLogger(
        broker_host=BROKER_HOST,
        broker_port=BROKER_PORT,
        username=USERNAME,
        password=PASSWORD,
        log_directory=LOG_DIRECTORY
    )
    
    logger.start_logging()

if __name__ == "__main__":
    main()