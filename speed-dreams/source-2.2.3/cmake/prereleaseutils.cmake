
#==============================================================================
#
#   file        : prereleaseutils.cmake
#   copyright   : (C) 2021 Joe Thompson
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
# ================================================
function(check_cars carlist)

   #file(STRINGS carlist.txt _installed_cars)
   set(_installed_cars ${carlist})
   list(TRANSFORM _installed_cars STRIP)

   cars_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-base.lst" base_car_lst)
   cars_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-hq-cars-and-tracks.lst" hq_car_lst)
   cars_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-more-hq-cars-and-tracks.lst" more_hq_car_lst)
   cars_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-wip-cars-and-tracks.lst" wip_car_lst)
   cars_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-unmaintained.lst" unmaintained_car_lst)

   set(_packaged_cars )
   list(APPEND _packaged_cars ${base_car_lst} ${hq_car_lst} ${more_hq_car_lst} ${wip_car_lst} ${unmaintained_car_lst})

   # Verify that a car is only in one package

   #message(STATUS "Checking for duplicate entries...")

   list_get_duplicates(_packaged_cars _ret_duplicates)
   list(LENGTH _ret_duplicates _length)
   if(_length)
      message("Duplicate car(s) in .lst file(s)")
      dump_list("${_ret_duplicates}")
   else()
      message(STATUS "No duplicate car(s) in .lst file(s)")
   endif()
   #message("\n")

   # Verify that all installed cars are in a package
   #message(STATUS "Checking for installed cars that are NOT packaged...")

   compare_lists(_installed_cars _packaged_cars _ret_installed _ret_packaged)
   list(LENGTH _ret_installed _length)
   if(_length)
      message("${_length} Installed car(s) missing from .lst file(s)")
      dump_list("${_ret_installed}")
   else()
      message(STATUS "No installed car(s) missing from .lst file(s)")
   endif()
   #message("\n")

   # Verify that packages contain no uninstalled cars
   #message(STATUS "Checking packing lists for cars that are NOT installed...")

   list(LENGTH _ret_packaged _length)
   if(_length)
      message(".lst file(s) have car(s) that are NOT installed")
      dump_list("${_ret_packaged}")
   else()
      message(STATUS "No missing car(s) in .lst file(s)")
   endif()
   #message("\n")

endfunction()
# ================================================
function(check_tracks tracklist)

   set(_installed ${tracklist})
   list(TRANSFORM _installed STRIP)

   tracks_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-base.lst" base_track_lst)
   tracks_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-hq-cars-and-tracks.lst" hq_track_lst)
   tracks_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-more-hq-cars-and-tracks.lst" more_hq_track_lst)
   tracks_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-wip-cars-and-tracks.lst" wip_track_lst)
   tracks_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-unmaintained.lst" unmaintained_track_lst)

   #dump_list("${base_track_lst}")
   #dump_list("${_installed}")

   set(_packaged )
   list(APPEND _packaged ${base_track_lst} ${hq_track_lst} ${more_hq_track_lst} ${wip_track_lst} ${unmaintained_track_lst})

   # Verify that a track is only in one package

   #message(STATUS "Checking for duplicate entries...")

   list_get_duplicates(_packaged _ret_duplicates)
   list(LENGTH _ret_duplicates _length)
   if(_length)
      message("Duplicate track(s) in .lst file(s)")
      dump_list("${_ret_duplicates}")
   else()
      message(STATUS "No duplicate track(s) in .lst file(s)")
   endif()
   #message("\n")

   # Verify that all installed tracks are in a package
   #message(STATUS "Checking for installed tracks that are NOT packaged...")

   compare_lists(_installed _packaged _ret_installed _ret_packaged)
   list(LENGTH _ret_installed _length)
   if(_length)
      message( "${_length} Installed track(s) missing from .lst file(s)")
      dump_list("${_ret_installed}")
   else()
      message(STATUS "No installed track(s) missing from .lst file(s)")
   endif()
   #message("\n")

   # Verify that packages contain no uninstalled tracks
   #message(STATUS "Checking packing lists for tracks that are NOT installed...")

   list(LENGTH _ret_packaged _length)
   if(_length)
      message(".lst file(s) have track(s) that are NOT installed")
      dump_list("${_ret_packaged}")
   else()
      message(STATUS "No missing track(s) in .lst file(s)")
   endif()
   #message("\n")


