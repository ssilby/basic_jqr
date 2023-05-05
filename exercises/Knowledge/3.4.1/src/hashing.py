def hash(key: str) -> int:
    hash = 17
    for c in key.encode('utf-8'):
        hash = 31 * hash + c
    return hash & 0xFFFFFFFFFFFFFFFF
