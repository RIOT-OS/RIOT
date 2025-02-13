# Build In Docker                                       {#build-in-docker}

Some parts of RIOT's build process can be performed inside a Docker container,
which comes with the necessary compilers and toolchains and is fully managed by
the build system. It can be enabled by passing `BUILD_IN_DOCKER=1` to make.

```shell
$ BUILD_IN_DOCKER=1 make
```

If your user does not have permissions to access the Docker daemon:

```shell
$ BUILD_IN_DOCKER=1 DOCKER="sudo docker" make
```

to always use Docker for building, set `BUILD_IN_DOCKER=1` (and if necessary
`DOCKER="sudo docker"`) in the environment:

```console
$ export BUILD_IN_DOCKER=1
```

## Targets ran in Docker: DOCKER_MAKECMDGOALS_POSSIBLE

Currently only build related targets are ran in the docker container, the exact
list is under `DOCKER_MAKECMDGOALS_POSSIBLE` variable.

## Environment Variables: DOCKER_ENV_VARS

When building in docker one might want for the command ran in docker to inherit
variables that might have been set in the command line. e.g.:

```shell
BOARD=samr21-xpro USEMODULE=xtimer make -C examples/essentials/hello-world
```

In `docker.ink.mk` the origin of a variable listed in `DOCKER_ENV_VARS` is checked
and if the origin is `environment` or `command` (for make command-line argument)
then those variables will be  passed to the docker container.

You can also set in `DOCKER_ENV_VARS` in the environment to add variables to the
list, e.g.:

```shell
DOCKER_ENV_VARS=BEER_TYPE BEER_TYPE="imperial stout" BUILD_IN_DOCKER=1 make -C examples/essentials/hello-world/
docker run --rm -t -u "$(id -u)" \
    ...
    -e 'BEER_TYPE=imperial stout' \
    -w '/data/riotbuild/riotbase/examples/essentials/hello-world/' \
    'riot/riotbuild:latest' make
```

Your application Makefile can also extend `DOCKER_ENV_VARS`.

### Directly Define Environment Variables: DOCKER_ENVIRONMENT_CMDLINE

`DOCKER_ENVIRONMENT_CMDLINE` can be used to add variables directly to the environment
but will need to be prefixed with `-e` (see [option-summary]).

e.g.:

```
DOCKER_ENVIRONMENT_CMDLINE='-e BEER_TYPE="imperial stout"' BUILD_IN_DOCKER=1 make -C examples/essentials/hello-world/
docker run --rm -t -u "$(id -u)" \
    ...
    -e 'BEER_TYPE=imperial stout' \
    -w '/data/riotbuild/riotbase/examples/essentials/hello-world/' \
    'riot/riotbuild:latest' make
```

## Command Line Variables: DOCKER_OVERRIDE_CMDLINE

Command line arguments are variables that are passed after `make` e.g.
`make target FOO=bar`, but different to environment variables a variable defined
through the command line will take precedence over all assignments of `FOO` within
the makefile (same effect as adding `-e` for environment variables, see
[option-summary] for more details.

To pass variables overriding the command-line to docker `DOCKER_OVERRIDE_CMDLINE`
may be used:

```shell
DOCKER_OVERRIDE_CMDLINE="BEER_TYPE='imperial stout'" BUILD_IN_DOCKER=1 make -C examples/essentials/hello-world/ RIOT_CI_BUILD=1
Launching build container using image "riot/riotbuild:latest".
sudo docker run --rm -t -u "$(id -u)" \
    ...
    -w '/data/riotbuild/riotbase/examples/essentials/hello-world/' \
    'riot/riotbuild:latest' make  BEER_TYPE='imperial stout' 'RIOT_CI_BUILD=1'
```

### Redefined or Overridden Variables: DOCKER_ENV_VARS_ALWAYS

There is a corner case for the handling of `DOCKER_ENV_VARS`. If a variable is
redefined (`+=`, `=`, `:=`) or overridden then the origin of the variable will be changed
to `file` and there is no way of detecting in Make how it was set.

If this happens after `docker.ink.mk` this is not an issue, but for all variables
susceptible to be defined in the application `Makefile` this is indeed the case.

A subset of these variables, namely variables relating to dependency resolution
are therefore unconditionally passed to docker. The complete list can be found
under `DOCKER_ENV_VARS_ALWAYS`.

#### CFLAGS

CFLAGS are not automatically passed to docker because they might contain spaces,
'"' or other characters that will require escaping. The solution is to pass it with
`DOCKER_ENVIRONMENT_CMDLINE` and escape every character as required.

e.g: if wanting to override STRING_WITH_SPACES

```
# normal build
CFLAGS=-DSTRING_WITH_SPACES='\"with space\" make
# in docker
DOCKER_ENVIRONMENT_CMDLINE='-e CFLAGS=-DSTRING_WITH_SPACES=\'\\\"with\ space\\\"\'' \
  BUILD_IN_DOCKER=1 make
```

Alternatively, it is often easier to define the CFLAGS in the Makefile which gets
evaluated inside the Docker image again), conditional on a less complex environment
variable that gets added to `DOCKER_ENV_VARS` in the Makefile.

[option-summary]: https://www.gnu.org/software/make/manual/html_node/Options-Summary.html
