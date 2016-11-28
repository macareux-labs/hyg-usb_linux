#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>

#include <libusb.h>

#include "config.h"

#define HYGUSB_VID      0x04D8
#define HYGUSB_PID      0xF2C4
#define MAX_DEVICE_ID   25

#define FALSE 0
#define TRUE !FALSE

typedef union {
	struct {
		u_int8_t msb ;
		u_int8_t lsb ;
	} ;
	u_int16_t value ;
} __DWORD ;

typedef struct __attribute__ ( ( packed ) ) {
	__DWORD hyg ;
	__DWORD temp ;

	u_int8_t green_led ;
	u_int8_t yellow_led ;
	u_int8_t red_led ;
	signed char parity ;
} __INTERNAL_DEVSTATE ;

typedef struct {

	enum { __DISPLAY_TEXT = 0, __DISPLAY_JSON= 1, __DISPLAY_CSV = 2} mode;

	int display_temp ;
	int display_hyg  ;
	int display_red  ;
	int display_green ;
	int display_yellow  ;

} __DISPLAY_SPECS;

int
parity_check ( __INTERNAL_DEVSTATE __dev_state ) {
	unsigned char *data ;

	if ( __dev_state.parity == 0 ) {
		/* Original firmware ? */
		/* can only confirm known data are ok */

		if ( __dev_state.green_led != 0
		     && __dev_state.green_led != 1
		     && __dev_state.green_led != 0xFF )
			return FALSE ;

		if ( __dev_state.yellow_led != 0
		     && __dev_state.yellow_led != 1
		     && __dev_state.yellow_led != 0xFF )
			return FALSE ;

		if ( __dev_state.red_led != 0
		     && __dev_state.red_led != 1
		     && __dev_state.red_led != 0xFF )
			return FALSE ;

		return TRUE ;
	} else {
		data = ( unsigned char * ) &__dev_state ;
		return data[7] ==
			( data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^
			  data[5] ^ data[6] ) ;
	}
}

void
print_usage (  ) {
	fprintf ( stdout, "Usage: hyg-usb [options]\n" ) ;
	fprintf ( stdout, "Options:\n" ) ;
	fprintf ( stdout, " -h	           \t display this help and exit\n" ) ;
	fprintf ( stdout,
		  " -v	           \t display version number and exit\n" ) ;
	fprintf ( stdout,
		  " -r [ON|OFF|AUTO]  \t set mode for red led to ON,OFF or AUTO\n" ) ;
	fprintf ( stdout,
		  " -y [ON|OFF|AUTO]  \t set mode for yellow led to ON,OFF or AUTO\n" ) ;
	fprintf ( stdout,
		  " -g [ON|OFF|AUTO]  \t set mode for green led to ON,OFF or AUTO\n" ) ;
	fprintf ( stdout,
		  " -s 001:002        \t select device by bus and device number\n" ) ;
	fprintf ( stdout,
		  " -T                \t display current temperature\n" ) ;
	fprintf ( stdout,
		  " -H                \t display current relative humidity\n" ) ;
	fprintf ( stdout,
		  " -R                \t display current mode of red led\n" ) ;
	fprintf ( stdout,
		  " -Y                \t display current mode of yellow led\n" ) ;
	fprintf ( stdout,
		  " -G                \t display current mode of green led\n" ) ;
	fprintf ( stdout,
		  " -c                \t display as CSV data\n" ) ;
	fprintf ( stdout,
		  " -j                \t display as JSON data\n" ) ;
	fprintf ( stdout, "\n" ) ;
	fprintf ( stdout, "Examples:\n" ) ;
	fprintf ( stdout,
		  "  hyg-usb -r ON                \t turns red led on \n" ) ;
	fprintf ( stdout,
		  "  hyg-usb -r ON -g OFF -y AUTO \t turns red led on, green led off, and yellow led will blink \n" ) ;
	fprintf ( stdout,
		  "  hyg-usb -T                   \t display current temperature\n" ) ;
}

