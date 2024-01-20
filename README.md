Version v0.2.2

This is a source code for a tool to generate lc3sim script and run on
the LC-3 Simulator developed by Steven S. Lumetta.
The -f or --file option let the tool collect names following it, and 
write them into a script file. Other arguments see -h or --help.
Please use g++ to compile this file. The executable may be copied to 
/usr/bin or /usr/local/bin if desired.

Compile command: g++ lc3script.cpp -o lc3script 
Install command: sudo cp ./lc3script /usr/bin
The install directory can be modified with any directories with 
environment settings.

Thanks for using.

CHANGELOG

v0.2.2      Handle errors returne by lc3as assembler.
            Auto remove script file without "-s" argument.
            Messages modified.

v0.2.1      Echoing message modified

v0.2        Calling lc3as assembler added

v0.1        Initial release

Copyright (c) 2023 by Yuanzhe Liu
The MIT License (MIT)
Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the “Software”), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.
The Software is provided “as is”, without warranty of any kind, express or 
implied, including but not limited to the warranties of merchantability, fitness 
for a particular purpose and noninfringement. In no event shall the authors or 
copyright holders be liable for any claim, damages or other liability, whether 
in an action of contract, tort or otherwise, arising from, out of or in connection 
with the software or the use or other dealings in the Software.