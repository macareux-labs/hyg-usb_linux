#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include <libusb.h>

#include "config.h"

#define HYGUSB_VID	0x04D8
#define HYGUSB_PID	0xF2C4


void print_usage() {
	fprintf(stdout,"Usage: hyg-usb [options]\n" ) ;
	fprintf(stdout,"Options:\n") ;
	fprintf(stdout," -h 	           \t display this help and exit\n"); 
	fprintf(stdout," -v 	           \t display version number and exit\n"); 
	fprintf(stdout," -r [ON|OFF|AUTO]  \t set red led to ON,OFF or AUTO\n");
	fprintf(stdout," -y [ON|OFF|AUTO]  \t set yellow led to ON,OFF or AUTO\n");
	fprintf(stdout," -g [ON|OFF|AUTO]  \t set green led to ON,OFF or AUTO\n");
	fprintf(stdout," -T                \t display current temperature\n");
	fprintf(stdout," -H                \t display current hygrometry\n");
	fprintf(stdout," -R                \t display current setting of red led\n");
	fprintf(stdout," -Y                \t display current setting of yellow led\n");
	fprintf(stdout," -G                \t display current setting of green led\n");
	fprintf(stdout,"\n") ;
	fprintf(stdout,"Examples:\n") ;
	fprintf(stdout,"  hyg-usb -r ON                \t turns red led on \n");
	fprintf(stdout,"  hyg-usb -r ON -g OFF -y AUTO \t turns red led on, green led off, and yellow led will blink \n");
	fprintf(stdout,"  hyg-usb -T                   \t display current temperature\n");
}

