Compile the codes:

```bash
gcc -o proc proc.c
gcc -o main main1.c
```

Start the target process

```bash
./proc
hello - 0x56366d9952a0
```

The target process displays the address to locate the string. Use the address as argument when calling the injector.

```bash
./main -p 18893 -v 2 -a 0x56366d9952a0 -m mundo
```

The injector will attach to the process, write the data and deattach.

```
Remote data pointer: 0x56366d9952a0
Local data pointer:  0x7ffd23fb34d6
Data: batata
Write 0x56366d9952a0 <- 0x7ffd23fb34d6: b
Write 0x56366d9952a1 <- 0x7ffd23fb34d7: a
Write 0x56366d9952a2 <- 0x7ffd23fb34d8: t
Write 0x56366d9952a3 <- 0x7ffd23fb34d9: a
Write 0x56366d9952a4 <- 0x7ffd23fb34da: t
Write 0x56366d9952a5 <- 0x7ffd23fb34db: a
```

Type any digit on the `proc` and see if the `hello` was replaced by `batata`.

```bash
./proc
hello - 0x56366d9952a0
1
batata - 0x56366d9952a0
```