/*
 * Copyright (c) 2010, Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of libmc1322x: see http://mc1322x.devl.org
 * for details.
 *
 *
 */

/* control reset and VREF2 lines */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <ftdi.h>

#define DEBUG 0

#define low(x)  (1 << x)
#define high(x) (1 << (x + 8))

#define REDBEE_ECONOTAG_RESET    high(2)
#define REDBEE_ECONOTAG_VREF2L   high(7)
#define REDBEE_ECONOTAG_VREF2H   high(6)
#define REDBEE_ECONOTAG_INTERFACE INTERFACE_A

#define REDBEE_USB_RESET    high(2)
#define REDBEE_USB_VREF2L   low(5)
#define REDBEE_USB_VREF2H   low(6)
#define REDBEE_USB_INTERFACE INTERFACE_B

#define FLEXIBITY_USB_RESET    high(2)
#define FLEXIBITY_USB_VREF2L   high(7)
#define FLEXIBITY_USB_VREF2H   high(6)
#define FLEXIBITY_USB_INTERFACE INTERFACE_A

#define BOARD REDBEE_USB

#define STR(x)         #x
#define STR2(x)        STR(x)
#define CAT(x,y)       x##y
#define CAT2(x, y, z)  x##y##z

#define dir(x)            ( CAT(x,_RESET) | CAT(x,_VREF2L) | CAT(x,_VREF2H))
#define interface(x)      ( CAT(x,_INTERFACE) )
#define reset_release(x)  ( CAT(x,_RESET)     )
#define reset_set(x)      ( 0 )
#define vref2_normal(x)   ( CAT(x,_VREF2H)    )
#define vref2_erase(x)    ( CAT(x,_VREF2L)    )

/* fgets input buffer length: for prompts and such */
#define BUF_LEN 32

struct layout {
    char *name;
    char *desc;
    enum ftdi_interface interface;
    uint16_t dir;
    uint16_t reset_release;
    uint16_t reset_set;
    uint16_t vref2_normal;
    uint16_t vref2_erase;
};

int print_and_prompt(struct ftdi_device_list *devlist);
int bb_mpsee(struct ftdi_context *ftdic, uint16_t dir, uint16_t val);
void reset(struct ftdi_context *ftdic, const struct layout *l);
void erase(struct ftdi_context *ftdic, const struct layout *l);
void usage(void);

#define std_layout(x)                        \
    .interface = interface(x),           \
        .dir = dir(x),                       \
    .reset_release = reset_release(x),   \
    .reset_set = reset_set(x),       \
    .vref2_normal = vref2_normal(x),     \
    .vref2_erase = vref2_erase(x),

static struct layout layouts[] = {
    {
        .name = "redbee-econotag",
        .desc = "Redbee Econotag",
        std_layout(REDBEE_ECONOTAG)
    },
    {
        .name = "redbee-usb",
        .desc = "Redbee USB stick",
        std_layout(REDBEE_USB)
    },
    {
        .name = "flexibity",
        .desc = "Flexibity USB Interface",
        std_layout(FLEXIBITY_USB)
    },
    { .name = NULL, /* end of table */ },
};

struct command {
    char *name;
    char *desc;
    void (*cmd)(struct ftdi_context *ftdic, const struct layout *l);
};

static const struct command commands[] = {
    {
        .name = "reset",
        .desc = "Toggles reset pin",
        .cmd = reset,
    },
    {
        .name = "erase",
        .desc = "Sets VREF2 erase mode; toggles reset; waits 2 sec.; sets normal; toggles reset again",
        .cmd = erase,
    },
    { .name = NULL, /* end of table */ },
};

struct layout *find_layout(char *str)
{
    uint32_t i = 0;

    while (layouts[i].name != NULL) {
        if (strcmp(layouts[i].name, str) == 0) {
            return &layouts[i];
        }

        i++;
    }

    return NULL;
}

static uint32_t vendid = 0x0403;
uint32_t prodid = 0x6010;

