#!/usr/bin/env python3

# Copyright (C) 2015  Hauke Petersen <dev@haukepetersen.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys
import os
from os import path
import argparse
import yaml
import templates

# Configuration data
conf_file = path.expanduser("~") + "/.riot/prep_conf.yaml"

# Value definition for configuration
conf_items = {
    'name': 'Your full name',
    'email': 'Your email address',
    'copyright': 'Your copyright',
    'riotbase': 'Absolute path to the RIOT base directory'
}

driver_items = {
    'id': 'Device name, used as module name in RIOT',
    'name': 'Human readable device descriptor, e.g. "ISL2920 light sensor"'
}

sys_items = {
    'name': 'Name of the new module',
    'desc': 'Short description of the module'
}

def load_config( ):
    if not path.isfile(conf_file):
        return {}
    else:
        with open(conf_file) as f:
            return yaml.load(f)

def store_config( conf ):
    tmp = yaml.dump(conf, default_flow_style=False)
    with open(conf_file, "w") as f:
        f.write(tmp)


def askfor( var, items ):
    print("Now just fill in the required information. Just press return to")
    print("use the default value that is given in parenthesis\n")
    for item, desc in items.items():
        if item in var.keys():
            val = var[item]
        else:
            val = ''
        print(desc + " (" + val + "):")
        line = sys.stdin.readline().rstrip()
        if line:
            var[item] = line
        else:
            var[item] = val

def make_makefile( location ):
    name = "%s/Makefile" % (location)
    if path.isfile(name):
        print("error: file '%s' does already exist" % (name))
        return
    with open(name, "w") as f:
        f.write("include $(RIOTBASE)/Makefile.base\n")

def make_mainheader( filename, copyright, group, module, desc, name, email ):
    if path.isfile(filename):
        print("error: file '%s' does already exist" % (filename))
        return
    with open(filename, "w") as f:
        f.write(templates.mainheader % (
                copyright, group, module, desc, group, name, email,
                module.upper(), module.upper(), module.upper()))

def make_subheader( filename, copyright, group, module, name, email ):
    if path.isfile(filename):
        print("error: file '%s' does already exist" % (filename))
        return
    with open(filename, "w") as f:
        f.write(templates.subheader % (
                copyright, group, module, name, email,
                module.upper(), module.upper(), module.upper()))

def make_cfile( filename, copyright, group, module, name, email ):
    if path.isfile(filename):
        print("error: file '%s' does already exist" % (filename))
        return
    with open(filename, "w") as f:
        f.write(templates.cfile % (copyright, group, module, name, email, module))

def make_makeinclude( filename, module, path ):
    with open(filename, "a") as f:
        f.write(templates.inc_entry % (module, path))


def cmd_help( ):
    print("commands:")
    for key, value in cmd.items():
        print(key.rjust(10) + " - " + value['desc'])

def cmd_conf( ):
    conf = load_config()
    askfor(conf, conf_items)
    store_config(conf)
    print("Wrote new configuration to '" + conf_file + "'")

def cmd_dri( ):
    conf = load_config()
    if not conf:
        print("error: unable to load configuration")
        return

    info = dict()
    askfor(info, driver_items)
    print("Preparing device driver with following information:")
    print(info)

    ddir = "%s/drivers/%s" % (conf['riotbase'], info['id'])
    copyright = templates.copyright(conf['copyright'])

    # check if driver exists
    if path.isdir(ddir):
        print("error: driver with the name %s already exists" % (info['id']))
        return
    # create folders and files
    os.makedirs("%s/include" % (ddir))
    make_mainheader("%s/drivers/include/%s.h" % (conf['riotbase'], info['id']),
                    copyright, 'drivers', info['id'], info['name'],
                    conf['name'], conf['email'])
    make_cfile("%s/%s.c" % (ddir, info['id']),
               copyright, 'drivers', info['id'], conf['name'], conf['email'])
    make_subheader("%s/include/%s_regs.h" % (ddir, info['id']),
                   copyright, 'drivers', info['id'], conf['name'], conf['email'])
    make_makeinclude("%s/drivers/Makefile.include" % (conf['riotbase']), info['id'],
                     "drivers/%s/include" % (info['id']))

def cmd_sys( ):
    conf = load_config()
    if not conf:
        print("error: unable to load configuration")
        return

    info = dict();
    askfor(info, sys_items)
    print("Preparing sys module with the following information:")
    print(info)

    sdir = "%s/sys/%s" % (conf['riotbase'], info['name'])
    cr = templates.copyright(conf['copyright'])

    # check if module already exists
    if path.isdir(sdir):
        print("error: sys module '%s' does already exist" % (info['name']))
        return
    # create the module
    os.makedirs(sdir)
    make_mainheader("%s/sys/include/%s.h" % (conf['riotbase'], info['name']),
                    cr, 'sys', info['name'], info['desc'], conf['name'], conf['email'])
    make_cfile("%s/%s.c" % (sdir, info['name']), cr, 'sys', info['name'],
               conf['name'], conf['email'])


# command definitions
cmd = {
    'help': {
        'func': cmd_help,
        'desc': 'Print the list of commands'
    },
    'conf': {
        'func': cmd_conf,
        'desc': 'Create or change a configuration file'
    },
    'driver': {
        'func': cmd_dri,
        'desc': 'Prepare the files/folders for a new device driver'
    },
    'sys': {
        'func': cmd_sys,
        'desc': 'Prepare a new sys module'
    }
}

if __name__ == "__main__":
    # Define some command line args
    p = argparse.ArgumentParser()
    p.add_argument("cmd", default="help", nargs="?", help="")
    p.add_argument("-c", default="", help="configuration file to use")
    args = p.parse_args()

    # override configuration file location if given
    if args.c:
        conf_file = args.c

    if args.cmd in cmd.keys():
        cmd[args.cmd]['func']()
    else:
        print("Error: cmd '" + args.cmd + "' invalid")

