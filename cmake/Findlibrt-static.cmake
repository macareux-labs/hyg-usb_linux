# - Try to find a static version of librt
#
#  LIBRT_FOUND        - system has librt
#  LIBRT_LIBRARIES    - Link these to use librt

### librt detection

find_library(LIBRT_LIBRARY
  NAMES
    rt 
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

set(LIBRT_LIBRARIES
    ${LIBRT_LIBRARY}
)

if (LIBRT_LIBRARIES)
   set(LIBRT_FOUND TRUE)
endif (LIBRT_LIBRARIES)

if (LIBRT_FOUND)
    message(STATUS " librt static libraries: ${LIBRT_LIBRARIES}")
else (LIBRT_FOUND)
  if (librt_static_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find a static version of librt")
  endif (librt_static_FIND_REQUIRED)
endif (LIBRT_FOUND)

# show the LIBRT_LIBRARIES variable only in the advanced view

mark_as_advanced( LIBRT_LIBRARIES)

