# Using a Black Box
Currently, the system is hard-coded to find a black box in a set location. 
  When the system needs to connect to a black box, it simply finds the file data/blackbox/Blackbox.exe, and runs it.

To run with a different black box, this means that you need to compile your black box to a .exe format, 
  name the file Blackbox.exe, and start your program.

The current Blackbox.exe is a compiled version of SDASteerSample, as included in the 
  [SDALib repository](https://github.com/red-panda-productions/SDALib).
  This black box requires the [IPCLib](https://github.com/red-panda-productions/SDALib) dll to run, which is why this 
  file is included in this folder as well.

Eventually, a selection menu will be added for a black box. 
  This means the user of the system has to select an executable manually instead of the system running an executable at
  a hard-coded path. The directory `data/blackbox` and all its contents --- including this 
  file --- can be safely deleted when this feature has been added.