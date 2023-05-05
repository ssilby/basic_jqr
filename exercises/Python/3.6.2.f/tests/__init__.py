#!/usr/bin/env python3

import unittest
import junit_xml
import pathlib
import importlib.util
import os
import ssl
import sys
import time
import random
import socket
import threading

from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding

import certauth
import gen_self_signed

TEST_FILE = os.environ.get("TEST_FILE")

TEST_SUITE_OUTPUT = "suite_results.xml"
TEST_SUITE_NAME = "3.6.2.f"

if TEST_FILE and \
        pathlib.Path(TEST_FILE).is_file():
    LIB_PATH = pathlib.Path(os.environ.get("TEST_FILE"))
    if LIB_PATH.is_dir():
        LIB_PATH = LIB_PATH / "__init__.py"
else:
    LIB_PATH = pathlib.Path.cwd() / pathlib.Path(__file__)
    LIB_PATH = LIB_PATH.parent.parent / "src/__init__.py"

SPEC = importlib.util.spec_from_file_location(LIB_PATH.name, LIB_PATH)
student = importlib.util.module_from_spec(SPEC)
sys.modules["student"] = student
SPEC.loader.exec_module(student)

HOSTNAME = 'localhost'
CERT_DIR = '/tmp/certs'
ROOT_CERT_NAME = '/tmp/myrootca.pem'

ca = certauth.CertificateAuthority('My Custom CA',
                                   ROOT_CERT_NAME,
                                   cert_cache=CERT_DIR)

def generate_data():
    alpha = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
    return ''.join(random.choice(alpha) for _ in range(32)).encode()


class TestGoodCertificate(unittest.TestCase):
    def _check_client(self, conn, cafile, data):
        try:
            conn.sendall(data)
        except BrokenPipeError:
            conn.close()
            return

        recvd = conn.recv(1024)

        with open(cafile, 'rb') as f:
            pem_data = f.read()
        priv_key = serialization.load_pem_private_key(pem_data, password=None)
        decrypted = priv_key.decrypt(
            recvd,
            padding.OAEP(mgf=padding.MGF1(algorithm=hashes.SHA256()),
                         algorithm=hashes.SHA256(),
                         label=None))

        self.assertEqual(data, decrypted)
        conn.close()

    def _serve(self, port, cafile, data):
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        context.load_cert_chain(cafile)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(('0.0.0.0', port))

        self.cv.acquire()
        self.server_bound = True
        self.cv.notify()
        self.cv.release()

        sock.listen(10)
        ssock = context.wrap_socket(sock, server_side=True)

        # We should expect 2 connections, 1 to communicate and 1 to get the
        # certificate
        conn, _ = ssock.accept()
        t1 = threading.Thread(target=self._check_client,
                              args=(conn, cafile, data))
        t1.start()

        conn, _ = ssock.accept()
        t2 = threading.Thread(target=self._check_client,
                              args=(conn, cafile, data))
        t2.start()

        t1.join(10)
        t2.join(10)

        ssock.close()
        sock.close()

    def _client_target(self, port):
        self.cv.acquire()
        while not self.server_bound:
            self.cv.wait()
        student.connect_to_server(ROOT_CERT_NAME, HOSTNAME, port)

    def test_good_certificate(self):
        self.cv = threading.Condition()
        self.server_bound = False
        port = random.randrange(10000, 65535)
        cert = ca.cert_for_host(HOSTNAME)
        data = generate_data()

        server_thread = threading.Thread(target=self._serve,
                                         args=(port, cert, data))
        server_thread.start()

        client_thread = threading.Thread(target=self._client_target,
                                         args=(port, ))
        client_thread.start()

        server_thread.join(20)
        client_thread.join(20)


class TestSelfSignedCertificate(unittest.TestCase):
    def _serve(self, port, cafile, data):
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        context.load_cert_chain(cafile)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(('0.0.0.0', port))

        self.cv.acquire()
        self.server_bound = True
        self.cv.notify()
        self.cv.release()

        sock.listen(10)
        ssock = context.wrap_socket(sock, server_side=True)

        try:
            conn, _ = ssock.accept()
            self.fail("Exception should have been raised")
        except:
            ssock.close()
            sock.close()

    def _client_target(self, port):
        self.cv.acquire()
        while not self.server_bound:
            self.cv.wait()
        try:
            student.connect_to_server(ROOT_CERT_NAME, HOSTNAME, port)
            self.fail("Should have raised an exception")
        except:
            return

    def test_self_signed_certificate(self):
        self.cv = threading.Condition()
        self.server_bound = False
        port = random.randrange(10000, 65535)
        cert = gen_self_signed.gen_self_signed_cert(CERT_DIR)
        data = generate_data()

        server_thread = threading.Thread(target=self._serve,
                                         args=(port, cert, data))
        server_thread.start()

        client_thread = threading.Thread(target=self._client_target,
                                         args=(port, ))
        client_thread.start()

        server_thread.join(20)
        client_thread.join(20)


class TestWrongName(unittest.TestCase):
    def _serve(self, port, cafile, data):
        context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        context.load_cert_chain(cafile)

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(('0.0.0.0', port))
        self.cv.acquire()
        self.server_bound = True
        self.cv.notify()
        self.cv.release()

        sock.listen(10)
        ssock = context.wrap_socket(sock, server_side=True)

        try:
            conn, _ = ssock.accept()
            self.fail("Exception should have been raised")
        except:
            ssock.close()
            sock.close()

    def _client_target(self, port):
        self.cv.acquire()
        while not self.server_bound:
            self.cv.wait()
        try:
            student.connect_to_server(ROOT_CERT_NAME, HOSTNAME, port)
            self.fail("Should have raised an exception")
        except:
            return

    def test_wrong_name(self):
        self.cv = threading.Condition()
        self.server_bound = False
        port = random.randrange(10000, 65535)
        cert = ca.cert_for_host('wrong.name')
        data = generate_data()

        server_thread = threading.Thread(target=self._serve,
                                         args=(port, cert, data))
        server_thread.start()

        client_thread = threading.Thread(target=self._client_target,
                                         args=(port, ))
        client_thread.start()

        server_thread.join(20)
        client_thread.join(20)


def export_suite(result):
    test_cases = []
    for index in range(result.__dict__['testsRun']):
        cur_test_case = junit_xml.TestCase(
            "{}-{}".format(TEST_SUITE_NAME, index), "test.class", 0.0, "", "")
        test_cases.append(cur_test_case)
    index = 0
    for failure in result.failures + result.errors:
        test_cases[index].add_failure_info(message=failure[1])
        index += 1
    test_suite = [junit_xml.TestSuite(TEST_SUITE_NAME, test_cases)]
    with open(TEST_SUITE_OUTPUT, 'w') as f:
        junit_xml.TestSuite.to_file(f, test_suite, prettyprint=True)


def run_tests():
    random.seed(time.time())
    x = unittest.main(exit=False, verbosity=3)
    export_suite(x.result)


if __name__ == '__main__':
    run_tests()
