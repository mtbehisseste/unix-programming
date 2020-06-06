After some observation about the `traceme` program with gdb, I noticed that the program will take some data from `0x201020` and call `f` function to do some calculation, then it will store the result at `0x201b80` for 0x24 times.  So I can simple break at the address after the program has looped for 0x24 times (`0x8ab` I choosed here). Then peek the data at `0x201b80` for the whole result.

```
gdb-peda$ b *0x5555555548ab
gdb-peda$ c
gdb-peda$ x/s 0x555555755b80
0x555555755b80 <output>:        "ASM{a_Pr0ce55_can_b_trac3d_0n1Y_0nc3}"
```
