#!/usr/bin/env bash

set -e

# List of libraries that have to be downloaded an unzipped
downloadables=(
  https://github.com/Makuna/Rtc/archive/refs/tags/2.3.5.zip
  https://github.com/mikalhart/TinyGPSPlus/archive/refs/tags/v1.0.3a.zip
  https://github.com/adafruit/Adafruit_BME280_Library/archive/refs/tags/2.2.2.zip
  https://github.com/adafruit/Adafruit_Sensor/archive/refs/tags/1.1.7.zip
  https://github.com/hjd1964/Arduino-DS1820-Temperature-Library/archive/refs/tags/3.8.0.zip
  https://github.com/hjd1964/OneWire/archive/refs/tags/v2.3.5.zip
  https://github.com/hjd1964/TMC2209/archive/refs/tags/8.0.3.zip
  https://github.com/teemuatlut/TMCStepper/archive/refs/tags/v0.7.3.zip
  https://github.com/odriverobotics/ODrive/archive/refs/tags/fw-v0.5.6.zip
  https://github.com/Dlloydev/QuickPID/archive/refs/tags/3.1.9.zip
  https://github.com/RobTillaart/PCF8575/archive/refs/tags/0.2.3.zip
  https://github.com/RobTillaart/TCA9555/archive/refs/tags/0.3.2.zip
  https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/archive/refs/tags/2.3.2.zip
  https://github.com/adafruit/Adafruit_BusIO/archive/refs/tags/1.16.1.zip
  https://github.com/hjd1964/Ethernet/archive/refs/heads/master.zip
  https://downloads.arduino.cc/libraries/github.com/plerup/EspSoftwareSerial-8.1.0.zip
  https://downloads.arduino.cc/libraries/github.com/arduino-libraries/ArduinoMDNS-1.0.0.zip
)

# these libraries do not have an active release, have to be cloned
clonables=(
  https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library.git
)

echo "Starting OnStepX environment setup"
target_dir="${HOME}/Arduino/libraries"
[ ! -d "${target_dir}" ] && mkdir -p "${target_dir}"
cd "${target_dir}"

for lib in ${downloadables[@]}
do
  libname="$(echo "${lib}" | cut -f5 -d'\/')"
  echo "Installing: [${libname}]"
  wget -O output.zip "${lib}"
  unzip output.zip
  rm output.zip
  echo "SUCCESS"
done

for lib in ${clonables[@]}
do
  echo "Cloning [${lib}]"
  git clone "${lib}"
  echo "SUCCESS"
done

echo "Environment setup is now complete!"
