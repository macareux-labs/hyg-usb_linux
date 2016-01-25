#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include "config.h"

void print_usage() {
	fprintf(stdout,"Usage: hyg_usb [options]\n" ) ;
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
	fprintf(stdout,"  hyg_usb -r ON                \t turns red led on \n");
	fprintf(stdout,"  hyg_usb -r ON -g OFF -y AUTO \t turns red led on, green led off, and yellow led will blink \n");
	fprintf(stdout,"  hyg_usb -T                   \t display current temperature\n");
}

int main( int argc, char** argv, char** envv ) {

	char red_led    = 'D' ;
	char green_led  = 'D' ;
	char yellow_led = 'D' ;

	int display_temp = 0 ;
	int display_hyg  = 0 ;
	int display_red = 0 ;
	int display_green = 0 ;
	int display_yellow = 0 ;

	int i ;
	opterr = 0 ;

        while ((i = getopt (argc, argv, "vhr:y:g:THRYG")) != -1) {
    		switch (i) {
      			case 'v':
				fprintf(stdout, "hyg_usb %d.%d (Linux)\n", VERSION_MAJOR, VERSION_MINOR ) ;
				return EXIT_SUCCESS ;
			case 'h':
				print_usage() ;
				return EXIT_SUCCESS ;

      			case 'c':
        			codepage = optarg;
        			break;
			case 'l':
				locale = optarg ;
				break;
      			case '?':
        			if ( (optopt == 'c') || (optopt == 'l') )
       					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        			else if (isprint (optopt))
          				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        			else
          				fprintf (stderr, "Unknown option character `\\x%x'.\n",optopt);

				fprintf(stderr,"Try \'hyg_usb -h\' for help\n") ; 
				return EXIT_FAILURE ;
      			default:
				abort();
      		}	
	}

	// FIXME 

        for (i = optind; i < argc; i++ )
      		printf ("Non-option argument %s\n", argv[i]);
	
	return EXIT_SUCCESS ;
}
