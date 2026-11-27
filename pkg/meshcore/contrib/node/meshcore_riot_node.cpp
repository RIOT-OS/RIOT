/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       MeshCore node: contacts, text messages and Public channel
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"

#ifdef MODULE_PERIPH_EEPROM
#  include "periph/eeprom.h"
#endif

#include "meshcore_riot_node.h"
#include "helpers/SimpleMeshTables.h"
#include "helpers/StaticPoolPacketManager.h"
#include "helpers/TxtDataHelpers.h"

/* Delay of the acknowledgment of a message, as MeshCore */
#define ACK_DELAY_MS        (200U)

namespace meshcore_riot {

/* Key of the Public channel of MeshCore, izOH6cXN6mrJ5e26oRXNcg== in base64 */
static const uint8_t _public_key[] = {
    0x8b, 0x33, 0x87, 0xe9, 0xc5, 0xcd, 0xea, 0x6a,
    0xc9, 0xe5, 0xed, 0xba, 0xa1, 0x15, 0xcd, 0x72,
};

/* Names of the payload types of MeshCore, NULL for the reserved types */
static const char *const _type_names[] = {
    "REQ", "RESPONSE", "TXT_MSG", "ACK", "ADVERT", "GRP_TXT", "GRP_DATA", "ANON_REQ",
    "PATH", "TRACE", "MULTIPART", "CONTROL", NULL, NULL, NULL, "RAW_CUSTOM",
};

#ifdef MODULE_PERIPH_EEPROM
/* The settings are stored in the EEPROM, when the board has one, so the node
 * keeps its identity across the reboots */
typedef struct {
    uint8_t magic[4];
    uint8_t identity[PRV_KEY_SIZE + PUB_KEY_SIZE];
    char name[node_name_len];
    uint8_t repeat;
} meshcore_stored_settings_t;

/* Marks a stored record: "MeshCore Settings", followed by the version of the layout */
static const uint8_t _settings_magic[] = { 'M', 'C', 'S', '1' };
#endif

static char _node_stack[CONFIG_MESHCORE_NODE_STACKSIZE];

static void _copy_name(char *dest, const char *src)
{
    strncpy(dest, src, node_name_len - 1);
    dest[node_name_len - 1] = '\0';
}

/* Length of a decrypted text, which is padded with zeroes */
static size_t _text_len(const uint8_t *text, size_t max_len)
{
    const uint8_t *end = (const uint8_t *)memchr(text, '\0', max_len);

    return end ? (size_t)(end - text) : max_len;
}

Node *Node::start(netdev_t *dev, const meshcore_radio_params_t &params)
{
    /* Created at runtime: the mesh reads the clock when it is constructed, and
     * no global object is kept in the applications which don't use the node */
    Radio *radio = new Radio(dev);
    Node *node = new Node(*radio, *new MillisClock(), *new RNG(), *new RTCClock(),
                          *new StaticPoolPacketManager(CONFIG_MESHCORE_NODE_PACKET_POOL_SIZE),
                          *new SimpleMeshTables());

    node->_params = params;

    thread_create(_node_stack, sizeof(_node_stack), CONFIG_MESHCORE_NODE_PRIO, 0, _thread,
                  node, "meshcore");
    mutex_lock(&node->_ready);

    if (node->_init_res < 0) {
        printf("Can't start the radio: %d\n", node->_init_res);
        return NULL;
    }
    return node;
}

Node::Node(Radio &radio, mesh::MillisecondClock &ms, mesh::RNG &rng,
           mesh::RTCClock &rtc, mesh::PacketManager &mgr, mesh::MeshTables &tables)
    : mesh::Mesh(radio, ms, rng, rtc, mgr, tables), _radio(radio), _runner(*this, radio)
{
    /* As MeshCore, the secret of a channel is its key padded with zeroes, and
     * the channel is identified by the hash of the key */
    memcpy(_public.secret, _public_key, sizeof(_public_key));
    mesh::Utils::sha256(_public.hash, sizeof(_public.hash), _public_key, sizeof(_public_key));
}

void *Node::_thread(void *arg)
{
    Node *node = static_cast<Node *>(arg);

    node->_init_res = node->_runner.init(node->_params);
    if (node->_init_res < 0) {
        mutex_unlock(&node->_ready);
        return NULL;
    }

    node->lock();
    if (node->_loadSettings()) {
        puts("Settings loaded from the EEPROM");
    }
    else {
        /* The identity is generated from the noise of the radio, combined with
         * the hardware random number generator of the board, if any */
        RadioRNG radio_rng(node->_radio);
        node->self_id = mesh::LocalIdentity(&radio_rng);

        /* The default name ends with the beginning of the public key, to tell
         * the nodes apart */
        snprintf(node->_name, sizeof(node->_name), "RIOT-%02x%02x",
                 node->self_id.pub_key[0], node->self_id.pub_key[1]);
        node->_saveSettings();
    }
    node->sendAdvert();
    node->unlock();
    mutex_unlock(&node->_ready);

    node->_runner.run();
    return NULL;
}

bool Node::_loadSettings()
{
#ifdef MODULE_PERIPH_EEPROM
    meshcore_stored_settings_t settings;

    eeprom_read(CONFIG_MESHCORE_NODE_EEPROM_POS, &settings, sizeof(settings));
    settings.name[node_name_len - 1] = '\0';
    if ((memcmp(settings.magic, _settings_magic, sizeof(_settings_magic)) != 0) ||
        !AdvertDataParser::isValidName(settings.name)) {
        return false;
    }

    self_id.readFrom(settings.identity, sizeof(settings.identity));
    _copy_name(_name, settings.name);
    _repeat = (settings.repeat != 0);
    return true;
#else
    return false;
#endif
}

void Node::_saveSettings()
{
#ifdef MODULE_PERIPH_EEPROM
    meshcore_stored_settings_t settings = {};

    memcpy(settings.magic, _settings_magic, sizeof(_settings_magic));
    self_id.writeTo(settings.identity, sizeof(settings.identity));
    _copy_name(settings.name, _name);
    settings.repeat = _repeat;
    eeprom_write(CONFIG_MESHCORE_NODE_EEPROM_POS, &settings, sizeof(settings));
#endif
}

bool Node::setName(const char *name)
{
    if (!AdvertDataParser::isValidName(name)) {
        return false;
    }
    _copy_name(_name, name);
    _saveSettings();
    return true;
}

void Node::setRepeater(bool repeat)
{
    _repeat = repeat;
    _saveSettings();
}

void Node::sendAdvert()
{
    uint8_t app_data[MAX_ADVERT_DATA_SIZE];
    AdvertDataBuilder builder(_repeat ? ADV_TYPE_REPEATER : ADV_TYPE_CHAT, _name);
    uint8_t len = builder.encodeTo(app_data);
    mesh::Packet *pkt = createAdvert(self_id, app_data, len);

    if (pkt) {
        sendFlood(pkt);
    }
}

bool Node::sendText(const char *dest, const char *text)
{
    int idx = _find(dest);
    size_t text_len = strlen(text);

    if ((idx < 0) || (text_len > node_max_text_len)) {
        return false;
    }

    /* timestamp, type and attempt number, text */
    uint8_t data[5 + node_max_text_len];
    uint32_t timestamp = getRTCClock()->getCurrentTimeUnique();
    memcpy(data, &timestamp, sizeof(timestamp));
    data[4] = TXT_TYPE_PLAIN << 2;
    memcpy(&data[5], text, text_len);

    uint8_t secret[PUB_KEY_SIZE];
    self_id.calcSharedSecret(secret, _contacts[idx].id);
    mesh::Packet *pkt = createDatagram(PAYLOAD_TYPE_TXT_MSG, _contacts[idx].id, secret,
                                       data, 5 + text_len);
    if (pkt == NULL) {
        return false;
    }

    /* The recipient acknowledges with a hash of the message and of the public
     * key of the sender */
    mesh::Utils::sha256((uint8_t *)&_pending_ack, sizeof(_pending_ack), data, 5 + text_len,
                        self_id.pub_key, PUB_KEY_SIZE);
    _pending_idx = idx;

    /* The messages are always flooded, the paths to the contacts aren't
     * tracked */
    sendFlood(pkt);
    return true;
}

bool Node::sendPublic(const char *text)
{
    /* timestamp, type and attempt number, text starting with the name of the
     * sender, as MeshCore */
    uint8_t data[5 + node_max_text_len + 1];
    uint32_t timestamp = getRTCClock()->getCurrentTimeUnique();

    memcpy(data, &timestamp, sizeof(timestamp));
    data[4] = TXT_TYPE_PLAIN << 2;
    int text_len = snprintf((char *)&data[5], node_max_text_len + 1, "%s: %s", _name, text);
    if ((text_len < 0) || ((size_t)text_len > node_max_text_len)) {
        return false;
    }

    mesh::Packet *pkt = createGroupDatagram(PAYLOAD_TYPE_GRP_TXT, _public, data, 5 + text_len);
    if (pkt == NULL) {
        return false;
    }

    sendFlood(pkt);
    return true;
}

void Node::onNewContact(const Contact &contact)
{
    printf("New contact: %s\n", contact.name);
}

void Node::onTextRecv(const Contact &from, const char *text, size_t len)
{
    printf("Message from %s: %.*s\n", from.name, (int)len, text);
}

void Node::onPublicRecv(const char *text, size_t len)
{
    printf("Public: %.*s\n", (int)len, text);
}

void Node::onTextAcked(const Contact &to)
{
    printf("Message to %s acknowledged\n", to.name);
}

bool Node::allowPacketForward(const mesh::Packet *packet)
{
    (void)packet;
    return _repeat;
}

void Node::onAdvertRecv(mesh::Packet *packet, const mesh::Identity &id, uint32_t timestamp,
                        const uint8_t *app_data, size_t app_data_len)
{
    (void)timestamp;
    AdvertDataParser parser(app_data, app_data_len);
    if (!parser.isValid() || !parser.hasName()) {
        return;
    }

    int idx = _find(id);
    bool added = false;
    if (idx < 0) {
        if (_num_contacts == CONFIG_MESHCORE_NODE_MAX_CONTACTS) {
            return;
        }
        idx = _num_contacts++;
        _contacts[idx].id = id;
        added = true;
    }
    _copy_name(_contacts[idx].name, parser.getName());
    _contacts[idx].type = parser.getType();
    _contacts[idx].hops = packet->getPathHashCount();
    if (added) {
        onNewContact(_contacts[idx]);
    }
}

int Node::searchPeersByHash(const uint8_t *hash)
{
    _num_matches = 0;
    for (unsigned i = 0; i < _num_contacts; i++) {
        if (_contacts[i].id.isHashMatch(hash)) {
            _matches[_num_matches++] = i;
        }
    }
    return _num_matches;
}

void Node::getPeerSharedSecret(uint8_t *dest_secret, int peer_idx)
{
    self_id.calcSharedSecret(dest_secret, _contacts[_matches[peer_idx]].id);
}

void Node::onPeerDataRecv(mesh::Packet *packet, uint8_t type, int sender_idx,
                          const uint8_t *secret, uint8_t *data, size_t len)
{
    if ((type != PAYLOAD_TYPE_TXT_MSG) || (len <= 5) || ((data[4] >> 2) != TXT_TYPE_PLAIN)) {
        return;
    }

    const Contact &from = _contacts[_matches[sender_idx]];
    size_t text_len = _text_len(&data[5], len - 5);
    onTextRecv(from, (const char *)&data[5], text_len);

    /* The acknowledgment is a hash of the message and of the public key of the
     * sender, followed by the extended attempt number and by a random byte */
    uint8_t ack[6];
    mesh::Utils::sha256(ack, 4, data, 5 + text_len, from.id.pub_key, PUB_KEY_SIZE);
    ack[4] = ((5 + text_len + 1) < len) ? data[5 + text_len + 1] : 0;
    getRNG()->random(&ack[5], 1);

    mesh::Packet *pkt;
    if (packet->isRouteFlood()) {
        /* the path to the sender carries the acknowledgment */
        pkt = createPathReturn(from.id, secret, packet->path, packet->path_len,
                               PAYLOAD_TYPE_ACK, ack, sizeof(ack));
    }
    else {
        pkt = createAck(ack, sizeof(ack));
    }
    if (pkt) {
        sendFlood(pkt, ACK_DELAY_MS);
    }
}

bool Node::onPeerPathRecv(mesh::Packet *packet, int sender_idx, const uint8_t *secret,
                          uint8_t *path, uint8_t path_len, uint8_t extra_type,
                          uint8_t *extra, uint8_t extra_len)
{
    (void)packet;
    (void)sender_idx;
    (void)secret;
    (void)path;
    (void)path_len;

    if ((extra_type == PAYLOAD_TYPE_ACK) && (extra_len >= sizeof(_pending_ack))) {
        _processAck(extra);
    }

    /* the path isn't used, so no path is sent in return */
    return false;
}

void Node::onAckRecv(mesh::Packet *packet, uint32_t ack_crc)
{
    if (_processAck((const uint8_t *)&ack_crc)) {
        /* the acknowledgment was for this node */
        packet->markDoNotRetransmit();
    }
}

int Node::searchChannelsByHash(const uint8_t *hash, mesh::GroupChannel channels[],
                               int max_matches)
{
    if ((max_matches < 1) || (hash[0] != _public.hash[0])) {
        return 0;
    }
    channels[0] = _public;
    return 1;
}

void Node::onGroupDataRecv(mesh::Packet *packet, uint8_t type, const mesh::GroupChannel &channel,
                           uint8_t *data, size_t len)
{
    (void)packet;
    (void)channel;

    if ((type != PAYLOAD_TYPE_GRP_TXT) || (len <= 5) || ((data[4] >> 2) != TXT_TYPE_PLAIN)) {
        return;
    }

    onPublicRecv((const char *)&data[5], _text_len(&data[5], len - 5));
}

void Node::logRx(mesh::Packet *packet, int len, float score)
{
    (void)score;

    if (_log_packets) {
        _printPacket("RX", packet, len);
        printf(", SNR %d dB, RSSI %d dBm\n", (int)_radio.getLastSNR(),
               (int)_radio.getLastRSSI());
    }
}

void Node::logTx(mesh::Packet *packet, int len)
{
    if (_log_packets) {
        _printPacket("TX", packet, len);
        puts("");
    }
}

void Node::_printPacket(const char *dir, const mesh::Packet *packet, int len)
{
    const char *type = _type_names[packet->getPayloadType()];

    printf("%s %s, ", dir, type ? type : "RESERVED");
    /* The repeaters add themselves to the path of a flood packet, and remove
     * themselves from the path of a direct packet */
    if (packet->isRouteFlood()) {
        printf("flood, %u hop(s)", (unsigned)packet->getPathHashCount());
    }
    else {
        printf("direct, %u hop(s) left", (unsigned)packet->getPathHashCount());
    }
    printf(", %d bytes", len);
}

bool Node::_processAck(const uint8_t *ack)
{
    if ((_pending_idx < 0) || (memcmp(ack, &_pending_ack, sizeof(_pending_ack)) != 0)) {
        return false;
    }

    onTextAcked(_contacts[_pending_idx]);
    _pending_idx = -1;
    return true;
}

int Node::_find(const char *name)
{
    for (unsigned i = 0; i < _num_contacts; i++) {
        if (strcmp(_contacts[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int Node::_find(const mesh::Identity &id)
{
    for (unsigned i = 0; i < _num_contacts; i++) {
        if (_contacts[i].id.matches(id)) {
            return i;
        }
    }
    return -1;
}

} /* namespace meshcore_riot */
