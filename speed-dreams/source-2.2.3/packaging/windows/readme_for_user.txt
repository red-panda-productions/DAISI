License
-------

The whole distribution is licensed according to the GPL 2+ and the Free Art Licenses.

Installation instructions
-------------------------

Among the 5 binary packages (speed-dreams-xxxx-2.1.0-rYYYY-win32-setup.exe),
only the "base" one is mandatory : it contains the core components of the game.
It is also a kind of "demo" package, as it also ships 1 track (Espie)
and 1 car set (the Supercars), plus the relevant AI drivers :
that way, you can quickly get the taste of Speed Dreams.

If you're reading this, sure you already installed it :-)

Then, you might want to download and install at least one
of the "HQ cars and tracks" packages :-)
You can install them in any order, either by directly running them,
or by re-running the "base" installer and "Add / re-install components".

Completing the installation
---------------------------

After installing Speed Dreams and starting it, if you get and error box 
that says something like "Can't find MSVCR80.dll" or so,
try installing the MS Visual C++ 2005 redistribuable package :
- Be carefull to use the x86 (32bits) version, 
  release 2.0.50727.762 (matching with Microsoft Visual C++ 2005 SP1 x86)
  (the one shipped with VC 2005 Express doesn't work).
- You can download it here, at Microsoft's official web site:
  http://www.microsoft.com/downloads/details.aspx?familyid=200B2FD9-AE1A-4A14-984D-389C36F85647&displaylang=en

Game Requirements
-----------------

Torcs was told to need at least a 600 MHz CPU (800 MHz recommended), 128 Mb RAM (256 Mb
recommended) and an OpenGL 1.3 capable graphics card with 32Mb GRAM (64Mb recommended).

You need more now, as Speed Dreams is constantly improving visual rendering :
- Video card : at least 128Mb ; better with 256 Mb or more 
- CPU        : at least 1 GHz ; better with 1.5 or 2.0 GHz
- RAM        : from at least 512 Mb (Win XP 32) to 2Gb (Win Vista 32) ; better with 1 to 3 Gb

For more details, see http://sourceforge.net/p/speed-dreams/wiki/HardSoftRequirements

Moreover: Make sure that you have installed the latest sound and graphics drivers.

Running the game
----------------

- Read the "User manual" :
  * Simon's great online tutorial videos http://www.youtube.com/user/mungewell/feed?feature=context
  * WIP online manual http://sourceforge.net/p/speed-dreams/wiki/SD2.0Manual
  * Windows : Start menu / Programs / Speed Dreams x.y.z / User Manual
    (Note: Now out-dated, as was written for 1.4.0 ...)  
  * Others (soon available online at http://www.speed-dreams.org) :
    - source distribution : open speed-dreams-x.y.z-src/doc/userman/how_to_drive.html 
      in a web browser
    - SubVersion repository : download a GNU tarball of the user manual here :
        http://speed-dreams.svn.sourceforge.net/p/speed-dreams/code/HEAD/tree/tags/x.y.z/doc/userman
      then uncompress it and finally open how_to_drive.html in a web browser
      (Note: Now out-dated, as was written for 1.4.0)  

- Start the game :
  * Windows : Start menu / Programs / Speed Dreams x.y.z / Speed Dreams
  * Linux : after (compiling it from the source distribution and) installing it,
     run /usr/local/games/speed-dreams-2 in a shell.

  Note: Under Windows XP at least, first-ever startups sometimes fail to correctly write
        the necesary stuff into <My documents>\speed-dreams-2.settings and this prevents
        the game from starting.
        If this happens, remove the <My documents>\speed-dreams-2.settings folder
        with the Windows explorer, and restart the game : it should now work.

- Press F1 in any screen to get help (do it as well during the ride, to learn about the options).

- Read the FAQ to learn about split screen multiplayer and more.

Getting Help
------------

See here : http://www.speed-dreams.org

Car Setups
----------

For the player driver, the default car parameters are located in the files
"<install>/drivers/human/car*.xml" depending on the car used. 

You can have your own customized version of these files by copying them
to <My documents>/speed-dreams-2.settings/drivers/human/car*.xml
and changing them with a text editor ; for more information look into the
robot tutorial chapter 5 (http://www.berniw.org/torcs/robot/ch5/properties.html).

Creating Tracks
---------------

There is a new track editor in development (by Mart Kelder) :
have a look soon at Speed Dreams SVN repository (in the 'subprojects' folder).

Robot programming
-----------------

You'll find a robot programming tutorial at www.berniw.org in the TORCS section.

Wolf-Dieter is also starting a new "Robot Programming Academy" : soon news !

Robot racing
------------

Visit www.berniw.org/trb for more information.
