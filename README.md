# rv32sysemu

Compile project and C code
```bash
bash build.sh
```
Clean
```bash
bash clean.sh
```
Run
```bash
./rv32sysemu programs/<your_file.bin>
```


Directory Structure
```
rv32sysemu
├── CMakeLists.txt
├── README.md
├── build.sh
├── clean.sh
├── core
│   ├── common
│   │   └── Exception.hpp
│   ├── kernel
│   │   ├── Kernel.cpp
│   │   └── Kernel.hpp
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
│   ├── syscall.c
│   ├── syscall.h
│   └── sysdef.h
├── programs
│   ├── test1.c
│   └── test2.c
├── rv32sysemu
└── scripts
    ├── compile.sh
    ├── linker.ld
    └── setup.sh
```