int main( int argc, char** argv, char** envv ) {

	libusb_device *dev = NULL ;
        libusb_device_handle* handle = NULL ;

	char red_led    = 'D' ;
	char green_led  = 'D' ;
	char yellow_led = 'D' ;

	int display_temp = 0 ;
	int display_hyg  = 0 ;
	int display_red = 0 ;
	int display_green = 0 ;
	int display_yellow = 0 ;

	int i,r ;
	int transferred ;
	unsigned char data_out[8] ;
	unsigned char data_in[8] ;
	
	int hyg_data ;
	int temp_data ;

	float hyg ;
	float temp ;


	// *** CLI options parsing 

	opterr = 0 ;

        while ((i = getopt (argc, argv, "vhr:y:g:THRYG")) != -1) {
    		switch (i) {
      			case 'v':
				fprintf(stdout, "hyg-usb %d.%d (Linux)\n", VERSION_MAJOR, VERSION_MINOR ) ;
				return EXIT_SUCCESS ;
			case 'h':
				print_usage() ;
				return EXIT_SUCCESS ;
			case 'T':
				display_temp = 1 ;
				break ;
			case 'H':
				display_hyg = 1 ;
				break ;
			case 'R':
				display_red = 1 ;
				break ;
			case 'Y':
				display_yellow = 1 ;
				break ;
			case 'G':
				display_green = 1 ;
				break ;
      			case 'r':
        			if ( strcmp(optarg,"ON") == 0 ) {
					red_led = 'A' ;
				} else if ( strcmp(optarg,"OFF") == 0  ) {
					red_led = 'B' ;
				} else if ( strcmp(optarg,"AUTO") == 0 ) {
					red_led = 'C'; 
				} else {
					fprintf(stderr, "Incorrect value for option -g\n") ;
					fprintf(stderr, "Possible values are ON, OFF and AUTO\n") ;
					return EXIT_FAILURE;
				}
        			break;
			case 'y':
        			if ( strcmp(optarg,"ON") == 0 ) {
					yellow_led = 'A' ;
				} else if ( strcmp(optarg,"OFF") == 0  ) {
					yellow_led = 'B' ;
				} else if ( strcmp(optarg,"AUTO") == 0 ) {
					yellow_led = 'C'; 
				} else {
					fprintf(stderr, "Incorrect value for option -g\n") ;
					fprintf(stderr, "Possible values are ON, OFF and AUTO\n") ;
					return EXIT_FAILURE;
				}
        			break;
			case 'g':
        			if ( strcmp(optarg,"ON") == 0 ) {
					green_led = 'A' ;
				} else if ( strcmp(optarg,"OFF") == 0  ) {
					green_led = 'B' ;
				} else if ( strcmp(optarg,"AUTO") == 0 ) {
					green_led = 'C'; 
				} else {
					fprintf(stderr, "Incorrect value for option -g\n") ;
					fprintf(stderr, "Possible values are ON, OFF and AUTO\n") ;
					return EXIT_FAILURE;
				}
        			break;
      			case '?':
        			if ( (optopt == 'c') || (optopt == 'l') || 
				     (optopt == 'r') || (optopt == 'y') || (optopt == 'g') ) {
       					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        			} else if (isprint (optopt)) {
          				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        			} else {
          				fprintf (stderr, "Unknown option character `\\x%x'.\n",optopt);
				}
				fprintf(stderr,"Try \'hyg-usb -h\' for help\n") ; 
				return EXIT_FAILURE ;
			default:
				abort();
      		}	
	}


	if ( optind < argc ) {
		fprintf(stderr,"Wrong arguments\n");
		fprintf(stderr,"Try \'hyg-usb -h\' for help\n") ; 
		return EXIT_FAILURE ;
	}


	// *** USB Communication
  
	r = libusb_init(NULL) ;
        if ( r != 0 ) {
		fprintf(stderr,"Could not initialize libusb. Exiting\n") ;
		return EXIT_FAILURE ;
	}

        handle = libusb_open_device_with_vid_pid ( NULL, HYGUSB_VID, HYGUSB_PID ) ;
	if ( handle == NULL ) {
		fprintf(stderr,"Device not found.\n");
		fprintf(stderr,"Please check hyg-usb is plugged and you are authorized to use USB\n") ;
		return EXIT_FAILURE ;
	}

	r = libusb_claim_interface (handle, 0) ;
	if ( r!= 0 ) {
		fprintf(stderr,"Could not claim usb interface. Exiting\n") ;
		return EXIT_FAILURE ;
	}
	
	// Send Data

	data_out[0] = green_led ;
	data_out[1] = yellow_led ;
	data_out[2] = red_led ;
	data_out[3] = 'A' ;

        r = libusb_interrupt_transfer(  handle, 0x01, data_out, 4,  &transferred, 5000 ) ;
	if ( r!= 0 ) {
		fprintf(stderr,"Could not send data to hyg-usb. Exiting.\n") ;
		return EXIT_FAILURE ;
	}
	if ( transferred < 4 ) {
		fprintf(stderr,"Short write to hyg-usb. Exiting.\n") ;
		return EXIT_FAILURE ;
	}

	// Read Data

	r = libusb_interrupt_transfer(  handle, 0x81, data_in, 8, &transferred, 5000 ) ;
	if ( r!= 0 ) {
		fprintf(stderr,"Could not read data from hyg-usb. Exiting.\n") ;
		return EXIT_FAILURE ;
	}
	if ( transferred < 8 ) {
		fprintf(stderr,"Short read from hyg-usb. Exiting.\n") ;
		return EXIT_FAILURE ;
	}

        libusb_exit(NULL ) ;
	 
	// *** Display Hyg
         
	hyg_data = data_in[0] << 8 ;
	hyg_data += data_in[1] ;

	hyg =  125.0 * hyg_data / 65536.0 - 6.0 ;
	
	if ( display_hyg )
	        fprintf(stdout,"Hyg : %.1f \n", hyg ) ;
	
	// *** Display Temp
	                  
	temp_data = data_in[2] << 8 ;
	temp_data += data_in[3] ;
	
	temp =  175.72 * temp_data / 65536.0 - 46.85 ;	

	if ( display_temp ) 
	        fprintf(stdout,"Temp : %.1f \n", temp ) ;
        
	// *** Display Green Led status
        
	if ( display_green ) {
		fprintf(stdout,"Green LED : ");
		switch (data_in[4]) {
			case 0x01:
				fprintf(stdout,"ON");
				break ;
			case 0x00:
				fprintf(stdout,"OFF");
				break ;
			case 0xFF:
				fprintf(stdout,"AUTO");
				break;
			default:
				fprintf(stdout,"Unknown (0x%02X)",data_in[4] & 0xFF );
				break;
		}
		fprintf(stdout,"\n") ;
	}

	// *** Display Yellow Led status


	if ( display_yellow ) {
		fprintf(stdout,"Yellow LED : ");
		switch (data_in[5]) {
			case 0x01:
				fprintf(stdout,"ON");
				break ;
			case 0x00:
				fprintf(stdout,"OFF");
				break ;
			case 0xFF:
				fprintf(stdout,"AUTO");
				break;
			default:
				fprintf(stdout,"Unknown (0x%02X)",data_in[5] & 0xFF );
				break;
		}
		fprintf(stdout,"\n") ;
	}

        // *** Display Red Led status
                                                                                                    
	if ( display_red ) {
		fprintf(stdout,"Red LED : ");
		switch (data_in[6]) {
			case 0x01:
				fprintf(stdout,"ON");
				break ;
			case 0x00:
				fprintf(stdout,"OFF");
				break ;
			case 0xFF:
				fprintf(stdout,"AUTO");
				break;
			default:
				fprintf(stdout,"Unknown (0x%02X)",data_in[6] & 0xFF );
				break;
		}
		fprintf(stdout,"\n") ;
	}

	
	return EXIT_SUCCESS ;
}
