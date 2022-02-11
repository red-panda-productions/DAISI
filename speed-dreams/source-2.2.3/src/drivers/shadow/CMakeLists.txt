INCLUDE(../../../cmake/macros.cmake)

# The ubiquitous robot module and its clones.
SET(ROBOT_NAME "shadow")

# Official-only shadow interface.
SET(ROBOT_INTERFACE LEGACY WELCOME)

SET(ROBOT_SOURCES
				src/Avoidance.cpp
				src/Avoidance.h
				src/CarBounds2d.cpp
				src/CarBounds2d.h
				src/CarModel.cpp
				src/CarModel.h
				src/ClothoidPath.cpp
				src/ClothoidPath.h
				src/Cubic.cpp
				src/Cubic.h
				src/CubicSpline.cpp
				src/CubicSpline.h
				src/LearnedGraph.cpp
				src/LearnedGraph.h
				src/LinearRegression.cpp
				src/LinearRegression.h
				src/Driver.cpp
				src/Driver.h
				src/MyTrack.cpp
				src/MyTrack.h
				src/Opponent.cpp
				src/Opponent.h
				src/ParametricCubic.cpp
				src/ParametricCubic.h
				src/ParametricCubicSpline.cpp
				src/ParametricCubicSpline.h
				src/Path.cpp
				src/Path.h
				src/PathOffsets.cpp
				src/PathOffsets.h
				src/PathOptions.cpp
				src/PathOptions.h
				src/PathRecord.cpp
				src/PathRecord.h
				src/PidController.cpp
				src/PidController.h
				src/PitPath.cpp
				src/PitPath.h
				src/PtInfo.cpp
				src/PtInfo.h
				src/Quadratic.cpp
				src/Quadratic.h
				src/Seg.cpp
				src/Seg.h
				src/Shadow.cpp
				src/Shared.cpp
				src/Shared.h
				src/Span.cpp
				src/Span.h
				src/SpringsPath.cpp
				src/SpringsPath.h
				src/Strategy.cpp
				src/Strategy.h
				src/Stuck.cpp
				src/Stuck.h
				src/TeamInfo.cpp
				src/TeamInfo.h
				src/Utils.cpp
				src/Utils.h
				src/Vec2d.h
				src/Vec3d.h
				src/WheelModel.cpp
				src/WheelModel.h)

# Official-only shadow instances.
SET(ROBOT_CLONES shadow_36GP shadow_67GP shadow_sc shadow_trb1 shadow_ls1 shadow_ls2
			shadow_mp1 shadow_mpa1 shadow_mpa11 shadow_mpa12)

IF(NOT OPTION_OFFICIAL_ONLY)
   SET(ROBOT_CLONES ${ROBOT_CLONES} shadow_mp5 shadow_ref shadow_srw shadow_lp1)
ENDIF(NOT OPTION_OFFICIAL_ONLY)

# The ubiquitous robot module and its clones.
ROBOT_MODULE(NAME ${ROBOT_NAME} VERSION 3.3.0 SOVERSION 1.0.0
             INTERFACE ${ROBOT_INTERFACE}
             SOURCES ${ROBOT_SOURCES}
             CLONENAMES ${ROBOT_CLONES})

# For data associated to the ubiquitous robot module and its clones,
# see data/drivers/shadow*/CMakeLists.txt
