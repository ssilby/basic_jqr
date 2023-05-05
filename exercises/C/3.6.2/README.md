# 3.6.2

```
title: Demonstrate skill in using secure coding techniques
condition: C
standard: B
sub_items:
  3.6.2.a: format string vulnerabilities
  3.6.2.b: safe buffer size allocation
  3.6.2.c: input sanitization
  3.6.2.d: input validation
  3.6.2.e: Modeling complex functionality as state-machines
  3.6.2.f: establish a secure communication channel using an SSL/TLS library
  3.6.2.g: securely zeroing-out memory (compiler optimizations)

```

## Items Covered
This exercise covers the following items: a, b, c, d, and g.

3.6.2.e is covered in the C exercise [3.1.15](../3.1.15/README.md).

3.6.2.f is covered in the Python exercise [3.6.2.f](../../Python/3.6.2.f/README.md).

## Instructions 
You have been provided with some code in `src/`. This code contains a variety 
of programming errors including multiple format string and buffer overflow 
vulnerabilities. You should edit this code to remove these vulnerabilities.

***You may not remove any functionality from the sample program.*** 

That is, you cannot just delete all the code and declare it safe.

You should also implement input sanitization and validation by implementing
the functions `name_is_valid(char *name)` and `sanitize_name(char *name)`. Valid names are captured
by the following regex `^[a-zA-Z0-9_]*$`. Be sure not to introduce any
vulnerabilities in your implementations.

You should also implement the function `zero_memory(void *ptr, size_t size)`
which will take a pointer to a region of memory and zero out its contents. We 
don't want to give away our secrets.


## Submission: 

Submit your solutions as
* `src/fixme_pls.c`
          

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
./build/3.6.2_exe
```


