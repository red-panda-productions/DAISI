
INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES 
	BrakeDecision.cpp
	CarController.cpp
	CarInfo.cpp
	DecisionMaker.cpp
	DecisionTuple.cpp
	DriveSituation.cpp
	EnvironmentInfo.cpp
	FileDataStorage.cpp
	InterventionExecutorAlwaysIntervene.cpp
	InterventionExecutorAskFor.cpp
	InterventionExecutorIndication.cpp
	InterventionExecutorNoIntervention.cpp
	InterventionExecutorPerformWhenNeeded.cpp
	InterventionFactory.cpp
	Mediator.cpp
	PlayerInfo.cpp
	Recorder.cpp
	SDAConfig.cpp
	SocketBlackBox.cpp
	SQLDatabaseStorage.cpp
	SteerDecision.cpp
	TrackPosition.cpp
	BlackBoxData.cpp
)

SET(_INLINES
	DecisionMaker.inl
	FileDataStorage.inl
	Mediator.inl
	SocketBlackBox.inl)

SET(_HEADERS 
	BrakeDecision.h
	CarController.h
	CarInfo.h
	ConfigEnums.h
	DecisionMaker.h
	DecisionTuple.h
	DriveSituation.h
	EnvironmentInfo.h
	FileDataStorage.h
	IDataStorage.h
	IDecision.h
	InterventionExecutor.h
	InterventionExecutorAlwaysIntervene.h
	InterventionExecutorAskFor.h
	InterventionExecutorIndication.h
	InterventionExecutorNoIntervention.h
	InterventionExecutorPerformWhenNeeded.h
	InterventionFactory.h
	Mediator.h
	PlayerInfo.h
	Recorder.h
	SDAConfig.h
	SocketBlackBox.h
	SQLDatabaseStorage.h
	SteerDecision.h
	TrackPosition.h
	BlackBoxData.h
)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability frontend)

ADD_SQLITE3_INCLUDEDIR()

SD_ADD_LIBRARY(backend STATIC ${_SOURCES} ${_HEADERS} ${_INLINES})
SET_TARGET_PROPERTIES(backend PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(backend frontend)

ADD_SDLIB_LIBRARY(backend tgf portability)

ADD_MSGPACK_PACKAGE(backend)

ADD_IPCLIB_PACKAGE(backend)

ADD_SQLITE3_LIBRARY(backend)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backend TARGETS backend)