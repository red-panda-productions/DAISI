FIND_PATH(MYSQL_INCLUDE_DIR mysql/jdbc.h
        HINTS ENV $MYSQLDIR
        PATH_SUFFIXES
        Headers include
        PATHS
        /usr/local
        /usr
        /sw # Fink
        /opt/local # DarwinPorts
        /opt/csw # Blastwave
        /opt
        /..
        /../3rdParty/)

MESSAGE(status "BANANA ${MYSQL_INCLUDE_DIR}")

FIND_LIBRARY(MYSQL_LIBRARY
        NAMES mysqlcppcon
        HINTS ENV MYSQLDIR
        PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
        PATHS
        /usr/local
        /usr
        /sw
        /opt/local
        /opt/csw
        /opt
        /..
        /../3rdParty)

SET(MYSQL_FOUND FALSE)
IF(MYSQL_LIBRARY)
    SET(MYSQL_FOUND TRUE)
ENDIF(MYSQL_LIBRARY)