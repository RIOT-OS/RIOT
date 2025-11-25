# Command Line Completion Using ZSH

This provides command line completion for `zsh` to increase productivity when
working with RIOT build system. The completion focuses on `make` targets,
options, and variables commonly used when interacting with the RIOT build system
from the shell. Aspects of the build system typically only used by scripts or
the CI are (intentionally) not added.

## Installation

1. Copy the `zsh-riot.sh` script where you can find it,
   e.g. `cp zsh-riot.sh ~/.zsh-riot.sh`
2. Hook up the custom RIOT specific make completion in `.zshrc` and use that
   instead of the default `make` completion if and only if the working
   directory is inside a RIOT repository. This can be done by adding the
   following snippet:

``` sh
source ~/.zsh-riot.sh
compdef '
if git rev-parse --is-inside-work-tree &> /dev/null && [[ -e "$(git rev-parse --show-toplevel)/.murdock" ]]; then
  _riot
else
  _make
fi
' make
```
