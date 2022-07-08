# iot-dust-meter
This is a project to transmit data by MQTT with low power by connecting the fine dust sensor and 18650 battery to the wemos d1 mini board!

![503-2_bb](https://user-images.githubusercontent.com/106683637/177984712-dec1d4cb-75da-4123-8438-540cbebef450.jpg)

Below is an explanation video! Sorry, it's in Korean!

[![Video Label](http://img.youtube.com/vi/rW2at4xO4T4/0.jpg)](https://youtu.be/rW2at4xO4T4?t=6386)

"dust-meter.ino" Upload the file to the wemos d1 mini board!

"node-red.flow" You can check the fine dust concentration by loading the file in Nord Red!

"csharp-winform" You can check the fine dust concentration by opening the file in visual studio!

"appinventor.aia" If you open the file with App Inventor, you can check the fine dust concentration!

1. Measure the pms7003 sensor value with the wemos d1 mini board!
2. Sensor data is sent to the server by MQTT!
3, After sending, it goes into deep sleep mode until the next upload!
4. Install an 18650 battery to ensure low power operation!
5. When the "리셋버튼" is pressed, the board wakes up from deep sleep!
6. If you press the "리셋버튼" while holding down the "설정버튼", the board operates in AP mode!
7. After connecting with a smartphone, you can set parameters by connecting to 192.168.4.1!
8. Parameters can adjust the id and password of the wifi router, the address and topic of the mqtt server, and the deep sleep interval!
9. After completing the setup, write the contents with spiffs and the board reboots!
