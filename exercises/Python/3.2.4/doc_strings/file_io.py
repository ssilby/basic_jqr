#!/usr/bin/env python3

import os

"""JQR Items Covered

3.2.4
title: Demonstrate the ability to perform file management operations in Python

sub_items:
  a: open an existing file
  b: read data from a file
  c: parse data from a file
  d: write data to a file
  e: modify data in a file
  f: close an open file
  g: print file information to the console
  h: create a new file
  i: append data to an existing file
  j: delete a file
  k: determine the size of a file
  l: determine location within a file
  m: insert data into an existing file


Brief: Add function parameters and your own implementation to complete
       the functions. Test your solution against the tests suite using
       "from tests import run_tests; run_tests()"
"""


def reading(f01):
    '''Open a file and return the contents'''

    pass


def parsing(f01):
    '''
    Parse data from the file and add all strings that start with "k" to a list and return it.
    '''

    pass


def writing(f01, quote):
    '''Write the quote to a file'''

    pass


def modifying(f01):
    '''
    Modify text in file named modify.txt. Replace the letter "f" with the word "cyber"
    '''

    pass


def console(f01):
    '''
    Print file information to console using os.
    On the first line print the filename.
    On the second line print the size of the file.
    One the third line print the time the file was modified.
    '''

    pass


def create(f01):
    '''Create a file with the provided filename'''

    pass


def append(f01, str1):
    '''Append string to the file'''

    pass


def delete(filename):
    '''Delete this file'''

    pass


def size(filename):
    '''Return the filesize of the given file'''

    pass


def path(filename):
    '''Return the path of the file'''

    pass


def insert(f01):
    '''Insert the word cyber after every word in the file.

    example: "dog cat mouse" becomes "dog cyber cat cyber mouse cyber"
    '''

    pass
