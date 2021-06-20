# libsysgba

Standard system call implementations for Game Boy Advance.

## Dependency

It needs libsysbase in [devkitPro](https://devkitpro.org/wiki/Getting_Started) to work.

## Feature

### Support File I/O in C++ and C

It allows you to read files in [gbfs](https://pineight.com/gba/#gbfs) with C standard library functions and C++ fstream.

```C
int fd = open("xxx.txt", O_RDONLY);
read(fd, dest, size);
```

```C
int i, j;
FILE *fp = fopen("xxx.txt", "r");
fscanf(fp, "%d %d", &i, &j);
```

```C++
ifstream fs("xxx.txt");
stringstream ss;
ss << fs.rdbuf();
```