#if __APPLE__
static void restore_ftdi_kext(void)
{
    system("sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext");
}
#endif

int main(int argc, char **argv)
{
    struct ftdi_context ftdic;
    struct ftdi_device_list *devlist;
    int dev_index = -1;
    int num_devs;
    char layout_str[BUF_LEN];
    struct layout layout;
    struct layout *l = NULL;
    int i, ret;

    /* overrides for layout parameters */
    int interface      = -1;
    int dir            = -1;
    int reset_release  = -1;
    int reset_set      = -1;
    int vref2_normal   = -1;
    int vref2_erase    = -1;

    layout.name = NULL;

    while (1) {
        int c;
        int option_index = 0;
        static struct option long_options[] = {
            {"layout",        required_argument, 0, 'l'},
            {"index",         required_argument, 0, 'i'},
            {"vendor",        required_argument, 0, 'v'},
            {"product",       required_argument, 0, 'p'},
            {"dir",           required_argument, 0,  0 },
            {"reset_release", required_argument, 0,  0 },
            {"reset_set",     required_argument, 0,  0 },
            {"vref2_normal",  required_argument, 0,  0 },
            {"vref2_erase",   required_argument, 0,  0 },
            {"interface",     required_argument, 0,  0 },
            {"help",          no_argument,       0, '?'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "i:l:v:p:",
                        long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
                /* process long opts */
            case 0:
                if (strcmp(long_options[option_index].name, "interface") == 0) {
                    sscanf(optarg, "%11i", &interface);
                }

                if (strcmp(long_options[option_index].name, "dir") == 0) {
                    sscanf(optarg, "%11i", &dir);
                }

                if (strcmp(long_options[option_index].name, "reset_release") == 0) {
                    sscanf(optarg, "%11i", &reset_release);
                }

                if (strcmp(long_options[option_index].name, "reset_set") == 0) {
                    sscanf(optarg, "%11i", &reset_set);
                }

                if (strcmp(long_options[option_index].name, "vref2_normal") == 0) {
                    sscanf(optarg, "%11i", &vref2_normal);
                }

                if (strcmp(long_options[option_index].name, "vref2_erase") == 0) {
                    sscanf(optarg, "%11i", &vref2_erase);
                }

                break;

            case 'l':
                strncpy(layout_str, optarg, BUF_LEN);
                break;

            case 'i':
                dev_index = atoi(optarg);
                break;

            case 'v':
                sscanf(optarg, "%11i", &vendid);
                break;

            case 'p':
                sscanf(optarg, "%11i", &prodid);
                break;

            default:
                usage();
                break;
        }
    }

    if (!(l = find_layout(layout_str)) &&
        !((interface >= 0) &&
          (dir >= 0) &&
          (reset_release >= 0) &&
          (reset_set >= 0) &&
          (vref2_normal >= 0) &&
          (vref2_erase >= 0))
       ) {

        printf("*** You must specify a layout or a complete set of overrides\n");
        return EXIT_FAILURE;
    }

    if (l) {
        memcpy(&layout, l, sizeof(struct layout));
    }

#define override(x) if(x > 0) { layout.x = x; }
    override(interface);
    override(dir);
    override(reset_release);
    override(reset_set);
    override(vref2_normal);
    override(vref2_erase);

    if ((num_devs = ftdi_usb_find_all(&ftdic, &devlist, vendid, prodid)) < 0) {
        fprintf(stderr, "ftdi_usb_find_all failed: %d (%s)\n",
                num_devs,
                ftdi_get_error_string(&ftdic));
        return EXIT_FAILURE;
    }

    if (ftdi_init(&ftdic) < 0) {
        fprintf(stderr, "ftdi_init failed\n");
        return EXIT_FAILURE;
    }

    if ((ret = ftdi_set_interface(&ftdic, layout.interface)) < 0) {
        fprintf(stderr, "couldn't set interface %d, err %d (%s)\n", layout.interface, ret, ftdi_get_error_string(&ftdic));
        return EXIT_FAILURE;
    }

    printf("Found %d devices with vendor id 0x%04x product id 0x%04x\n",
           num_devs, vendid, prodid);

    if (num_devs == 0) {
        return EXIT_SUCCESS;
    }

    if (num_devs == 1) {
        dev_index = 0;
    }

    while ((dev_index < 0) || (dev_index >= num_devs)) {
        dev_index = print_and_prompt(devlist);
    }

    if (layout.name != NULL) {
        printf("Opening device %d interface %d using layout %s\n",
               dev_index, layout.interface, layout.name);
    }
    else {
        printf("Opening device %d interface %d without a layout.\n",
               dev_index, layout.interface);
    }

    if ((ret = ftdi_usb_open_desc_index(
                   &ftdic,
                   vendid,
                   prodid,
                   NULL,
                   NULL,
                   dev_index)) < 0) {
#if __APPLE__

        if ((ret == -5) && (0 == system("sudo kextunload /System/Library/Extensions/FTDIUSBSerialDriver.kext"))) {
            // Try again without the FTDI kext loaded this time
            atexit(&restore_ftdi_kext);
            ret = ftdi_usb_open_desc_index(
                      &ftdic,
                      vendid,
                      prodid,
                      NULL,
                      NULL,
                      dev_index
                  );
        }

        if (ret)
#endif // __APPLE__
        {
            fprintf(stderr, "couldn't open dev_index %d, err %d (%s)\n", dev_index, ret, ftdi_get_error_string(&ftdic));
            return EXIT_FAILURE;
        }
    }


    for (i = 0; commands[i].name != NULL; i++) {
        if ((argv[optind] != NULL) &&
            (strcmp(commands[i].name, argv[optind]) == 0)) {
            break;
        }
    }

    if (commands[i].name != NULL) {
        commands[i].cmd(&ftdic, &layout);
    }
    else {
        printf("invalid command\n");

        ftdi_list_free(&devlist);
        ftdi_deinit(&ftdic);

        return EXIT_FAILURE;
    }

    printf("done.\n");

    ftdi_list_free(&devlist);
    ftdi_deinit(&ftdic);

    return EXIT_SUCCESS;
}

void usage(void)
{
    int i;
    printf("Usage: bbmc [options|overrides] -l|--layout layout command \n");
    printf("Commands:\n");

    for (i = 0; commands[i].name != NULL; i++) {
        printf("           %s: %s\n", commands[i].name, commands[i].desc);
    }

    printf("\n");
    printf("Required options:\n");
    printf("           -l|--layout\t specifiy which board layout to use\n");
    printf("                      \t layout is not necessary with a full\n");
    printf("                      \t set of overrides\n");
    printf("\nLayout overrides:\n");
    printf("           --interface\t\t FTDI interface to use\n");
    printf("           --dir\t\t direction (1 is output)\n");
    printf("           --reset_release\t reset release command\n");
    printf("           --reset_set\t\t reset set command\n");
    printf("           --vref2_normal\t vref2 normal\n");
    printf("           --vref2_erase\t vref2 erase\n");
    printf("\n");
    printf("Layouts:\n");

    for (i = 0; layouts[i].name != NULL; i++) {
        printf("\t%s: %s\n", layouts[i].name, layouts[i].desc);
        printf("\n");
        printf("\t\tinterface: \t0x%04x\n", layouts[i].interface);
        printf("\t\tdir: \t\t0x%04x\n", layouts[i].dir);
        printf("\t\treset release: \t0x%04x\n", layouts[i].reset_release);
        printf("\t\treset hold:    \t0x%04x\n", layouts[i].reset_set);
        printf("\t\tvref2 normal:  \t0x%04x\n", layouts[i].vref2_normal);
        printf("\t\tvref2 erase:   \t0x%04x\n", layouts[i].vref2_erase);
        printf("\n");
    }

    printf("\n");
    printf("Options:\n");
    printf("           -i|--index     specifiy which device to use (default 0)\n");
    printf("           -v|--vendor    set vendor id (default 0x0403)\n");
    printf("           -p|--product   set vendor id (default 0x6010)\n");
}

int print_and_prompt(struct ftdi_device_list *devlist)
{
    int i;
    struct ftdi_context ftdic;
    struct ftdi_device_list *curdev;
    char manufacturer[128], description[128], serial[128];
    char input[BUF_LEN];
    char *s;
    int sel = -1;

    printf("\n");

    i = 0;

    for (curdev = devlist; curdev != NULL; i++) {
        printf("  [%d]   ", i);

        int ret = ftdi_usb_get_strings(&ftdic,
                                       curdev->dev,
                                       manufacturer, 128,
                                       description, 128,
                                       serial, 128);
        if (0 > ret) {
            fprintf(stderr, "ftdi_usb_get_strings failed: %d (%s)\n",
                    ret, ftdi_get_error_string(&ftdic));
            return EXIT_FAILURE;
        }

        printf("Manufacturer: %s, Description: %s, Serial %s\n",
               manufacturer, description, serial);
        curdev = curdev->next;
    }

    printf("\nUse which device? ");

    s = fgets(input, BUF_LEN, stdin);

    if (s != NULL) {
        size_t last = strlen(input) - 1;

        if (input[last] == '\n') {
            /* cppcheck: input is accessed later via *s */
            /* cppcheck-suppress unreadVariable */
            input[last] = '\0';
        }

        sscanf(s, "%11i", &sel);
    }

    return sel;
}

void reset(struct ftdi_context *ftdic, const struct layout *l)
{

    /* using MPSSE since it give access to high GPIO*/
    /* set as inputs for now */
    ftdi_set_bitmode(ftdic, 0 , BITMODE_MPSSE);

    printf("toggle reset\n");

    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_normal));
    bb_mpsee(ftdic, l->dir, (l->reset_set     | l->vref2_normal));
    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_normal));

    return;

}


