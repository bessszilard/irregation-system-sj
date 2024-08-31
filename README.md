# irregation-system-sj

## TODOs for the ControlCentre

- [x] Simple led blinking
- [ ] Update the schematic according to the I2C expander
- [ ] Add code for BME280
  - [ ] Calibrate + set the range
- [ ] Add code for DHT11
  - [ ] Calibrate + set the range
- [ ] Add code for YF-G1
  - [ ] Calibrate + set the range
- [ ] Add code for HC-12
  - [ ] Calibrate + set the range
- [ ] Add code for I2C expander
  - [ ] Handle relays as an array
- [ ] Create commands
- [ ] Event handling
  - [ ] Add some command and test
  - [ ] Add sensor compatibility
  - [ ] Add portable station compatibility
  - [ ] Store commands permanently - to work automatically
  - [ ] Display what command controls the relay -> use priorities
- [ ] Add Mqtt compatibility
  - [ ] List topics
  - [ ] Sensor 
  - [ ] Add event handling
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