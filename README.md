![unit-tests-windows](https://github.com/red-panda-productions/speed-dreams/actions/workflows/test-windows.yml/badge.svg)
![unit-tests-linux](https://github.com/red-panda-productions/speed-dreams/actions/workflows/test-linux.yml/badge.svg)
![integration-tests-windows](https://github.com/red-panda-productions/speed-dreams/actions/workflows/integration-tests-windows.yml/badge.svg)
![coverage-windows](https://github.com/red-panda-productions/speed-dreams/actions/workflows/code-coverage-windows.yml/badge.svg) 
![coverage-linux](https://github.com/red-panda-productions/speed-dreams/actions/workflows/code-coverage-linux.yml/badge.svg) 
![linter](https://github.com/red-panda-productions/speed-dreams/actions/workflows/lint.yml/badge.svg) 

# DAISI

Driving Assistance Interface for Simulated Interventions.

## Description

DAISI is a driving simulator integrated with an intervention interface. Driving intervention algorithms can be hooked into the simulator and can take over control from the user, or give warnings. 

## Getting Started (Users)

The latest released version can be found under [Releases](https://github.com/red-panda-productions/speed-dreams/releases).
Download the version for your OS (Release version is recommended) and launch the installer. 

## Getting Started (Developers)

DAISI is a CMake project; setting it up is easy, as long as all of the dependencies are installed correctly.

### Prerequirements

 - CMake 3.15 or higher
 - Boost 1.79.0 or higher

#### Windows

 - MSVC v142 - VS 2019 C++ x64/x86 build tools
 - Windows 10 SDK (10.0.19041.0)
 - C++ CMake tools for windows

All other dependencies are in [3rdParty](speed-dreams/3rdParty)

#### Linux

 - GCC 11

```sh
wget https://github.com/red-panda-productions/ipc-lib/releases/download/v1.1.0/ipclib.deb
sudo apt install ./ipclib.deb libplib-dev libexpat1-dev libopenscenegraph-dev freeglut3-dev libvorbis-dev libsdl2-dev libopenal-dev libenet-dev libjpeg-dev libpng-dev libcurl4-openssl-dev libmysqlcppconn-dev libmsgpack-dev ninja-build
```

### Optionally for testing:
 - A local MySQL database with:
   - Username: `SDATest`
   - Password: `PASSWORD`

### Installation

#### CLI
1. Clone the repository
  ```sh
  git clone https://github.com/red-panda-productions/speed-dreams.git
  ```
2. Configure CMake
  ```sh
  cmake -B './build' -G Ninja
  ```
3. Build
  ```sh
  cd build
  cmake --build .
  ```
4. Optionally run the tests
  ```sh
  ctest -V
  ```

#### CLion or Visual Studio

1. Clone the repository
  ```sh
  git clone https://github.com/red-panda-productions/speed-dreams.git
  ```
2. Open [speed-dreams/source-2.2.3](speed-dreams/source-2.2.3)
3. Configure your project to use x86 compiler
4. Wait for CMake configuration to finish
5. Use `build all`, do not assume CLion or Visual Studio will correctly figure out which files have changed, they seem to have trouble understanding the relations between the different modules

#### CMake options

 - `COVERAGE_OUTPUT_FOLDER=PATH` where to generate the coverage run scripts. Default: [speed-dreams/source-2.2.3](speed-dreams/source-2.2.3)
 - `OPTION_PACKAGING=BOOL` whether to configure the installer generator or not. Default: ON
 - `OPTION_INTEGRATION_TESTS=BOOL` whether to configure the integration tests. Default: OFF
 - `OPTION_SD_CMAKE_BUILD_LOGS=BOOL` whether to show debug logs while configuring cmake. Default: OFF

## Usage

One example algorithm is shipped with the source code and can be found in [speed-dreams/source-2.2.3/data/blackbox](speed-dreams/source-2.2.3/data/blackbox). Note that this is windows only, for more samples or to compile one for linux go to [SDALib](https://github.com/red-panda-productions/SDALib/).

To start an experiment with a blackbox select the blackbox executable and start the run. The blackbox will automatically be started and connected to DAISI.

To change icons or sounds check the examples in [speed-dreams/source-2.2.3/data/data/indicators](speed-dreams/source-2.2.3/data/data/indicators)

For more information check the documentation [here](documentation/).

## Project structure

The [src/simulatedDrivingAssitance](speed-dreams/source-2.2.3/src/simulatedDrivingAssistance) folder contains most DAISI specific code and tests.  

Code responsible for managing the connection to the driving algorithms can be found in [src/simulatedDrivingAssitance/backend](speed-dreams/source-2.2.3/src/simulatedDrivingAssistance/backend).

Code responsible for loading intervention settings can be found in [src/simulatedDrivingAssitance/frontend](speed-dreams/source-2.2.3/src/simulatedDrivingAssistance/frontend).

## Acknowledgments

This project is a fork of the [Speed Dreams](http://www.speed-dreams.org/) project. All changes to their code are documented in [CHANGES.txt](speed-dreams/source-2.2.3/src/simulatedDrivingAssistance/CHANGES.txt). 

