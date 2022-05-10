INCLUDE(../../cmake/macros.cmake)

ENABLE_TESTING()

ADD_SUBDIRECTORY(backend)
ADD_SUBDIRECTORY(backendUnitTests)
ADD_SUBDIRECTORY(unitTestUtilities)
ADD_SUBDIRECTORY(unitTestUtilitiesTests)
ADD_SUBDIRECTORY(frontend)
ADD_SUBDIRECTORY(frontendUnitTests)
ADD_SUBDIRECTORY(rppUtilsTests)