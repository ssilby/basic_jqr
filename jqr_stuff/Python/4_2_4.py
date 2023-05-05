#!/usr/bin/env python3

import os

def main():
    '''Demonstrate the ability to perform file management operations in Python:'''

    # Open an existing file
    f = open("Python/sample.txt")
    # Read data from a file
    contents = f.read()
    print(contents)
    # Parse data from a file
    f.readlines()
    f.close()
    # Write data to a file
    f = open("Python/sample.txt", "w")
    f.write("new stuff")
    # Modify data in a file
    with open("Python/sample.txt", "a+") as f:
        f.seek(3)
        f.write("aha!")
        cont = f.read()
        print(cont)

    # Close an open file
    f.close()
    # Print file information to the console
    print(os.stat("Python/sample.txt"))
    # Create a new file
    n = open("Python/newfile.txt", "w+")
    n.close()
    # Append data to an existing file
    f = open("Python/sample.txt", "a+")
    f.write("added to end of file")
    contents = f.read()
    print(contents)
    # Delete a file
    os.remove("Python/newfile.txt")
    # Determine the size of a file
    print("size:", os.path.getsize("Python/sample.txt"))
    # Determine location within a file
    print("location:", f.tell())
    f.close()
    # Insert data into an existing file
    with open("Python/sample.txt", "r") as f:
        contents = f.readlines()
        contents.insert(2, "inserted text")
    
    with open("Python/sample.txt", "w") as f:
        contents = "".join(contents)
        f.write(contents)

    # look at the rest of the code


if __name__ == "__main__":
    main()