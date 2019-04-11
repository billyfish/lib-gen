# Libgen - A library converter to AmigaOS from other platforms

## Introduction

Libgen is a tool to take the source of a non-AmigaOS library and generate an
AmigaOS library from it. It's primarily aimed at developers/porters. The
reason for developing it is I wanted to be able to run the cross-platform tools
that I develop for my day job on my Amiga. With Windows and Linux it's pretty easy
to develop shared libraries and I wanted the same on the Amiga side.

AmigaOS 4.x comes with a tool to do this called idltool and a lot of the code
that libgen produces is based upon how idltool does it. The difference between
the two is that with idltool you write an XML file that generates the skeleton code
that you then populate. With Libgen, you take the already existing code and generate
the library from that.

## License

Libgen is Public Domain software. You may redistribute the package as long as
it is unaltered and the authorship remains intact.

## Usage


| Argument |Description |
|----------|------------|
|I=Input/A | The directory of source files to generate the library for.|
|R=Recurse/S|A switch to recurse through the source directory and its children.|
|L=LibraryName/A|The name of the library to generate.|
|HP=HeaderFilePattern/K|The regular expression specifying the header files to read. This defaults to #?.h.|
|SP=SourceFilePattern/K|The regular expression specifying the source files to read. This defaults to #?.c.|
|PP=PrototypePattern/K|The regular expression specifying the signature of functions to put in the library. This regular expression omits the trailing semi-colon. This should include the {} brackets for the capturing group. This defaults to {#?}. |
|VER=Version/N|The version of the generated library's interface.|
|FL=Flags/K|The flags to put int the flags element within the generated XML file for idltool.|
|GC=GenerateCode/S|This switch will get Libgen to generate the code rather than just generate the XML file to use with idltool.|
|V=Verbose/N|Set to 0, 1 or 2 for increasing level of messages as Libgen is ran.|
|NL=Newlib/S|A switch to get the generated library to open and close the Newlib interface in its init and expunge routines.|
|IGN=Ignore/F|One or more paths to ignore parsing in the given input directory|
</table>


An examples of running it for libnsbmp is:

```
libgen i libnsbmp l nsbmp gc r nl ign test examples .git
```

with the options specifying the following:


  * i libnsbmp: This sets the input directory to ```libnsbmp```
  * l nsbmp: This sets the name for the generated library to ```nsbmp.library```
  * gc: This tells Libgen to generate the Amiga library code
  * r: This tells Libgen to recurse through the input directory.
  * nl: This tells Libgen to add the opening and closing of the Newlib library to the nbsmp.library 
	init and expunge functions.
  * ign test examples .git: This tells Libgen to ignore the ```test```, ```examples```
	and ```.git``` directories within the ```libnsbmp``` input directory when searching for appropriate header
  files.


As a test of the nsbmp library, an example program based upon the ```decode_bmp``` program which comes as part of the libnsbmp project, 
is in the ```nsbmp_example``` directory.




## Example

An example is contained in this archive to create a "sample.library" which is an extended
library based upon the example available at the
<a href="http://wiki.amigaos.net/wiki/How_to_create_an_AmigaOS_4_library">AmigaOS Wiki</a>.

It consists of two folders; the first is "sample_impl" which contains the code for the library
and "sample_impl_test" which has an example file that opens the library and calls its
functions.

So we start by getting Libgen to create the source code for the Amiga library for "sample.library"


```
libgen i sample_impl l sample gc r
```


Taking each set of arguments in turn:

  * i sample_impl: This sets the input directory to ```sample_impl```
  * l sample: This sets the name for the generated library to ```sample.library```
  * gc: This tells Libgen to generate the Amiga library code
  * r: This tells Libgen to recurse through the input directory.



This will create two files, ```sample.makefile```, which is used to build the library, and ```sample.xml```, 
which can be used with ```idltool``` along with a 
directory called ```sample_source``` containing the generated library code with the following content:

```
7.Data:Projects/libgen> dir all sample_source
     include (dir)
          interfaces (dir)
            sample.h
          proto (dir)
            sample.h
          inline4 (dir)
            sample.h
  init.c                       init.h
  lib_init.c                   lib_init.h
  sample_addition_impl.c       sample_addition_impl.h
  sample_division_impl.c       sample_division_impl.h
  sample_modulus_impl.c        sample_modulus_impl.h
  sample_multiplication_impl.c sample_multiplication_impl.h
  vectors.h
 ```



To build ```sample.library```, the first task is to set the version data with:


```
make -f sample.makefile revision
```

followed by

```
make -f sample.makefile
```

to build the library, ```sample.library```

To test this library, copy it into the ```sample_impl_test``` directory

```copy sample.library sample_impl_test/```

and cd into that directory. By running ```compile_test_sample```, a test program
```test_sample``` will be built which can be run to test the generated library:

```
7.Data:Projects/libgen/sample_impl_test> test_sample 
Addition: 6 + 4 = 10
Subtract: 6 - 4 = 2
Multiply: 6 * 4 = 24
Divide: 6 / 4 = 1.500000
Modulus: 6 % 4 = 2
7.Data:Projects/libgen/sample_impl_test> 
```

## History

### v1.1
  * Added option to read a windows exported functions (.def) file to control which functions are exported via the library interface
  * Fixed lib_init.c not being generated if libgen was not being run from its home directory
  * Fixed clean target in generated makefile using delete as it was treating non-exsitant files as errors
  * Fixed lib makefile target not dependent upon the target library
  * Fixed autoinit filenames and corresponding makefile target
  * Fixed parsed function definitions with a (void) or () signature producing a comma after the corresponding generated interface declaration<

### v1.0 
  * Added option to ignore paths within the input directory
  * Made references to headers from the input directories no longer absolute
	 and added the appropriate search paths to the generated makefiles instead
  * Added the generation of the autoinit code.
  * Fixed extern interface declaration in proto header
  * Fixed inline4 header generation
  * Fixed error message when failing to copy lib manager template referring to init.c rather than lib_init.c
  * ReadArgs parser out of sync for verbose parameter due to output format parameter being removed
  * Fixed generated function signatures with a single parameter missing the comma after the interface parameter

### v0.9 (20/12/16)
  * Initial release

## Credits
Thanks to Hypex, Chris, Salass00 and others for help and explanations

## Copyright
Libgen is &copy;2016 Simon Tyrrell and you can contact me at tyrrell.simon@gmail.com.
