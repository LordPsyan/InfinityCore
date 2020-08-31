# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(sources_Debugging "")

if( SERVERS )
  include_directories(
    ${CMAKE_SOURCE_DIR}/src/common/Debugging
  )
  if (WIN32)
      set(sources_Debugging
        ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.cpp
        ${CMAKE_SOURCE_DIR}/src/common/Debugging/WheatyExceptionReport.h
      )
  else(WIN32)
    if (UNIX)
	set(sources_Debugging
        ${CMAKE_SOURCE_DIR}/src/common/Debugging/UnixDebugger.cpp
        ${CMAKE_SOURCE_DIR}/src/common/Debugging/UnixDebugger.h
      )
    endif (UNIX)
  endif(WIN32)

  add_subdirectory(genrev)
  add_subdirectory(framework)

  add_subdirectory(common)
  add_subdirectory(server)
else()
  if( TOOLS )
    add_subdirectory(collision)
  endif()
endif()

if(TOOLS)
  add_subdirectory(tools)
endif(TOOLS)
