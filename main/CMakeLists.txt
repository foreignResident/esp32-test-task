idf_component_register(SRCS "init.c"
                             "receiver/message_handler.c"
                             "receiver/multicast_listener.c"
                             "receiver/receiver_wifi_station.c"
                             "sender/multicast_publisher.c"
                             "sender/wifi_ap.c"
                       PRIV_REQUIRES esp_driver_gpio esp_wifi nvs_flash spi_flash driver
                       INCLUDE_DIRS "" "receiver" "sender")
