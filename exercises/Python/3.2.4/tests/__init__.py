#!/usr/bin/env python3

import unittest
import sys
import random
import importlib.util
import pathlib
import junit_xml
import os
import string
import time
from collections import Counter

START_DIR= "src/"
TEST_FILE = os.environ.get("TEST_FILE")

TEST_SUITE_OUTPUT = "suite_results.xml"
TEST_SUITE_NAME = "3.2.4"

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



def ranName():
    name = ''.join(random.choice(string.ascii_lowercase)
                        for i in range(random.randint(5, 10)))
    return "{}{}".format(START_DIR,name)

def ranWord():
    word = ''.join(random.choice(string.ascii_lowercase)
                         for i in range(random.randint(20, 250)))
    return word

def ranSentence():
    words = []
    for _ in range(10, random.randint(100, 200)):
        words.append(''.join([random.choice(string.ascii_lowercase)
                        for _ in range(1, random.randint(2, 15))]))
    sentence = ' '.join(words)
    return sentence

class LocalTestCase(unittest.TestCase):

    name = ""

    def tearDown(self):
        cur_file = pathlib.Path(self.name)
        if cur_file.exists():
            cur_file.unlink()

class Reading(LocalTestCase):
    '''Create a random string that is written into output.txt'''
    name = ranName()
    word = ranWord()

    def setUp(self):
        self.f = open(self.name, "w+")
        self.f.write(self.word)
        self.f.close()

    def test_reading(self):
        '''Compare the user's file read with the correct one'''
        ans = student.reading(self.name)
        os.remove(self.name)
        self.assertEqual(ans, self.word)


class Parsing(LocalTestCase):
    '''Create a random set of strings with random lengths to join into a sentence.'''
    name = ranName()
    sentence = ranSentence()

    def setUp(self):
        self.f = open(self.name, "w+")
        self.f.write(self.sentence)
        self.f.close()

    def test_parsing(self):
        '''Check if the user's list is the same as the correct one'''
        ans = student.parsing(self.name)
        os.remove(self.name)
        for w in ans:
            self.assertEqual(w[0], 'k')
            self.assertTrue(w in self.sentence)
        counts = Counter(w[0] for w in self.sentence.split())
        self.assertEqual(len(ans), counts['k'])


class Writing(LocalTestCase):
    '''Create a random string to be written by the user'''
    name = ranName()
    word = ranWord()

    def test_writing(self):
        student.writing(self.name,self.word)
        self.f = open(self.name, "r")
        self.cmp = self.f.read()
        self.f.close()
        os.remove(self.name)
        self.assertEqual(self.cmp, self.word)


class Modifying(LocalTestCase):
    '''Create a random string and write into file to be modified by the user'''
    name = ranName()
    word = ranWord()

    def setUp(self):
        self.f = open(self.name, "w+")
        self.f.write(self.word)
        self.f.close()

    def test_modifying(self):
        '''Check if the user successfully modified the string in the file'''
        student.modifying(self.name)
        self.f = open(self.name, "r")
        self.studentans = self.f.read()
        self.f.close()
        os.remove(self.name)

        self.assertTrue('f' not in self.studentans)

        i = 0
        j = 0
        word_len = len(self.word)
        ans_len = len(self.studentans)
        while (i < word_len and j < ans_len):
            if self.word[i] == 'f':
                self.assertEqual(self.studentans[j:j+5], 'cyber')
                i += 1
                j += 5
                continue
            self.assertEqual(self.word[i], self.studentans[j]) 
            i += 1
            j += 1

        self.assertEqual(i, word_len)
        self.assertEqual(j, ans_len)


class Console(LocalTestCase):
    '''Print file information'''
    name = ranName()
    stdout = ranName()
    word = ranWord()

    def setUp(self):
        self.saved_stdout, sys.stdout = sys.stdout, open(self.stdout, 'w')
        self.f = open(self.name, 'w')
        self.m_time = int(time.time())
        self.f.write(self.word)
        self.f.close()

    def test_console(self):
        '''Test if user printed correct string to console'''
        student.console(self.name)
        sys.stdout.close()
        sys.stdout = self.saved_stdout
        f = open(self.stdout)
        ans = f.read().split()
        f.close()
        os.remove(self.stdout)
        os.remove(self.name)

        self.assertEqual(len(ans), 3)
        self.assertEqual(ans[0], self.name)
        self.assertEqual(int(ans[1]), len(self.word))
        self.assertEqual(int(float(ans[2])), self.m_time)


class Create(LocalTestCase):
    '''Create a new file'''
    name = ranName()

    def setUp(self):
        student.create(self.name)

    def test_create(self):
        correct = False
        if os.path.isfile(self.name):
            correct = True
            os.remove(self.name)
        self.assertTrue(correct)


class Append(LocalTestCase):
    '''Append data to existing file named append.txt'''
    name = ranName()
    word = ranWord()
    append = ranWord()

    def setUp(self):
        self.f = open(self.name, 'w+')
        self.f.write(self.word)
        self.f.close()
        student.append(self.name, self.append)

    def correct(self,):
        '''Returns the correct appended string'''
        self.new = self.word + self.append
        return self.new

    def test_append(self):
        '''Tests if the user appended the string correctly'''
        self.f = open(self.name, "r")
        self.cmp = self.f.read()
        self.f.close()
        os.remove(self.name)
        self.assertEqual(self.cmp, self.correct())


class Delete(LocalTestCase):
    '''Delete a file'''
    name = ranName()

    def setUp(self):
        self.f = open(self.name, "w+")
        self.f.close()
        student.delete(self.name)

    def test_delete(self):
        isDeleted = True
        if os.path.isfile(self.name):
            os.remove(self.name)
            isDeleted = False
        self.assertTrue(isDeleted)


class Size(LocalTestCase):
    '''Determine the size of a file'''
    name = ranName()
    word = ranWord()

    def setUp(self):
        '''Creates a randomly sized file'''
        self.f = open(self.name, "w")
        self.len= len(self.word)
        self.f.write(self.word)
        self.f.close()

    def test_size(self):
        '''Tests if the user returned the correct size of the file'''
        studentAns = student.size(self.name)
        os.remove(self.name)
        self.assertEqual(studentAns, self.len)


class Path(LocalTestCase):
    '''Determine location within a file'''
    name = ranName()
    word = ranWord()

    def setUp(self):
        '''Creates a file'''
        self.f = open(self.name, "w+")
        self.f.write(self.word)
        self.f.close()

    def test_path(self):
        '''Tests if the user returned the correct path of the file'''
        studentAns = student.path(self.name)
        try:
            ans = open(studentAns)
        except FileNotFoundError:
            self.fail("Provided path does not exist")
        cmp = ans.read()
        ans.close()
        self.assertEqual(self.word, cmp)



class Insert(LocalTestCase):
    '''Insert data into an existing file'''
    name = ranName()
    sentence = ranSentence()

    def setUp(self):
        self.f = open(self.name, "w+")
        self.f.write(self.sentence)
        self.f.close()
        student.insert(self.name)

    def test_insert(self):
        '''Tests if the user inserted "stuff" successfully'''
        self.f = open(self.name, "r+")
        self.cmp = self.f.read()
        self.f.close()
        os.remove(self.name)
        for i, w in enumerate(self.cmp.split()):
            if i % 2 == 1 and w != 'cyber':
                self.assertEqual(w, 'cyber')



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
