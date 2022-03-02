INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp
        Config.cpp
        DecisionMaker.cpp
        InterventionFactory.cpp)
SET(_HEADERS CarInfo.h
        DecisionMaker.h
        DriveSituation.h
        EnvironmentInfo.h
        ConfigEnums.h
        Mediator.h
        TrackPosition.h
        PlayerInfo.h
        Config.h
        InterventionFactory.h
        InterventionMaker.h)

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)
