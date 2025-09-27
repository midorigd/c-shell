# CShell (csh)

This is a custom implementation of a UNIX shell in C.

The fundamentals of this project are based on Stephen Brennan's "Write a Shell in C" tutorial. The shell is currently in a very basic stage, but I hope to expand on it and add many more features over time.

## Current Features

In addition to program calls, the shell supports the following built-in commands:

```zsh
cd
help
exit
```

## Coming Soon

I'm planning to add these features next:

- Support for escape sequences and string literals in arguments
- More builtins (pwd)
- Configuration files
- UX improvements
  - Display current working directory in prompt
  - ANSI color formatted messages
- Pipelining

## References

[Write a Shell in C (Stephen Brennan)](https://brennan.io/2015/01/16/write-a-shell-in-c/)
