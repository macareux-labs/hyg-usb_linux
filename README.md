# hyg-usb_linux

hyg-usb is a small temperature and relative humidity sensor YOU CAN HACK. It is based on a PIC 16F1455
and a Si7021 sensor and comes with a preinstalled firmware ( see http://www.macareux-labs.com for details )

This command line tool will allow you to read its data and control the three leds interactively under Linux.

## Installation

### Dependencies

To compile this project, you will need to install:

	1. libusb-1.0
	2. bzip2
	3. iconv

If you are running Debian or one of its siblings, just type as root :

	apt-get install libusb-1.0-0-dev

iconv and bzip2 should already be installed on most systems.

### Getting the sources

The sources are available on github: https://github.com/macareux-labs/hyg-usb_linux
You can retrieve them as a zip file with your favorite web browser or,better, just use git :

	git clone https://github.com/macareux-labs/hyg-usb_linux.git

### Compiling and installing

Enter the 'build' directory :

	$ cd build

Run CMake, pointing it to the parent directory (where CMakeLists.txt is located)

	$ cmake ..

Now run the make command to build the executable and the manpages:

	$ make

Install the programm into the system to use

	$ sudo make install

## Synopsis

       hyg-usb [-h] [-v] [-T] [-H] [-R] [-Y] [-G]
               [-r led_mode] [-y led_mode] [-g led_mode]

Each led can be in three modes : ON, OFF and AUTO. 
When set to AUTO, a given LED will blink each time a new measurement is made by the sensor.


## Options

       -h     display a short help text and exit

       -v     dispay version number and exit

       -T     display current temperature in Celcius

       -H     display current relative humidity in %

       -R     display current mode for red led

       -G     display current mode for green led

       -Y     display current mode for yellow led

       -r led_mode
              set mode for red led ( can be ON,OFF or AUTO )

       -g led_mode
              set mode for green led ( can be ON,OFF or AUTO )

       -y led_mode
              set mode for yellow led ( can be ON,OFF or AUTO )


when used without any option, hyg-usb will display temperature and relative humidity.
														       

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request !

## History

Feb 2016: V0.1 - First release

## License

This project is placed under the MIT License. See LICENSE file for details

