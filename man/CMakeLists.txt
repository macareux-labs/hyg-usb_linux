### CMakefile for hyg-usb-linux
###
###
###   - Feb 2016: Initial version <bulot@macareux-labs.com>
###
###
###############################################################################

option ( INSTALL_MANPAGES "Generate and install manpages ?" ON )

if ( INSTALL_MANPAGES )




FIND_PROGRAM(NIX_BZIP2_PATH bzip2 PATHS /bin /usr/bin/ ENV PATH NO_DEFAULT_PATH )

FIND_PROGRAM(NIX_ICONV_PATH iconv PATHS /bin /usr/bin/ ENV PATH NO_DEFAULT_PATH )

if(NOT NIX_BZIP2_PATH)
	message(WARNING "Could not find 'bzip2' command. Manpages will not be compressed.")
endif(NOT NIX_BZIP2_PATH)


if(NOT NIX_ICONV_PATH)
	message(WARNING "Could not find 'iconv' command. Only UTF-8 manpages will be generated.")
endif(NOT NIX_ICONV_PATH)


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


### fr.ISO8859-1 manpage
######


if(NIX_ICONV_PATH)

	add_custom_target( "man_fr_iso" ALL rm -fr ./fr.ISO8859-1 && cp -r "${PROJECT_SOURCE_DIR}/man/fr" fr.ISO8859-1
		COMMAND  iconv -f UTF-8 -t ISO-8859-1
		fr.ISO8859-1/man1/hyg-usb.1 > fr.ISO8859-1/man1/hyg-usb.ISO && mv  fr.ISO8859-1/man1/hyg-usb.ISO  fr.ISO8859-1/man1/hyg-usb.1  
		COMMENT "Generating FR.ISO8859-1 manpage" )
	
	if(NIX_BZIP2_PATH)

		add_custom_target( "man_fr_iso.gz" ALL ${NIX_BZIP2_PATH} -k -f "fr.ISO8859-1/man1/hyg-usb.1" 
			COMMENT "Compressing fr.ISO8859-1 manpage" )

		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/fr.ISO8859-1/man1/hyg-usb.1.bz2"
			DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr.ISO8859-1/man1" COMPONENT doc )

	else(NIX_BZIP2_PATH)

		install(FILES "fr.ISO8859-1/man1/hyg-usb.1"
			DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/man/fr.ISO8859-1/man1" COMPONENT doc )
		
	endif(NIX_BZIP2_PATH)
endif(NIX_ICONV_PATH)


endif ( INSTALL_MANPAGES )
