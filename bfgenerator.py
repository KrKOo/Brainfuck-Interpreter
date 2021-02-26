"""
===============================================
Project: Brainfuck-Interpreter
Author: KrKOo
Email: krkoo.123@gmail.com
===============================================
"""


import math

string = """HTTP/1.1 200 OK
Connection:close
Accept-Ranges: bytes
Content-Length: 15
Content-Type: text/html

<h1>KrKO_o</h1>

"""
str_array = []
str_array[:] = string
code = "" 
strlen = 0

for char in str_array:
    ascii_value = ord(char)
    i = math.floor(math.sqrt(ascii_value))
    j = ascii_value // i
    diff = ascii_value- (i * j)
    code+='>>' + i*'+' + '[<' + j*'+' + '>-]<' + diff*'+' + '\n'
    strlen+=1

print(code)
print(strlen)
print(string)
