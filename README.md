Door Sensor 935
===============

Arduino and Google App Engine python app for tracking a door's opening and closing. The Arduino setup uses 
a Normally-Opened magnetic switch to detect door state, and a Wifly shield to access home access point. 

Initially had built it using a Normally-Closed reed sensor, but quickly discovered I'd run out of batteries much too quickly. 
![](http://github.com/ncoronges/DoorSensor935/raw/master/assets/doorsensor.jpg) 

Changed the design so that the unit doesnt power up until the door is opened, by using a normally-open switch, and wiring the battery directly with the normally-open magnetic switch. 
![](http://distilleryimage11.s3.amazonaws.com/8aa946402cc111e2892d22000a1fc2ac_7.jpg)


