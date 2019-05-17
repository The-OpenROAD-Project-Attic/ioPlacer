# IO Placement

IO Placement finds an optimal placement for IO pins through the use of a Hungarian Algorithm.

## Getting Started
#### Pre-Requisite

- GCC compiler and libstdc++ static library
- boost library

#### Clone Repository

```
git clone https://github.com/mgwoo/RePlAce-pin
```

#### How to Compile

````
cd IOPlacement/
make release
`````

The binary file will be on IOPlacement's base folder.

#### IO Placement Arguments

To properly run IO Placement there are a few mandatory arguments that must be defined.

- **-i** : Input DEF file, the DEF file that will be evaluated (e.g.: -i ispd18_test2.input.def)
- **-o** : Output DEF file, the name of the output file, where the new placement will be saved (e.g.: -o ispd18_test2.output.def)
- **-h** : Horizontal metal layer, this should be an integer number indicating the metal layer (e.g.: -h 5)
- **-v** : Vertical metal layer, the usage is the same as horizontal metal layer

There is an optional flag that can be used.

- **-l** : Indicates whether or not the IO nets HPWL is returned, by default this flag is set to false


## Basic Usage

### Run a single placement

Your command line to run IO Placement should look like this.

````

./ioPlacement -i ispd18_test2.input.def -o ispd18_test2.output.def -h 5 -v 6 -l 1

````

You can also check the arguments and a brief description by running the following command.

````

./ioPlacement -h

````

### Manual integration with RePlAce

[...]
