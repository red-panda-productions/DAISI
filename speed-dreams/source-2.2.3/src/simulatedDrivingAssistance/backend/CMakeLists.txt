INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Mediator.cpp
        SocketBlackBox.cpp
        SteerDecision.cpp
        BrakeDecision.cpp
        SDAConfig.cpp
        DecisionMaker.cpp
        InterventionFactory.cpp
        CarController.cpp
        DriveSituation.cpp
        DecisionTuple.cpp
        InterventionExecutorAlwaysIntervene.cpp
        InterventionExecutorAskFor.cpp
        InterventionExecutorIndication.cpp
        InterventionExecutorNoIntervention.cpp
        InterventionExecutorPerformWhenNeeded.cpp
        EnvironmentInfo.cpp
        TrackPosition.cpp
        CarInfo.cpp
        PlayerInfo.cpp)

SET(_INLINES Mediator.inl
        DecisionMaker.inl
        SocketBlackBox.inl)

SET(_HEADERS CarInfo.h
        DecisionMaker.h
        DriveSituation.h
        EnvironmentInfo.h
        ConfigEnums.h
        Mediator.h
        TrackPosition.h
        PlayerInfo.h
        SocketBlackBox.h
        DecisionTuple.h
        ../rppUtils/RppUtils.hpp
        SDAConfig.h
        InterventionFactory.h
        InterventionExecutor.h
        InterventionExecutorAlwaysIntervene.h
        InterventionExecutorAskFor.h
        InterventionExecutorIndication.h
        InterventionExecutorNoIntervention.h
        InterventionExecutorPerformWhenNeeded.h
        CarController.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability)

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS} ${_INLINES})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

ADD_MSGPACK_PACKAGE(backend)

ADD_IPCLIB_PACKAGE(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)
