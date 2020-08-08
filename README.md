# DOXYGEN Documentation  
https://orsoeric.github.io/2020-07-26-Longan-Screen-Class-Demo/main_8cpp.html
  
# 2020-07-26-Longan-Screen-Class-Demo
My objective for this project is to develop drivers for the Longan Nano evaluation board  
I need a lightweight LCD screen driver that uses little CPU and print characters on the screen asynchronously  
I need a time class to profile execution times and schedule tasks for the hardwired scheduler  
I added a LED class to practice with C++ scoping and use of combined header and implementation since I use lots of inline, and inline functions and template functions need to be declared alongside the header anyway
  
# 2020-07-26 Demos
1 - clear screen to random color every 500ms  
2 - print a character every 1ms  
3 - print a colored character every 1ms  
4 - print a string every 25ms  
5 - print a colored string every 25ms  
Youtube Video
[![VID](http://img.youtube.com/vi/NfgCHvb00Ao/0.jpg)](http://www.youtube.com/watch?v=NfgCHvb00Ao "Screen Driver Demo")
# 2020-07-31 Demos
6 - print number on screen with reserved space and left/right adjust  
7 - profiler with engineering number format, four significant digits and si suffix and time profiling and cpu usage of the demos  
8 - profiler with colors  
GIF of DEMO7 profiler  
<img src="https://user-images.githubusercontent.com/30684972/89022296-100f2c00-d322-11ea-85a3-86236ec6eb70.gif" alt="Longan Nano profiling Demo" height="200">  
Youtube Video  
[![VID](https://user-images.githubusercontent.com/30684972/89022806-e0acef00-d322-11ea-847f-03e5cac872fa.jpg)](http://www.youtube.com/watch?v=axC4QhXqJkA "Screen Driver Demo")  

# Conclusions  
The interrupts, clock system and DMA are more or less figured out  
The Screen driver uses 1.9% of the CPU when scheduled every 100us and while printing fast updating numbers with changing colors  
The driver for the LCD is divided in a Display class that handles the HAL, and a Screen class that handles the sprite based abstraction layer. This allows to massively reduce the bandwidth by not updating sprites already on screen and allow to hopefully change screen in the future without much trouble thanks to the ABI interface  
The Chrono class allows to measure time using the integrated 64bit 27MHz SysTick timer, and allow to build an hardwired scheduler for my tasks  
For the next step I'm going to build a template application I can start with for a fresh project  
My first application will be the development of the motor controller for my OrangeBot robotic platform, with the aim of increasing the precision of the controls, and give an healthy amount of feedback on the screen, including voltage, power, currents, encoders, errors and more  

