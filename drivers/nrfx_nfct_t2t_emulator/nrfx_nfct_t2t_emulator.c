/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_nrfx_nfct_t2t_emulator
 * @{
 * @file
 * @brief       NFCT Type 2 Tag Emulator for the NRFX boards
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 *
 * @}
 */

#include "nrfx_nfct_t2t_emulator.h"

#include "nrfx_nfct.h"
#include "container.h"
#include "log.h"
#include "nfct_t2t_emulator.h"
#include "net/nfc/t2t/t2t.h"
#include "event.h"

#define BUFFER_SIZE 256

static nfc_t2t_t *tag;
static event_queue_t event_queue;

static char thread_stack[THREAD_STACKSIZE_MAIN];
static uint16_t thread_pid = 0;

static bool initialized = false;
static bool enabled = false;
static bool selected = false;

static uint8_t data_buffer_rx[BUFFER_SIZE];
static uint8_t data_buffer_tx[BUFFER_SIZE];
static const uint32_t buffer_size = BUFFER_SIZE;

static nrfx_t2t_receive_event_t receive_event;
static event_t disable_event;
static event_t enable_event;
static event_t disable_event;
static event_t enable_event;
static event_t uninitialize_event;
static event_t select_event;
static event_t end_of_transmission_event;
static event_t field_detected_event;
static event_t start_of_rx_event;

/**
 * @brief Enables autocollision resolution in the NFCT peripheral
 *
 * @note Without autocollision resolution, the user would have to implement
 * their own autocollision procedure
 *
 * @param[in] nfctid1       Pointer to the NFCID1 to use
 * @param[in] nfctid1_size  Size of the NFCID1
 */
static void configure_autocolres(uint8_t *nfctid1, uint8_t nfctid1_size)
{
    nrfx_nfct_nfcid1_t nfcid1 = {
        .p_id = nfctid1,
        .id_size = nfctid1_size,
    };

    nrfx_nfct_param_t param = {
        .id = NRFX_NFCT_PARAM_ID_NFCID1,
        .data.nfcid1 = nfcid1
    };
    nrfx_nfct_parameter_set(&param);
    nrfx_nfct_autocolres_enable();
}

/**
 * @brief Helper function that prints bytes in hex format
 *
 * @param[in] bytes Bytes to print
 * @param[in] size  Size of the bytes
 */
static void print_bytes_as_hex(const uint8_t *bytes, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        LOG_DEBUG("%02X ", bytes[i]);
    }
    LOG_DEBUG("\n");
}

/**
 * @brief Sends ack or nack response
 *
 * @param[in] ack sends ack if true, nack if false
 */