int
parse_led_setstate ( char *led, char *optarg, char arg ) {
	if ( strcmp ( optarg, "ON" ) == 0 ) {
		*led = 'A' ;
	} else if ( strcmp ( optarg, "OFF" ) == 0 ) {
		*led = 'B' ;
	} else if ( strcmp ( optarg, "AUTO" ) == 0 ) {
		*led = 'C' ;
	} else {
		fprintf ( stderr, "Incorrect value for option -%c\n", arg ) ;
		fprintf ( stderr, "Possible values are ON, OFF and AUTO\n" ) ;
		return EXIT_FAILURE ;
	}
	return 0 ;
}

const char*
get_led_status ( int state ) {
	switch ( state ) {
	case 0x01:
		return "ON" ;
		break ;
	case 0x00:
		return "OFF" ;
		break ;
	case 0xFF:
		return "AUTO" ;
		break ;
	default:
		return "Unknown" ;
		break ;
	}
}

void
display_led_status ( const char *sPrefix, const char *sLedId, int state ) {
	fprintf ( stdout, "[%s] %s LED : %s\n",
		  sPrefix, sLedId, get_led_status ( state ) ) ;
}

int
process_device ( libusb_device_handle * handle, const char *deviceId,
		 char red_led, char green_led, char yellow_led,
		 __DISPLAY_SPECS *ds ) {

	int i, r ;
	int transferred ;

	__INTERNAL_DEVSTATE __dev_state ;
	unsigned char data_out[12] ;

	int hyg_data ;
	int temp_data ;

	float hyg ;
	float temp ;

	data_out[0] = green_led ;
	data_out[1] = yellow_led ;
	data_out[2] = red_led ;
	data_out[3] = 'A' ;

	int count = 0 ;
	int success = 0 ;

	do {
		count++ ;

		// Send Data
		r = libusb_interrupt_transfer ( handle, 0x01, data_out, 4,
						&transferred, 5000 ) ;
		if ( r != 0 ) {
			fprintf ( stderr,
				  "Could not send data to hyg-usb (%s). Exiting.\n",
				  libusb_error_name ( r ) ) ;
			return EXIT_FAILURE ;
		}

		if ( transferred < 4 ) {
			fprintf ( stderr, "Short write to hyg-usb. Exiting.\n" ) ;
			return EXIT_FAILURE ;
		}

		// Read Data
		r = libusb_interrupt_transfer ( handle, 0x81,
						( unsigned char * )
						&__dev_state,
						sizeof ( __INTERNAL_DEVSTATE ),
						&transferred, 5000 ) ;

		if ( r != 0 ) {
			fprintf ( stderr,
				  "Could not read data from hyg-usb (%s). Retrying ...\n",
				  libusb_error_name ( r ) ) ;
			continue ;
		}

		if ( transferred < 8 ) {
			fprintf ( stderr,
				  "Short read from hyg-usb. Retrying ...\n" ) ;
			continue ;
		}

		if ( !parity_check ( __dev_state ) ) {
			fprintf ( stderr,
				  "Parity Check Failed. Retrying ...\n" ) ;
			continue ;
		}

		success = 1 ;

	} while ( !success && ( count < 3 ) ) ;


	if ( !success ) {

		fprintf ( stderr,
			  "Still failing after %d attempts. Exiting.\n",
			  count ) ;
		return EXIT_FAILURE ;
	}

	// Calculate human readable value for hygro / temp
	hyg = 125.0 * ntohs ( __dev_state.hyg.value ) / 65536.0 - 6.0 ;
	temp = 175.72 * ntohs ( __dev_state.temp.value ) / 65536.0 - 46.85 ;

	if ( ds -> mode == __DISPLAY_TEXT ) {
		// *** Display Hyg
		if ( ds -> display_hyg )
			fprintf ( stdout, "[%s] Hyg  : %6.2f%%\n", deviceId, hyg ) ;

		// *** Display Temp
		if ( ds -> display_temp )
			fprintf ( stdout, "[%s] Temp : %6.2f°C\n", deviceId, temp ) ;

		// *** Display Green Led status
		if ( ds -> display_green ) {
			display_led_status ( deviceId, "Green ",
					     __dev_state.green_led ) ;
		}
		// *** Display Yellow Led status
		if ( ds -> display_yellow ) {
			display_led_status ( deviceId, "Yellow",
					     __dev_state.yellow_led ) ;
		}
		// *** Display Red Led status
		if ( ds -> display_red ) {
			display_led_status ( deviceId, "Red   ",
					     __dev_state.red_led ) ;
		}
	} else {
		if ( ds -> mode == __DISPLAY_JSON ) {

			fprintf ( stdout, "\"%s\" : { \"measures\": { \""
				  "temperature\" : %5.2f, \"hygrometry\" : %5.2f },"
				  " \"leds\" : { \"green\": \"%s\", \"yellow\":"
				  " \"%s\", \"red\" : \"%s\" } }",
				  deviceId, temp, hyg,
				  get_led_status ( __dev_state.green_led ),
				  get_led_status ( __dev_state.yellow_led ),
				  get_led_status ( __dev_state.red_led )

				) ;

		} else {
			/* CSV Output */
			fprintf ( stdout, "%s,%5.2f,%5.2f,%s,%s,%s\n",
				  deviceId, temp, hyg,
				  get_led_status ( __dev_state.green_led ),
				  get_led_status ( __dev_state.yellow_led ),
				  get_led_status ( __dev_state.red_led )

				) ;

		}
	}

	return EXIT_SUCCESS ;
}

