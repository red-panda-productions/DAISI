INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
                  sc-boxer-96 sc-spirit-300 sc-cavallo-360 sc-fmc-gt4 
                  sc-lynx-220 sc-murasama-nsx sc-kanagawa-z35gts sc-kongei-kk8s
                  tracks)

ROBOT_DATA(NAME dandroid_sc
           FILES dandroid_sc.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg *.csv)
