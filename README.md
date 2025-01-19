# irregation-system-sj

## TODOs for the ControlCentre

- [x] Simple led blinking
- [X] Update the schematic according to the shift register
- [X] Add code for BME280
  - [ ] Calibrate + set the range
- [x] Add code for DS18B20
  - [ ] Calibrate + set the range
- [x] Add code for DHT11
  - [ ] Calibrate + set the range
- [S] Add code for YF-G1
  - [ ] Calibrate + set the range
- [X] Add code for HC-12
- [X] Add code for RTC
  - [X] Calibrate + set the range
- [X] Add code for NTP
  - [X] Calibrate + set the range
- [X] Add code for shift register expander
  - [X] Handle relays as an array
- [ ] Add FRAM
  - [ ] Design memory block
- [X] Create commands
- [ ] Cmd handling
  - [X] Add some command and test
  - [ ] Add sensor compatibility
  - [ ] Add portable station compatibility
  - [ ] Store commands permanently - to work automatically
  - [ ] Display what command controls the relay -> use priorities
- [ ] Add Mqtt compatibility
  - [ ] List topics
  - [ ] Sensor 
  - [ ] Add cmd handling
- [ ] Handle timing

## TODOs for the PortableStations

- [ ] Design + draw the circuit (solar panel + battery)
- [ ] Create the circuit
- [ ] Basic LED blinking
- [ ] test the moisture sensor
- [ ] create protocol that will give option to use multiple stations
- [ ] Measure current consumption


## Next steps
1. Add commands via SolenoidManager
2. CmdToStr
3. Indicate which command controls the solenoid
4. Set priorities
5. Test scenarios


# MQTT topics
```
/sensors
  /temperature
  /humidity
  /barometric-pressure
  /soil-moisture
    /node1
      /value
      /battery
    /node2
    ...
    /nodeN
/relays
  /relay1
    /state
    /active-command
  /relay2
  ...
  /relay12
/command
  /add-command (listen)
  /remove-command (listen)
  /stored-command-list
/wifi-strength
```