static void send_ack_nack(bool ack)
{
    uint8_t buffer[1];
    if (ack) {
        LOG_DEBUG("Sending ACK");
        buffer[0] = T2T_ACK_RESPONSE;
    }
    else {
        LOG_DEBUG("Sending NACK");
        buffer[0] = T2T_NACK_RESPONSE;
    }
    nrfx_nfct_data_desc_t data_description = {
        .data_size = 1,
        .p_data = buffer
    };

    nrfx_nfct_tx(&data_description, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
}

/**
 * @brief Process a write command
 *
 * @param block_no  Block number to write to
 * @param bytes     4 bytes of data to write
 */
static void process_write_command(uint8_t block_no, const uint8_t *bytes)
{
    /* write 4 bytes to the address */
    if (t2t_handle_write(tag, block_no, bytes) != 0) {
        LOG_WARNING("Tag is not writeable\n");
        send_ack_nack(false);
        return;
    }

    LOG_DEBUG("Wrote 4 bytes to block number %d", block_no);
    send_ack_nack(true);
    print_bytes_as_hex(tag->memory, tag->memory_size);
}

/**
 * @brief Event loop for the NRFX T2T emulator
 *
 * @note This should run in a separate thread as it pulls events from the event queue with a
 * blocking call to event_wait.
 *
 * @param[in] arg Argument of the thread
 * @return void* Returns nothing
 */
static void *nrfx_event_loop(void *arg)
{
    (void)arg;
    event_queue_init(&event_queue);

    while (1) {
        event_t *event = event_wait(&event_queue);
        LOG_DEBUG("[EVENT QUEUE] Event received\n");
        event_handler_t handler = event->handler;
        handler(event);
    }
    return NULL;
}

/**
 * @brief Processes a read command
 *
 * @param[in] block_no block number to read from
 */
static void process_read_command(uint8_t block_no)
{
    t2t_handle_read(tag, block_no, data_buffer_tx);

    /* Set up the data descriptor for the response */
    nrfx_nfct_data_desc_t data_desc_tx = {
        .p_data = data_buffer_tx,
        .data_size = 16
    };

    /* Transmit the response */
    nrfx_err_t error = nrfx_nfct_tx(&data_desc_tx, NRF_NFCT_FRAME_DELAY_MODE_WINDOWGRID);
    assert(error == NRFX_SUCCESS);
}

/**
 * @brief Process received data
 *
 * @note Depending on the content either a read, write or halt command is processed
 *
 * @param[in] data Data to process
 * @param[in] size Size of the data
 */
static void parse_received_data(const uint8_t *data, uint32_t size)
{
    if (size == 0) {
        LOG_DEBUG("Received empty data\n");
        return;
    }
    else if (size == 1) {
        LOG_DEBUG("Received command byte only\n");
        return;
    }

    uint8_t command = data[0];
    uint8_t *data_buffer = (uint8_t *)data + 1;
    uint8_t data_size = size - 1;

    if (command == T2T_READ_COMMAND) {
        uint8_t block_address = data_buffer[0];
        LOG_DEBUG("Read command received for block %d\n", block_address);
        process_read_command(block_address);
    }
    else if (command == T2T_HALT_COMMAND) {
        LOG_DEBUG("Halt command received\n");
        /* TODO: work out redundant scanning of tag
        nrf_nfct_shorts_disable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK);
        nrfx_nfct_init_substate_force(NRFX_NFCT_ACTIVE_STATE_SLEEP); */
        selected = false;
    }
    else if (command == T2T_WRITE_COMMAND) {
        if (data_size != 5) {
            LOG_WARNING("Write command does not contain the correct amount of bytes");
            return;
        }
        process_write_command(*data_buffer, data_buffer + 1);
    }
    else if (command == T2T_SECTOR_SELECT_COMMAND) {
        send_ack_nack(false);
    }
    else {
        LOG_WARNING("Unknown command received (0x%02X)\n", command);
        print_bytes_as_hex(data, data_size);
    }
}

/**
 * @brief Handles a receive event
 *
 * @param[in] event Receive event to process
 */
static void receive_handler(event_t *event)
{
    nrfx_t2t_receive_event_t *e = container_of(event, nrfx_t2t_receive_event_t, super);
    parse_received_data(data_buffer_rx, e->size);
}

/**
 * @brief Handles an enable event
 *
 * @param[in] event Enable event to process
 */
static void enable_handler(event_t *event)
{
    (void)event;
    LOG_DEBUG("[EVENT] Enabling T2T\n");

    nrfx_nfct_enable();
    enabled = true;
}

/**
 * @brief Handles a disable event
 *
 * @param[in] event Disable event to process
 */
static void disable_handler(event_t *event)
{
    (void)event;

    nrfx_nfct_disable();
    enabled = false;
}

/**
 * @brief Handles an uninitialize event
 *
 * @param[in] event Uninitialize event to process
 */
static void uninitialize_handler(event_t *event)
{
    (void)event;

    nrfx_nfct_uninit();
    thread_zombify();
}

/**
 * @brief Handles the end of a transmission event
 *
 * @param[in] event End of transmission event to process
 */
static void end_of_tx_handler(event_t *event)
{
    (void)event;
    memset(data_buffer_rx, 0, buffer_size);
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    nrfx_nfct_rx(&data_desc_rx);
}

/**
 * @brief Handles the start of a receive event
 *
 * @param[in] event Start of receive event to process
 */
static void start_of_rx_handler(event_t *event)
{
    (void)event;
    LOG_DEBUG("Receiving data\n");
}

/**
 * @brief Handles a select event
 *
 * @param[in] event Select event to process
 */
static void select_handler(event_t *event)
{
    (void)event;
    memset(data_buffer_rx, 0, buffer_size);
    nrfx_nfct_data_desc_t data_desc_rx = {
        .p_data = data_buffer_rx,
        .data_size = buffer_size
    };
    selected = true;
    nrfx_nfct_rx(&data_desc_rx);
}

/**
 * @brief Field detected event handler
 *
 * @param[in] event Field detected event to process
 */
static void field_detected_handler(event_t *event)
{
    (void)event;

    LOG_DEBUG("[EVENT] Field detected\n");
    /* nrf_nfct_shorts_enable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK); */
}

/*
static void print_state(nrf_nfct_tag_state_t state) {

   if (state == NRF_NFCT_TAG_STATE_DISABLED) {
       LOG_DEBUG("Tag is disabled\n");
   } else if (state == NRF_NFCT_TAG_STATE_RAMP_UP) {
       LOG_DEBUG("Tag is ramping up\n");
   } else if (state == NRF_NFCT_TAG_STATE_IDLE) {
       LOG_DEBUG("Tag is idle\n");
   } else if (state == NRF_NFCT_TAG_STATE_RECEIVE) {
       LOG_DEBUG("Tag is receiving data\n");
   } else if (state == NRF_NFCT_TAG_STATE_FRAME_DELAY) {
       LOG_DEBUG("Tag is counting frame delay time\n");
   } else if (state == NRF_NFCT_TAG_STATE_TRANSMIT) {
       LOG_DEBUG("Tag is transmitting data\n");
   }
}
*/

/**
 * @brief IRQ event handler called by the NRFX NFCT driver
 *
 * @param[in] event event to process
 */
static void irq_event_handler(const nrfx_nfct_evt_t *event)
{
    LOG_DEBUG("[IRQ] ");
    if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMEEND) {
        LOG_DEBUG("Received frame of size %lu\n", event->params.rx_frameend.rx_data.data_size);
        receive_event.super.handler = receive_handler;
        receive_event.size = event->params.rx_frameend.rx_data.data_size;
        event_post(&event_queue, (event_t *)&receive_event);
    }
    else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMEEND) {
        LOG_DEBUG("End of transmission\n");
        end_of_transmission_event.handler = end_of_tx_handler;
        event_post(&event_queue, &end_of_transmission_event);
    }
    else if (event->evt_id == NRFX_NFCT_EVT_SELECTED) {
        LOG_DEBUG("Field has been selected\n");
        select_event.handler = select_handler;
        event_post(&event_queue, &select_event);
    }
    else if (event->evt_id == NRFX_NFCT_EVT_TX_FRAMESTART) {
        LOG_DEBUG("Transmitting data\n");
    }
    else if (event->evt_id == NRFX_NFCT_EVT_RX_FRAMESTART) {
        start_of_rx_event.handler = start_of_rx_handler;
        event_post(&event_queue, &start_of_rx_event);
    }
    else if (event->evt_id == NRFX_NFCT_EVT_ERROR) {
        nrfx_nfct_error_t error = event->params.error.reason;
        if (error == NRFX_NFCT_ERROR_FRAMEDELAYTIMEOUT) {
            if (selected) {
                LOG_DEBUG("ERROR: %X\n", event->params.error.reason);
            }
        }
    }
    else if (event->evt_id == NRFX_NFCT_EVT_FIELD_DETECTED) {
        LOG_DEBUG("Field detected\n");
        field_detected_event.handler = field_detected_handler;
        event_post(&event_queue, &field_detected_event);
    }
    else if (event->evt_id == NRFX_NFCT_EVT_FIELD_LOST) {
        LOG_DEBUG("Field lost\n");
        selected = false;
    }
    else {
        LOG_WARNING("Unknown event received\n");
    }
}

