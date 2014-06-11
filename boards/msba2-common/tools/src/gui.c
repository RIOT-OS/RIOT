/*
 * LPC 2000 Loader, http://www.pjrc.com/arm/lpc2k_pgm
 * Copyright (c) 2004, PJRC.COM, LLC, <paul@pjrc.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/* If this code fails to build, please provide at least the following
 * information when requesting (free) technical support.
 *
 * 1: Complete copy of all messages during the build.
 * 2: Output of "gtk-config --version"
 * 3: Output of "gtk-config --libs"
 * 4: Output of "gtk-config --cflags"
 * 5: Output of "uname -a"
 * 6: Version of GTK installed... eg, type: ls -l /lib/libgtk*
 * 7: Other info... which linux distribution, version, other software
 */


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gui.h"
#include "settings.h"
#include "serial.h"
#include "lpc2k_pgm.h"
#include "download.h"


static GtkWidget *firmware_label, *firmware_entry, *program_button;
static GtkWidget *port_label, *port_entry, *baud_label, *baud_combo;
static GtkWidget *crystal_label, *crystal_entry, *mhz_label;
static GtkWidget *reboot_button, *bootloader_button, *quit_button;
static GtkWidget *line1_hbox, *line2_hbox, *line3_hbox, *line4_hbox;
static GtkWidget *main_vbox, *main_window;

static int port_timeout = 0;
static int baud_timeout = 0;
static int download_in_progress = 0;

gint do_quit(GtkWidget *widget, gpointer *data)
{
    gtk_main_quit();
    return FALSE;
}

gint do_program(GtkWidget *widget, gpointer *data)
{
    if (download_in_progress) {
        // error... not supposed to get here
        gtk_widget_set_sensitive(program_button, FALSE);
        return FALSE;
    }

    download_in_progress = 1;
    gtk_widget_set_sensitive(program_button, FALSE);
    gtk_widget_set_sensitive(reboot_button, FALSE);
    gtk_widget_set_sensitive(bootloader_button, TRUE);
    download_begin();
    return FALSE;
}

int file_exists(const char *filename)
{
    struct stat file_stats;
    int r;

    r = stat(filename, &file_stats);

    if (r != 0) {
        return 0;
    }

    if (!S_ISREG(file_stats.st_mode)) {
        return 0;
    }

    return 1;
}

void done_program(int still_in_bootloader)
{
    download_in_progress = 0;

    if (file_exists(gtk_entry_get_text(GTK_ENTRY(firmware_entry)))) {
        gtk_widget_set_sensitive(program_button, TRUE);
    }
    else {
        gtk_widget_set_sensitive(program_button, FALSE);
    }

    gtk_widget_set_sensitive(bootloader_button, TRUE);
    gtk_widget_set_sensitive(reboot_button, TRUE);
}

gint do_reboot(GtkWidget *widget, gpointer *data)
{
    if (download_in_progress) {
        download_cancel(NULL);
        gtk_widget_set_sensitive(program_button, FALSE);
        gtk_widget_set_sensitive(reboot_button, FALSE);
        gtk_widget_set_sensitive(bootloader_button, FALSE);
    }

    gtk_widget_set_sensitive(program_button, FALSE);
    gtk_widget_set_sensitive(reboot_button, FALSE);
    gtk_widget_set_sensitive(bootloader_button, FALSE);

    hard_reset_to_user_code();

#if 0
    download_in_progress = 1;
    soft_reboot_begin();
#endif

    if (file_exists(gtk_entry_get_text(GTK_ENTRY(firmware_entry)))) {
        gtk_widget_set_sensitive(program_button, TRUE);
    }
    else {
        gtk_widget_set_sensitive(program_button, FALSE);
    }

    gtk_widget_set_sensitive(bootloader_button, TRUE);
    return FALSE;
}

gint do_bootloader(GtkWidget *widget, gpointer *data)
{
    if (download_in_progress) {
        download_cancel(NULL);
        gtk_widget_set_sensitive(program_button, FALSE);
        gtk_widget_set_sensitive(reboot_button, FALSE);
        gtk_widget_set_sensitive(bootloader_button, FALSE);
    }

    hard_reset_to_bootloader();

    if (file_exists(gtk_entry_get_text(GTK_ENTRY(firmware_entry)))) {
        gtk_widget_set_sensitive(program_button, TRUE);
    }
    else {
        gtk_widget_set_sensitive(program_button, FALSE);
    }

    gtk_widget_set_sensitive(reboot_button, TRUE);
    gtk_widget_set_sensitive(bootloader_button, TRUE);
    return FALSE;
}

