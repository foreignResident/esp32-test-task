# Test task requierments

Use 2 esp32-s3 boards to setup board to board confuguration over WiFi.

1. Launch and connect WiFi.
2. Exchange data.
3. Send code sequence and output it to a signal board pin.

# Building from the source

The building requiers two board configuration - receiver and sender. The variable `IS_SERVER_BOARD` should be set `y` or `true` for sender and `n` or `false` for receiver board. `LED_PIN` sensetive to show up signal commands. `-> Custom Project Configuration` menu section to modify.

Set of commands to operate with.
```
source ~/.espressif/tools/activate_idf_v6.0.sh
idf.py set-target esp32s3

idf.py menuconfig
idf.py build

idf.py -p /dev/cu.* flash
idf.py -p /dev/cu.* monitor
```

# Build config variables

Go to main/Kconfig.projbuild to setup variables or execute idf.py menuconfig to modify

| Variable | Default | Description |                                                                                    
|----------|---------|-------------|        
| `LED_PIN` | 12 | Pin used to signal commands on receiver board. |
| `IS_SERVER_BOARD` | y | Set yes for wifi transmitter board and no for receiver. |
| `MULTICAST_GROUP_IP` | "239.0.0.1" | Multicast group to subscribe/send events over. |
| `MULTICAST_PORT` | 4444 | Port for multicast group. |
| `MULTICAST_TTL` | 1 | Leave 1 to stay on the current subnet only. |
| `ESP_WIFI_SSID` | "test" | WiFi SSID (network name). |
| `ESP_WIFI_PASSWORD` | "123456789" | WiFi password (WPA or WPA2). |
| `ESP_WIFI_CHANNEL` | 1 | WiFi channel (1-13). |
| `ESP_MAX_STA_CONN` | 4 | Max number of STA connections to AP. |

# Resources

Multicast
https://espressif.github.io/esp32-c3-book-en/chapter_8/8.2/8.2.2.html
https://github.com/espressif/book-esp32c3-iot-projects/blob/main/test_case/multicast_discovery/main/app_main.c

Wifi
https://github.com/espressif/esp-idf/blob/v6.0/examples/wifi/softap_sta/main/softap_sta.c

GPIO
https://github.com/espressif/esp-idf/blob/v6.0/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c