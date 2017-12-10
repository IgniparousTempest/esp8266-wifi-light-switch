# esp8266-wifi-light-switch
A wifi controlled light switch with an ESP8266


## Program the ESP8266-01
1. Connect GPIO0 to ground.
2. Disconenct power
3. Connect GPIO0 to switch.
4. Reconnect power.

## Debug POST methods
    curl -H "Content-Type: application/json" -X POST -d '{"active": false}' http://93.184.216.34/light
