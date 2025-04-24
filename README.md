# lines (cli tool)

## requirements

- C compiler in your path that is called when `gcc` is typed
- GNU make
- `clang-format` if you want to format the code after making changes

## install

`$ make help`

## usage

### standard

`$ lines`

### debug

`$ DEBUG=. lines`

### examples

Standard mode:
```
$ lines ".gitignore" "Makefile" "*.md" "*.c"
      53 ./Makefile
      30 ./README.md
      99 ./main.c
       1 ./.gitignore
     183 total
```
Debug mode:
```
$ DEBUG=. lines ".gitignore" "Makefile" "*.md" "*.c"
PATTERN[1] = ".gitignore"
PATTERN[2] = "Makefile"
PATTERN[3] = "*.md"
PATTERN[4] = "*.c"
      53 ./Makefile
      30 ./README.md
      99 ./main.c
       1 ./.gitignore
     183 total
```

## why

I didn't want to learn how to write portable shell to create this in the CLI.
It was faster to just write the tool in C.

The real problem was building the
`find . -name [arg1] (-o -name arg2) ... (-o -name argN)` command at runtime.
(I also just like having an excuse to write some C code :D)
