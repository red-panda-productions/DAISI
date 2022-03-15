INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp
        SocketBlackBox.cpp
        SteerDecision.cpp
        BrakeDecision.cpp
        SDAConfig.cpp
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
        SocketBlackBox.h
        ../rppUtils/RppUtils.hpp
        SDAConfig.h
        InterventionFactory.h
        InterventionMaker.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math)

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

ADD_MSGPACK_PACKAGE(backend)

ADD_IPCLIB_PACKAGE(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)
