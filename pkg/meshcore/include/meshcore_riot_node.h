/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       MeshCore node: contacts, text messages and Public channel
 *
 * The node advertises itself, keeps the contacts from the advertisements it
 * receives, sends and acknowledges text messages, and sends and receives the
 * messages of the Public channel. It can repeat the packets of the other nodes.
 *
 * The mesh is processed in a thread of its own, created by
 * @ref meshcore_riot::Node::start. The other threads must lock the node to use
 * it.
 *
 * On the boards with an EEPROM (periph_eeprom), the identity, the name and the
 * repeater setting are stored, so the node keeps its identity across the
 * reboots. On the other boards, a new identity is generated at each startup.
 *
 * The events are printed on stdio by default: override the hooks to handle
 * them differently.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"

#ifdef __cplusplus
}
#endif

#include "meshcore_riot.h"
#include "helpers/AdvertDataHelpers.h"

/**
 * @brief   Maximum number of contacts
 */
#ifndef CONFIG_MESHCORE_NODE_MAX_CONTACTS
#  define CONFIG_MESHCORE_NODE_MAX_CONTACTS (16U)
#endif

/**
 * @brief   Number of packets in the pool of the node
 */
#ifndef CONFIG_MESHCORE_NODE_PACKET_POOL_SIZE
#  define CONFIG_MESHCORE_NODE_PACKET_POOL_SIZE (8U)
#endif

/**
 * @brief   Position of the settings in the EEPROM, if any
 */
#ifndef CONFIG_MESHCORE_NODE_EEPROM_POS
#  define CONFIG_MESHCORE_NODE_EEPROM_POS   (0U)
#endif

/**
 * @brief   Stack size of the thread of the node
 *
 * The signatures and the key exchanges need a large stack.
 */
#ifndef CONFIG_MESHCORE_NODE_STACKSIZE
#  define CONFIG_MESHCORE_NODE_STACKSIZE    (THREAD_STACKSIZE_DEFAULT * 4)
#endif

/**
 * @brief   Priority of the thread of the node
 */
#ifndef CONFIG_MESHCORE_NODE_PRIO
#  define CONFIG_MESHCORE_NODE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#endif

namespace meshcore_riot {

/**
 * @brief   Maximum length of a name, terminating zero included
 */
constexpr size_t node_name_len = MAX_ADVERT_DATA_SIZE;

/**
 * @brief   Maximum length of a text message, as MeshCore
 */
constexpr size_t node_max_text_len = 10 * CIPHER_BLOCK_SIZE;

/**
 * @brief   Contact, known from its advertisement
 */
struct Contact {
    mesh::Identity id;              /**< identity */
    char name[node_name_len];       /**< name */
    uint8_t type;                   /**< type, ADV_TYPE_* */
    uint8_t hops;                   /**< hops of the last advertisement */
};

/**
 * @brief   MeshCore node
 */
class Node : public mesh::Mesh {
public:
    /**
     * @brief   Creates the node and its thread, initializes the radio, then
     *          loads or generates the identity and sends an advertisement
     *
     * Can be called only once.
     *
     * @param[in] dev       LoRa netdev device, set up but not initialized
     * @param[in] params    LoRa parameters
     *
     * @return  the node
     * @return  NULL if the radio can't be initialized
     */
    static Node *start(netdev_t *dev, const meshcore_radio_params_t &params = default_radio_params);

    /**
     * @brief   Constructor, use @ref start to create the node
     */
    Node(Radio &radio, mesh::MillisecondClock &ms, mesh::RNG &rng,
         mesh::RTCClock &rtc, mesh::PacketManager &mgr, mesh::MeshTables &tables);

    /**
     * @brief   Locks the node, to use it from another thread
     */
    void lock()
    {
        _runner.lock();
    }

    /**
     * @brief   Unlocks the node
     */
    void unlock()
    {
        _runner.unlock();
    }

    /**
     * @brief   Returns the radio of the node
     */
    const Radio &getRadio() const
    {
        return _radio;
    }

    /**
     * @brief   Returns the name sent in the advertisements
     */
    const char *getName() const
    {
        return _name;
    }

    /**
     * @brief   Sets the name sent in the advertisements, and stores it
     *
     * @param[in] name  the name, truncated to node_name_len - 1 characters
     *
     * @return  true on success
     * @return  false if the name has characters not allowed by MeshCore
     */
    bool setName(const char *name);

    /**
     * @brief   Tells whether the node repeats the packets of the other nodes
     */
    bool isRepeater() const
    {
        return _repeat;
    }

    /**
     * @brief   Sets whether the node repeats the packets of the other nodes,
     *          and stores it
     *
     * The node is advertised as a repeater when it repeats.
     */
    void setRepeater(bool repeat);

