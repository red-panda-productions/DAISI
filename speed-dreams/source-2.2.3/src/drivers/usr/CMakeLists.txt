INCLUDE(../../../cmake/macros.cmake)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

SET(ROBOT_NAME "usr")

# Official-only USR interface.
SET(ROBOT_INTERFACE LEGACY WELCOME)

SET(ROBOT_SOURCES
    src/usr.cpp
    src/cubic.cpp
    src/cubic.h
    src/datalog.cpp
    src/datalog.h
    src/dataloghead.h
    src/driver.cpp
    src/driver.h
    src/filter.cpp
    src/filter.h
    #src/message.cpp
    #src/message.h
    src/MuFactors.cpp
    src/MuFactors.h
    src/MuFactorsSect.h
    src/MyCar.cpp
    src/MyCar.h
    src/MyParam.cpp
    src/MyParam.h
    src/MyTrack.cpp
    src/MyTrack.h
    src/MyTrackSeg.h
    src/opponent.cpp
    src/opponent.h
    src/opponents.cpp
    src/opponents.h
    src/Path.cpp
    src/Path.h
    src/PathMargins.cpp
    src/PathMargins.h
    src/PathMarginsSect.h
    src/PathSeg.h
    src/PathState.cpp
    src/PathState.h
    src/PathType.h
    src/pidcontroller.cpp
    src/pidcontroller.h
    src/pit.cpp
    src/pit.h
    src/spline.cpp
    src/spline.h
    src/Timer.h
    src/tires.cpp
    src/tires.h
    src/Utils.cpp
    src/Utils.h
    src/Vec2.h
    src/Vec2d.h
    src/Vec3.h
    src/Vec3d.h)

# Official-only USR instances.
SET(ROBOT_CLONES usr_36GP usr_67GP usr_ls1 usr_ls2 usr_mpa1 usr_mpa11 usr_mpa12 usr_rs usr_sc usr_trb1)

IF(NOT OPTION_OFFICIAL_ONLY)
   SET(ROBOT_CLONES ${ROBOT_CLONES} usr_mp10 usr_srw usr_lp1)
ENDIF(NOT OPTION_OFFICIAL_ONLY)

ROBOT_MODULE(NAME ${ROBOT_NAME} VERSION 3.0.0 SOVERSION 1.0.0
              INTERFACE ${ROBOT_INTERFACE}
              SOURCES ${ROBOT_SOURCES}
              CLONENAMES ${ROBOT_CLONES})

# For data associated to the ubiquirous USR robot module and its clones,
# see data/drivers/usr*/CMakeLists.txt
