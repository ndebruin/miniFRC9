# AlfredoConnect-Receive
*v1.0.0*

This library for Arduino and ESP32 parses input from [AlfredoConnect](https://github.com/AlfredoElectronics/AlfredoConnect-Desktop), a desktop application for sending keyboard and gamepad input over serial.

## Getting started

### Installation

1. **Get the Arduino IDE**. If you don't already have it, download it from the [Arduino website](https://www.arduino.cc/en/main/software) and install it.

2. **Download this repository as a ZIP**. Click [here][download] to start the download, or click the green button on this page that says `Code`, then `Download ZIP`.

3. **Add this library to the Arduino IDE**. In the Arduino IDE, Click `Sketch` > `Include Library` > `Add .ZIP Library...`, and select the ZIP file you downloaded.

### ESP32

This tutorial will guide you through uploading an example program to your ESP32 that toggles an LED using AlfredoConnect.

You should already have the Arduino IDE with this library added. If not, follow the [installation](#installation) tutorial.

1. **Configure the Arduino IDE to upload to an ESP32.** Check out [this board configuration tutorial](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/) for instructions on how to do that. STOP AFTER STEP 2 OF 'Testing the Installation'.

2. **Open the `LEDControlSerialESP32` example**. In the Arduino IDE, go to `File` > `Examples` > `AlfredoConnect-Receive` > `LEDControlSerialESP32`. This will open a new window with the example opened. You can close the old one.

3. **Connect your ESP32 and select the COM port**. Connect your ESP32 to your computer using a micro USB cable. In the Arduino IDE, select the corresponding COM port under `Tools` > `Port`. The correct COM port may say `(Silicon Labs)` next to it. If none of them do, you can unplug and replug the USB to see which COM port disappears and reappears. If you cannot find any COM port for the ESP32, try installing the appropriate driver from https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers.

4. **Select ESP32 as the board type**. In the Arduino IDE, go to `Tools` > `Board` > `ESP32 Arduino` and select `ESP32 Dev Module`. On older versions of Arduino, it may just be `Tools` > `Board` > `ESP32 Dev Module`.

5. **Upload `LEDControlSerialESP32` to your ESP32**. In the window with `LEDControlSerialESP32` opened, click the upload button (the arrow in the top left). If the console on the bottom of the window starts showing `Connecting....._____.....`, hold down the `BOOT` button on the ESP32.

6. **Download, install, and open AlfredoConnect**. Go to the [AlfredoConnect Releases page](https://github.com/AlfredoElectronics/AlfredoConnect-Desktop/releases) and follow the instructions to download the latest release. Run the downloaded executable, which will install AlfredoConnect on your computer. Then, run the installed program.

7. **Connect AlfredoConnect to the ESP32**. With the ESP32 connected to your computer, click the `Serial port` dropdown and select the same COM port you used to upload to the ESP32. Click `Open` and a connection to the ESP32 should start. Now, holding the `W` key on your keyboard should light up the built-in blue LED on the ESP32. Once you're done, be sure to click `Close`.

8. **Open the `LEDControlBluetoothESP32` example**. In the Arduino IDE, go to `File` > `Examples` > `AlfredoConnect-Receive` > `LEDControlBluetoothESP32`.

9. **Edit the example and upload**. Before uploading the example, it's recommended that you edit the line that says `bluetooth.begin("ESP32 Bluetooth")` to change `ESP32 Bluetooth` to a unique name for your ESP32. After you pair with the ESP32 under a certain name, you'll need to restart your computer to re-pair if the name changes, so use this same name every time you upload a program that uses Bluetooth to the ESP32. Once that's done, upload the example.

10. **Pair with the ESP32**. In your Windows 10 settings, go to `Devices` > `Bluetooth & other devices` > `Add Bluetooth or other device` > `Bluetooth`. If the ESP32 is powered and running the example we just uploaded, the name you assigned it should appear. Pair with it. It typically doesn't ask for a passcode, but if it does, the default is usually `1234`.

11. **Find the Bluetooth COM port of the ESP32**. Previously, AlfredoConnect communicated with the ESP32 using a COM port over USB. Bluetooth pairing with the ESP32 has created a new COM port on your machine for communicating with the ESP32 over Bluetooth. To find it, in the `Bluetooth & other devices` settings page, click `More Bluetooth options` on the right panel. Go to the `COM Ports` tab, and a port should be listed with the name you gave your ESP32. Remember the one with the direction `Outgoing`.

12. **Connect AlfredoConnect to the ESP32**. Open AlfredoConnect and click the `Serial port` dropdown. This time, select the COM port that we found in the previous step. Click `Open`, and AlfredoConnect will start communicating with the ESP33 over Bluetooth. Like before, hold `W` to turn on the blue built-in LED.

### Alfredo NoU2

This tutorial will guide you through uploading an example program to your ESP32 with an attached Alfredo NoU2 that spins motors using input from AlfredoConnect.

1. **Download the Alfredo-NoU2 library as a ZIP**. Click [here](https://github.com/AlfredoElectronics/Alfredo-NoU2/archive/refs/heads/master.zip) to start the download.

2. **Add this library to the Arduino IDE**. In the Arduino IDE, Click `Sketch` > `Include Library` > `Add .ZIP Library...`, and select the ZIP file you downloaded.

3. **Open an ESP32 Robot example**. In the Arduino IDE, go to `File` > `Examples` > `AlfredoConnect-Receive`. Open `ArcadeBotKeyboardESP32`.

4. **Edit the example and upload**. Before uploading, remember to change the line `bluetooth.begin("ESP32 Bluetooth")` to use the unique name you chose for your ESP32. Once that's done, upload the example.

5. **Open AlfredoConnect**. If you're using the gamepad example, connect the gamepad to your computer and press some buttons. It should show up in the `Gamepads` panel in AlfredoConnect. You can click on the label with the gamepad's name to see the axes and buttons associated with it. The numbers labelling the axes and buttons are the same ones used to access the axes and buttons in your ESP32 program.

6. **Connect to the ESP32**. Select the Bluetooth COM port for your ESP32 and click `Open`. The example should drive the motors connected to ports 1 and 2 on the NoU2 when you provide input. For the gamepad example, you can control them with axes 0 and 1. For the keyboard example, you can control them with the `WASD` keys.

After completing this tutorial, your computer should be all set up to develop code to control an ESP32 and NoU2 using AlfredoConnect.

[download]: https://github.com/AlfredoElectronics/AlfredoConnect-Receive/archive/refs/heads/master.zip
