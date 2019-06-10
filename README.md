# IO Placement

IO Placement finds an optimal placement for IO pins through the use of a Hungarian Algorithm.

## Getting Started
#### Pre-Requisite

- GCC compiler and libstdc++ static library
- boost library

#### Clone Repository

```
https://github.com/abk-openroad/ioPlacer.git
```

#### How to Compile

##### Standalone binary

````
cd ioPlacer/
make release
`````

The binary file will be on IOPlacement's base folder.

##### Static library

````
cd ioPlacer/
make lib
`````

The library file (libioPlace.a) will be on the 'ioPlacer/lib` folder

#### IO Placement Arguments

To properly run IO Placement there are a few mandatory arguments that must be defined.

- **-i** : Input DEF file, the DEF file that will be evaluated (e.g.: -i ispd18_test2.input.def)
- **-o** : Output DEF file, the name of the output file, where the new placement will be saved (e.g.: -o ispd18_test2.output.def)
- **-h** : Horizontal metal layer, this should be an integer number indicating the metal layer (e.g.: -h 5)
- **-v** : Vertical metal layer, the usage is the same as horizontal metal layer

There are optional flags that can be used.

- **-w** : Indicates whether or not the IO nets HPWL is returned, by default this flag is set to false
- **-f** : Force pins to be spread in core, i.e., try to respect number os slots
- **-n** : Number of slots per section
- **-s** : Increase factor (%) of slots per section
- **-m** : Percentage of usage for each section
- **-x** : Increase factor (%) of usage for each section
- **-b** : Name of the file containing the blocked areas. One blocked area per line, format: initialX initialY finalX finalY (e.g.: 0 1000 0 902900)

## Basic Usage

### Run a single placement

Your command line to run IO Placement should look like this.

````

./ioPlacement -i ispd18_test2.input.def -o ispd18_test2.output.def -h 5 -v 6 -w 1

````

You can also check the arguments and a brief description by running the following command.

````

./ioPlacement -h

````

### Manual integration with RePlAce

You can run IOPlacement with RePlAce, using the following script:

````

bash ./RePlAce-pin/scripts/replace_ioplace_loop.sh <N>

````
This script implement a loop with RePlAce and IOPlacement, as shown bellow:

<img src="doc/loop.png" alt="loop-ioplace-replace" width="50%" heigth="50%"/>

To execute the script, you will need both RePlAce and IOPlacement repositories in the same directory:

````

--root_dir
------RePlAce
------RePlAce-pin

````

In the root directory, just run the following command line:

````

bash ./RePlAce-pin/scripts/replace_ioplace_loop.sh <N>

````

where N is the number of iterations of the loop.

## API

#### Disclaimers
- The type *point* is a xy position defined in IOPlacement.h, e.g., point(lowerX, lowerY).
- The type *box* is a rectangle defined by two points, e.g., box(point(lowerX, lowery), point(upperX, upperY))
- The type DBU is a long long int

#### Initialize Data Structures

Before running IOPlacement the data structures should be initialized.

`````
void initCore(point lowerBounds, point upperBounds, DBU minSpacingX,
                      DBU minSpacingY, DBU initTrackX, DBU initTrackY);
``````

The function *initCore* initiliazes the circuit core. Six parameters should be passed to this function:

- **lowerBounds** - the lower left core bound. 
- **upperBounds** - the upper right core bound.
- **minSpacingX** - the minimum distance between IO pins on the x-axis.
- **minSpacingY** - the minimum distance between IO pins on the y-axis.
- **iniTrackX** - position of the first track in X.
- **initTrackY** - position of the first track in Y.

````
void setMetalLayers(int horizontalMetalLayer, int verticalMetalLayer);
``````

The function *setMetalLayers* defines on which layers the pins should be placed. 

````
void addIOPin(std::string name, std::string netName, box bounds,
                      std::string direction);
`````

The function *addIOPin* adds an IOPin to the data structure. All IO Pins should be individually added through this function.

- **name** - IO pin name.
- **netName** - net to which this pin is connected.
- **bounds** - bounds that indicate the shape of the pin.
- **direction** - direction of the pin. A pin can have one of the following directions: "INPUT", "OUTPUT" or "INOUT".

`````
void addInstPin(std::string net, std::string pinName, point pos);
`````

The function *addInstPin* adds a pin of an instance to the data structure. All pins should be individually added through this function.

- **net** - net to which this pin is connected.
- **pinName** - pin name.
- **pos** - pin position.

#### Optional Parameters

A few parameters in IOPlacement are optional, they are set to a default value but can be changed if needed. 

``````
void setSlotsPerSection(unsigned slotsPerSection, float increaseFactor);
``````

The function *setSlotsPerSection* sets the number of slots per section and the increase factor. If the algorithm can not assign each pin to a section, the number of slots per section is increased by this factor, e.g., 0.1f = 10% increase.

`````
void setSlotsUsagePerSection(float usagePerSection, float increaseFactor);
``````

The function *setSlotsUsagePerSection* sets the maximum slot usage per section.
- **usagePerSection** - maximum slot usage, e.g., 0.2f = 20% of the slots in a section can be utilized.
- **increaseFactor** - if the algorithm can not assign each pin to a section, the slot usage is increased by this factor, e.g., 0.1f = 10% increase.

```````
void forcePinSpread(bool force);
```````

The function *forcePinSpread* defines if IOPlacement should try to force the spread of IO Pins if the algorithm can not assign each pin to a section or if it should immediately increase the section's usage.

#### Running IOPlacement and Retrieving Results

````
std::vector<pinS> run(bool returnHPWL = false);
`````

After everything is properly set, the *run* function should be called. If a *true* argument is passed, this function prints the IO Pins HPWL. 
This function returns a vector with pinS structs. This struct is defined in IOPlacement.h, it contains the name of the IO pin, its new position and its orientation (e.g., 'N', 'S', 'W' or 'E').

