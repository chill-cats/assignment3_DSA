from random import randint
from utils import probing
 
def generateSetupLine():
    option = randint(0, 2)
    line = ""
    if option == 0:
        line = probing.generateLinear()
    elif option == 1:
        line = probing.generateDouble()
    elif option == 2:
        line = probing.generateQuadratic()

    shouldMutate = randint(0, 1)
    if shouldMutate == 1:
        line = probing.mutateLine(line)

    return line