#==============================================================================
#
#   file        : CMakeLists.txt
#   copyright   : (C) 2019 Joe Thompson
#   email       : beaglejoe@users.sourceforge.net
#   web         : www.speed-dreams.org
#   version     : $Id: CMakeLists.txt 7604 2021-07-15 01:22:05Z beaglejoe $
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
INCLUDE(../cmake/macros.cmake)

SET(SD_PACKAGEDIR "${CMAKE_BINARY_DIR}/packages" CACHE PATH "Location for the created installers")
MARK_AS_ADVANCED(SD_PACKAGEDIR)

IF(OPTION_OSGGRAPH)
   IF((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))

      # Grab the first OSG library and get its path
      LIST(GET OPENSCENEGRAPH_LIBRARIES 0 _OSGLIB)
      GET_FILENAME_COMPONENT(_OSG_LIB_PATH "${_OSGLIB}" PATH)

      # the PlugIns are in a subdir based on OPENSCENEGRAPH_VERSION
      SET(OSG_PLUGIN_DIR osgPlugins-${OPENSCENEGRAPH_VERSION})

      # Glob all the .so files in the Plugins dir ie (_OSG_LIB_PATH/OSG_PLUGIN_DIR)
      FILE(GLOB OSG_PLUGINS "${_OSG_LIB_PATH}/${OSG_PLUGIN_DIR}/*${CMAKE_SHARED_MODULE_SUFFIX}")

      # Loop through and install the plugins
      FOREACH(pi ${OSG_PLUGINS})
         INSTALL(FILES ${pi} DESTINATION ./${SD_BINDIR}/PlugIns/${OSG_PLUGIN_DIR})
         GET_FILENAME_COMPONENT(fname ${pi} NAME)
         SET_PROPERTY(GLOBAL APPEND PROPERTY SD_OSG_PLUGIN_LIST "${SD_BINDIR}/PlugIns/${OSG_PLUGIN_DIR}/${fname}")
      ENDFOREACH()

   ENDIF((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))
ENDIF(OPTION_OSGGRAPH)
#==============================================================================

IF((APPLE) AND ("${CMAKE_INSTALL_PREFIX}" MATCHES "\\.app$"))
   GET_PROPERTY(SD_MODULE_LIST_ITEMS GLOBAL PROPERTY SD_MODULE_LIST)
   GET_PROPERTY(SD_ROBOT_LIST_ITEMS GLOBAL PROPERTY SD_ROBOT_LIST)
   GET_PROPERTY(SD_OSG_PLUGIN_LIST_ITEMS GLOBAL PROPERTY SD_OSG_PLUGIN_LIST)
   #GET_PROPERTY(SD_EXE_LIST_ITEMS GLOBAL PROPERTY SD_EXE_LIST)

   # Loop through the GLOBAL lists and build up the 'extra libs' for FIXUP_BUNDLE()
   SET(_loadables )

   foreach(_MODULE ${SD_MODULE_LIST_ITEMS})
      LIST(APPEND _loadables "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${_MODULE}")
   endforeach()

   foreach(_ROBOT ${SD_ROBOT_LIST_ITEMS})
      LIST(APPEND _loadables "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${SD_LIBDIR}/drivers/${_ROBOT}/${_ROBOT}${CMAKE_SHARED_MODULE_SUFFIX}")
   endforeach()

   foreach(_PLUGIN ${SD_OSG_PLUGIN_LIST_ITEMS})
      #message(STATUS "${_PLUGIN}")
      LIST(APPEND _loadables "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${_PLUGIN}")
   endforeach()

   #================
   # TODO figure out if this is needed...
   MESSAGE(STATUS "TODO: Hard coded loadable libraries...") 
   IF(NOT OPTION_OFFICIAL_ONLY)
      LIST(APPEND _loadables "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${SD_LIBDIR}/lib/libephemeris.dylib")
   ENDIF(NOT OPTION_OFFICIAL_ONLY)
   LIST(APPEND _loadables "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${SD_LIBDIR}/lib/liblearning.dylib")
   #================

   # Build up the 'dirs' collection for FIXUP_BUNDLE() to use for finding dependencies
   SET(_search_dirs )
   IF(CMAKE_PREFIX_PATH)
      LIST(APPEND _search_dirs "${CMAKE_PREFIX_PATH}")
   ENDIF(CMAKE_PREFIX_PATH)
   LIST(APPEND _search_dirs "${CMAKE_INSTALL_PREFIX}/Frameworks")
   LIST(APPEND _search_dirs "${CMAKE_INSTALL_PREFIX}/MacOS")
   LIST(APPEND _search_dirs "${CMAKE_INSTALL_PREFIX}/${SD_LIBDIR}/lib")
   LIST(APPEND _search_dirs "${CMAKE_INSTALL_PREFIX}/${SD_BINDIR}/Plugins/${OSG_PLUGIN_DIR}")



   # Build up the 'app' path for FIXUP_BUNDLE()
   SET(SD_FU_BUNDLE "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}/${SD_BINDIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
   
   SET(SD_FU_PLUGINS ${_loadables})

   SET(SD_FU_SEARCHDIRS ${_search_dirs})


   CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/fixbundle.cmake.in
                  ${CMAKE_BINARY_DIR}/fixbundle.cmake @ONLY)

   INSTALL(SCRIPT "${CMAKE_BINARY_DIR}/fixbundle.cmake")

   #ADD_CUSTOM_TARGET(${_UNINST_TGT_NAME} "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" )
ENDIF()

#==============================================================================
IF(NOT MSVC)
   ADD_CUSTOM_TARGET(package_tars)
   ADD_CUSTOM_COMMAND(TARGET package_tars
                        COMMAND "${CMAKE_CURRENT_LIST_DIR}/sources/build.sh" "${VERSION}-r${SVN_REV}"
                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                        COMMENT "Create tar source archives")
ENDIF(NOT MSVC)
