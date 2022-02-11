INCLUDE(../../../cmake/macros.cmake)

# The data for the SC clone of the ubiquitous SHADOW robot module.

SET(ROBOT_SUBDIRS 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
                  sc-boxer-96 sc-cavallo-360 sc-deckard-conejo sc-fmc-gt4
                  sc-kanagawa-z35gts sc-lynx-220 sc-murasama-nsx sc-spirit-300)

ROBOT_DATA(NAME shadow_sc
           FILES shadow_sc.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
