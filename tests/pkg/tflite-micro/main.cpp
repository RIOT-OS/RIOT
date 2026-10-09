/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       TensorFlow Lite test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

/* Provide an Arduino-Like API to be able to easily reuse the code from
   upstream examples */
void setup();
void loop();

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    setup();

    while (true) {
      loop();
    }
}
