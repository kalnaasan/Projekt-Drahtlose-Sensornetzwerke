#!/bin/bash

# Update package list
sudo apt-get update

# Install required packages
sudo apt-get install -y git

# Clone the ot-nrf528xx repository
cd ~
rm -vrf ot-nrf528xx
git clone --recursive https://github.com/openthread/ot-nrf528xx.git

# Navigate to ot-nrf528xx directory
cd ot-nrf528xx

# Bootstrap the build environment
./script/bootstrap

# Build the RCP firmware
./script/build nrf52840 USB_trans -DOT_BOOTLOADER=USB
arm-none-eabi-objcopy -O ihex build/bin/ot-rcp ot-rcp.hex

# Clone the ot-br-posix repository
cd ~
git clone https://github.com/openthread/ot-br-posix

# Navigate to ot-br-posix directory
cd ot-br-posix

# Bootstrap the BorderRouter software
NAT64=1 NAT64_SERVICE=openthread ./script/bootstrap
INFRA_IF_NAME=wlan0 ./script/setup

# Reboot the system
sudo reboot