#!/usr/bin/bash
sudo chown root:input /dev/uinput
sudo chmod g+r,g+w /dev/uinput
sudo chown root:input /dev/input
sudo chmod g+w /dev/input
sudo pactl load-module module-bluetooth-discover
sudo ../controller/emc.out