void erase(struct ftdi_context *ftdic, const struct layout *l)
{
    printf("setting VREF2 erase\n");

    /* using MPSSE since it give access to high GPIO*/
    /* set as inputs for now */
    ftdi_set_bitmode(ftdic, 0 , BITMODE_MPSSE);

    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_normal));
    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_erase));

    printf("toggle reset\n");

    bb_mpsee(ftdic, l->dir, (l->reset_set     | l->vref2_erase));
    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_erase));

    printf("waiting for erase\n");

    sleep(2);

    printf("setting VREF2 normal\n");

    bb_mpsee(ftdic, l->dir, (l->reset_release | l->vref2_normal));

    reset(ftdic, l);

    return;

}


int bb_mpsee(struct ftdi_context *ftdic, uint16_t dir, uint16_t val)
{
    uint8_t buf[3];

    /* command "set data bits low byte" */
    buf[0] = 0x80;
    buf[1] = (val & 0xff);
    buf[2] = dir & 0xff;
#if DEBUG
    fprintf(stderr, "write %x %x %x\n", buf[0], buf[1], buf[2]);
#endif

    if ((ret = (ftdi_write_data(ftdic, buf, 3))) < 0) {
        perror("ft2232_write error");
        fprintf(stderr, "ft2232_write command %x\n", buf[0]);
        return EXIT_FAILURE;
    }


    /* command "set data bits high byte" */
    buf[0] = 0x82;
    buf[1] = (val >> 8);
    buf[2] = dir >> 8;
#if DEBUG
    fprintf(stderr, "write %x %x %x\n", buf[0], buf[1], buf[2]);
#endif

    if ((ftdi_write_data(ftdic, buf, 3)) < 0) {
        perror("ft2232_write error");
        fprintf(stderr, "ft2232_write command %x\n", buf[0]);
        return EXIT_FAILURE;
    }

    return 0;

}
