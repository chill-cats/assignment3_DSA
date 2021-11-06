import random
import string

LEN_OBJECT = 8
LEN_LIST = 100


def create_name_list():
    list_name = []
    for times in range(LEN_LIST):
        _characters = [random.choice(string.ascii_letters + string.digits) for times in range(LEN_OBJECT)]
        random.shuffle(_characters)
        list_name.append(''.join(char for char in _characters))

    return list_name

def getStr():
