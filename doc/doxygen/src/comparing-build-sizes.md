There is a make target for build size comparison. You can use it like that:
```
$ cd RIOT/test/test_something

$ git checkout master
$ BINDIRBASE=master-bin make buildtest

$ git checkout my-branch
$ BINDIRBASE=my-branch-bin make buildtest

$ OLDBIN=master-bin NEWBIN=my-branch-bin make info-buildsizes-diff
text    data    bss     dec     BOARD/BINDIRBASE

0       0       0       0       avsextrem    **← this line contains the diff**
57356   1532    96769   155657  master-bin
57356   1532    96769   155657  my-branch-bin

...
```
**Note:** make info-buildsizes-diff needs bash 4, so your system must not be older than 20-Feb-2009 …

Check it out, the output contains colors. ;)

(This page is mostly a copy of René Kijewski's explanation in https://github.com/RIOT-OS/RIOT/pull/1033.)
