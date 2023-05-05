# JQR Exercises for section 3.1.5

```
title: Demonstrate the ability to perform basic arithmetic operations using appropriate
  C operators while ensuring proper order of operations (PEMDAS)
condition: C
standard: B
sub_items:
  3.1.5.a: Addition (+)
  3.1.5.b: Subtraction (-)
  3.1.5.c: Multiplication (*)
  3.1.5.d: Division (/)
  3.1.5.e: Modulus (%)
  3.1.5.f: Increment (++)
  3.1.5.g: Decrement (--)
```

## Instructions


Your task is to implement the functions as defined in the header files within the include directory:
* `include/operators.h`


## Submission: 

Submit your solutions as
* `src/operators.c`
          

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
./build/3.1.5_exe
```
