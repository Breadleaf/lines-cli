# lines (cli tool)

`lines` is a tool that I created to tell me the line count of files in my
projects as specified by the glob patterns. running it from the root of a
project will recurse into the sub-directories, and find all files. I didn't
write any of the hard parts of this tool, I am basically just automating the cli
commands `find`, `xargs`, and `wc`.

## requirements

- C compiler in your path that is called when `gcc` is typed
- GNU make
- `clang-format` if you want to format the code after making changes
- coreutils
- find
- xargs

## install

`$ make help`

## usage

### standard

`$ lines`

### debug

`$ DEBUG=. lines`

### ignore git

- This mode can also be used with debug mode: `DEBUG=. IGNORE_GIT=. lines`
- This mode will igore all files matching `./.git*`

`$ IGNORE_GIT=. lines`

### examples

Standard mode:
```
$ ls -Ap
.git/ .gitignore lines main.c Makefile README.md
$ ./lines "*.c" "*.md" "Makefile"
      72 ./Makefile
      82 ./README.md
     134 ./main.c
     288 total
```
Debug mode:
```
$ ls -Ap
.git/ .gitignore lines main.c Makefile README.md
$ DEBUG=. ./lines "*.c" "*.md" "Makefile"
PATTERN[1] = "*.c"
PATTERN[2] = "*.md"
PATTERN[3] = "Makefile"
Running 'find' command:
(find . -name *.c -o -name *.md -o -name Makefile -type f)
Running 'xargs' command:
(xargs wc -l)
      72 ./Makefile
      82 ./README.md
     134 ./main.c
     288 total
```
Ignore Git mode:
```
$ ls -Ap
.git/ .gitignore lines main.c Makefile README.md
$ INGORE_GIT=. ./lines *
      72 ./Makefile
      82 ./README.md
     134 ./main.c
      18 ./lines
     306 total
```

### tips

#### specifying files

When you want to refer to `*.py` and you are using `venv`, you may think to use
`lines "*.py"` but this will not work. This will recurse into `venv` or other
folders with `*.py` files within. To get around this, let the shell expand it
for you by using `lines *.py`.

#### specifying dirs

This feature is very weird. I am going to do a rewrite at some point to fix it.

## why

I didn't want to learn how to write portable shell to create this in the CLI.
It was faster to just write the tool in C.

The real problem was building the
`find . -name [arg1] (-o -name arg2) ... (-o -name argN)` command at runtime.
(I also just like having an excuse to write some C code :D)
