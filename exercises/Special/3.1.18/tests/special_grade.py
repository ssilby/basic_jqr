#!/usr/bin/env python3
import pathlib
import requests
import base64
import hashlib
import re
import yaml
from junit_xml import TestSuite, TestCase

ORACLE_HOSTNAME = "special"

TEST_SUITE_OUTPUT = "suite_results.xml"

GRADED_PHRASE = { False : "Incorrect" , True : "Correct" }

SHOW_ALL = False

def read_and_remove_whitespace(fn):
    with open(fn, "r") as in_file:
        return re.sub(r"[ \t\n]", "", in_file.read())

def ask_oracle(jqr_item,dict_data):
    resp = requests.post("http://{}/{}".format(ORACLE_HOSTNAME,jqr_item),data = None, json=dict_data)
    return resp.text == "True"

def build_a_dictionary():
    flag = read_and_remove_whitespace("3.1.18.a.txt")
    print("Submission: |{}|".format(flag))
    
    seed = read_and_remove_whitespace("random.txt")
    print("Random Seed: |{}|".format(seed))
    
    submission_dictionary = { "action" : "3.1.18.a", "code" : flag, "random" : seed }
    return submission_dictionary

def build_b_dictionary():
    flag = read_and_remove_whitespace("3.1.18.b.txt")
    print("Submission: |{}|".format(flag))

    seed = read_and_remove_whitespace("random.txt")
    print("Random Seed: |{}|".format(seed))
   
    submission_dictionary = { "action" : "3.1.18.b", "code" : flag, "random" : seed }
    return submission_dictionary


def build_c_dictionary():
    with open("./3.1.18.c/3.1.18.c", "rb") as bin_file:
        bin_contents = base64.b64encode(bin_file.read()).decode("utf-8")
    submission_dictionary = { "action" : "3.1.18.c" , "binary" : bin_contents }
    return submission_dictionary


builders = {"3.1.18.a" : build_a_dictionary, "3.1.18.b" : build_b_dictionary, "3.1.18.c" : build_c_dictionary}

test_cases = []

def test_item(item_name):
    item_passes = False
    try:
        submission_dictionary = builders[item_name]()
        item_passes = ask_oracle("3.1.18",submission_dictionary)
        if not item_passes:
            print("Submission was incorrect: {}".format(submission_dictionary["action"]))

    except Exception as e:
        print("There was an error on {}: {}".format(item_name,e))

    cur_test_case = TestCase(item_name,"test.class",0.0,item_name, "pass: {}".format(item_passes))
    if not item_passes:
        cur_test_case.add_failure_info("incorrect")
    return(cur_test_case)

## 3.1.18.a
test_cases.append(test_item("3.1.18.a"))

## 3.1.18.b
test_cases.append(test_item("3.1.18.b"))

## 3.1.18.c
test_cases.append(test_item("3.1.18.c"))

test_suite = TestSuite("3.1.18",test_cases)
with open(TEST_SUITE_OUTPUT, 'w') as f:
    TestSuite.to_file(f, [test_suite], prettyprint=True)
