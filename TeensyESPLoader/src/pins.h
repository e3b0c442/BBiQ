
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

    ESP_RST,

    OLED_DC = 5,

    ESP_IO2 = 9,
    ESP_UART1_TXD = 9,

    OLED_CS,

    OLED_MOSI,

    OLED_SCK = 13,

    OLED_RST = 15,

    BUTTON_2 = 22,

    BUTTON_3 = 24,

    BUTTON_1
};