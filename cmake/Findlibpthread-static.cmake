# - Try to find a static version of libpthread
#
#  LIBPTHREAD_FOUND        - system has libpthread
#  LIBPTHREAD_LIBRARIES    - Link these to use libpthread

### libpthread detection

find_library(LIBPTHREAD_LIBRARY
  NAMES
    pthread 
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

set(LIBPTHREAD_LIBRARIES
    ${LIBPTHREAD_LIBRARY}
)

if (LIBPTHREAD_LIBRARIES)
   set(LIBPTHREAD_FOUND TRUE)
endif (LIBPTHREAD_LIBRARIES)

if (LIBPTHREAD_FOUND)
    message(STATUS " libpthread static libraries: ${LIBPTHREAD_LIBRARIES}")
else (LIBPTHREAD_FOUND)
  if (libpthread_static_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find a static version of libpthread")
  endif (libpthread_static_FIND_REQUIRED)
endif (LIBPTHREAD_FOUND)

# show the LIBPTHREAD_LIBRARIES variable only in the advanced view

mark_as_advanced( LIBPTHREAD_LIBRARIES)

