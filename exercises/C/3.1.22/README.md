# JQR 3.1.22

```
title: 'Demonstrate skill in controlling memory:'
sub_items:
  3.1.22.a: Identify memory leaks
  3.1.22.b: Remove identified memory leaks
  3.1.22.c: Make effective use of valgrind with --leak-check=full to identify 
            and correct memory leaks and context errors
```

## Overview

This exercise comes with a pre-built adventure game. The author took some 
shortcuts and made some mistakes with memory management in the heap. 

You can find the exercise in `src/game.c`.

Your task is to fix the memory leaks in the program. When valgrind passes
in the pipeline without errors, your pipeline will pass correctly.

## Submission: 

Submit your solution as the changes necessary to game.c to remove memory leaks:
* `src/game.c`

## Test Cases

The test cases play the game and follow certain paths in the game. 
You shouldn't need to modify the test cases and there should be no 
leaks coming from the test cases. If you see a memory leak caused by 
the test cases themselves, notify your mentor or instructor
(or fix it yourself, if you find it -- but still notify someone).

## Run the game locally

You can compile the game as a standalone binary using this command:

```
gcc -DSTANDALONE_MODE game.c -o game
```

Additionally, the pipeline is running the following command to test
for leaks and errors:

```
valgrind --error-exitcode=1 --track-origins=yes --leak-check=full -s ./game
```

