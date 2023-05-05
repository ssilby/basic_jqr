# JQR Exercises for section 3.1.3

```
title: 'Demonstrate the proper declaration, understanding, and use of C data types
  and underlying structures:'
condition: C
standard: B
sub_items:
  3.1.3.a: WORD
  3.1.3.b: DWORD
  3.1.3.c: QUADWORD
  3.1.3.d: short
  3.1.3.e: integer (int)
  3.1.3.f: float (float)
  3.1.3.g: character (char)
  3.1.3.h: double (double)
  3.1.3.i: long (long)
```

## Instructions

Your task is to implement the functions in `include/data_types.h`

A shell source file has been created for you in `src/data_types.c`

Your solution should be within this shell file. 

## Submission: 

Submit your changes to `src/data_types.c`.

## CI Pipeline

Note: if your CI pipeline does not run when you commit, please ensure you have made a merge request to merge
your branch into the primary branch. This will trigger the rules for the CI Job configured for this item.


## Build instructions 

To test your C code locally, from this folder, run the following command:

```
bash ../../../scripts/build.sh
```

Alternatively, you can run the cmake build commands directly with:

```
cmake -H. -Bbuild
cmake --build build
```

After your build is complete, run the executable

```
./build/3.1.3_exe
```

