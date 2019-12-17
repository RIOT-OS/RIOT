# ESP log output test

## About

The application corresponds exactly to the application `tests/shell`.
It is only for testing the ESP-specific logging functions enabled by the
`esp_log_startup` and `esp_log_tagged` modules. In fact, the application is
nothing more than the `tests/shell` application compiled with the following
command:

```
LOG_LEVEL=4 USEMODULE=esp_log_tagged make -C tests/shell
```

## Expected Results

Since the application corresponds exactly to the application `tests/shell`,
it can be used in same way. However, with this application, you should get
a log with additional information, including the board configuration, at
startup. All output is in a tagged format such as

```
D (1470) [system_init] Heap free: 185612 bytes
```

where the `D` stands for a debug message, the number in parentheses is the
system time in milliseconds and the name in brackets is the module name or
the function that generated the output.
