# Brainfuck-Interpreter
## A simple brainfuck interpreter with syscall support

### Syscall Format

1. The value at the current cell is considered the syscall code
2. The following cell is considered the number of arguments
3. The following cells are the argument in the following format:
    1. The first cell is a boolean called 'isPointer'. It indicates if the argument holds a value or a pointer to a cell.
    2. The second cell indicated the length of the argument in bytes. (not implemented yet)
    3. The third cell is the argument content. (only 1 byte for now)

The syscall is called with the '%' symbol and the return value is set to the current cell. 

You can check the list of syscalls here: https://filippo.io/linux-syscall-table/

The argument order must remain the same as in the standard C library.

### Example code

#### Write "Hi" to stdout using syscalls:
``` C
    +                       //syscall code = 1 (write)
    >+++                    //Argument count = 3    
//----1st Argument----
    >                       //The argument content is not a pointer
    >+                      //Arument length = 1 cell/byte
    >+                      //Arguemnt content = 1 => stdout
//----2nd Argument----
    >+                      //The argument content is a pointer
    >+                      //Arguemnt length = 1 cell/byte
    >+++++++++++            //Argument content = 11 (pointer to the message on the 11th cell)   
//----3rd Argument----
    >                       //The argumenn content is not a pointer
    >+                      //Arguemnt length = 1 cell/byte   
    >++                     //Arguent content = 2 (lenght of the message)
//----The message----
    >>++++++++[<+++++++++>-]<               //72 -> 'H'
    >>++++++++++[<++++++++++>-]<+++++       //105 -> 'i'
//----Return to cell 0
<<<<<<<<<<<<

//Call the syscall -> syscall(1, 1, message, 2);
%
```

### Using the interpreter

The interpreter takes a single argument which is the path to the brainfuck source code.

```
./interpreter filename.bf
```

### Brainfuck generator
The included bfgenerator.py helps you initialize the cells with ascii values. It generates a simple brainfuck code sequence, which populates your cells according to the given string.