gint do_new_port(GtkWidget *widget, gpointer *data)
{
    port_timeout = 12;
    return FALSE;
}

gint do_new_baud(GtkWidget *widget, gpointer *data)
{
    baud_timeout = 7;
    return FALSE;
}

gint do_new_file(GtkWidget *widget, gpointer *data)
{
    const char *filename;

    filename = gtk_entry_get_text(GTK_ENTRY(firmware_entry));

    if (file_exists(filename)) {
        new_file_setting(filename);

        if (download_in_progress) {
            gtk_widget_set_sensitive(program_button, FALSE);
        }
        else {
            gtk_widget_set_sensitive(program_button, TRUE);
        }
    }
    else {
        gtk_widget_set_sensitive(program_button, FALSE);
    }

    return FALSE;
}

gint do_new_crystal(GtkWidget *widget, gpointer *data)
{
    const char *xtal;

    xtal = gtk_entry_get_text(GTK_ENTRY(crystal_entry));
    new_crystal_setting(xtal);
    return FALSE;
}


gint do_timer(gpointer data)
{
    if (port_timeout && --port_timeout == 0) {
        open_serial_port(gtk_entry_get_text(GTK_ENTRY(port_entry)));
    }

    if (baud_timeout && --baud_timeout == 0) {
        change_baud(gtk_entry_get_text(GTK_ENTRY(
                                           GTK_COMBO(baud_combo)->entry)));
    }

    if (download_in_progress) {
        download_timer();
    }

    return TRUE;
}

void do_term_input(gpointer data, int fd, GdkInputCondition cond)
{
    char buf[256];
    int num, flags;

    flags = fcntl(term_fd, F_GETFL);
    fcntl(term_fd, F_SETFL, flags | O_NONBLOCK);
    num = read(term_fd, buf, sizeof(buf));
    fcntl(term_fd, F_SETFL, flags);

    if (num > 0) {
        if (download_in_progress) {
            download_rx_term(buf, num);
        }
        else {
            write_serial_port(buf, num);
        }
    }
}

void do_port_input(gpointer data, int fd, GdkInputCondition cond)
{
    char buf[256];
    int num;

    num = read_serial_port_nb((unsigned char *)buf, sizeof(buf));

    if (num > 0) {
        if (download_in_progress) {
            download_rx_port(buf, num);
        }
        else {
            write(term_fd, buf, num);
        }
    }
}


void run_gui(void)
{
    gtk_signal_connect(GTK_OBJECT(main_window), "delete_event",
                       GTK_SIGNAL_FUNC(do_quit), NULL);
    gtk_signal_connect(GTK_OBJECT(quit_button), "pressed",
                       GTK_SIGNAL_FUNC(do_quit), NULL);
    gtk_signal_connect(GTK_OBJECT(port_entry), "changed",
                       GTK_SIGNAL_FUNC(do_new_port), NULL);
    gtk_signal_connect(GTK_OBJECT(GTK_COMBO(baud_combo)->entry), "changed",
                       GTK_SIGNAL_FUNC(do_new_baud), NULL);
    gtk_signal_connect(GTK_OBJECT(firmware_entry), "changed",
                       GTK_SIGNAL_FUNC(do_new_file), NULL);
    gtk_signal_connect(GTK_OBJECT(crystal_entry), "changed",
                       GTK_SIGNAL_FUNC(do_new_crystal), NULL);
    gtk_signal_connect(GTK_OBJECT(program_button), "pressed",
                       GTK_SIGNAL_FUNC(do_program), NULL);
    gtk_signal_connect(GTK_OBJECT(reboot_button), "pressed",
                       GTK_SIGNAL_FUNC(do_reboot), NULL);
    gtk_signal_connect(GTK_OBJECT(bootloader_button), "pressed",
                       GTK_SIGNAL_FUNC(do_bootloader), NULL);

    gtk_timeout_add(100, do_timer, NULL);
    gdk_input_add(term_fd, GDK_INPUT_READ, do_term_input, NULL);
    gdk_input_add(serial_port_fd(), GDK_INPUT_READ, do_port_input, NULL);

    gtk_main();
}


