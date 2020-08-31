# This file is part of the OregonCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if (USE_COREPCH)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB sources_localdir *.cpp *.h)

file(GLOB_RECURSE sources_game 
  Accounts/*.*
  AI/*.*
  AuctionHouse/*.*
  AuctionHouseBot/*.*
  Battlegrounds/*.*
  Chat/*.*
  Combat/*.*
  Conditions/*.*
  DataStores/*.*
  Entities/*.*
  Events/*.*
  Globals/*.*
  Grids/*.*
  Groups/*.*
  Guilds/*.*
  Handlers/*.*
  Instances/*.*
  Loot/*.*
  Mails/*.*
  Maps/*.*
  Miscellaneous/*.*
  Movement/*.*
  OutdoorPvP/*.*
  Pools/*.*
  Quests/*.*
  Reputation/*.*
  Scripting/*.*
  Server/*.*
  Skills/*.*
  Spells/*.*
  Storages/*.*
  Teleport/*.*
  Texts/*.*
  Tickets/*.*
  Tools/*.*
  Warden/*.*
  Weather/*.*
  World/*.*
)

file(GLOB_RECURSE sources_luaengine
 LuaEngine/*.*
)

# Create game-libary

if (USE_COREPCH AND MSVC)
  set(game_STAT_SRCS
    PrecompiledHeaders/gamePCH.cpp
    PrecompiledHeaders/gamePCH.h
  )
endif()

set(game_STAT_SRCS
  ${game_STAT_SRCS}
  ${sources_localdir}
  ${sources_game}
)

if( ELUNA )
  list(APPEND game_STAT_SRCS
   ${sources_luaengine}
  )
endif()
  
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/mersennetwister
  ${CMAKE_SOURCE_DIR}/dep/SFMT
  ${CMAKE_SOURCE_DIR}/dep/zlib

  ${CMAKE_SOURCE_DIR}/src/framework
  ${CMAKE_SOURCE_DIR}/src/common/Collision
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  
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
  
  ${CMAKE_CURRENT_SOURCE_DIR}
  
  ${CMAKE_CURRENT_SOURCE_DIR}/Accounts
  ${CMAKE_CURRENT_SOURCE_DIR}/AI
  ${CMAKE_CURRENT_SOURCE_DIR}/AI/CoreAI
  ${CMAKE_CURRENT_SOURCE_DIR}/AI/ScriptedAI
  ${CMAKE_CURRENT_SOURCE_DIR}/AI/SmartScripts
  ${CMAKE_CURRENT_SOURCE_DIR}/AuctionHouse
  ${CMAKE_CURRENT_SOURCE_DIR}/AuctionHouseBot
  ${CMAKE_CURRENT_SOURCE_DIR}/Battlegrounds
  ${CMAKE_CURRENT_SOURCE_DIR}/Battlegrounds/Zones
  ${CMAKE_CURRENT_SOURCE_DIR}/Chat
  ${CMAKE_CURRENT_SOURCE_DIR}/Chat/Channels
  ${CMAKE_CURRENT_SOURCE_DIR}/Combat
  ${CMAKE_CURRENT_SOURCE_DIR}/Conditions
  ${CMAKE_CURRENT_SOURCE_DIR}/DataStores
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Corpse
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Creature
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DynamicObject
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/GameObject
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Item
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Item/Container
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Object
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Object/Updates
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Pet
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Player
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Totem
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Transport
  ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Unit
  ${CMAKE_CURRENT_SOURCE_DIR}/Events
  ${CMAKE_CURRENT_SOURCE_DIR}/Globals
  ${CMAKE_CURRENT_SOURCE_DIR}/Grids
  ${CMAKE_CURRENT_SOURCE_DIR}/Grids/Cells
  ${CMAKE_CURRENT_SOURCE_DIR}/Grids/Dynamic
  ${CMAKE_CURRENT_SOURCE_DIR}/Grids/Notifiers
  ${CMAKE_CURRENT_SOURCE_DIR}/Groups
  ${CMAKE_CURRENT_SOURCE_DIR}/Guilds
  ${CMAKE_CURRENT_SOURCE_DIR}/Handlers
  ${CMAKE_CURRENT_SOURCE_DIR}/Instances
  ${CMAKE_CURRENT_SOURCE_DIR}/Loot
  ${CMAKE_CURRENT_SOURCE_DIR}/Mails
  ${CMAKE_CURRENT_SOURCE_DIR}/Maps
  ${CMAKE_CURRENT_SOURCE_DIR}/Miscellaneous
  ${CMAKE_CURRENT_SOURCE_DIR}/Movement
  ${CMAKE_CURRENT_SOURCE_DIR}/Movement/MovementGenerators
  ${CMAKE_CURRENT_SOURCE_DIR}/Movement/Spline
  ${CMAKE_CURRENT_SOURCE_DIR}/Movement/Waypoints
  ${CMAKE_CURRENT_SOURCE_DIR}/OutdoorPvP
  ${CMAKE_CURRENT_SOURCE_DIR}/Pools
  ${CMAKE_CURRENT_SOURCE_DIR}/Quests
  ${CMAKE_CURRENT_SOURCE_DIR}/Reputation
  ${CMAKE_CURRENT_SOURCE_DIR}/Scripting
  ${CMAKE_CURRENT_SOURCE_DIR}/Server
  ${CMAKE_CURRENT_SOURCE_DIR}/Server/Protocol
  ${CMAKE_CURRENT_SOURCE_DIR}/Skills
  ${CMAKE_CURRENT_SOURCE_DIR}/Spells
  ${CMAKE_CURRENT_SOURCE_DIR}/Spells/Aura
  ${CMAKE_CURRENT_SOURCE_DIR}/Storages
  ${CMAKE_CURRENT_SOURCE_DIR}/Teleport
  ${CMAKE_CURRENT_SOURCE_DIR}/Texts
  ${CMAKE_CURRENT_SOURCE_DIR}/Tickets
  ${CMAKE_CURRENT_SOURCE_DIR}/Tools
  ${CMAKE_CURRENT_SOURCE_DIR}/Warden
  ${CMAKE_CURRENT_SOURCE_DIR}/Warden/Modules
  ${CMAKE_CURRENT_SOURCE_DIR}/Weather
  ${CMAKE_CURRENT_SOURCE_DIR}/World
  
  
  ${CMAKE_SOURCE_DIR}/src/server/scripts/PrecompiledHeaders
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

if( ELUNA )
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/lualib
    ${CMAKE_SOURCE_DIR}/src/server/game/LuaEngine 
  )
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

if( ELUNA )
  add_library(game STATIC ${game_STAT_SRCS} ${lua_STAT_SRCS})
elseif( NOT ELUNA )
  add_library(game STATIC ${game_STAT_SRCS})
endif()



add_dependencies(game revision.h)

if( ELUNA )
  add_dependencies(game lualib)

  target_link_libraries(game
   lualib
  )
endif()


# Generate precompiled header
if (USE_COREPCH)
  if (CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(game ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/gamePCH.h)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_clang_pch(game "${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders" "gamePCH.h" "gamePCH.cpp")
  elseif (MSVC)
    add_native_precompiled_header(game ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/gamePCH)
  endif()
endif()

if( ELUNA )

  if( WIN32 )
    add_custom_command(TARGET game
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/lua_scripts/extensions/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/LuaEngine/extensions ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/lua_scripts/extensions/
    )
  endif()
  
  if( UNIX )
    install(DIRECTORY LuaEngine/extensions DESTINATION bin/lua_scripts/)
  elseif( WIN32 )
    install(DIRECTORY LuaEngine/extensions DESTINATION "${CMAKE_INSTALL_PREFIX}/lua_scripts/")
  endif()
 
endif()
