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

## tests
```
make compile ll.c test_ll.c
make link build/ll.o build/test_ll.o --output build/test_ll.out
./build/test_ll.out
```
