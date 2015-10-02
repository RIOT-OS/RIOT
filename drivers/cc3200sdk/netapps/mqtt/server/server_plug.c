/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

#include "server_plug.h"
#include "server_core.h"
#include "server_util.h"

#define MAX_PLUGINS            PG_MAP_MAX_ELEMS
#define PG_NAME_LEN            32

static struct plugin_desc {

        c8                          *name;
        u8                          index;
        u8                          inuse;

        struct mqtt_server_app_cbs  app_cbs;

} plugins[MAX_PLUGINS];

#define PLUGIN(app_hnd) ((struct plugin_desc*) app_hnd)

static inline bool is_inuse(struct plugin_desc *plugin)
{
        return plugin->inuse? true : false;
}

static inline void inuse_set(struct plugin_desc *plugin, bool inuse)
{
        plugin->inuse = inuse? 0x01 : 0x00;
}

static struct plugin_desc *acl_pg = NULL;

static struct plugin_core_msg_cbs *msg_cbs, msg_cbacks = {NULL, NULL, NULL};

static inline struct plugin_desc *plugin_find(i32 idx)
{
        return plugins + idx;
}

static void plugin_reset(struct plugin_desc *plugin)
{
        //plugin->app_cbs = NULL; ==> TBD
        inuse_set(plugin, false);

        return;
}

static struct plugin_desc *plugin_alloc(void)
{
        struct plugin_desc *plugin = NULL;;
        i32 idx = 0;

        for(idx = 0; idx < MAX_PLUGINS; idx++) {
                plugin = plugins + idx;
                if(false == is_inuse(plugin)) {
                        inuse_set(plugin, true);
                        break;
                }
        }

        DBG_INFO("Plugin alloc %s\n\r",
                 (MAX_PLUGINS == idx)? "Failed" : "Success");

        return (MAX_PLUGINS != idx)? plugin : NULL;
}

#if  0
static void plugin_free(struct plugin_desc *plugin)
{
        plugin_reset(plugin);
}
#endif

u16 plugin_connect(const struct utf8_string *clientId,
                   const struct utf8_string *username,
                   const struct utf8_string *password,
                   void **app_usr)
{
        u16 rv = CONNACK_RC_REQ_ACCEPT; /* Accept everything from MQTT network */

        *app_usr = NULL;
        if(acl_pg)
                rv = acl_pg->app_cbs.connect(clientId, username,
                                             password, app_usr);

        return rv;
}

i32 plugin_publish(u8 pg_map, const struct utf8_string *topic,
                   const u8 *payload, u32 pay_len,
                   bool dup, u8 qos, bool retain)
{
        i32 i = 0;
        for(i = 0; i < MAX_PLUGINS; i++) {
                if(PG_MAP_HAS_VALUE(pg_map, i)) {
                        struct plugin_desc *plugin = plugin_find(i);

                        DBG_INFO("Publishing to Plugin ID: %d (%s)\n\r",
                                 plugin->index, plugin->name);
                        
                        if(false == is_inuse(plugin)) 
                                continue; /* Must not happen */
                        
                        plugin->app_cbs.publish(topic, payload, pay_len,
                                                dup, qos, retain);
                }
        }

        /* TBD for error value return. */

        return pay_len;
}

i32 plugin_disconn(const void *app_usr, bool due2err)
{
        if(acl_pg)
                acl_pg->app_cbs.disconn(app_usr, due2err);

        /* TBD for error value return. */

        return 0;
}

i32 mqtt_server_topic_enroll(const void *app_hnd, 
                             const struct utf8_string *topic, enum mqtt_qos qos)
{
        return  app_hnd? 
                msg_cbs->topic_enroll(PLUGIN(app_hnd)->index, topic, qos) : -1;
}

i32 mqtt_server_topic_disenroll(const void *app_hnd, 
                                const struct utf8_string *topic)
{
        return  app_hnd?
                msg_cbs->topic_cancel(PLUGIN(app_hnd)->index, topic) : -1;
}

i32 mqtt_server_app_pub_send(const  struct utf8_string *topic, 
                             const u8 *data_buf, u32 data_len,
                             enum mqtt_qos qos, bool retain)
{
        return msg_cbs->publish(topic, data_buf, data_len, qos, retain);
}

static void *server_app_register(const struct mqtt_server_app_cbs *cbs,
                                 const c8 *name)
{
        struct plugin_desc *plugin = plugin_alloc();
        if(NULL != plugin) {
                strncpy(plugin->name, name, PG_NAME_LEN - 1);
                memcpy(&plugin->app_cbs, cbs,
                       sizeof(struct mqtt_server_app_cbs));

                if((NULL == acl_pg) && cbs->connect)
                        acl_pg = plugin;

        }
        return plugin;
}

void *mqtt_server_app_register(const struct mqtt_server_app_cbs *cbs,
                               const c8 *name)
{
        if((NULL == cbs)                         || 
           ((!!cbs->connect) ^ (!!cbs->disconn)) ||
           (acl_pg && cbs->connect))
                return NULL;

        return server_app_register(cbs, name);
}

static bool inited = false;

i32 plugin_init(const struct plugin_core_msg_cbs *cbs)
{
        i32 idx = 0;

        if(inited)
                return -1;

        if(NULL == cbs)
                return -2;

        if(!(cbs->topic_enroll && cbs->topic_cancel && cbs->publish))
                return -3;

        for(idx = 0; idx < MAX_PLUGINS; idx++) {
                struct plugin_desc *plugin = plugins + idx;
                plugin->index = idx;

                plugin_reset(plugin);
        }

        msg_cbs = &msg_cbacks;
        memcpy(msg_cbs, cbs, sizeof(struct plugin_core_msg_cbs));

        inited = true;

        USR_INFO("Plugin module has been initialized.\n\r");
        return 0;
}

