Custom Keyboard Arduino Code

Copyright (c) 2017 [James Schmitz](http://ixora.io/)

This Arduino code is intended for use in a custom made keyboard using the Keyboard library. This was initiated as part of a class at ITP Camp 2016. The class was taught by [Claire Kearney-Volpe](http://www.takinglifeseriously.com/index.html) and [Ben Light](http://blightdesign.com/).

There are 3 Arduino sketches:

* keyboard_diagnostics - Useful for when nothing is working with your shift registers. The idea is instead of flipping the clock and latch pins on and off, set one of them to HIGH and keep it there. Then you can effectively use a multimeter to diagnose a bad connection.
* keyboard_test_code - Useful for testing and development work. Each keypress is identified with a simple message identifying which shift register and input was pressed.
* keyboard_production_code - This is the final product. It properly handles multiple key presses and key repeats. Key repeats can be adjusted with at the operating system. Code also detects (some) hardware problems and will signal by blinking the Arduino's built-in LED.
