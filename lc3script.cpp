/*

version v0.2.2

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

vo.2.2      Handle errors returne by lc3as assembler.
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

*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

#define _MaxInputCount 5
#define _MaxArgs 20
#define _MaxNameLen 100
#define _DefaultOutput ".run_script"
#define _DefaultLc3Output "lc3sim.out"
#define _DefaultDumpArg ""
#define _Assembler "lc3as "


char  arguments[] = {'-', 'h', 'f', 's', 'c', 'd', 'r', 'o', 'a'};
char* commands[] = {"--help", "--run", "--file", "--assemble"};

void printStartMsg(){
    using namespace std;
    cout << "Usage: lc3script [-r | --run] -f [Target File] [-s Output Script Name]"<< endl;
    cout << "[-c disable] [-d label | location] [-o simulator output] [-a | --assemble]" << endl;
    cout << "Or use lc3script -h | --help for help." << endl;
}

void printHelpMsg(){
    printStartMsg();
    using namespace std;
    cout << endl;
    cout << "Lc3 runscript generator" << endl;
    cout << "Use -h or --help for help" << endl;
    cout << "Recommended usage: lc3script -f yourfilename -o -r" << endl;
    cout << "Commands: " << endl;
    cout << "    -r    --run             Run lc3sim after writing the script." << endl;
    cout << "    -h    --help            Show help message." << endl;
    cout << "    -f    --file            Set input .obj file(s) in sequence." << endl;
    cout << "                            The order is how lc3sim load each file. " << endl;
    cout << "                            Filename extension is not required." << endl;
    cout << "    -s    [output filename] Set output script name, default: .run_script" << endl;
    cout << "                            Default name is [input].out" << endl;
    cout << "    -c    disable/enable    Automatically continue in lc3sim, default: enable." << endl;
    cout << "    -d    [label/location]  Call dump to see memory contents." << endl;
    cout << "                            Arguments are labels or locations given to lc3sim." << endl;
    cout << "    -o    [lc3sim output]   Let the lc3sim to save its output in file." << endl;
    cout << "    -a    --asemble         Use assembler to build lc3 binary before launch." << endl;
    cout << endl;
}

using namespace std;

int main(int argc, char* argv[]){
    //variable declaration
    char  inputNames[_MaxInputCount][_MaxNameLen];          //String array contains input filenames
    char  outputScriptName[_MaxNameLen] = _DefaultOutput;     //String contains output script filename
    char  lc3simOutput[_MaxNameLen] = _DefaultLc3Output;    //String contains lc3sim output filename
    char  assembler[_MaxNameLen] = _Assembler;
    char  dumpArgInit[_MaxNameLen] = _DefaultDumpArg;       //String contains dump arguments
    char* dumpArg = dumpArgInit;                            //Trying to avoid segmentation fault...
    int  argsCount, tmpCount;                               //Used in argument calculation loop
    int  fileCount = 0;
    int  invalidLocations[_MaxArgs];
    int  invalidCount = 0;
    int  i;
    //Mode control, and their default values.
    //They are modified through argument calculation loop
    bool useDefauleOutput = true;
    bool autoContinue = true;
    bool autoDump = false;
    bool autoRun = false;
    bool writeOutput = false;
    bool useAssembler = false;
    bool saveScript = false;
    bool invalid;

    char launchCommand[500] = "lc3sim ";

    //Check if there is no args
    if(argc == 1){
        printStartMsg();
        return 1;
    }

    //start to go through args
    for(argsCount = 0; argsCount < argc; argsCount++){
        //help message
        if( (!strcmp(argv[argsCount], "-h")) || (!strcmp(argv[argsCount], "--help"))){
            printHelpMsg();
            return 1;
        }

        //run option
        if( (!strcmp(argv[argsCount], "--run")) || (!strcmp(argv[argsCount], "-r")) ){
            autoRun = true;
        }

        //collect input filename
        if( (!strcmp(argv[argsCount], "--file")) || (!strcmp(argv[argsCount], "-f")) ){
            if(argsCount != argc - 1){
                for(tmpCount = argsCount + 1; tmpCount < argc && argv[tmpCount][0] != '-'; tmpCount++);
                fileCount = tmpCount - argsCount - 1;
                for(i = 0; i < fileCount; i++){
                    strcpy(inputNames[i], argv[i + argsCount + 1]);
                }
            }
        }

        //set output filename
        if(!strcmp(argv[argsCount], "-s")){
            saveScript = true;
            if(argsCount != argc - 1 && argv[argsCount + 1][0] != '-'){
                strcpy(outputScriptName, argv[argsCount + 1]);
            }
        }

        //check if let lc3sim to continue itself
        if(!strcmp(argv[argsCount], "-c")){
            if(argsCount != argc - 1){
                if(!strcmp(argv[argsCount + 1], "disable")){
                    cout<<"Continue disabled." << endl;
                    autoContinue = false;
                }
            }
        }

        //let the lc3sim run dump
        if(!strcmp(argv[argsCount], "-d")){
            autoDump = true;
            if(argsCount != argc - 1)
            {
                for(tmpCount = argsCount + 1; tmpCount < argc && argv[tmpCount][0] != '-'; tmpCount++);
                for(i = 0; i < tmpCount - argsCount - 1; i++){
                    dumpArg = strcat(dumpArg, argv[argsCount + i + 1]);
                    if(i < tmpCount - argsCount - 2)
                        dumpArg = strcat(dumpArg, " ");
                }
            }
        }

        //let the lc3sim write output a specified file
        if(!strcmp(argv[argsCount], "-o")){
            writeOutput = true;
            if(argsCount != argc - 1 && argv[argsCount + 1][0] != '-'){
                strcpy(lc3simOutput, argv[argsCount + 1]);
                useDefauleOutput = false;
            }
        }

        //check if assembler desired
        if( (!strcmp(argv[argsCount], "--assemble")) || (!strcmp(argv[argsCount], "-a")) ){
            useAssembler = true;
        }

        //invalid args check
        if(argv[argsCount][0] == '-'){
            invalid = true;
            //compare the arg with preset args array
            if(argv[argsCount][1] == '-'){
                for(int i = 0; i < sizeof(commands)/sizeof(char*); i++){
                    if(!strcmp(argv[argsCount], commands[i]))
                        invalid = false;
                }
            }
            else{
                for(i = 0; i < sizeof(arguments)/sizeof(char); i++){
                    if(argv[argsCount][1] == arguments[i])
                        invalid = false;
                }
            }
            if(invalid){
                invalidLocations[invalidCount] = argsCount;
                invalidCount += 1;
            }
        }
    }

    //exit when recieved invalid args
    if(invalidCount){
        cout << "Invalid arguments: ";
        for(i = 0; i < invalidCount; i++){
            cout << argv[invalidLocations[i]] << " ";
        }
        cout << endl;
        printStartMsg();
        return 2;
    }

    //print messages
    if(fileCount){
        cout << fileCount << " filenames recieved: ";
        for(i = 0; i < fileCount; i++){
            cout<< inputNames[i] << " ";
        }
        cout << endl;
    }
    else{
        cout << "No filename recieved. Script launch abropt." << endl;
        return 3;
    }

    //Call assembler
    invalid = false;
    if(useAssembler){
        cout << endl;
        for(i = 0; i < fileCount; i++){
            cout << "Assembling object file " << inputNames[i] << ".obj using lc3as..." << endl;
            strcpy(assembler, _Assembler);
            strcat(assembler, inputNames[i]);
            if(system(assembler))
                invalid = true;
            cout << endl;
        }
        if(invalid){
            cout << "The assembler returns an error. Launch Abropt." << endl;
            return 4;
        }
    }

    //Write File
    ofstream script(outputScriptName, ios::out);
    for(i = 0; i < fileCount; i++){
        script << "file " << inputNames[i] << endl;
    }
    if(autoContinue)
        script << "c" << endl;
    if(autoDump)
        script << "d " << dumpArg << endl;
    script.close();
    if(saveScript)
        cout << "Script written in: " << outputScriptName << endl;


    //start lc3sim
    //initial and extend the command with output args
    strcpy(launchCommand, "lc3sim ");
    if(useDefauleOutput){
        strcpy(lc3simOutput, inputNames[fileCount - 1]);
        strcat(lc3simOutput, ".out");
    }
    //set the script to run
    strcat(launchCommand, "-s ");
    strcat(launchCommand, outputScriptName);

    if(writeOutput){
        strcat(launchCommand, " > ");
        strcat(launchCommand, lc3simOutput);
    }
    if(autoRun){
        cout << "Starting lc3sim..." << endl;
        //cout << "Running command: " << launchCommand << endl;
        system(launchCommand);
    }
    //echo messages
    if(autoContinue && autoRun)
        cout << "lc3sim done. ";
    if(writeOutput)
        cout << "Output written in "  << lc3simOutput;
    if(autoContinue || writeOutput)
        cout << endl;
    //remove script
    if(!saveScript && autoRun){
        strcpy(launchCommand, "rm ");
        strcat(launchCommand, outputScriptName);
        cout << "Removing script..." << endl;
        system(launchCommand);
    }
    
    cout << "All works done." << endl;
    return 0;
}
