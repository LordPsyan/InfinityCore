# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# No recurse for common root becuase we have collision are using standalong cmake
file(GLOB sources_commonlocal 
  ${CMAKE_SOURCE_DIR}/src/common/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/*.h
)

file(GLOB_RECURSE sources_common
  ${CMAKE_SOURCE_DIR}/src/common/Configuration/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Configuration/*.h
  ${CMAKE_SOURCE_DIR}/src/common/Cryptography/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Cryptography/*.h
  ${CMAKE_SOURCE_DIR}/src/common/DataStores/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/DataStores/*.h
  ${CMAKE_SOURCE_DIR}/src/common/Logging/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Logging/*.h
  ${CMAKE_SOURCE_DIR}/src/common/Platform/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Platform/*.h
  ${CMAKE_SOURCE_DIR}/src/common/Threading/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Threading/*.h
  ${CMAKE_SOURCE_DIR}/src/common/Utilities/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/common/Utilities/*.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/Packets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Packets/*.h
  
  ${CMAKE_CURRENT_SOURCE_DIR}/DataStores/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DataStores/*.h
  
)


file(GLOB_RECURSE sources_database 
  ${CMAKE_SOURCE_DIR}/src/server/database/*.cpp 
  ${CMAKE_SOURCE_DIR}/src/server/database/*.h
)

file(GLOB sources_localdir *.cpp *.h)

#
# Build shared sourcelist
#

SET(shared_STAT_SRCS
  ${shared_STAT_SRCS}
  
  ${sources_commonlocal}
  ${sources_common}
  ${sources_database}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/mersennetwister
  ${CMAKE_SOURCE_DIR}/dep/sockets/include
  ${CMAKE_SOURCE_DIR}/dep/utf8cpp
  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src

  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_SOURCE_DIR}/src/common/Configuration
  ${CMAKE_SOURCE_DIR}/src/common/Cryptography
  ${CMAKE_SOURCE_DIR}/src/common/Cryptography/Authentication
  ${CMAKE_SOURCE_DIR}/src/common/DataStores
  ${CMAKE_SOURCE_DIR}/src/common/Logging
  ${CMAKE_SOURCE_DIR}/src/common/Platform
  ${CMAKE_SOURCE_DIR}/src/common/Threading
  ${CMAKE_SOURCE_DIR}/src/common/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/database
  ${CMAKE_SOURCE_DIR}/src/server/database/Database

  ${CMAKE_SOURCE_DIR}/src/server/game
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object/Updates
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Packets
  ${CMAKE_CURRENT_SOURCE_DIR}/Database
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_library(shared STATIC ${shared_STAT_SRCS})

target_link_libraries(shared
  ${ACE_LIBRARY}
)
