Conventions for defining CPU in the build system {#build-system-cpu-conventions}
================================================

[TOC]

Introduction
============

This page describes some conventions for defining CPU in the build system.
Not all of them are currently listed but only particular ones.


`CPU` defining multiple `CPU_MODEL`s         {#cpu-defining-multiple-cpu-models}
====================================

Context                                                               {#context}
-------

With the increasing number of supported CPU models which have lots of
similarities with other ones from the same family, there is a need to declare
them in a single directory but still allow model specific definitions
and behaviors. This leads to the difference between `CPU` and `CPU_MODEL`.

Several handling were implemented in the build system using shell invocations
and algorithmic resolution based on the models characters.
However, due to the algorithmic resolution one cannot know anymore the value of
the specific CPU model definitions without executing the build system and the
shell `sed` magic.

This causes both issue to read the definition and slow execution time for
definitions that are a static mapping from the exact CPU model.

With the way of defining boards and CPU, some informations should be defined
either for the description, dependency resolution or compilation
(respectively in the `Makefile.features`, the `Makefile.dep` or `Makefile.include`)
so may want to define only each piece of information required for each step.


Different solutions                                       {#different-solutions}
-------------------

This describes different possible solutions, and the cases on which each of them applies.

If only one should be used, then it should be solution 1 to go toward
declarative syntax and support all cases.
But due to the overhead, intermediate single file declarative solution 3 and 4
are good candidates too.

As currently dependencies are not declarative, only solutions 1 and 2 would be
able to declare specific `CPU_MODEL` dependencies. Solutions 3 and 4 are
declarative only.

The main goal is to remove the algorithmic and shell based resolutions for this
if possible.


### 1. File based handling                                {#file-based-handling}

Somehow, having all `CPU_MODEL` in one directory is just to save common
parts and keep the `cpu` directory tidy, all the different ones could still
be defined in their own directory, but nested in the common one.

This could transfer to have for each `CPU_MODEL` for a `CPU` the following files:

~~~~~~~~~~~~~~~~~~~ {.mk}
$(RIOTCPU)/$(CPU)/models/$(CPU_MODEL)/Makefile.features
$(RIOTCPU)/$(CPU)/models/$(CPU_MODEL)/Makefile.dep
$(RIOTCPU)/$(CPU)/models/$(CPU_MODEL)/Makefile.include
$(RIOTCPU)/$(CPU)/models/$(CPU_MODEL)/doc.txt
~~~~~~~~~~~~~~~~~~~

And then each main `$(CPU)` file will include the `$(CPU_MODEL)` specific one.

This would match the way to do it in RIOT but could create many
directories and files for maybe no or only few lines.

It could make sense to go with this if the files, especially the
`Makefile.dep` or `Makefile.include` ones would need many specific definitions
and handling that are more than variables configuration for the main `CPU`
files.


### 2. `ifneq` based handling                            {#ifneq-based-handling}

To not add any files, the main `CPU` files could add specific definitions if the
`CPU_MODEL` matches certain values.

~~~~~~~~~~~~~~~~~~~ {.mk}
ifneq (,$(filter abc1 abd0 ade5,$(CPU_MODEL)))
  FEATURES_PROVIDED += periph_hwrng
endif
~~~~~~~~~~~~~~~~~~~

This allows simple handling when the benefit of having per model definition
would currently not overcome the overhead of defining it.

An issue with this is that it is not declarative and does not have equivalent
in a declarative only language that is not Turing complete as `make`.
But as long as it is used with explicit inclusion lists (no wildcard) and no
blacklists, it is easy to map it to a per model definition from the other
methods.


### 3. Exhaustive per model definition        {#exhaustive-per-model-definition}

In the case of only declarative values of specific variables, handling different
`CPU_MODEL` is simply defining one per `CPU_MODEL`.
When not relying on an `if` evaluation as before, one could define dictionaries
which are implemented in `make` when using only one file with "namespaced"
variables names including the `key` and so the `CPU_MODEL` value in the name.

~~~~~~~~~~~~~~~~~~~ {.mk}
ROM_LEN_stm32f103c8 = 64K
ROM_LEN_stm32f103cb = 128K
ROM_LEN_stm32f103rb = 128K
ROM_LEN_stm32f103re = 512K

ROM_LEN = ROM_LEN_$(CPU_MODEL)
~~~~~~~~~~~~~~~~~~~

This represents a direct mapping of the `CPU_MODEL` to the variable value.
This exact format makes sense when there are not too many variables or for
variables whose value is a list of values (this is a case where solution 4 does
not apply).
This is true for example for `FEATURES_PROVIDED`.

It is easy to read and to grep, but can give quite indigests lists if all
`ROM_LEN`, `ROM_ADDR`, `RAM_LEN`, `RAM_ADDR` are all defined for many models.

To not clutter the main `CPU` definition makefiles, these definitions can be
moved to separate files `models/features.inc.mk` `models/include.inc.mk` or
similar names. We would still put the `ROM_LEN = ROM_LEN_$(CPU_MODEL)` in the
main file to separate the "database" from setting the value.


### 4. Grouped per model definition              {#grouped-per-model-definition}

Again in the case only delcarative values of specific variables, it can happen
that many variables are somehow highly correlated.
For example the `ROM`, `RAM` size and address or different classification
that makes sense for that manufacturer like "family", "series", "core".
In that case, defining one variable for each may seem tedious and one could
prefer defining one variable for all of these and refer to the array index.

Defining `Makefile.features` variables:

~~~~~~~~~~~~~~~~~~~ {.mk}
# CPU_FAM CPU_ARCH
STM32F1_INFOS_FEATURES_stm32f103c8 = stm32f1 cortex-m3
STM32F1_INFOS_FEATURES_stm32f103cb = stm32f1 cortex-m3
STM32F1_INFOS_FEATURES_stm32f103rb = stm32f1 cortex-m3
STM32F1_INFOS_FEATURES_stm32f103re = stm32f1 cortex-m3
# I know they are all the same here but its an example
CPU_FAM  = $(word 1,$(STM32F1_INFOS_FEATURES_$(CPU_MODEL)))
CPU_ARCH = $(word 2,$(STM32F1_INFOS_FEATURES_$(CPU_MODEL)))
~~~~~~~~~~~~~~~~~~~

Defining `Makefile.include` variables:
~~~~~~~~~~~~~~~~~~~ {.mk}
# ROM_LEN RAM_LEN ROM_START_ADDR RAM_START_ADDR
STM32F1_INFOS_INCLUDE_stm32f103c8 = 64K 0x08000000 20K 0x20000000
STM32F1_INFOS_INCLUDE_stm32f103cb = 128K 0x08000000 20K 0x20000000
STM32F1_INFOS_INCLUDE_stm32f103rb = 128K 0x08000000 20K 0x20000000
STM32F1_INFOS_INCLUDE_stm32f103re = 512K 0x08000000 64K 0x20000000

ROM_LEN        = $(word 1,$(STM32F1_INFOS_INCLUDE_$(CPU_MODEL)))
ROM_START_ADDR = $(word 2,$(STM32F1_INFOS_INCLUDE_$(CPU_MODEL)))
RAM_LEN        = $(word 3,$(STM32F1_INFOS_INCLUDE_$(CPU_MODEL)))
RAM_START_ADDR = $(word 4,$(STM32F1_INFOS_INCLUDE_$(CPU_MODEL)))
~~~~~~~~~~~~~~~~~~~

This represents the mapping of the `CPU_MODEL` to a group of values describing
it that can then easily be extracted. It takes a tiny bit more effort to know
what is in the 4th column at first sight but the declarations are more concise.
It is similar to the previous solution but with variables merging.
However it does not work when a value may be a list of variable length like
`FEATURES_PROVIDED`.

In that example I did not add names for the index like `CPU_FAM_INDEX = 1` but
would help for readability and avoid magic numbers.
