Building the dependencies for Speed-Dreams makes use of CMake's ExternalProject
 module. The source for each is downloaded from each project's site, patched if
 necessary, and built. This can take considerable time and accesses several
 different sites. See the thirdpartydefinitions.cmake file for the exact sites.
 The installation also contains a folder 3rdParty/source_info in which you
 will find the source link for each project.


 =============================================================================
 Windows
 As of version 2.3, this will download approximately 35MB of compressed source 
 files.
 You will need more than 1GB of free disk space for the build.

 Prerequisites:
 DirectX sDK (June 2010) - needed by SDL and possibly OpenAL
 http://www.microsoft.com/en-us/download/details.aspx?id=6812

 CMake version 3.4 or greater.

  =============================================================================
 OS X
 TODO

   =============================================================================
 Linux
 TODO
 