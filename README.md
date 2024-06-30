# BKIT-lab4
In lab 4 we will discuss about RTC (Read Time Clock) and IC RTC DS3231

# DS3231
# Information about bit century in DS3231

Current implementation of counter for DS3231 thread century bit following way:
0 - it is 1900 century
1 - it is 2000 century

The documentation of DS3231 does not specify the meaning of the bit, it only says that it will toggle when the years register overflows from 99 to 00

The problem is when you sync DS3231 in external linux machine (e.g. RaspberryPi ) using hwclock -w it will ignore this century bit.
Then reading time from Zephyr application will indicate wrong date.

My suggestion is set the meaning of zero in CENTURY bit in menuconfig:
e.g. CONFIG_DS3231_CENTURY=20
would be for CENTURY bit.
0 - it is 2000 century
1 - it is 2100 century

It would make it much easier for applications when you sync RTC clock by PC connected to ntp server and then use it in your embedded project with proper real time clock.
It would also be more flexible and accurate implementation according to datasheet of DS3231.
