# CShell (csh)

This is a custom implementation of a UNIX shell in C.

The fundamentals of this project are based on Stephen Brennan's "Write a Shell in C" tutorial. The shell is currently in a fairly basic stage, but I hope to expand on it and add many more features over time.

## Current features

10.11.25: The shell now supports advanced parsing, such as escape sequences and quoted literals. This includes:

- Escaping spaces to treat multiple words as a single token
- Common escape-sequences characters like `\n`, `\t`
- Single-quoted strings are interpreted literally
- Double-quoted strings support escape sequences

In addition to program calls, the shell supports the following built-in commands:

```zsh
cd
help
exit
```

## Coming soon

I'm planning to add these features next:

- More builtins (pwd)
- Configuration files
- UX improvements
  - Display current working directory in prompt
  - ANSI color formatted messages
- Pipelining

## Building the project

Run the following from the terminal:

```zsh
git clone https://github.com/midorigd/c-shell

make [OPT_CFLAGS=-DDEBUG_MODE] clean
```

### Flags

`-DDEBUG_MODE`: Enables debug mode, prints tokens as they are parsed and built

## Running the project

Run the following from the project directory:

```zsh
./csh
```

## References

[Write a Shell in C (Stephen Brennan)](https://brennan.io/2015/01/16/write-a-shell-in-c/)