void initialize_t2t(nfc_t2t_t *_tag)
{
    if (initialized) {
        LOG_WARNING("T2T already initialized\n");
        return;
    }

    tag = _tag;

    initialized = true;
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = NRFX_NFCT_EVT_FIELD_DETECTED |
                         NRFX_NFCT_EVT_ERROR |
                         NRFX_NFCT_EVT_SELECTED |
                         NRFX_NFCT_EVT_FIELD_LOST |
                         NRFX_NFCT_EVT_RX_FRAMEEND |
                         NRFX_NFCT_EVT_RX_FRAMESTART |
                         NRFX_NFCT_EVT_TX_FRAMEEND |
                         NRFX_NFCT_EVT_TX_FRAMESTART
        /* NRF_NFCT_INT_RXERROR_MASK    |
                         NRF_NFCT_RX_FRAME_STATUS_CRC_MASK    |
                         NRF_NFCT_RX_FRAME_STATUS_PARITY_MASK |
                         NRF_NFCT_RX_FRAME_STATUS_OVERRUN_MASK |
                         NRF_NFCT_ERROR_FRAMEDELAYTIMEOUT_MASK*/
        ,
        .cb = irq_event_handler,
        .irq_priority = 2
    };

    nrfx_err_t error = nrfx_nfct_init(&config);
    initialized = true;
    assert(error == NRFX_SUCCESS);

    LOG_DEBUG("Initialization of NFCT driver successful!\n");

    /* the nfcid is part of the tag */
    uint8_t *nfcid1 = tag->sn.sn;
    uint8_t nfcid1_size = tag->sn.sn_length;
    configure_autocolres(nfcid1, nfcid1_size);

    nrf_nfct_shorts_enable(NRF_NFCT, NRF_NFCT_SHORT_FIELDDETECTED_ACTIVATE_MASK |
                                         NRF_NFCT_SHORT_FIELDLOST_SENSE_MASK);

    if (thread_pid == 0) {
        LOG_DEBUG("Creating thread for NFCT Type 2 Tag Emulator\n");
        thread_pid = thread_create(thread_stack, sizeof(thread_stack), THREAD_PRIORITY_MAIN - 1,
                                   0, nrfx_event_loop, NULL,
                                   "NRFX NFCT Type 2 Tag Emulator Thread");
    }
    else {
        LOG_WARNING("Thread already created, waking up thread\n");
        thread_wakeup(thread_pid);
    }
}

void enable_t2t(void)
{
    if (!initialized) {
        LOG_WARNING("T2T not initialized\n");
        return;
    }

    if (enabled) {
        LOG_WARNING("T2T already enabled\n");
        return;
    }

    LOG_DEBUG("Enabling T2T\n");

    enable_event.handler = enable_handler;
    event_post(&event_queue, &enable_event);
}

void uninitialize_t2t(void)
{
    if (!initialized) {
        LOG_WARNING("T2T is not initialized\n");
        return;
    }

    assert(thread_pid != 0);
    uninitialize_event.handler = uninitialize_handler;
    event_post(&event_queue, &uninitialize_event);

    while (thread_getstatus(thread_pid) != STATUS_ZOMBIE) {
    }

    if (thread_kill_zombie(thread_pid) != 1) {
        LOG_DEBUG("Cannot find thread or thread is not a zombie\n");
        return;
    }
    LOG_DEBUG("T2T Event Thread killed\n");

    enabled = false;
    initialized = false;
    thread_pid = 0;
}

void disable_t2t(void)
{
    if (!enabled) {
        LOG_ERROR("T2T is not enabled\n");
        return;
    }

    disable_event.handler = disable_handler;
    event_post(&event_queue, &disable_event);
}
