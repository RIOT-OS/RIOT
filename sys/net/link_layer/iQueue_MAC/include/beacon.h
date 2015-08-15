#define DEFAULT_SUBFRAME_DURATION 300000
#define A_UNIT_CP_PERIOD_uS      18000
#define MAX_LOCAL_NODE_NUM 40

#define SLOT_DURATION 5000
#define TDMA_TIME_BUFFER_DURATION 1


typedef unsigned char unit8_t;
typedef unsigned short unit16_t;
typedef unsigned long unit32_t;


/* queue of packets to be send during next transmission period */
typedef struct node_regis
{
    unit16_t node_ID;
    unit8_t queue_length;
} node_regis_struc;




void mac_creat_beacon(void);
void mac_vtdma_list_generate(void);
void tdma_list_init(void);

