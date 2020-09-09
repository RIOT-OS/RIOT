/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
