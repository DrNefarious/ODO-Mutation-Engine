# ODO-Mutation-Engine
A modular program which accepts custom payload and mutator code to create a functional polymorphic or metamorphic program.

--INSTALLATION--

To install the program simply clone the directory to your system. 

If you wish to recompile the .cpp file to create new binary simply use your favourite c++ compiler. You will also need to provide additional arguments to use c++17 as well as the filesystem library. Example code of how this could be achieved using gnu c++ is shown below.

g++ main.cpp -o MetaProgram -std=c++17 -lstdc++fs

The repository has a makefile which contains this code.


--USAGE--

To use the program simply navigate to the directory of the ODO program within a terminal and execute the program with a ./odo command. This will open the ODO interface. From this interface additional help screens can be accessed using a help command.

Commands within the ODO program can be executed without accessing the ODO interface by calling the ODO program along with a valid command. For example, typing ./odo help into the terminal will display the help screen from ODO without opening the interface.


--CREATING CUSTOM MODULES--

To create a custom payload or mutator you need to follow some simple rules!!!

When creating a custom module, the .odo format needs to be followed. In order to see the appropriate format simply either open an existing module file, or open the template.txt file to see a blank template. The only part of the template which is technically required are the <code> tags. Custom payload code needs to be placed between these tags. The code MUST be in bytecode format, with no spaces. The payload code must end with a return instruction. The payload section will always commence from address 0x402000 and must not exceed 512 bytes.

The above rules also apply to any custom mutator modules created.

The following addresses are hard coded and can be used within any module.

0x42460 - Address containing the size of the file.
0x42468 - Address of the start of 16kb of uninitialised memory.


--EDITING EXISTING MODULES--
When editing existing modules please take special care to adjust any addressing used. Failure to do so will have unexpected results.
