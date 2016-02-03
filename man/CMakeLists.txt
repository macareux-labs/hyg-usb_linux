### CMake file for hyg-usb-linux
###
###
###   - Feb 2016: Initial version <bulot@macareux-labs.com>
###
###
###############################################################################


FIND_PROGRAM(NIX_BZIP2_PATH bzip2 PATHS /bin /usr/bin/ ENV PATH NO_DEFAULT_PATH )

if(NOT NIX_BZIP2_PATH)
	message(WARNING "Could not find 'bzip2' command. Manpages will not be compressed.")
endif(NOT NIX_BZIP2_PATH)


### Default manpage 
######

if(NIX_BZIP2_PATH)

	add_custom_target( "man.gz" ALL cp -r "${PROJECT_SOURCE_DIR}/man/man1" .
		COMMAND ${NIX_BZIP2_PATH} -k -f "man1/hyg-usb.1" 
		COMMENT "Compressing default manpage" )

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/man1/hyg-usb.1.bz2"
		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/man1" COMPONENT doc )

else(NIX_BZIP2_PATH)

	install(FILES "man1/hyg-usb.1"
       		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/man1" COMPONENT doc )

endif(NIX_BZIP2_PATH)


### fr et fr.UTF-8 manpages
######


if(NIX_BZIP2_PATH)

	add_custom_target( "man_fr.gz" ALL cp -r "${PROJECT_SOURCE_DIR}/man/fr" .
		COMMAND ${NIX_BZIP2_PATH} -k -f "fr/man1/hyg-usb.1" 
		COMMENT "Compressing fr(UTF-8) manpage" )

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fr/man1/hyg-usb.1.bz2"
		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr/man1" COMPONENT doc )

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fr/man1/hyg-usb.1.bz2"
		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr.UTF-8/man1" COMPONENT doc )

else(NIX_BZIP2_PATH)

	install(FILES "fr/man1/hyg-usb.1"
       		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr/man1" COMPONENT doc )
	
	install(FILES "fr/man1/hyg-usb.1"
		DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr.UTF-8/man1" COMPONENT doc )


endif(NIX_BZIP2_PATH)






