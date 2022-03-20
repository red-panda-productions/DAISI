INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp
        SocketBlackBox.cpp
        SteerDecision.cpp
        BrakeDecision.cpp
        SDAConfig.cpp
        DecisionMaker.cpp
        InterventionFactory.cpp
        SQLDatabaseStorage.cpp
        CarController.cpp
        DriveSituation.cpp
        DecisionTuple.cpp
        FileDataStorage.cpp
        InterventionExecutorAlwaysIntervene.cpp
        InterventionExecutorAskFor.cpp
        InterventionExecutorIndication.cpp
        InterventionExecutorNoIntervention.cpp
        InterventionExecutorPerformWhenNeeded.cpp
        EnvironmentInfo.cpp
        TrackPosition.cpp
        CarInfo.cpp
        PlayerInfo.cpp
        Recorder.cpp)

SET(_INLINES Mediator.inl
        DecisionMaker.inl
        SocketBlackBox.inl)

SET(_HEADERS CarInfo.h
        DecisionMaker.h
        DriveSituation.h
        EnvironmentInfo.h
        FileDataStorage.h
        ConfigEnums.h
        Mediator.h
        TrackPosition.h
        PlayerInfo.h
        SocketBlackBox.h
        DecisionTuple.h
        ../rppUtils/RppUtils.hpp
        SDAConfig.h
        InterventionFactory.h
        IDataStorage.h
        SQLDatabaseStorage.h
        InterventionExecutor.h
        InterventionExecutorAlwaysIntervene.h
        InterventionExecutorAskFor.h
        InterventionExecutorIndication.h
        InterventionExecutorNoIntervention.h
        InterventionExecutorPerformWhenNeeded.h
        CarController.h
        Recorder.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability frontend)

ADD_SQLITE3_INCLUDEDIR()

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS} ${_INLINES})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(backend frontend)

ADD_MSGPACK_PACKAGE(backend)

ADD_IPCLIB_PACKAGE(backend)

ADD_SQLITE3_LIBRARY(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)
