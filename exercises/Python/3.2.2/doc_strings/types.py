#!/usr/bin/env python

"""JQR Items Covered

3.2.2
title: Demonstrate the proper declaration and use of Python data types and
       object-oriented constructs

sub_items:
    a - Integer (int)
    b - Float (float)
    c - String (str)
    d - List (list)
    e - Multi-dimensional list
    f - Dictionary (dict)
    g - Tuple (tuple)
    h - Singleton

Brief: Add function parameters and your own implementation to complete
       the functions. Test your solution against the tests suite using
       "from tests import run_tests; run_tests()"
"""

def integer():
    '''integer

    Declare a variable as an integer and return it
    '''
    pass

def floater():
    '''float

    Declare a variable as a float value and return it
    '''
    pass

def strings():
    '''string

    Declare a variable as a string value and return it
    '''
    pass

def lists():
    '''list

    Declare a list with four values in it and return it
    '''
    pass

def multi_list():
    '''multilist

    Create a multi dimensional list and return it
    '''
    pass

def dictionary():
    '''dictionary

    Create a dictionary and return it
    '''
    pass

def toople():
    '''tuple

    Declare a tuple containing four values and return it
    '''
    pass

# TODO implement the singleton decorator


# TODO @singleton
class MyLogger:
    """
    Implement a class that will function as a logger.

    Decorate this class with a singleton
    """
    def __init__(self, file_logger=False, file_name=None):
        """
        Initialize the logger.

        The logger should just write messages to stdout.
        """
        pass

    def log(self, msg):
        """
        Write msg to stdout.
        """
        pass



