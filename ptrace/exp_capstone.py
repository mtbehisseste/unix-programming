#!/usr/bin/env python3
from pwn import *
from capstone import *
from binascii import unhexlify

p = remote('aup.zoolab.org', 2530)
md = Cs(CS_ARCH_X86, CS_MODE_64)  # arch: x86 with 64 bits mode

for _ in range(10):  # loop times should be give..
    p.recvuntil('>>> ')

    # hex with '\x' prefix
    code = unhexlify(p.recvline()[:-1])  

    instructions = []
    for i in md.disasm(code, 0x0):
        # complete instructions are consist of mnemonic + operands
        instructions.append(i.mnemonic + ' ' + i.op_str)

    # convert asm string to ascii
    ans = ''
    for ins in instructions:
        ans += ''.join([hex(ord(i))[2:] for i in ins])  
        ans += '0a'  # newline between each instruction
    p.sendlineafter('Your answer: ', ans)

p.interactive()
