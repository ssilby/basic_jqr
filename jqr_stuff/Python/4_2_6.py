#!/use/bin/env python3


def main():
    while True:
        num = input('enter a number 1 to 10: ')
        try:
            num = int(num)
        except ValueError:
            print('Enter a valid integer')
            continue
        if 0 <= num <= 10:
            break
        else:
            print('outside bounds')

    if num < 5:
        print('less than 5')
    elif num > 5:
        print('greater than 5')
    else:
        print('you entered 5')

    for x in range(num):
        print('X')
        
if __name__ == "__main__":
    main()