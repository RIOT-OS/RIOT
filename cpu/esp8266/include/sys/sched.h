/* in the xtensa-esp8266-elf toolchain in the docker container
 * "sys/types.h" includes "sys/sched.h", which in turn results in conflicts with
 * RIOT's scheduler. This file is an ugly workaround to fix the issues */

struct sched_param {
    int this_is_just_a_mock_type_for_api_compatibility;
};
