
INCLUDE(../../../../cmake/macros.cmake)

SET(_SOURCES 
	BrakeDecision.cpp
	CarController.cpp
	DecisionMaker.cpp
	DecisionTuple.cpp
	FileDataStorage.cpp
	InterventionExecutorAlwaysIntervene.cpp
	InterventionExecutorAskFor.cpp
	InterventionExecutorIndication.cpp
	InterventionExecutorNoIntervention.cpp
	InterventionExecutorPerformWhenNeeded.cpp
	InterventionFactory.cpp
	Mediator.cpp
	Recorder.cpp
	SDAConfig.cpp
	SocketBlackBox.cpp
	SQLDatabaseStorage.cpp
	SteerDecision.cpp
	BlackBoxData.cpp
	AccelDecision.cpp
	IPCPointerManager.cpp
)

SET(_INLINES
	DecisionMaker.inl
	FileDataStorage.inl
	Mediator.inl
	SocketBlackBox.inl
	IPCPointerManager.inl)

SET(_HEADERS 
	BrakeDecision.h
	CarController.h
	ConfigEnums.h
	DecisionMaker.h
	DecisionTuple.h
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
	Recorder.h
	SDAConfig.h
	SocketBlackBox.h
	SQLDatabaseStorage.h
	SteerDecision.h
	BlackBoxData.h
	AccelDecision.h
	IPCPointerManager.h
)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability frontend)

ADD_SQLITE3_INCLUDEDIR()

SD_ADD_LIBRARY(backendLint STATIC ${_SOURCES} ${_HEADERS} ${_INLINES})
SET_TARGET_PROPERTIES(backendLint PROPERTIES LINKER_LANGUAGE CXX)

TARGET_LINK_LIBRARIES(backendLint frontend)

ADD_SDLIB_LIBRARY(backendLint tgf portability)

ADD_MSGPACK_PACKAGE(backendLint)

ADD_IPCLIB_PACKAGE(backendLint)

ADD_SQLITE3_LIBRARY(backendLint)

ADD_MYSQL_PACKAGE(backendLint)

ADD_BOOST_PACKAGE(backendLint)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/backendLint TARGETS backendLint)