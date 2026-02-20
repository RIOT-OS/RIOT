#include "bplib_init.h"
#include "bplib_cla_udp.h"
#include "bplib.h"

#include "shell.h"

#include <inttypes.h>

BPLib_PI_ChannelTable_t bplib_pi_channeltable = {
    .Configs = {
        {
            .RequestCustody = true,
            .CrcType = BPLib_CRC_Type_CRC16,
            .DestEID = {
                .Scheme = BPLIB_EID_SCHEME_IPN,
                .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                .Allocator = 0,
                .Node = 100,
                .Service = 60
            },
            .ReportToEID = {
                .Scheme       = BPLIB_EID_SCHEME_DTN,
                .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                .Allocator    = 0,
                .Node         = 0,
                .Service      = 0
            },
            .LocalServiceNumber = 60,
            .AgeBlkConfig = {
                .IncludeBlock = false,
                .BlockNum = 2
            },
            .Lifetime = 3600000
        }
    }
};

BPLib_CLA_ContactsTable_t bplib_cla_contactstable =
{
    .ContactSet = {
        {
            .DestEIDs               = {{
                                        .Scheme       = BPLIB_EID_SCHEME_IPN,
                                        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
                                        .MaxAllocator = 0,
                                        .MinAllocator = 0,
                                        .MaxNode      = 1000,
                                        .MinNode      = 1,
                                        .MaxService   = 1000,
                                        .MinService   = 1},},
            .CLAType                = 1, /*CLA Type, uint32 */
            .ClaInAddr              = "fd00::9191:11", /* Address to bind to */
            .ClaOutAddr             = "fd00::9191:13", /* CL out ip address */
            .ClaInPort              = 4556, /* Port Number, int32 */
            .ClaOutPort             = 4556,
            /* The following are NOP , not yet implemented by bplib */
            .RetransmitTimeout      = 10, /*bundle reforwarding timeout in seconds, uint32*/
            .CSTimeTrigger          = 10, /*Custody Signal time trigger in seconds, uint32*/
            .CSSizeTrigger          = 10 /*Custody signal size trigger in bytes, size_t*/
        },
    }
};


static bplib_cla_udp_t cla_udp1;


char stack_egress[THREAD_STACKSIZE_MEDIUM];
void* poll_bp(void *) {
    BPLib_Status_t rv;
    char buffer[256];
    size_t size;
    while(1) {
        rv = BPLib_PI_Egress(&bplib_instance_data.BPLibInst, 0, buffer, &size, sizeof(buffer), 1000);
        if (rv == 0) {
            buffer[size] = '\0';
            printf("Received Bundle of size %zd with content: %s\n", size, buffer);
        } else if (rv != BPLIB_PI_TIMEOUT) {
            printf("Egress error: %i\n", rv);
        }
    }
    
}

/******************************************************************************
 * Main
 ******************************************************************************/
int main(void)
{
    /*char cmd[] = "ifconfig 5 add fd00::9191:11/64";
    shell_handle_input_line(NULL, cmd);*/

    /* General init function. Time should be provided before calling this, since this,
     * by default, relies on walltime. */
    int rv = bplib_init();
    if (rv != 0) {
        printf("Error Initializing bplib: %i\n", rv);
        return 1;
    }

    /* Consume the incoming bundles */
    thread_create(stack_egress, sizeof(stack_egress),
        THREAD_PRIORITY_MAIN - 1, 0, (void* (*)(void*)) poll_bp,
        NULL, "bplib APP IN");

    /* Add and start the application level I/O */
    BPLib_PI_AddApplication(0);
    BPLib_PI_StartApplication(0);

    /* Start the implementation of the CLA */
    bplib_cla_udp_start(&cla_udp1, 0);

    /* Let bplib know the contact started */
    BPLib_CLA_ContactSetup(0);
    BPLib_CLA_ContactStart(0);

    char buffer[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, buffer, SHELL_DEFAULT_BUFSIZE);

    /* Cleanup (although not reachable), gives an idea of how this would be done if
     * bplib would be stopped in between. */

    BPLib_CLA_ContactStop(0);
    BPLib_CLA_ContactTeardown(&bplib_instance_data.BPLibInst, 0);
    bplib_cla_udp_stop(&cla_udp1);

    BPLib_PI_StopApplication(0);
    BPLib_PI_RemoveApplication(&bplib_instance_data.BPLibInst, 0);

    bplib_terminate();
}

static int _bp(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: bp <channel> <message>\n");
        return 1;
    }

    BPLib_PI_Ingress(&bplib_instance_data.BPLibInst, atoi(argv[1]), argv[2], strlen(argv[2]));

    return 0;
}
SHELL_COMMAND(bp, "Send BP Test Bundles", _bp);
