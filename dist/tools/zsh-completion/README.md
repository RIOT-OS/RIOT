# Command Line Completion Using ZSH

This provides command line completion for `zsh` to increase productivity when
working with RIOT build system. The completion focuses on `make` targets,
options, and variables commonly used when interacting with the RIOT build system
from the shell. Aspects of the build system typically only used by scripts or
the CI are (intentionally) not added.

## Installation

1. Copy the `zsh-riot.zsh` script where you can find it,
   e.g. `cp zsh-riot.zsh ~/.zsh-riot.zsh`
2. Source it after the completion definition of `make` is loaded, e.g. at the
   end of your `~/.zshrc`:

``` sh
# ~/.zsh
# [...]
source ~/.zsh-riot.zsh
```
