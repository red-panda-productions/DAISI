INCLUDE(../../../cmake/macros.cmake)

SET(_RACE_FILES quickrace.xml)

SD_INSTALL_FILES(DATA config/raceman USER config/raceman FILES ${_RACE_FILES})

SET(_EXTRA_FILES extra/skill.xml)

SD_INSTALL_FILES(DATA config/raceman/extra USER config/raceman/extra
                 FILES ${_EXTRA_FILES})

# Fictive = no-output target, for having source files available in IDEs.
ADD_CUSTOM_TARGET(raceman SOURCES ${_RACE_FILES} ${_EXTRA_FILES})

