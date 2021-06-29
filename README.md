# libsysgba

![workflow](https://github.com/laqieer/libsysgba/actions/workflows/c-cpp.yml/badge.svg)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Flaqieer%2Flibsysgba.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Flaqieer%2Flibsysgba?ref=badge_shield)

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

It allows you to access backup media with File I/O in C++ and C.

Special filenames preserved for that:

- `sram:`
- `sram_64KB:`
- `sram_512Kb:`
- `eeprom:`
- `eeprom_512B:`
- `eeprom_4Kb:`
- `eeprom_8KB:`
- `eeprom_64Kb:`

```C
int fd = open("sram:", O_RDWR);
FILE *fp = fopen("sram:", "r+");
```

```C
int fd = open("eeprom:", O_RDWR);
FILE *fp = fopen("eeprom:", "r+");
```


## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Flaqieer%2Flibsysgba.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Flaqieer%2Flibsysgba?ref=badge_large)