```nushell
use make.nu
```
```nushell
make compile llwm.c x/events.c | make link --output build/llwm.out
```
```nushell
./startx ./build/llwm.out
```

## tests
```
make compile ll.c test_ll.c | make link --output build/test_ll.out
./build/test_ll.out
```
