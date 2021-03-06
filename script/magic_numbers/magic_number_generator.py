import math

nmax = 2**128 - 1
nbits = 128

# DEFINE THE NUMBER THAT YOU WANT TO GET A MAGIC NUMBER OF
d = 999

print("128 bit magic number is - Put this in the Map MagicMap128BitConstantDivision")

nc = ((nmax+1)//d)*d - 1
for p in range(0, 2*nbits + 1):
    if 2**p > nc*(d - 1 - (2**p - 1)%d):
        m = (2**p + d - 1 - (2**p - 1)%d)//d
        if(nmax + 1 - m > 0):
            print("Magic Number - " + hex(m))
            print("P - " + str(p))        
            print("Algo"+str(0))
            break
        else:
            print("Magic Number - " + hex(m - nmax - 1))
            print("P - " + str(p))        
            print("Algo"+str(1))
        break

print("256 bit magic number is - Put this in the Map MagicMap256BitConstantDivision")

nmax = 2**256 - 1
nbits = 256

nc = ((nmax+1)//d)*d - 1
for p in range(0, 2*nbits + 1):
    if 2**p > nc*(d - 1 - (2**p - 1)%d):
        m = (2**p + d - 1 - (2**p - 1)%d)//d
        if(nmax + 1 - m > 0):
            print("Magic Number - " + hex(m))
            print("P - " + str(p))        
            print("Algo"+str(0))
            break
        else:
            print("Magic Number - " + hex(m - nmax - 1))
            print("P - " + str(p))        
            print("Algo"+str(1))
        break

