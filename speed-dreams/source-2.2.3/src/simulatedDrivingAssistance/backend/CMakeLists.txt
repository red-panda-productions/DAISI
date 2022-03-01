INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp)
SET(_HEADERS CarInfo.h
        DecisionMaker.h
        DriveSituation.h
        EnvironmentInfo.h
        InterventionType.h
        Mediator.h
        TrackPosition.h
        PlayerInfo.h)

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

ADD_MSGPACK_PACKAGE(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)

