CI specific scripts
===================

Static tests
------------

The [static_tests.sh] script runs all static tests and outputs them in a unified
output. You can also run this script by running `make static-test` in the base
directory.

To add a new static test, just write a `check.sh` script or call your static
checker directly and add it to the list of static tests in [static_tests.sh]:

```sh
ENV1=foobar ENV2=snafu run <your test command> --args
```

Github annotations
------------------
Using [github_annotate.sh] you can generate [Github annotations] for your
tests. You can generate both warnings and errors. Before doing anything however,
you include [github_annotate.sh]\ (assuming `${RIOTTOOLS is set to}` [dist/tools/])

```sh
. "${RIOTTOOLS}"/ci/github_annotate.sh
```

and set-up the mechanism

```sh
github_annotate_setup
```

If your tests generate output you now can pipe that to the file `${LOGFILE}` by
using `${LOG}`, e.g.

```sh
my_awesome_test | ${LOG}
```

Don't worry, your test will function normal still, if you don't run it in a
Github Action!

Now you can use `github_annotate_error` and `github_annotate_warning` to
generate the actual errors and warnings. Both commands expect 3 parameters:
1. `FILENAME`: The name of the file the warning or error occurred in,
2. `LINENUM`: The number of the line the warning or error occurred in, and
3. `DETAILS`: Some descriptive details or the message of your warning or error.

You can parse those from `${LOGFILE}` (e.g. using tools such as `grep`, `sed`,
or `awk`) or generate them on the fly, if your script allows for that. E.g.

```sh
cat ${LOGFILE} | grep '^.*:[0-9]\+:' | while read error; do
    github_annotate_error \
        $(echo "${error}" | cut -d: -f1) \
        $(echo "${error}" | cut -d: -f2) \
        $(echo "${error}" | cut -d: -f3-)
done
```

If your output has the common output format `<filename>:<lineno>:<details>` you
can also use the function `github_annotate_parse_log_default`. It takes the
annotation function it should call on every line as an optional parameter with
`github_annotate_error` being the default. E.g.

```sh
github_annotate_parse_log_default github_annotate_warning
```

does the same as the last example snippet, but uses `github_annotate_warning`
instead.

If you do not need to provide a file with your error or warning, you can also
use `github_annotate_error_no_file` or `github_annotate_warning_no_file`,
respectively. Both take a just message as single parameter:

```sh
github_annotate_error_no_file "Something is wrong!"
```

After all errors or warnings are annotated, call `github_annotate_teardown` to
finish annotations.

**Note:** `github_annotate_report_last_run` is called within [static_tests.sh]
to attach the actual annotations to your PR. You don't need to call it from
within your test if you are adding that test to [static_tests.sh].

Checking if Fast CI Runs are Sufficient
---------------------------------------

The script `can_fast_ci_run.py` checks if a given change set a PR contains
justifies a full CI run, or whether only building certain apps or all apps for
certain boards is sufficient and which those are. The script will return with
exit code 0 if a fast CI run is sufficient and yield a JSON containing the
apps which need to be rebuild (for all boards) and the list of boards for which
all apps need to be rebuild.

### Usage

1. Pull the current upstream state into a branch (default: `master`)
2. Create a temporary branch that contains the PR either rebased on top of the
   upstream state or merged into the upstream state
3. Check out the branch containing the state of upstream + your PR
4. Run `./dist/tools/ci/can_fast_ci_run.py`

#### Options

- If the script is not launched in the RIOT repository root, provide a path
  to the repo root via `--riotbase` parameter
- If the upstream state is not in `master`, the `--upstreambranch` parameter
  can be used to specify it (or a commit of the current upstream state)
- If the script opts for a full rebuild, the passing `--explain` will result
  in the script explaining its reasoning
- To inspect the classification of changed files, the `--debug` switch will
  print it out

#### Gotchas

- If the script is not launched in a branch that contains all changes of the
  upstream branch, the diff set will be too large.
- The script relies on the presence of a `Makefile` to detect the path of
  modules. If changed files have no parent directory containing a `Makefile`
  (e.g. because a module was deleted), the classification will fail. This
  results in a full CI run, but this is the desired behavior anyway.
- Right now, any change in a module that is not a board, or in any package, will
  result in a full CI run. Maybe the KConfig migration will make it easier to
  get efficiently get a full list of applications depending on any given module,
  so that fast CI runs can also be performed when modules and/or packages are
  changed.

[static_tests.sh]: ./static_tests.sh
[Github annotations]: https://github.blog/2018-12-14-introducing-check-runs-and-annotations/
[github_annotate.sh]: ./github_annotate.sh
[dist/tools]: ../
