# canToHTMLDecoder
Arduinocode zur Darstellung von CAN-Nachrichten auf einer HTML-Seite mit einem ESP8266.


## MCP2515 Connection
| MCP2515 | ESP8266 |
| ------- | ------- |
| INT     | D4      |
| SCK     | D5      |
| MOSI    | D7      |
| MISO    | D6      |
| CS      | D8      |
| GND     | GND     |
| VCC     | 3V3     |
| VCC1    | 5V *USB/external* |


## Libraries
https://github.com/esp8266/Arduino

https://github.com/coryjfowler/MCP_CAN_lib



## Parts
NodeMCU V3 Board

JoyIt Can Module  https://joy-it.net/de/products/SBC-CAN01


## License
This project is licensed under the MIT License - see the [license file](LICENSE) for details.
