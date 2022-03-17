INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp
        SocketBlackBox.cpp
        SteerDecision.cpp
        BrakeDecision.cpp
        Config.cpp
        DecisionMaker.cpp
        InterventionFactory.cpp
        SQLDatabaseStorage.cpp)

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
        Config.h
        InterventionFactory.h
        InterventionMaker.h
        IDataStorage.h
        SQLDatabaseStorage.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math)
ADD_SQLITE3_INCLUDEDIR()

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

ADD_MSGPACK_PACKAGE(backend)

ADD_IPCLIB_PACKAGE(backend)

ADD_SQLITE3_LIBRARY(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)
