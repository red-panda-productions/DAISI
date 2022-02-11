INCLUDE(../../../cmake/macros.cmake)

FILE(GLOB glf *.glf)
FILE(GLOB ttf *.ttf)

SD_INSTALL_FILES(DATA data/fonts FILES ${glf} ${ttf})

FILE(GLOB 5by7 5by7/*.*)
FILE(GLOB ArmataHL ArmataHL/*.*)
FILE(GLOB Montserrant Montserrant/*.*)
FILE(GLOB vera vera/*.*)

SD_INSTALL_FILES(DATA data/fonts/5by7 FILES ${5by7})
SD_INSTALL_FILES(DATA data/fonts/ArmataHL FILES ${ArmataHL})
SD_INSTALL_FILES(DATA data/fonts/Montserrant FILES ${Montserrant})
SD_INSTALL_FILES(DATA data/fonts/vera FILES ${vera})