endfunction()
# ================================================
function(check_robots robotlist)

   set(_installed ${robotlist})
   list(TRANSFORM _installed STRIP)

   robots_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-base.lst" base_lst)
   robots_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-hq-cars-and-tracks.lst" hq_lst)
   robots_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-more-hq-cars-and-tracks.lst" more_hq_lst)
   robots_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-wip-cars-and-tracks.lst" wip_lst)
   robots_from_lst_file("${CMAKE_SOURCE_DIR}/packaging/sources/sd-src-unmaintained.lst" unmaintained_lst)

   #dump_list("${base_lst}")
   #dump_list("${_installed}")

   set(_packaged )
   list(APPEND _packaged ${base_lst} ${hq_lst} ${more_hq_lst} ${wip_lst} ${unmaintained_lst})

   # Verify that a robot is only in one package

   #message(STATUS "Checking for duplicate entries...")

   list_get_duplicates(_packaged _ret_duplicates)
   list(LENGTH _ret_duplicates _length)
   if(_length)
      message("Duplicate robot(s) in .lst file(s)")
      dump_list("${_ret_duplicates}")
   else()
      message(STATUS "No duplicate robot(s) in .lst file(s)")
   endif()
   #message("\n")

   # Verify that all installed robots are in a package
   #message(STATUS "Checking for installed robots that are NOT packaged...")

   compare_lists(_installed _packaged _ret_installed _ret_packaged)
   list(LENGTH _ret_installed _length)
   if(_length)
      message("${_length} Installed robot(s) missing from .lst file(s)")
      dump_list("${_ret_installed}")
   else()
      message(STATUS "No installed robot(s) missing from .lst file(s)")
   endif()
   #message("\n")

   # Verify that packages contain no uninstalled robots
   #message(STATUS "Checking packing lists for robots that are NOT installed...")

   list(LENGTH _ret_packaged _length)
   if(_length)
      message(".lst file(s) have robot(s) that are NOT installed")
      dump_list("${_ret_packaged}")
   else()
      message(STATUS "No missing robot(s) in .lst file(s)")
   endif()
   #message("\n")


endfunction()
# ================================================
function(cars_from_lst_file  lst_file outlist)
   file(STRINGS ${lst_file} _file_content)
   foreach(_line ${_file_content})
      #message(STATUS "${_line}")
      string(STRIP "${_line}" _line)
      #message(STATUS "${_line}")
      string(TOLOWER "${_line}" _line)
      #message(STATUS "${_line}")
      string(FIND "${_line}" "data/cars/models/" _index)
      if(NOT ${_index} EQUAL -1)
         string(REPLACE "data/cars/models/" "" _car "${_line}")
         list(APPEND _car_list "${_car}")
         #message(STATUS "${_car}")
      endif()
      #message(STATUS "${_line}")
   endforeach()
   list(REMOVE_ITEM _car_list "cmakelists.txt")
   
   set(${outlist} ${_car_list} PARENT_SCOPE)
   #foreach(_car ${_car_list})
   #   message(STATUS "${_car}")
   #endforeach()

endfunction()
# ================================================
function(tracks_from_lst_file  lst_file outlist)
   file(STRINGS ${lst_file} _file_content)
   foreach(_line ${_file_content})
      #message(STATUS "${_line}")
      string(STRIP "${_line}" _line)
      #message(STATUS "${_line}")
      string(TOLOWER "${_line}" _line)
      #message(STATUS "${_line}")
      string(FIND "${_line}" "data/tracks/" _index)
      if(NOT ${_index} EQUAL -1)
         string(REPLACE "data/tracks/" "" _track "${_line}")
         string(FIND "${_line}" "cmakelists.txt" _index)
         if(${_index} EQUAL -1)
            list(APPEND _track_list "${_track}")
            #message(STATUS "${_track}")
         endif()
      endif()
      #message(STATUS "${_line}")
   endforeach()
   list(REMOVE_ITEM _track_list "cmakelists.txt")

   set(${outlist} ${_track_list} PARENT_SCOPE)
   #foreach(_track ${_track_list})
   #   message(STATUS "${_track}")
   #endforeach()

