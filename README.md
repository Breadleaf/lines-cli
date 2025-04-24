# lines (cli tool)

## requirements

- C compiler in your path that is called when `gcc` is typed
- GNU make
- `clang-format` if you want to format the code after making changes

## usage

`$ make help`

## why

I didn't want to learn how to write portable shell to write this in the CLI.
It was faster to just write the tool in C.

The real problem was building the
`find . -name [arg1] (-o -name arg2) ... (-o -name argN)` command at runtime.
(I also just like having an excuse to write some C code :D)
