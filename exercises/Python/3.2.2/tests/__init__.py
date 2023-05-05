#!/usr/bin/env python

import unittest
from unittest.mock import patch
import random
import junit_xml
import pathlib
import importlib.util
import string
import os
import sys
import io

TEST_FILE = os.environ.get("TEST_FILE")

TEST_SUITE_OUTPUT = "suite_results.xml"
TEST_SUITE_NAME = "3.2.2"

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


class Integer(unittest.TestCase):
    def correct(self):
        ans = 1
        return type(ans)
    def test_interger(self):
        self.assertEqual(type(student.integer()),self.correct())

class Float(unittest.TestCase):
    def correct(self):
        ans = 1.5
        return type(ans)
    def test_float(self):
        self.assertEqual(type(student.floater()),self.correct())

class Strings(unittest.TestCase):
    def correct(self):
        ans = "string"
        return type(ans)
    def test_string(self):
        self.assertEqual(type(student.strings()),self.correct())

class List(unittest.TestCase):
    def correct(self):
        ans = [1,2,3,4]
        return type(ans)
    def test_list(self):
        self.assertEqual(type(student.lists()),self.correct())

class MultiDList(unittest.TestCase):
    def test_multidlist(self):
        studentans = student.multi_list()
        m1 = isinstance(studentans[0],list)
        m2 = isinstance(studentans[1],list)
        correct = False
        if m1 and m2:
            correct = True
        self.assertTrue  (correct)

class Dictionary(unittest.TestCase):
    def correct(self):
        d = {}
        return type(d)
    def test_dictionary(self):
        studentans = student.dictionary()
        self.assertEqual(type(studentans),self.correct())

class Toople(unittest.TestCase):
    def correct(self):
        t = ()
        return type(t)
    def test_toople(self):
        studentans = student.toople()
        self.assertEqual(type(studentans),self.correct())

class Logger(unittest.TestCase):
    @patch('sys.stdout', new_callable=io.StringIO)
    def test_logger(self, mock_print):
        # Test creating a logger and writing to stdout
        log1 = student.MyLogger() 
        msg1 = 'Hello from logger 1'
        log1.log(msg1)
        self.assertEqual(mock_print.getvalue(), msg1 + '\n')

        # reset the StringIO object
        mock_print.seek(0)
        mock_print.truncate(0)

        # Test creating 2nd logger still writes and is the same as the first
        # instance
        log2 = student.MyLogger()
        msg2 = 'Hello from logger 2'
        self.assertEqual(log1, log2)
        log2.log(msg2)
        self.assertEqual(mock_print.getvalue(), msg2 + '\n')

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
