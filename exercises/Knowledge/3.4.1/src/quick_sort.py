def partition(arr, start, end):
    pivot = arr[start]
    lo = start + 1
    hi = end

    while True:
        while lo <= hi and arr[hi] >= pivot:
            hi -= 1
        while lo <= hi and arr[lo] <= pivot:
            lo += 1

        if lo <= hi:
            arr[lo], arr[hi] = arr[hi], arr[lo]
        else:
            break

    arr[start], arr[hi] = arr[hi], arr[start]

    return hi

def quick_sort(arr, start, end):
    if start >= end:
        return
    
    p = partition(arr, start, end)
    quick_sort(arr, start, p - 1)
    quick_sort(arr, p + 1, end)
