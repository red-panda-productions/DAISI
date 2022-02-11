#==============================================================================
#
#   file        : packagemaker-dmg.cmake
#   copyright   : (C) 2020 Joe Thompson
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

IF((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))

MESSAGE(STATUS "TODO - need to clean this up.")
   #ADD_CUSTOM_TARGET(package_base DEPENDS install)
      # can't depend on built-in target install, so we ADD_CUSTOM_COMMAND
      # below to make sure install is done
   ADD_CUSTOM_TARGET(packing_install)

MESSAGE(STATUS "TODO - fix up the dependencies. Try to not run the install multiple times.")
#[[
   ADD_CUSTOM_TARGET(package_all DEPENDS package_base
                                         package_full)

   ADD_CUSTOM_TARGET(package_base DEPENDS install_base)
   ADD_CUSTOM_TARGET(package_full DEPENDS packing_install)
   ADD_CUSTOM_TARGET(install_base DEPENDS packing_install)
   ]]

   ADD_CUSTOM_TARGET(package_full)
   ADD_CUSTOM_TARGET(package_base)
   ADD_CUSTOM_TARGET(install_base)

   
   ADD_CUSTOM_COMMAND(TARGET packing_install
                      COMMAND "${CMAKE_COMMAND}" --build . --target install --config $<CONFIG>
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Building install...")

   ADD_CUSTOM_COMMAND(TARGET install_base
                        COMMAND "${CMAKE_COMMAND}" -P createbaseapp.cmake 
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Creating base app...")


   ADD_CUSTOM_COMMAND(TARGET package_base
                        COMMAND "${CMAKE_COMMAND}" -P packagebasedmg.cmake 
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Building base package...")


# COMMAND ${CMAKE_CPACK_COMMAND} -G "DragNDrop" --config ../CPackDMGFullConfig.cmake
   ADD_CUSTOM_COMMAND(TARGET package_full
                        COMMAND "${CMAKE_COMMAND}" -P packagefulldmg.cmake 
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        COMMENT "Building full package...")



ELSE((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))
      MESSAGE(WARNING "Custom DMG creation requires *.app")
ENDIF((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))