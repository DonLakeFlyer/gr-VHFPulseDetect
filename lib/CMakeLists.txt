# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND VHFPulseDetect_sources
    pulse_detect__ff_impl.cc
)

set(VHFPulseDetect_sources "${VHFPulseDetect_sources}" PARENT_SCOPE)
if(NOT VHFPulseDetect_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT VHFPulseDetect_sources)

add_library(gnuradio-VHFPulseDetect SHARED ${VHFPulseDetect_sources})
target_link_libraries(gnuradio-VHFPulseDetect ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-VHFPulseDetect PROPERTIES DEFINE_SYMBOL "gnuradio_VHFPulseDetect_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-VHFPulseDetect PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-VHFPulseDetect
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)
