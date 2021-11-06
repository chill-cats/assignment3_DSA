from random import choice, randint
import string

def generateLinear():
    line = "LINEAR "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    return line


def generateDouble():
    line = "DOUBLE "
    param = randint(0, 999999)
    line += str(param)
    line += ' '
    param = randint(0, 999999)
    line += str(param)
    return line


def generateQuadratic():
    line = "QUADRATIC "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    line += " "
    param = randint(0, 999999)
    line += str(param)
    return line


def mutateLine(line):
    lineToMutate = list(line)
    alphabets = string.digits + string.ascii_letters
    length = len(line)
    mutationCount = randint(0, int(length / 2))
    for _ in range(mutationCount):
        mutationPos = randint(0, length - 1)
        randomLetter = choice(alphabets)
        lineToMutate[mutationPos] = randomLetter

    addSpaceBegin = randint(0, 1)
    if addSpaceBegin == 1:
        lineToMutate = " " + line

    addSpaceEnd = randint(0, 1)
    if addSpaceEnd == 1:
        lineToMutate += " "

    return "".join(lineToMutate)

