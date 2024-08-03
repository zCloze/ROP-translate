# 580 ROP Translation
translation tool 580vnx <-> 991cnx
## Usage
Open file cnx2vnx to trans 580 -> 991 and else
```cmd
[vnx2cnx.exe]
 > [gadget or hex value]
 > 17b34 # or 34 7b 31 30 is same
17b34 -> 09354
17b34 -> 16134
17b34 -> 16484
17b34 -> 16f0c
17b34 -> 179be
17b34 -> 179ce
17b34 -> 1da92
17b34 -> 2208a
17b34 -> 2209a
Best: 179ce # best is 100% correct
Closest: 1da92
```
## How it works
### Vnx2Cnx
Search for a list of disas\
eg: `17b34` -> search all [F02E F28E] disas
### Vnx2CnxClosest
Get closest with the input in the Vnx2Cnx list
### Vnx2CnxStrong
Search for same function in 991 then get the addr # very strong\
eg: input: `17B34` -> search for same function in 991
```
f_179C2:
179C2 F8CE PUSH LR
179C4 F06E PUSH XR0
179C6 F0E5 MOV ER0, ER14
179C8 F001F62 BL f_0AF62
179CC FE05 MOV ER14, ER0
179CE F02E POP XR0
179D0 F28E POP PC
```
Then search for pop xr0, pop pc -> `179ce`
### Cnx2Vnx
same

## License

[MIT](https://choosealicense.com/licenses/mit/)