# poresize

## Requirements

* a c++-14-compliant compiler (compilation has been tested with gcc only)
* cmake (>= 2.8)
* libboost1.62-dev (older versions are also fine)
* libnlopt-dev

## Compilation

Download the poresize archive or clone it via `git`. 

The code can be compiled with the following commands:

	cd poresize       # enter the poresize folder
	mkdir build       # create a new build folder. It is good practice to compile out-of-source
	cd build
	cmake ..          # here you can specify additional options, see next section
	make -j4          # compile poresize. The -jX make option makes it compile the code in parallel by using X threads.
	
The poresize executable will be placed in the `bin` folder.

## cmake options

Here is a list of options that can be passed to cmake during the pre-compilation stage:

* `-DDebug=On|Off`			Compiles with|without debug symbols and without|with optimisation flags (defaults to `Off`)
* `-DG=On|Off`				Compiles with|without debug symbols and optimisation flags (defaults to `Off`)

## Usage

```
poresize input_file input_file_type=oxDNA|LAMMPS r_cut histogram_bins steps
```

poresize requires 5 arguments:

1. The configuration file.
2. The type of configuration file. Only `LAMMPS` and `oxDNA` are supported.
3. The size of the cells. The number of cells should be odd, and the code supports non-cubic boxes. As a result, the real size of the cells will be different from the one given, which is used only as a starting point. Tests have shown that, under normal circumstances, a value of 1 or 2 gives optimal results (performance wise). 
4. The number of bins of the cumulative histogram that is the output of the program.
5. The number of attempts at computing the radius of the largest sphere that contain a given point and does not overlap with any particle.
