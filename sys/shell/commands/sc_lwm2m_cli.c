/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for interacting with the LWM2M client instance
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandru Razvan Caciulescu <alex.darredevil@gmail.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "liblwm2m.h"
#include "net/ipv6/addr.h"

#include "net/lwm2m_cli.h"
#include "net/lwm2m_cli/objects.h"
#include "net/lwm2m_cli/object_util.h"
#include "net/lwm2m_cli/inst_util.h"

extern lwm2m_context_t *lwm2m_cli_ctx;


static int _init(int argc, char **argv)
{
    ipv6_addr_t addr;
    uint16_t port;

    if (argc < 5) {
        printf("usage: %s %s <client name> <server IP> <server port>\n",
               argv[0], argv[1]);
        puts("\t client name: Name of the client");
        puts("\t   server IP: IP address of the LWM2M server");
        puts("\t server port: Port the server is listening to");
        return 1;
    }

    /* try to parse server IP and port */
    if (ipv6_addr_from_str(&addr, argv[3]) == NULL) {
        puts("error: unable to parse given address");
        return 1;
    }
    port = (uint16_t)atoi(argv[4]);
    if (port == 0) {
        puts("error: given port is invalid");
        return 1;
    }

    printf("Initializing as client with name: '%s'\n", argv[2]);
    printf("Connecting to server at coap://[%s]:%s\n", argv[3], argv[4]);
    return lwm2m_cli_init(argv[2], &addr, port);
}

static int _bootstrap(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Bootstrapping not supported, yet");
    return 0;
}

static int _list(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    lwm2m_server_t *server = lwm2m_cli_ctx->serverList;

    if (server == NULL) {
        puts("not connected to any server");
        return 0;
    }

    puts("Servers:");
    for (; server != NULL; server = server->next) {
        printf("id: %6d - status: ", server->shortID);
        switch(server->status) {
            case STATE_DEREGISTERED:
                puts("DEREGISTERED");
                break;
            case STATE_REG_PENDING:
                puts("REGISTRATION PENDING");
                break;
            case STATE_REGISTERED:
                puts("REGISTERED");
                printf("\t\tlocation: %s\n", server->location);
                printf("\t\tlifetime: %lus\n", (unsigned long)server->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                puts("REGISTRATION UPDATE PENDING");
                break;
            case STATE_DEREG_PENDING:
                puts("DEREGISTRATION PENDING");
                break;
            case STATE_REG_FAILED:
                puts("REGISTRATION FAILED");
                break;
        }
        puts("");
    }
    return 0;
}

static int _update(int argc, char **argv)
{
    uint16_t server_id;

    if (argc < 3) {
        printf("usage: %s %s <server_id>\n", argv[0], argv[1]);
        return 1;
    }
    /* get the server id */
    server_id = (uint16_t)atoi(argv[2]);
    /* trigger a registration update */
    if (lwm2m_update_registration(lwm2m_cli_ctx, server_id) != 0)
    {
        puts("error: registration update failed");
        return 1;
    }
    return 0;
}

static int _ls(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint16_t i;

    for (i = 0 ; i < lwm2m_cli_ctx->numObject ; i++)
    {
        lwm2m_object_t * objectP;

        objectP = lwm2m_cli_ctx->objectList[i];
        if (objectP->instanceList == NULL)
        {
            fprintf(stdout, "/%d ", objectP->objID);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                fprintf(stdout, "/%d/%d  ", objectP->objID, instanceP->id);
            }
        }
        fprintf(stdout, "\r\n");
    }

    return 0;
}

