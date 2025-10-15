```nushell
use make.nu
```
```nushell
make compile llwm.c x/events.c
make link build/llwm.o build/events.o
```
```nushell
./startx ./build/llwm
```
