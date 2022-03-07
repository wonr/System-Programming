# 2018-2 System Programming(SCE311)

## Team HARD-BOILED

- [Demo Video(Youtube)](https://youtu.be/3D7TPvi5aac)

### Motivation

Conventional egg boiling machine used steam. It uses egg-frame therefore the number is limited. To solve these problems, we decided to use boiling water instead of steam. Besides, we wanted to make a machine that can be used in other places than simple egg boiling machine. So, our second goal was to make egg custard. Many ways to make an egg custard, we chose cooking in a double boiler.


### Difficulty

There were some difficulties and trial and error in making hard boiled. Initially, we tried to replace the modules according to the type of cooking we had configured in a modular format. However, this was very inefficient. So, we used a cooking filter to solve this problem. If we use a cooking filter, we can cook egg custard simply by adding a zip -lock bag without having to replace the module. In addition, it can also boil noodles and fry.


### Limited

There were some limitations in making hard boiled. The first is the servo motor power. Servo motor that can rotate 360 degrees were too weak. So, we changed to a motor that can rotate 180 degrees. Motors that can rotate 180 degrees have a strong force, but they can rotate 180 degrees, so the length may be short and lifting the cooking filter may be difficult. And the motor that can rotate 180 degrees is strong, but it is difficult to lift a large amount of eggs.

Second, there was a difficulty in mechanical design. Initially, the cooking filter was designed to be connected to the servo motor and lifted. However, if only one servo motor is used, the cooking filter shakes a lot and the servo motor load also increases. To solve these problems, we fixed the cooking filter in the pot and lifted the edge to solve the problem.


### How to use

* First, set the timer using the button sensor.
* When the timer is set, the temperature sensor checks the temperature and waits until the temperature becomes suitable for cooking.
* When the temperature reaches a suitable temperature for cooking, the servomotor operates to lower the cooking filter.
* Raise the cooking filter after waiting for the timer that was set in the beginning.
* When the cooking is finished, the buzzer rings and displays the status.

#### Exception handling

If the servo motor malfunctions or the line connected to the cooking filter is cut off, the gyro sensor attached to the cooking filter detects the condition and sounds an alarm and terminates the program.


### Sensor

* Switch Sensor

    We use two switch sensors to set up time for boiling eggs. If a switch is pushed, the entire boiling process starts.

* Buzzer Sensor

    We use buzzer sensor to let a user knows specific situation such as state of the progress and the machine. If some signal is transmitted, the buzzer rings loudly, so user can recognize this.

* Servo motor Sensor

    We use servo motor to lift up and down the basket containing eggs. If a user sets the time for boiling by pushing switch and the water temperature is over some degree, servo motor lowers basket during settled time.

* Temperature Sensor

    We use temperature sensor to measure water temperature. If the temperature beyond settled value, the sensor notifies servo motor to start boiling process.

* Gyro Sensor

    We use gyro sensor to detect the operational failure of machine. If the wire used for winding up cooking filter is derailed, gyro sensor perceives this condition and sends a signal to raspberry pi.

### Learn

* If you can, use a simple solution. Don’t curve too much.
* IoT device first cost is so cheap. Rest of price is labor cost.
* Don’t compromise with the trouble and don’t lose first goal.
* Don’t scary about Linux kernel it is just C code.
* If you want good embedded programmer, you must learn math and science.
* If you adapt Kalman filter, you can get more precise value of gyro sensor.


<!--END-->
