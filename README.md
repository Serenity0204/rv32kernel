# rv32sysemu

Compile project and C code
```
make all
```
Run
```
./rv32sysemu programs/<your_file.bin>
```


Directory Structure
```
rv32sysemu
├── Makefile
├── README.md
├── core
│   ├── machine
│   │   ├── Bus.cpp
│   │   ├── Bus.hpp
│   │   ├── CPU.cpp
│   │   ├── CPU.hpp
│   │   ├── Common.hpp
│   │   ├── Decoder.cpp
│   │   ├── Decoder.hpp
│   │   ├── Executor.cpp
│   │   ├── Executor.hpp
│   │   ├── Memory.cpp
│   │   ├── Memory.hpp
│   │   ├── RegFile.cpp
│   │   ├── RegFile.hpp
│   │   ├── Utils.cpp
│   │   └── Utils.hpp
│   └── main.cpp
├── lib
│   ├── start.S
│   └── syscall.c
├── programs
│   ├── test1.c
│   └── test2.c
├── rv32sysemu
└── scripts
    ├── compile.sh
    ├── linker.ld
    └── setup.sh
```
