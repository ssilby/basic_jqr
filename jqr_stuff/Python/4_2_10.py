#!/usr/bin/env python3

'''Demonstrate the ability to serialize and de-serialize variable sized data structures between systems of differing endianness'''

import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('protocol', metavar='protocol', type=str, help='enter tcp or udp for protocol')
    args = parser.parse_args()

    protocol = args.protocol

    print("the protocol you chose is", protocol)

if __name__ == "__main__":
    main()