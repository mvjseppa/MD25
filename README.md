Linux kernel driver for Robot Electronics MD25 motor controller in i2c mode.
Supports setting both motor speeds and reading both encoder values
through sysfs interface.

Currently the driver can only be loaded as a kernel module and it is
Raspberry Pi 2 specific.

Purpose of this project is to learn about Linux device drivers.
In a normal use case a user space application would be sufficient.

For more info about MD25:
https://www.robot-electronics.co.uk/htm/md25tech.htm