    /**
     * @brief   Sets whether the packets received and sent are logged
     */
    void setLogPackets(bool log)
    {
        _log_packets = log;
    }

    /**
     * @brief   Floods an advertisement of the node
     */
    void sendAdvert();

    /**
     * @brief   Sends a text message to a contact
     *
     * The message is flooded, it isn't sent again when it isn't acknowledged.
     *
     * @param[in] dest  name of the contact
     * @param[in] text  text, of node_max_text_len characters at most
     *
     * @return  true if the message is sent
     * @return  false if the contact is unknown, the text too long or no packet
     *          is free
     */
    bool sendText(const char *dest, const char *text);

    /**
     * @brief   Sends a message to the Public channel
     *
     * The text starts with the name of the node, as MeshCore.
     *
     * @param[in] text  text
     *
     * @return  true if the message is sent
     * @return  false if the message is too long or no packet is free
     */
    bool sendPublic(const char *text);

    /**
     * @brief   Returns the number of contacts
     */
    unsigned getNumContacts() const
    {
        return _num_contacts;
    }

    /**
     * @brief   Returns a contact
     *
     * @param[in] idx   index of the contact, lower than getNumContacts()
     */
    const Contact &getContact(unsigned idx) const
    {
        return _contacts[idx];
    }

protected:
    /**
     * @brief   Called when a new contact is added
     */
    virtual void onNewContact(const Contact &contact);

    /**
     * @brief   Called when a text message is received from a contact
     */
    virtual void onTextRecv(const Contact &from, const char *text, size_t len);

    /**
     * @brief   Called when a message of the Public channel is received, the
     *          text starts with the name of the sender
     */
    virtual void onPublicRecv(const char *text, size_t len);

    /**
     * @brief   Called when the last text message sent is acknowledged
     */
    virtual void onTextAcked(const Contact &to);

    /** @brief Tells whether the packets are repeated: the repeater setting */
    bool allowPacketForward(const mesh::Packet *packet) override;
    /** @brief Adds or updates the contact of a received advertisement */
    void onAdvertRecv(mesh::Packet *packet, const mesh::Identity &id, uint32_t timestamp,
                      const uint8_t *app_data, size_t app_data_len) override;
    /** @brief Finds the contacts matching a hash, returns their number */
    int searchPeersByHash(const uint8_t *hash) override;
    /** @brief Computes the shared secret with a contact found by searchPeersByHash() */
    void getPeerSharedSecret(uint8_t *dest_secret, int peer_idx) override;
    /** @brief Handles a text message from a contact, and acknowledges it */
    void onPeerDataRecv(mesh::Packet *packet, uint8_t type, int sender_idx,
                        const uint8_t *secret, uint8_t *data, size_t len) override;
    /** @brief Handles the acknowledgment carried by a path returned by a contact */
    bool onPeerPathRecv(mesh::Packet *packet, int sender_idx, const uint8_t *secret,
                        uint8_t *path, uint8_t path_len, uint8_t extra_type,
                        uint8_t *extra, uint8_t extra_len) override;
    /** @brief Handles an acknowledgment of a text message */
    void onAckRecv(mesh::Packet *packet, uint32_t ack_crc) override;
    /** @brief Finds the channels matching a hash: the Public channel */
    int searchChannelsByHash(const uint8_t *hash, mesh::GroupChannel channels[],
                             int max_matches) override;
    /** @brief Handles a text message of the Public channel */
    void onGroupDataRecv(mesh::Packet *packet, uint8_t type, const mesh::GroupChannel &channel,
                         uint8_t *data, size_t len) override;
    /** @brief Logs a received packet, when the log is enabled */
    void logRx(mesh::Packet *packet, int len, float score) override;
    /** @brief Logs a sent packet, when the log is enabled */
    void logTx(mesh::Packet *packet, int len) override;

private:
    static void *_thread(void *arg);
    bool _loadSettings();
    void _saveSettings();
    void _printPacket(const char *dir, const mesh::Packet *packet, int len);
    bool _processAck(const uint8_t *ack);
    int _find(const char *name);
    int _find(const mesh::Identity &id);

    Radio &_radio;
    Runner _runner;
    meshcore_radio_params_t _params = default_radio_params;
    int _init_res = 0;
    mutex_t _ready = MUTEX_INIT_LOCKED;
    char _name[node_name_len] = {};
    bool _repeat = false;
    bool _log_packets = false;
    Contact _contacts[CONFIG_MESHCORE_NODE_MAX_CONTACTS] = {};
    mesh::GroupChannel _public = {};
    unsigned _num_contacts = 0;
    unsigned _matches[CONFIG_MESHCORE_NODE_MAX_CONTACTS] = {};
    unsigned _num_matches = 0;
    uint32_t _pending_ack = 0;
    int _pending_idx = -1;
};

} /* namespace meshcore_riot */

/** @} */
