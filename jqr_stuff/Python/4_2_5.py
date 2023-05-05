#!/usr/bin/env python3
'''Demonstrate the ability to create and implement functions to meet a requirement:'''

# A function that returns multiple values
def multiple_returns():
    str = "this is a string"
    num = 7
    return [str, num]

# A function that receives input from a user
def user_input():
    val = input("write something interesting: ")
    return val

# A recursive function
def factorial(num):
    if num == 1:
        return 1
    else:
        return (num * factorial(num - 1))



def main():
    print ("multiple returns: ", multiple_returns())
    print("user input: ", user_input())
    print("the factorial of 5 is ", factorial(5))

if __name__ == "__main__":
    main()