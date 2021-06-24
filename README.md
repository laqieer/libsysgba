# libsysgba

![workflow](https://github.com/laqieer/libsysgba/actions/workflows/c-cpp.yml/badge.svg)

Standard system call implementations for Game Boy Advance.

## Dependency

It needs libsysbase in [devkitPro](https://devkitpro.org/wiki/Getting_Started) to work.

## Build
```
make
make test
```

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
std::ifstream fs("xxx.txt");
std::stringstream ss;
ss << fs.rdbuf();
```

### Access Backup Media as File

It allows you to access backup media with File I/O. Special filename "sram:" is preserved for that.

```C
int fd = open("sram:", O_RDWR);
```

```C
FILE *fp = fopen("sram:", "r+");
```
