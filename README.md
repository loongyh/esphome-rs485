# esphome-rs485

Connect various UART devices over a multipoint/multidrop interface such as RS485 to an ESP8266 or
ESP32 using the [ESPHome](https://esphome.io) framework.

## Features
* Control devices from and poll state updates to Home Assistant.
* State updates of default 500ms interval between devices.

## Requirements
* ESPHome 1.18.0 or greater
* All RS485 devices connected on the same bus must be using the same baud rate

## Supported Microcontrollers
This library should work on most ESP8266 or ESP32 platforms. It has been tested
with the following MCUs:
* Generic ESP-01S board (ESP8266)
* WeMos D1 Mini (ESP8266)
* Generic ESP32 Dev Kit (ESP32)

## Supported Devices
* Dooya Shade RS485 Motor DM35EQL/J & DM45EQL/J
* Chenyang RS485 Window Opener CP12, CP14, CP16, CP31, CP95
* GM40 RS485 Curtain Motor

## Usage
### Step 1: Link up the devices and microcontroller to a multipoint bus.

Connect the RS485 devices and microcontroller together.
You can use a UART-RS485 board such as the SP3485 to interface the microcontroller to the RS485 devices.

### Step 2: Use ESPHome 1.18.0 or higher

The code in this repository makes use of the
[external components](https://esphome.io/components/external_components.html) feature in the 1.18.0
version of ESPHome.

### Step 3: Add this repository as an external component

Add the uart_multi component along with the devices you need to your ESPHome config:

```yaml
external_components:
  - source: github://loongyh/esphome-rs485
    components: [ uart_multi, chenyang, dooya, ... ]
```

Define the UART pins your UART-RS485 board is connected to, as well as the baud rate of the RS485 devices:

```yaml
uart:
  tx_pin: 17
  rx_pin: 16
  baud_rate: 9600
```

### Step 4: Configure the devices

Chenyang:

```yaml
cover:
  - platform: chenyang
    name: Chenyang Window
    address: 0xFF
    device_class: window

# Optional binary sensor for when window position is unknown on startup.
# An automation can be triggered to open and close the window to initialize the positioning.
binary_sensor:
  - platform: chenyang
    unknown_position:
      name: Chenyang Window Unknown Position
```

Dooya:

```yaml
cover:
  - platform: dooya
    name: Dooya Shade
    address: 0xFEFE
    device_class: shade
```

GM40:

```yaml
cover:
  - platform: gm40
    name: GM40 Curtain
    address: 0x00
    device_class: curtain

# Optional binary sensor for when curtain position is unknown on startup.
# An automation can be triggered to open and close the curtain to initialize the positioning.
binary_sensor:
  - platform: gm40
    unknown_position:
      name: GM40 Curtain Unknown Position
```

On ESP8266 you'll need to disable logging to serial because it conflicts with
the UART:

```yaml
logger:
  baud_rate: 0
```

On ESP32 you can change `hardware_uart` to `UART1` or `UART2` and keep logging
enabled on the main serial port.

# Example configuration

Below is an example configuration which will include wireless strength
indicators and permit over the air updates. You'll need to create a
`secrets.yaml` file inside of your `esphome` directory with entries for the
various items prefixed with `!secret`.

```yaml
substitutions:
  name: rs485test
  friendly_name: Test RS485


esphome:
  name: ${name}
  platform: ESP8266
  board: esp01_1m
  # Boards tested: ESP-01S (ESP8266), Wemos D1 Mini (ESP8266); ESP32 Wifi-DevKit2

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "${friendly_name} Fallback Hotspot"
    password: !secret fallback_password

captive_portal:

# Enable logging
logger:
  # ESP8266 only - disable serial port logging, as the HeatPump component
  # needs the sole hardware UART on the ESP8266
  baud_rate: 0

# Enable Home Assistant API
api:

ota:

# Sync time with Home Assistant.
time:
  - platform: homeassistant
    id: homeassistant_time

external_components:
  - source: github://loongyh/esphome-rs485

uart:
  tx_pin: 17
  rx_pin: 16
  baud_rate: 9600

uart_multi:
  update_interval: 500ms

cover:
  - platform: dooya
    name: Dooya Shade
    address: 0xFEFE
    device_class: shade
  - platform: chenyang
    name: Chenyang Window
    address: 0xFF
    device_class: window
  - platform: gm40
    name: GM40 Curtain
    address: 0x00
    device_class: curtain
```

## Configuration variables that affect this library directly

* *hardware\_uart* (_Optional_): the hardware UART instance to use for
  communcation with the multipoint bus. On ESP8266, only `UART0` is usable. On ESP32,
  `UART0`, `UART1`, and `UART2` are all valid choices. Default: `UART0`
* *baud\_rate* (_Required_): Serial BAUD rate used to communicate with the
  multipoint bus.
* *update\_interval* (_Optional_, time): The delay polling for state updates
  between each device, in milliseconds. Smaller values at the expense of a
  busier bus and more collisions. Larger values at the expense of slower state
  updates. Default: 500ms

## Contributing devices

Happy to accept contributions of new multipoint devices.
