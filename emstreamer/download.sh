#!/usr/bin/bash
rsync --ignore-existing -r -u -v --rsh=ssh -e 'ssh -i ~/EmController/emstreamer/EC2-Trial1-Keys.pem' --exclude=*.m4v  --exclude='Music/iTunes/Album Artwork/**'  "ubuntu@ec2-13-53-136-123.eu-north-1.compute.amazonaws.com:/home/ubuntu/Audio" ~/EmController/music
