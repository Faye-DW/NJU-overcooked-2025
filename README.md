# Overcooked

This is the lab for Problem Solving Ⅳ in Nanjing University in 2025.

This lab is to write strategy for the simplified version of the Overcooked game.

## Get Started

```shell
$ git clone https://git.nju.edu.cn/psv/overcooked-2023.git
```

## Compile

On Windows, install CLion or Visual Studio and open lab folder.

Or, simply e.g. :

```shell
$ cd ./source
$ g++ --std=c++20 -I ../include *.cpp # which will generate a.exe
$ g++ --std=c++20 -I ../include *.cpp -o ../a.exe
$ ./QtOvercooked.exe -p a.exe -l ./maps/level1/level1-1.txt
$ ./runner.exe -p a.exe -l ./maps/level1/level1-1.txt
```
level2-4中，玩家1到dirty-plate-wait处无dirty-plate回到了sink

On macOS, install CLion or use make directly:

```shell
$ make
```

## Submit

To submit manually, remove all files except `.git` and create a zip archive of your lab folder, then upload it to online judge. The archive should not exceed 10MiB in size and its content looks like:

```
XXXXXX.zip
  - Lab (any folder)
    - .git
```

or

```
XXXXXX.zip
  - .git
```

On macOS, run `make submit TOKEN=token` or submit manually.