void create_window(int *argc, char ***argv)
{
    GList *gtk_baud_list = NULL;
    int i;

    gtk_init(argc, argv);

    firmware_label = gtk_label_new("Firmware:");
    gtk_label_set_justify(GTK_LABEL(firmware_label), GTK_JUSTIFY_RIGHT);
    gtk_widget_show(firmware_label);

    firmware_entry = gtk_entry_new();
    gtk_widget_set_usize(firmware_entry, 110, 0);
    gtk_entry_set_text(GTK_ENTRY(firmware_entry), file_setting());
    gtk_widget_show(firmware_entry);

    program_button = gtk_button_new_with_label("Program Now");

    if (file_exists(file_setting())) {
        gtk_widget_set_sensitive(program_button, TRUE);
    }
    else {
        gtk_widget_set_sensitive(program_button, FALSE);
    }

    gtk_widget_show(program_button);

    line1_hbox = gtk_hbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line1_hbox), firmware_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line1_hbox), firmware_entry, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line1_hbox), program_button, FALSE, FALSE, 2);
    gtk_widget_show(line1_hbox);


    port_label = gtk_label_new("Port:");
    gtk_label_set_justify(GTK_LABEL(port_label), GTK_JUSTIFY_RIGHT);
    gtk_widget_show(port_label);

    port_entry = gtk_entry_new();
    gtk_widget_set_usize(port_entry, 80, 0);
    gtk_entry_set_text(GTK_ENTRY(port_entry), port_setting());
    open_serial_port(port_setting());
    gtk_widget_show(port_entry);

    baud_label = gtk_label_new("Baud:");
    gtk_label_set_justify(GTK_LABEL(baud_label), GTK_JUSTIFY_RIGHT);
    gtk_widget_show(baud_label);

    baud_combo = gtk_combo_new();

    for (i = 0; baud_list[i] != NULL; i++) {
        gtk_baud_list = g_list_append(gtk_baud_list, baud_list[i]);
    }

    gtk_combo_set_popdown_strings(GTK_COMBO(baud_combo), gtk_baud_list);
    gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(baud_combo)->entry), FALSE);
    gtk_widget_set_usize(baud_combo, 75, 0);

    for (i = 0; baud_list[i] != NULL; i++) {
        if (strcmp(baud_list[i], baud_setting()) == 0) {
            gtk_list_select_item(GTK_LIST(GTK_COMBO(baud_combo)->list), i);
            break;
        }
    }

    gtk_widget_show(baud_combo);

    line2_hbox = gtk_hbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line2_hbox), port_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line2_hbox), port_entry, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line2_hbox), baud_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line2_hbox), baud_combo, FALSE, FALSE, 2);
    gtk_widget_show(line2_hbox);


    crystal_label = gtk_label_new("Crystal:");
    gtk_label_set_justify(GTK_LABEL(crystal_label), GTK_JUSTIFY_RIGHT);
    gtk_widget_show(crystal_label);

    crystal_entry = gtk_entry_new();
    gtk_widget_set_usize(crystal_entry, 80, 0);
    gtk_entry_set_text(GTK_ENTRY(crystal_entry), crystal_setting());
    gtk_widget_show(crystal_entry);

    mhz_label = gtk_label_new("(MHz)");
    gtk_label_set_justify(GTK_LABEL(mhz_label), GTK_JUSTIFY_LEFT);
    gtk_widget_show(mhz_label);

    line3_hbox = gtk_hbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line3_hbox), crystal_label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(line3_hbox), crystal_entry, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line3_hbox), mhz_label, FALSE, FALSE, 2);
    gtk_widget_show(line3_hbox);


    reboot_button = gtk_button_new_with_label("Reboot");
    gtk_widget_set_sensitive(reboot_button, TRUE);
    gtk_widget_show(reboot_button);

    bootloader_button = gtk_button_new_with_label("Booloader");
    gtk_widget_show(bootloader_button);

    quit_button = gtk_button_new_with_label("Quit");
    gtk_widget_show(quit_button);

    line4_hbox = gtk_hbox_new(TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line4_hbox), reboot_button, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line4_hbox), bootloader_button, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(line4_hbox), quit_button, TRUE, TRUE, 2);
    gtk_widget_show(line4_hbox);

    main_vbox = gtk_vbox_new(FALSE, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), line1_hbox, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), line2_hbox, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), line3_hbox, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(main_vbox), line4_hbox, TRUE, TRUE, 2);
    gtk_widget_show(main_vbox);

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
    gtk_widget_show(main_window);
}
