# Parts List:

- LCD Display: DigiKey part number [NHD-C12864A1Z-FSW-FBW-HTT-ND](https://www.digikey.com/product-detail/en/newhaven-display-intl/NHD-C12864A1Z-FSW-FBW-HTT/NHD-C12864A1Z-FSW-FBW-HTT-ND/3767469)
- Atmel ATmega328p Microcontroller: DigiKey part number [ATMEGA328P-AU-ND](https://www.digikey.com/product-detail/en/microchip-technology/ATMEGA328P-AU/ATMEGA328P-AU-ND/1832260)
- MAX31855 Thermocouple-to-Digital: DigiKey part number [MAX31855KASA+-ND](https://www.digikey.com/product-detail/en/maxim-integrated/MAX31855KASA-/MAX31855KASA--ND/2591564)
- Fixed Inductor: DigiKey part number SRR1280A-331KCT-ND
- MOSFET: DigiKey part number 497-3157-1-ND
- 5V Regulator: DigiKey part number LM2575S-5.0/NOPB-ND
- 3.3V Regulator: DigiKey part number 497-13047-1-ND
- Bridge Rectifier: DigiKey part number 641-1348-5-ND

# Programming

The board is compatible with the Arduino Pro 3.3V 8MHz bootloader. Use an ISP programmer (or Arduino as ISP) to write the bootloader, then you can upload the firmware (or write your own) through Arduino, PlatformIO or your environment of choice.

# Design

My version of the T962A came with a Solid State Relay to control the lamps, and a simple BT139 triac circuit to control the fan. Based on another controller board by [ESTechnical](http://www.estechnical.co.uk/reflow-controllers/reflow-oven-controller-t962a-complete-upgrade-package), I opted to use a dual SSR configuration to control the lamp and fan. These SSRs are driven by MOSFET switches on the control board. Two thermocouple-to-digital ICs provide the temperature readings, and the board also takes care of driving the tiny PCB fan and the status indicators. A smaller, more compact LCD screen based on the ST7565 was used in favor of the parallel interface of the included LCD screen. The board is controlled by an Atmel ATmega328p microcontroller.

I opted to forgo an onboard FTDI solution for now, that will be included in a later revision. There is a header for an FTDI cable on the board, used for programming and communication with the PC-based software.