endfunction()
# ================================================
function(robots_from_lst_file  lst_file outlist)
   file(STRINGS ${lst_file} _file_content)
   foreach(_line ${_file_content})
      #message(STATUS "${_line}")
      string(STRIP "${_line}" _line)
      #message(STATUS "${_line}")
      string(TOLOWER "${_line}" _line)
      #message(STATUS "${_line}")
      string(FIND "${_line}" "src/drivers/" _index)
      if(NOT ${_index} EQUAL -1)
         string(REPLACE "src/drivers/" "" _robot "${_line}")
         string(FIND "${_robot}" "/" _index)
         if(${_index} EQUAL -1)
            list(APPEND _robot_list "${_robot}")
            #message(STATUS "\t${_robot}")
         endif()
         string(FIND "${_robot}" "/src" _index)
         if(NOT ${_index} EQUAL -1)
            # TODO These need to go to  Customizable robots binaries (Section /o "Basic mod Tools" SEC02)
            string(REPLACE "/src" "" _robot "${_robot}")
            list(APPEND _robot_list "${_robot}")
            #message(STATUS "\t\t${_robot}")
         endif()
      endif()

      string(FIND "${_line}" "data/drivers/" _index)
      if(NOT ${_index} EQUAL -1)
         string(REPLACE "data/drivers/" "" _robot "${_line}")
         string(FIND "${_line}" "cmakelists.txt" _index)
         if(${_index} EQUAL -1)
            list(APPEND _robot_list "${_robot}")
            #message(STATUS "\t\t\t${_robot}")
         endif()
      endif()
      #message(STATUS "${_line}")
   endforeach()
   list(REMOVE_ITEM _robot_list "cmakelists.txt")
#[[
   foreach(_robot ${_robot_list})
      message(STATUS "${_robot}")
   endforeach()
   message("\n")
]]
   list(REMOVE_DUPLICATES _robot_list)
   #message("\n")
   set(${outlist} ${_robot_list} PARENT_SCOPE)
#[[
   foreach(_robot ${_robot_list})
      message(STATUS "${_robot}")
   endforeach()
   message("\n")
]]

endfunction()
# ================================================
function(list_get_duplicates list_in ret_list_duplicates)
   set(_duplicates )
   list(TRANSFORM ${list_in} TOLOWER OUTPUT_VARIABLE _lc_list)
   list(SORT _lc_list)

   set(_prev_item "???")
   foreach(_item ${_lc_list})
      string(COMPARE EQUAL ${_item} ${_prev_item} _result)
      if(_result)
         list(APPEND _duplicates "${_item}")
      endif()
      set(_prev_item ${_item})
   endforeach()

set(${ret_list_duplicates} ${_duplicates} PARENT_SCOPE)

endfunction()
# ================================================
# compares 2 lists and returns Lists with ONLY the
# the unique items from each list
#
# example: 
# compare_lists(list_installed_cars list_packaged_cars _ret_installed _ret_packaged)
#
# ${_ret_installed} will contain the cars that were in ${list_installed_cars}
#     AND NOT in ${list_packaged_cars}
# ${_ret_packaged} will contain the cars that were in ${list_packaged_cars}
#     AND NOT in ${list_installed_cars}
#
# If both ${_ret_installed} and ${_ret_packaged} are empty
# the list were identical
# ================================================
function(compare_lists list_a list_b ret_list_a ret_list_b)

   list(TRANSFORM ${list_a} TOLOWER OUTPUT_VARIABLE _lc_a)
   list(TRANSFORM ${list_a} TOLOWER OUTPUT_VARIABLE _lc_a_2)
   list(TRANSFORM ${list_b} TOLOWER OUTPUT_VARIABLE _lc_b)

   list(REMOVE_ITEM _lc_a ${_lc_b})
   list(REMOVE_ITEM _lc_b ${_lc_a_2})

   #dump_list("${_lc_a}")
   #dump_list("${_lc_b}")

   set(${ret_list_a} ${_lc_a} PARENT_SCOPE)
   set(${ret_list_b} ${_lc_b} PARENT_SCOPE)
    
endfunction()
# ================================================
function(dump_list _list)
   foreach(_item ${_list})
      message("${_item}")
   endforeach()
endfunction()