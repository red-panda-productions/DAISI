#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Vorbis::vorbis" for configuration "Release"
set_property(TARGET Vorbis::vorbis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbis PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vorbis.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vorbis.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbis )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbis "${_IMPORT_PREFIX}/lib/vorbis.lib" "${_IMPORT_PREFIX}/bin/vorbis.dll" )

# Import target "Vorbis::vorbisenc" for configuration "Release"
set_property(TARGET Vorbis::vorbisenc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbisenc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vorbisenc.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vorbisenc.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbisenc )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbisenc "${_IMPORT_PREFIX}/lib/vorbisenc.lib" "${_IMPORT_PREFIX}/bin/vorbisenc.dll" )

# Import target "Vorbis::vorbisfile" for configuration "Release"
set_property(TARGET Vorbis::vorbisfile APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Vorbis::vorbisfile PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/vorbisfile.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/vorbisfile.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Vorbis::vorbisfile )
list(APPEND _IMPORT_CHECK_FILES_FOR_Vorbis::vorbisfile "${_IMPORT_PREFIX}/lib/vorbisfile.lib" "${_IMPORT_PREFIX}/bin/vorbisfile.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
