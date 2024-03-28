mkdir build
cd build
cmake -G "NMake Makefiles" -S .. -B . -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && copy compile_commands.json "../compile_commands.json" /y && nmake && START /D .. game
