# JQR Exercises for section 3.1.21

```

title: Demonstrate skill in using the C preprocessor
```



## Instructions


Your task is to implement the functions as defined in the header files within the include directory:
* `include/preprocessor.h`


## Submission: 

Submit your solutions as
* `src/preprocessor.c`
          

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
./build/3.1.21_exe
```


