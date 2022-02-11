INCLUDE(../../../cmake/macros.cmake)

# Official car sets.
SD_INSTALL_FILES(DATA cars/categories
                 FILES 36GP.xml 67GP.xml LS-GT1.xml LS-GT2.xml MP1.xml MP5.xml
                 MPA1.xml MPA11.xml MPA12.xml RS.xml Preview.xml
                 Supercars.xml SupercarsWithRearWing.xml TRB1.xml)

# Internal / non-official / work-in-progress car sets
# (we deliver all of them in case some advanced users / developers
#  manually install cars from these categories).
SD_INSTALL_FILES(DATA cars/categories
                 FILES 86MP1.xml Fun-Racer.xml LS-P1.xml LS-P2.xml MP10.xml Nascar.xml Raid-T1.xml References.xml Stock.xml)

