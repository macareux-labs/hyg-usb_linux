### CMake file for hyg-usb-linux
###
###
###   - Jan 2016: Initial version <bulot@macareux-labs.com>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (hyg-usb)
set (hyg-usb_VERSION_MAJOR 0)
set (hyg-usb_VERSION_MINOR 1)

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )

set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )


# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
        set(var INSTALL_${p}_PATH)
        if(NOT IS_ABSOLUTE "${${var}}")
                set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
        endif()
endforeach()


### Search for Lib/Headers

include( GNUInstallDirs )

find_package( libusb-1.0 REQUIRED)

### Config File


configure_file ( "${PROJECT_SOURCE_DIR}/config.h.in"
		 "${PROJECT_SOURCE_DIR}/src/config.h" )


### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib/include"
	             "${CMAKE_CURRENT_BINARY_DIR}"
		     "${LIBUSB_1_INCLUDE_DIRS}" )

# link_directories( "${VISION_LIBRARY_DIR}" )


### File lists

file( GLOB src "src/*.c"  )
file( GLOB hdr "src/*.h"  "include/*.h" )


### Targets

add_executable( "hyg-usb" ${src} ${hdr} ) 


### Linking

target_link_libraries( "hyg-usb" ${LIBUSB_1_LIBRARIES} )

### Install 


install( TARGETS "hyg-usb"
	    RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
	    LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
            ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )

install(FILES "man/hyg-usb.1"
	DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/man1" COMPONENT doc )



