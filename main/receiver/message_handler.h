#pragma once

#define LED_PIN            CONFIG_LED_PIN

void init_handler();
void process_comand(int commands[], int size);