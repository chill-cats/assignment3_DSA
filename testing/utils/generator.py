from random import randint, choice
from utils import probing
from rstr import xeger


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

def generate_insert():
    instr = "INSERT " 
    name = xeger(r'(?:[a-z]\w{0,10})$')  
    instr += name;
    isFun = choice([0, 1])
    if isFun == 1:
        instr += ' ';
        instr += str(randint(0, 999999))
    return instr
