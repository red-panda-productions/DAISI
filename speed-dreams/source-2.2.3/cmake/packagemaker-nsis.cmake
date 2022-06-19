#==============================================================================
#
#   file        : .cmake
#   copyright   : (C) 2019 Joe Thompson
#   email       : beaglejoe@users.sourceforge.net
#   web         : www.speed-dreams.org
#   version     : $Id:$
#
#==============================================================================
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#==============================================================================
FIND_PACKAGE(NSIS)

Message(STATUS "NSIS_FOUND = ${NSIS_FOUND}")
Message(STATUS "NSIS_MAKE_EXE = ${NSIS_MAKE_EXE}")
Message(STATUS "NSIS_INET_PLUGIN = ${NSIS_INET_PLUGIN}")


if(NSIS_FOUND AND NSIS_MAKE_EXE AND NSIS_INET_PLUGIN)

   SET(NSIS_INSTALL_DIR_FOUND FALSE)
   FILE(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}" NSIS_INSTALL_DIR)

   IF(NOT EXISTS ${NSIS_INSTALL_DIR})
        FILE(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}" NSIS_INSTALL_DIR)

        IF(NOT EXISTS ${NSIS_INSTALL_DIR})
            FILE(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}" NSIS_INSTALL_DIR)
        ELSE(NOT EXISTS ${NSIS_INSTALL_DIR})
            SET(NSIS_INSTALL_DIR_FOUND TRUE)
        ENDIF(NOT EXISTS ${NSIS_INSTALL_DIR})

   ELSE(NOT EXISTS ${NSIS_INSTALL_DIR})
        SET(NSIS_INSTALL_DIR_FOUND TRUE)
   ENDIF(NOT EXISTS ${NSIS_INSTALL_DIR})

   IF(NOT NSIS_INSTALL_DIR_FOUND)
        MESSAGE(STATUS "WARNING: NSIS_INSTALL_DIR MIGHT NOT BE CREATED YET")
        MESSAGE(STATUS "CURRENT FOLDER PATH: ${NSIS_INSTALL_DIR}")
        MESSAGE(STATUS "SEARCHED FOLDERS: ${CMAKE_INSTALL_PREFIX} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}")
        MESSAGE(STATUS "BUILD OF INSTALLER MIGHT NOT WORK, MAYBE RELOAD CMAKE AFTER BUILD IF NSIS FAILS")
   ENDIF(NOT NSIS_INSTALL_DIR_FOUND)

   CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/packaging/windows/readme_for_user.txt" 
                  "${CMAKE_BINARY_DIR}/packaging/readme_for_user.txt" COPYONLY)

   CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/packaging/windows/speed-dreams.ini" 
                  "${CMAKE_BINARY_DIR}/packaging/speed-dreams.ini" COPYONLY)

   CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/packaging/windows/speed-dreams.nsh.in" 
                  "${CMAKE_BINARY_DIR}/packaging/speed-dreams.nsh" @ONLY)

   CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/packaging/windows/speed-dreams-base.nsi" 
                  "${CMAKE_BINARY_DIR}/packaging/speed-dreams-base.nsi" @ONLY)

   #ADD_CUSTOM_TARGET(PACKAGE_BASE DEPENDS INSTALL)
      # can't depend on built-in target INSTALL, so we ADD_CUSTOM_COMMAND
      # below to make sure INSTALL is done
   ADD_CUSTOM_TARGET(PACKING_INSTALL)

   ADD_CUSTOM_TARGET(PACKAGE_ALL DEPENDS PACKAGE_BASE)

   ADD_CUSTOM_TARGET(PACKAGE_BASE DEPENDS PACKING_INSTALL PACKAGE_MKDIR)
   ADD_CUSTOM_TARGET(PACKAGE_MKDIR)
   
   ADD_CUSTOM_COMMAND(TARGET PACKING_INSTALL
                      COMMAND "${CMAKE_COMMAND}" --build . --target install --config $<CONFIG>
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      COMMENT "Building INSTALL...")

   ADD_CUSTOM_COMMAND(TARGET PACKAGE_BASE
                        COMMAND ${NSIS_MAKE_EXE} packaging/speed-dreams-base.nsi
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                        COMMENT "Building base package...")

   # Make sure that SD_PACKAGEDIR is not empty or PACKAGE_MKDIR will fail.
   # Treat empty string as current directory
   if(SD_PACKAGEDIR STREQUAL "")
     set(SD_PACKAGEDIR "." CACHE PATH "Location for the created installers" FORCE)
   endif(SD_PACKAGEDIR STREQUAL "")

   ADD_CUSTOM_COMMAND(TARGET PACKAGE_MKDIR
                        COMMAND "${CMAKE_COMMAND}" -E make_directory ${SD_PACKAGEDIR}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                        COMMENT "Create ${SD_PACKAGEDIR} directory for Packages dir")

else(NSIS_FOUND AND NSIS_MAKE_EXE AND NSIS_INET_PLUGIN)
   if(NOT NSIS_FOUND)
      Message(WARNING "NSIS NOT FOUND Packaging targets NOT added.")
   elseif(NOT NSIS_INET_PLUGIN)
      Message(WARNING "INetC.dll NOT FOUND Packaging targets NOT added.")
   endif(NOT NSIS_FOUND)
endif()