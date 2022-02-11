INCLUDE(../../cmake/macros.cmake)

# User settings.
SET(_CONFIG_USER_FILES logging.xml raceengine.xml screen.xml graph.xml sound.xml osghudconfig.xml)

IF(OPTION_WEBSERVER)
   SET(_CONFIG_USER_FILES ${_CONFIG_USER_FILES} webserver.xml)
ENDIF(OPTION_WEBSERVER)

#IF(OPTION_CLIENT_SERVER)
   #SET(_CONFIG_USER_FILES ${_CONFIG_USER_FILES} networking.xml)
#ENDIF(OPTION_CLIENT_SERVER)

# User settings.
   SD_INSTALL_FILES(DATA config USER config FILES ${_CONFIG_USER_FILES}) 


# Other config files.
SET (_OTHER_CONFIG_FILES raceresults.xsl)
SD_INSTALL_FILES(DATA config FILES ${_OTHER_CONFIG_FILES})

# Fictive = no-output target, for having source files available in IDEs.
ADD_CUSTOM_TARGET(config_xml SOURCES ${_CONFIG_USER_FILES} ${_OTHER_CONFIG_FILES})

# Race modes.
ADD_SUBDIRECTORY(raceman)