static int _disp(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (lwm2m_cli_ctx->objectList == NULL) {
        puts("Unable to list objects: none are defined\n");
        return 1;
    }

    for (int i = 0; i < lwm2m_cli_ctx->numObject; i++) {
        lwm2m_object_t *obj = lwm2m_cli_ctx->objectList[i];
        if (NULL != obj) {
            switch (obj->objID) {
                case LWM2M_SECURITY_OBJECT_ID:
                    printf("Object %i: SECURITY_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    display_security_object(obj);
                    break;
                case LWM2M_SERVER_OBJECT_ID:
                    printf("Object %i: SERVER_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    display_server_object(obj);
                    break;
                case LWM2M_ACL_OBJECT_ID:
                    printf("Object %i: ACL_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    break;
                case LWM2M_DEVICE_OBJECT_ID:
                    printf("Object %i: DEVICE_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    display_device_object(obj);
                    break;
                case LWM2M_CONN_MONITOR_OBJECT_ID:
                    printf("Object %i: CONN_MONITOR_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    break;
                case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                    printf("Object %i: FIRMWARE_UPDATE_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    break;
                case LWM2M_LOCATION_OBJECT_ID:
                    printf("Object %i: LOCATION_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    break;
                case LWM2M_CONN_STATS_OBJECT_ID:
                    printf("Object %i: CONN_STATS_OBJ [%" PRIu16 "]\n",
                           i, obj->objID);
                    break;
            }
        }
    }
    return 0;
}

static int _dispb(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* TODO: add support for backup objects and means to list them */
    puts("display backup data is not supported, yet");

    return 0;
}

static int _dump(int argc, char **argv)
{
    lwm2m_uri_t uri;
    lwm2m_object_t *obj;

    if (argc < 3) {
        printf("usage: %s %s <uri>\n", argv[0], argv[1]);
        puts("         uri: URI of the object, e.g. /3/0");
        return 1;
    }

    /* parse and check URI argument */
    if (lwm2m_stringToUri(argv[2], strlen(argv[2]), &uri) == 0) {
        puts("error: unable to parse URI");
        return 1;
    }
    if (uri.flag & LWM2M_URI_FLAG_RESOURCE_ID) {
        puts("error: LWM2M_URI_FLAG_RESSOURCE_ID is set for given URI");
        return 1;
    }
    /* get the object from the object store */
    obj = lwm2m_cli_obj_find(lwm2m_cli_ctx, uri.objectId);
    if (obj == NULL) {
        puts("error: object not found");
        return 1;
    }
    /* if just a single instance was select, dump that one */
    if (uri.flag & LWM2M_URI_FLAG_INSTANCE_ID) {
        lwm2m_cli_inst_dump(obj, uri.instanceId);
    }
    /* else dump all instances of the selected object */
    else {
        lwm2m_list_t *inst;

        for (inst = obj->instanceList; inst != NULL; inst = inst->next) {
            lwm2m_cli_inst_dump(obj, inst->id);
        }
    }
    return 0;
}

static int _change(int argc, char **argv)
{
    lwm2m_uri_t uri;

    if(argc < 4) {
        printf("usage: %s %s <uri> <data>\n", argv[0], argv[1]);
        puts("\t\t uri: URI of the resource to change (e.g. /3/0[/2]");
        puts("\t\tdata: new value");
        return 1;
    }

    /* parse URI */
    if (lwm2m_stringToUri(argv[2], strlen(argv[2]), &uri) == 0) {
        puts("error: unable to parse URI");
        return 1;
    }

    /* write the new value to the selected resource */
    if (lwm2m_cli_res_change(&uri, (uint8_t *)argv[3], strlen(argv[3])) < 0) {
        printf("error: unable to change value of %s\n", argv[2]);
        return 1;
    }
    return 0;
}

int _lwm2m_cli(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s <cmd> [params]\n", argv[0]);
        puts("\tinit:\t         initialize as client and connect to server");
        puts("\tbootstrap:\t    initiate a ");
        puts("\tlist:\t         list known servers");
        puts("\tupdate:\t       trigger a registration update");
        puts("\tls:\t           list objects and instances");
        puts("\tdisp:\t         display objects");
        puts("\tdispb:\t        display backup of objects");
        puts("\tdump:\t         dump a selected object");
        puts("\tchange:\t       change the value of a resource");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0) {
        _init(argc, argv);
    }
    else if (strcmp(argv[1], "bootstrap") == 0) {
        _bootstrap(argc, argv);
    }
    else if (strcmp(argv[1], "list") == 0) {
        _list(argc, argv);
    }
    else if (strcmp(argv[1], "update") == 0) {
        _update(argc, argv);
    }
    else if (strcmp(argv[1], "ls") == 0) {
        _ls(argc, argv);
    }
    else if (strcmp(argv[1], "disp") == 0) {
        _disp(argc, argv);
    }
    else if (strcmp(argv[1], "dispb") == 0) {
        _dispb(argc, argv);
    }
    else if (strcmp(argv[1], "dump") == 0) {
        _dump(argc, argv);
    }
    else if (strcmp(argv[1], "change") == 0) {
        _change(argc, argv);
    }
    else {
        puts("error: command not recognized");
    }
    return 0;
}
