# PathPlanningProject
## Description
Project in development...
## Getting started
To build and run the project you should have compiler in C++17 standart.

# Installing

Download current repository to your local machine. Use

```
git clone https://github.com/Ch0p1k3/PathPlanningAlgorithms
```

The project can be build in two ways:
- Using QtCreator and qmake;
- Using CMake;
When using QtCreator, you need to open the file `ASearch.pro` which is located in the directory` ... / PathPlanningProject / Src / `and set up the project with the required assembly kit.
![qt_open](./Images/qt1.png)

After selecting a project, you need to set the input file name as a command line argument. Use the file `... / PathPlanningProject / Examples / example.xml` as a first example. To set command line arguments, go to the project launch settings and enter the desired file path in the "Command line parameters" field.

![qt_arg](./Images/qt2.png)

After setting the command line argument, you can test the program. The following output should appear as a result of running:

```
Parsing the map from XML:
Map OK!
Parsing configurations (algorithm, log) from XML:
short
Warning! Value of 'logpath' tag is missing!
Value of 'logpath' tag was defined to 'current directory'.
Warning! Value of 'logfilename' tag is missing.
Value of 'logfilename' tag was defined to default (original filename +'_log' + original file extension.
Configurations OK!
Creating log channel:
Log OK!
Start searching the path:
Search is finished!
Path NOT found!
numberofsteps=0
nodescreated=0
time=0
Results are saved (if chosen) via created log channel.
```

When using CMake, building and launching can be done both from the command line and using various IDEs. Below are the build and run scripts using the command line.

### Linux и Mac
Release building:
```bash
cd PathPlanningProject
cmake -DCMAKE_BUILD_TYPE="Release"
make
make install
```

Debug building:
```bash
cd PathPlanningProject
cmake -DCMAKE_BUILD_TYPE="Debug"
make
make install
```

Launching:
```bash
cd .Bin/{Debug|Release}/
./Ivan_Luchsh_ASearch ../../Examples/example.xml
```
Launch result:

![cmake_run](./Images/cmake1.png)

### Windows
Release building:
```cmd
cd PathPlanningProject
set PATH
cmake -DCMAKE_BUILD_TYPE="Release" -G "MinGW Makefiles"
mingw32-make
mingw32-make install
```

Debug building:
```cmd
cd PathPlanningProject
set PATH
cmake -DCMAKE_BUILD_TYPE="Debug" -G "MinGW Makefiles"
mingw32-make
mingw32-make install
```

Launching:
```cmd
cd Bin/{Debug|Release}/
Ivan_Luchsh_ASearch.exe ../../Examples/example.xml
```

Launch result:
![cmake_run2](./Images/cmake.png)
 
Tests are run from the directory `.../PathPlanningProject` using the command:
```
ctest -C {Debug|Release}
```

For more detailed output:
```
ctest -C {Debug|Release} --output-on-failure
```
## Input and output
Input files

Input files are an XML files with a specific structure.
Input file should contain:
* Mandatory tag <root>. It describes the parameters.
  * Tag <map>. It describes the map.
    * width - the width of the field.
    * height - the height of the field.
    * cellsize - ...
    * startx - a start coordinate x.
    * starty - a start coordinate y.
    * finishx - a finish coordinate x.
    * finishy - a finish coordinate y.
    * Tag <grid> describes your map, where each line is separated by a <line> tag. 0 is free cell, 1 is obstruction.
  * Tag <algorithm> describes the algrithm options.
    * searchtype - the type of the search.
    * metrictype - the heuristic for A* algorithm. There are: diagonal, manhattan, euclidean, chebyshev.
    * breakingties - ...
    * hweight - the hweight value for A* algorithm.
    

## Mentors
**Яковлев Константин Сергеевич**
- kyakovlev@hse.ru
- [HSE website](https://www.hse.ru/staff/yakovlev-ks)
- Telegram: @KonstantinYakovlev

**Дергачев Степан**
- sadergachev@edu.hse.ru
- Telegram: @haiot4105
