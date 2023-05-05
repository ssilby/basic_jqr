#!/usr/bin/env python
import unittest
import random
import pathlib
import importlib.util
import string
import junit_xml
import os
import sys
from unittest.mock import patch

TART_DIR= "src/"
TEST_FILE = os.environ.get("TEST_FILE")

TEST_SUITE_OUTPUT = "suite_results.xml"
TEST_SUITE_NAME = "3.2.5"

if TEST_FILE and \
        pathlib.Path(TEST_FILE).is_file():
    LIB_PATH = pathlib.Path(
            os.environ.get("TEST_FILE"))
    if LIB_PATH.is_dir():
        LIB_PATH = LIB_PATH / "__init__.py"
else:
    LIB_PATH = pathlib.Path.cwd() / pathlib.Path(__file__)
    LIB_PATH = LIB_PATH.parent.parent / "src/__init__.py"

SPEC = importlib.util.spec_from_file_location(
        LIB_PATH.name, LIB_PATH)
student = importlib.util.module_from_spec(SPEC)
sys.modules["student"] = student
SPEC.loader.exec_module(student)

def ranWord():
    word = ''.join(random.choice(string.ascii_lowercase)
                         for i in range(random.randint(20, 250)))
    return word

class returnValues(unittest.TestCase):
    def setUp(self):
        self.l = []
        for i in range(random.randint(20,100)):
            self.l.append(ranWord())

    def correct(self):
        self.ans = ()
        self.ans = tuple(self.l)
        return self.ans

    def test_returnValues(self):
        self.assertEqual(student.returnValues(self.l),self.correct())

'''
Would not work when declaring the variable inside the class. Would say it was
not defined.
'''
tested_word = ranWord()
class userInput(unittest.TestCase):
    @patch('builtins.input', return_value=tested_word)
    def test_userInput(self,mock_input):
        self.ans = student.userInput()
        self.assertEqual(self.ans, tested_word)


class Recursion(unittest.TestCase):
    def setUp(self):
        self.counter = 0
        self.word = ranWord()
    def check_rec(self, fct):
        def wrapped(*args, **kwargs):
            self.counter += 1
            return fct(*args, **kwargs)
        return wrapped
    def correct(self):
        return len(self.word)
    def test_recursion(self):
        with patch('student.recursion',
                        self.check_rec(student.recursion)):
            result = student.recursion(self.word)
            self.assertEqual(result,self.correct())
            self.assertGreater(self.counter, 1)



def export_suite(result):
    test_cases = []
    for index in range(result.__dict__['testsRun']):
        cur_test_case = junit_xml.TestCase("{}-{}".format(TEST_SUITE_NAME,index),"test.class",0.0,"","")
        test_cases.append(cur_test_case)
    index = 0;
    for failure in result.failures + result.errors:
        test_cases[index].add_failure_info(message=failure[1])
        index += 1
    test_suite = [junit_xml.TestSuite(TEST_SUITE_NAME,test_cases)]
    with open(TEST_SUITE_OUTPUT, 'w') as f:
        junit_xml.TestSuite.to_file(f, test_suite, prettyprint=True)


def run_tests():
    x = unittest.main(exit=False,verbosity=3)
    export_suite(x.result)

if __name__ == '__main__':
    run_tests()
