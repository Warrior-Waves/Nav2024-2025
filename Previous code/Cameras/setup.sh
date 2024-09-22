#!/bin/bash

# Script to be run ONCE at the first boot

#Checks if running as root
check_rootuser() 
{
if [[ "$(id -u)" != 0 ]]
  then echo "Please run as root"
  exit
fi
}

# Checks internet connectivity
check_internet() 
{
  if ! curl -s --head http://google.com | grep "200 OK" > /dev/null; then
    echo "Please connect to the internet to install libraries"
    exit 
  fi
}

# Makes sure the system/PI is up to date
update_system() {
  echo "Checking system..."
  apt update -y
  rpi-update -y
}

# Checks if ncat and nmap are installed and then installs them if they are not
# Needed for the raspivid function to stream the camera
install_packages() {
  echo "Installing required packages..."
  apt install -y ncat nmap
}

#Configures the Camera

Camera_Config () 
{
raspi-config nonint do_legacy 0

# give more memory
echo -e "$(sed '/gpu_mem/d' /boot/config.txt)" > /boot/config.txt
echo "gpu_mem=256" >> /boot/config.txt

# turn off the red light. if you leave it on, it reflects off the glass
echo "disable_camera_led=1" >> /boot/config.txt
}

# Calling all the functions 

check_root
check_internet
update_system
install_packages
Camera_Config

reboot now
