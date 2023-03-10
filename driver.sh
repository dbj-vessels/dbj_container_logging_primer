#!/bin/sh 
#
# somelegacyapp is made to runn for 
# approx 15 seconds
#
# this script will start the legacy app 
# redirect all of its standard streams to /dev/console
# and finally run as daemon
#
./somelegacyapp &>/dev/console &