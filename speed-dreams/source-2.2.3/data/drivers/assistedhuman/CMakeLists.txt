# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_NAME "assistedhuman")

ROBOT_DATA(NAME ${ROBOT_NAME}
           FILES logo.png
           SUBDIRS cars PATTERNS *.xml)

ROBOT_DATA(NAME ${ROBOT_NAME}
           FILES ${ROBOT_NAME}.xml preferences.xml car.xml
           USER)
