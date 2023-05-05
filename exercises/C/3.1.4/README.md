
# JQR Exercises for section 3.1.4

```
title: 'Demonstrate the ability to create and implement a function that uses different
  arrays:'
condition: C
standard: B
sub_items:
  3.1.4.a: an array
  3.1.4.b: a multi-dimensional array

```

## Instructions


Your task is to implement the functions as defined in the header files within the include directory:
* `include/arrays.h`


## Submission: 

Submit your solutions as
* `src/arrays.c`
          

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
./build/3.1.4_exe
```

## Special Instructions

Please note, for the test cases to monitor your file descriptors, you will want to set these variables in your solution:

```
#include <unistd.h>

volatile int g_arrays_fd_stdout = STDOUT_FILENO;
volatile int g_arrays_fd_stderr = STDERR_FILENO;
volatile int g_arrays_fd_stdin  = STDIN_FILENO;

```

This allows you to set the descriptors to the normal STDIN/STDOUT/STDERR for local tests, and for those values to be overridden for the test cases.

A function has been provided for you, which uses file descriptors:

```
// these have been provided for the student
static void local_printf(const char * format, ...) 
{
  va_list args; 
  va_start( args, format);
  vdprintf( g_arrays_fd_stdout, format, args );
  va_end( args );
}
```

