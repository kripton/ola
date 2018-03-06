PiGPIO Plugin
=====================

This plugin uses the pigpio library and daemon to emit DMX frames
on the GPIO pins of one ore more RaspberryPis.
The pigpio daemon needs to be running as root in order for this to work.

## Config file: `ola-pigpio.conf`

`enabled = true`  
Enable this plugin (DISABLED by default).

### Per Device Settings

**Note:** Substitute `<device>` with the raspberry's IP and port, e.g.
`192.168.1.100:8888`.

`192.168.1.100:8888-MaxUniverses = 8`  
How many universes this device supports at maximum.
