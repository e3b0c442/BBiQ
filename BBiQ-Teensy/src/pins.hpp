
#pragma once

enum class Pin : uint8_t
{
    ESP_TXD = 0,
    ESP_UART0_TXD = 0,
    ESP_GPIO1 = 0,

    ESP_RXD,
    ESP_UART0_RXD = 1,
    ESP_GPIO3 = 1,

    ESP_IO0,
    ESP_GPIO0 = 2,

    ESP_RST = 3,

    OLED_DC = 5,

    ESP_IO2 = 7,
    ESP_UART1_TXD = 7,

    OLED_CS = 10,

    OLED_MOSI = 11,

    OLED_SCK = 13,

    OLED_RST = 15,

    BUTTON_2 = 8, // breadboard; final = 22,

    BUTTON_1 = 9, // breadboard; final = 24,

    BUTTON_3 = 6, // breadboard; final = 25,
};