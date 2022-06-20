# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../cmake/macros.cmake)

ENABLE_TESTING()

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(backendUnitTests)
ADD_SUBDIRECTORY(unitTestUtilities)
ADD_SUBDIRECTORY(unitTestUtilitiesTests)
ADD_SUBDIRECTORY(frontend)
ADD_SUBDIRECTORY(frontendUnitTests)
ADD_SUBDIRECTORY(rppUtilsTests)

IF(OPTION_INTEGRATION_TESTS)
	ADD_SUBDIRECTORY(integrationTests)
ENDIF(OPTION_INTEGRATION_TESTS)