int
main ( int argc, char **argv, char **envv ) {
	libusb_device **devs ;
	libusb_device *dev ;
	libusb_device_handle *handle = NULL ;

	char red_led = 'D' ;
	char green_led = 'D' ;
	char yellow_led = 'D' ;

	__DISPLAY_SPECS ds = { 0, 0, 0, 0, 0, 0 } ;

	int selectBus = 0, selectAddress = 0 ;
	int i, r, m ;
	ssize_t cnt ;

	char sDeviceAddress[MAX_DEVICE_ID] ;
	// *** CLI options parsing

	opterr = 0 ;

	while ( ( i = getopt ( argc, argv, "vhr:y:g:THRYGs:cj" ) ) != -1 ) {
		switch ( i ) {
		case 'v':
			fprintf ( stdout, "hyg-usb %d.%d (Linux)\n",
				  VERSION_MAJOR, VERSION_MINOR ) ;
			return EXIT_SUCCESS ;
		case 'h':
			print_usage (  ) ;
			return EXIT_SUCCESS ;
		case 'T':
			ds . display_temp = 1 ;
			break ;
		case 'H':
			ds . display_hyg = 1 ;
			break ;
		case 'R':
			ds . display_red = 1 ;
			break ;
		case 'Y':
			ds . display_yellow = 1 ;
			break ;
		case 'G':
			ds . display_green = 1 ;
			break ;
		case 'c':
			ds . mode = __DISPLAY_CSV ;
			break ;
		case 'j':
			ds . mode = __DISPLAY_JSON ;
			break ;
		case 'r':
			if ( parse_led_setstate ( &red_led, optarg, i ) != 0 )
				return EXIT_FAILURE ;
			ds . display_red = 1 ;
			break ;
		case 'y':
			if ( parse_led_setstate ( &yellow_led, optarg, i ) !=
			     0 )
				return EXIT_FAILURE ;
			ds . display_yellow = 1 ;
			break ;
		case 'g':
			if ( parse_led_setstate ( &green_led, optarg, i ) !=
			     0 )
				return EXIT_FAILURE ;
			ds . display_green = 1 ;
			break ;
		case 's':
			if ( sscanf
			     ( optarg, "%d:%d", &selectBus, &selectAddress )
			     < 1 ) {
				fprintf ( stderr,
					  "Invalid syntax for device selection.\n" ) ;
				return EXIT_FAILURE ;
			}
			break ;
		case '?':
			if ( ( optopt == 'c' ) || ( optopt == 'l' ) ||
			     ( optopt == 'r' ) || ( optopt == 'y' )
			     || ( optopt == 'g' ) ) {
				fprintf ( stderr,
					  "Option -%c requires an argument.\n",
					  optopt ) ;
			} else if ( isprint ( optopt ) ) {
				fprintf ( stderr, "Unknown option `-%c'.\n",
					  optopt ) ;
			} else {
				fprintf ( stderr,
					  "Unknown option character `\\x%x'.\n",
					  optopt ) ;
			}
			fprintf ( stderr, "Try \'hyg-usb -h\' for help\n" ) ;
			return EXIT_FAILURE ;
		default:
			abort (  ) ;
		}
	}

	if ( optind < argc ) {
		fprintf ( stderr, "Wrong arguments\n" ) ;
		fprintf ( stderr, "Try \'hyg-usb -h\' for help\n" ) ;
		return EXIT_FAILURE ;
	}

	if ( optind == 1 ) {
		ds . display_temp = 1 ;
		ds . display_hyg = 1 ;
	}
	// *** USB Communication
	r = libusb_init ( NULL ) ;
	if ( r != 0 ) {
		fprintf ( stderr,
			  "Could not initialize libusb (%s). Exiting\n",
			  libusb_error_name ( r ) ) ;
		return EXIT_FAILURE ;
	}

	cnt = libusb_get_device_list ( NULL, &devs ) ;
	if ( cnt < 0 ) {
		fprintf ( stderr, "Could not find usb devices. Exiting\n" ) ;
		return ( int ) cnt ;
	}

	if ( ds . mode == __DISPLAY_JSON ) {
		fprintf ( stdout, "{ " ) ;
	}

	i = 0 ; m = 0 ;
	while ( ( dev = devs[i++] ) != NULL ) {
		struct libusb_device_descriptor desc ;

		r = libusb_get_device_descriptor ( dev, &desc ) ;
		if ( r < 0 ) {
			fprintf ( stderr,
				  "failed to get device descriptor (%s). Exiting\n",
				  libusb_error_name ( r ) ) ;
			return EXIT_FAILURE ;
		}

		if ( desc.idVendor == HYGUSB_VID
		     && desc.idProduct == HYGUSB_PID ) {
			int devBus = libusb_get_bus_number ( dev ) ;
			int devAddress = libusb_get_device_address ( dev ) ;

			if ( ( selectBus > 0 && devBus != selectBus ) ||
			     ( selectAddress > 0
			       && devAddress != selectAddress ) )
				continue ;

			r = libusb_open ( dev, &handle ) ;
			if ( r != 0 ) {
				fprintf ( stderr,
					  "Could not open device (%s), exiting\n",
					  libusb_error_name ( r ) ) ;
				return EXIT_FAILURE ;
			}

			r = libusb_claim_interface ( handle, 0 ) ;
			if ( r != 0 ) {
				fprintf ( stderr,
					  "Could not claim usb interface (%s). Exiting\n",
					  libusb_error_name ( r ) ) ;
				return EXIT_FAILURE ;
			}

			m ++ ; // Matched one device

			if ( m > 1 ) {
				if ( ds . mode == __DISPLAY_JSON ) {
					fprintf ( stdout, "," ) ;
				} else {
					if ( ds . mode == __DISPLAY_TEXT ) {
						fprintf ( stdout, "\n" ) ;
					}
				}
			}

			if ( desc.iSerialNumber > 0 ) {
				r = libusb_get_string_descriptor_ascii
					( handle, desc.iSerialNumber,
					  sDeviceAddress, MAX_DEVICE_ID ) ;

				if ( r < 0 ) {
					snprintf ( sDeviceAddress, 20,
						   "%03d:%03d", devBus,
						   devAddress ) ;
				}
			} else {
				snprintf ( sDeviceAddress, 20, "%03d:%03d",
					   devBus, devAddress ) ;
			}

			r = process_device ( handle, sDeviceAddress, red_led,
					     green_led, yellow_led, &ds ) ;

			libusb_close ( handle ) ;
		}
	}

	libusb_free_device_list ( devs, 1 ) ;

	if ( ds . mode == __DISPLAY_JSON ) {
		fprintf ( stdout, " }\n" ) ;
	}

	if ( handle == NULL ) {
		fprintf ( stderr, "No device(s) found.\n" ) ;
		fprintf ( stderr,
			  "Please check hyg-usb is plugged and you are authorized to use USB\n" ) ;
		return EXIT_FAILURE ;
	}

	libusb_exit ( NULL ) ;

	return r ;
}

/* Local Variables:    */
/* mode: c             */
/* c-basic-offset: 8   */
/* indent-tabs-mode: t */
/* End:                */
