# heart2heart-arduino
A project for connecting two (or more) arduinos via websockets to show some love.

## What is this?
Imagine having two boxes with a button each.  
Pressing on the button on one box activates the other box.  
Activating, in this case, means a heart is moving out of a slot in the top of the box and LEDs are flashing red, mimicking a heartbeat.

This is not all though, because long pressing the button switches between one of two "modes".
#### Mode 1
* The heart moves up, the LEDs are flashing like a heartbeat and the heart moves down again after a few seconds.
#### Mode 2
* The heart moves up and stays up. The LEDs start flashing less intrusively and just fade in and out once every few seconds.  
* The button on this box has to get pressed to move the heart down again, which in turn moves it up at the other box until that button is pressed and so on.

Once the mode is switched on one of the boxes, the mode is updated at the other box too to keep them in sync.  
Also every time a button is pressed, that box's LEDs flash briefly to indicate a successful user interaction.

There is also a "nightmode" switch at the back which disables the LEDs and makes the servo move very slowly to reduce noise.

## How it works
Each box contains an arduino that is connected to WiFi. Once the arduino receives power, it securely connects to a [server](https://github.com/tsomic/heart2heart) via a websocket and a JWT.  
The server forwards any message to every connected client except the one that sent the message. Each message tells the other box(es) which action was performed.
* **pressed** - whether it was a press or a mode change
* **mode** - which mode the box is currently in

According to this data, each receiving box then performs any actions this may trigger.

## What you need for the project
* A publicly accessible server to run the [backend](https://github.com/tsomic/heart2heart) on
### Electronics
* An ESP8266 Arduino. I used a WEMOS D1 Mini ESP8266, but any ESP8266 arduino should work
* A servo, I used an SG90
* Two addressable LED strips with 32 LEDs each that can be daisy chained. It is probably easiest to buy a shortenable LED strip and cut it. I used [this one](https://www.amazon.de/dp/B08JJ77F1S)
* A push button
* A switch
* A power supply port, I used USB-C (USB is recommended to allow for flashing of the arduino after it is encased)

Of course you need to increase the part numbers if you plan on making more than one box, which I recommend.

### Parts
* 3D-Printed parts (3D model files are not yet published)
* A thin, stable rod to hold the heart up
* Some acrylic glass
* (optional) diffusing foil to put between LEDs and acrylic glass

## Setup
### Software
You need to add your credentials in [heart2heart-arduino.ino](https://github.com/tsomic/heart2heart-arduino/blob/7c176dcd3313ef321c702467c5e1538366b4cd9e/heart2heart-arduino.ino).  
This includes your local WiFi credentials (WIFI_SSID and WIFI_PASSWORD) as well as your backend servers IP-address (HOST) and port (PORT).

The backend server also requires a JSON Web Token to authenticate each user. You add it to the configuration by setting HEADERS to "token: [JWT]", where [JWT] is your token.  
To find out more about where to get this token, check out the [backend project on GitHub](https://github.com/tsomic/heart2heart).

You can delete lines 9 and 10.

### Hardware
The pins I used are the following:
* **4** - LED data
* **5** - Servo
* **13** - Button
* **14** - Switch

Servo and LED should receive their power directly from the power supply to not stress the arduino.

I will upload a schmematic shortly.
