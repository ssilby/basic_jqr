# JQR Exercises for section 3.1.20

```
title: Demonstrate skill in using bitwise operators
condition: C
standard: B
sub_items:
  3.1.20.a: and (&)
  3.1.20.b: or (|)
  3.1.20.c: xor (^)
  3.1.20.d: bitwise complement (~)
  3.1.20.e: shift left (<<)
  3.1.20.f: shift right (>>)
```



## Instructions


Your task is to implement the functions as defined in the header files within the include directory:
* `include/bitwise_operators.h`


## Submission: 

Submit your solutions as
* `src/bitwise_operators.c`
          

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
./build/3.1.20_exe
```


