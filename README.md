# [PROJECT HAS MOVED] esp32-wendigo
Wendigo **will be** a Flipper Zero application with an intuitive user interface that identifies and monitors radio signals transmitted by personal devices - Wireless, Bluetooth Classic, and Bluetooth Low Energy. The application is composed of two discrete programs, a Flipper Zero program providing a user interface and analytics, and an ESP32 program that interfaces with the radio spectrum.

Unlike ESP32-Gravity (and many other ESP applications that run on/with Flipper), the ESP32 component of Wendigo **is not** designed to be run without a Flipper Zero. This allows a more robust and complete UI to be implemented on Flipper Zero, as opposed to other applications that perform all meaningful operations on the ESP32 and use Flipper Zero as a basic serial console. Performing all operations on the ESP32 allows the application to be used with any device able to establish a serial connection with the ESP32 (such as a laptop, or even a smartphone), but places severe limitations on how sophisticated the application and user interface can be.

You can find Wendigo at https://github.com/chris-bc/wendigo

