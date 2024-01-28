# Introduction

This application simply checks the backend selection of ztimer when trying to
select an optional `periph_rtc`.


# Conflict of optional features

This brings to light a problem with the rtc/rtt modeling, specifically with
`periph_rtc_rtt` and modules such as `rtt_rtc`.


Let's use this as an example:

```
- module_a
  - periph_rtc

- module_b
  - module_c
    - periph_rtt

- module_d
  - module_e
    -  module_f
       - periph_rtc
```

Let's go through each make iteration and assume the include order always
requires another iteration, meaning, the first condition is f, e, d,
c, b, a.

```make
ifneq (,$(filter module_f,$(USEMODULE)))
  FEATURES_OPTIONAL += periph_rtc
endif

ifneq (,$(filter module_e,$(USEMODULE)))
  USEMODULE += module_f
endif

ifneq (,$(filter module_d,$(USEMODULE)))
  USEMODULE += module_e
endif


ifneq (,$(filter module_c,$(USEMODULE)))
  FEATURES_OPTIONAL += periph_rtt
endif

ifneq (,$(filter module_b,$(USEMODULE)))
  USEMODULE += module_c
endif


ifneq (,$(filter module_a,$(USEMODULE)))
  FEATURES_OPTIONAL += periph_rtc
endif
```

## Case 1
```
USEMODULE += module_a
USEMODULE += module_b
```

0. `module_a module_b`
1.0. `module_a module_b module_c`
1.1. `module_a module_b module_c periph_rtc` feature resolution step
2.0. `periph_rtc` prevents `periph_rtt` from `module_c` being selected

## Case 2
```
USEMODULE += module_b
USEMODULE += module_d
```

0. `module_b module_d`
1.0. `module_b module_d module_e`
1.1. `module_b module_d module_e module_c`
2.0. `module_b module_d module_e module_c module_f`
2.1. `module_b module_d module_e module_c module_f periph_rtt` feature resolution step
3.0. `periph_rtt` prevents `periph_rtc` from `module_f` being selected

So we can see different selections depending on how far down the optional dependency is.

This will also have different results if the depending on the order of resolution.

## Case 3

The resolution order is d, e, f, c, b:
```make
ifneq (,$(filter module_d,$(USEMODULE)))
  USEMODULE += module_e
endif

ifneq (,$(filter module_e,$(USEMODULE)))
  USEMODULE += module_f
endif

ifneq (,$(filter module_f,$(USEMODULE)))
  FEATURES_OPTIONAL += periph_rtc
endif

ifneq (,$(filter module_c,$(USEMODULE)))
  FEATURES_OPTIONAL += periph_rtt
endif

ifneq (,$(filter module_b,$(USEMODULE)))
  USEMODULE += module_c
endif
```

```
USEMODULE += module_b
USEMODULE += module_d
```

0. `module_b module_d`
1.0. `module_b module_d module_e`
1.1. `module_b module_d module_e module_f`
1.2. `module_b module_d module_e module_f module_c`
1.3. `module_b module_d module_e module_f module_c periph_rtc` feature resolution step
2.0. `periph_rtc` prevents `periph_rtt` from `module_c` being selected

This shows that simply the order of when an optional feature is requested
matters.

# Conflict of modules

It also seems like there are issues with the `rtt_rtc` that uses the
`periph_rtt` to emulate `periph_rtc`. This means no other module should use
the `periph_rtt` features.

Some CPUs bring that in by default such as the ESP or CC2538.

We cannot predict when the `rtt_rtc` actually gets brought in so we cannot
make any decisions what to use or not use based off of that. Furthermore,
we cannot use the feature conflict since this will have both `periph_rtc` and
`periph_rtt` by it's nature.

# Preferring feature when a conflict happens

There is an issue with make, since it runs several times, other modules may already assume a feature is used and bring in other modules before a conflict would prevent that. Thus, simply saying "if conflict then take the first one in the list" could not work... Though for single cycle resolvers it would be fine.

# Solution

In order to really understand how the ztimer backend selection actually goes we
can do the following:

1. If there are any conflicts just use the `periph_timer` as the backend, this
is potentially less power efficient but much simpler.
2. Add a flag via `FEATURES_PROVIDED` that describes the hardware using `rtt_rtc` to provide a `periph_rtc` interface.
