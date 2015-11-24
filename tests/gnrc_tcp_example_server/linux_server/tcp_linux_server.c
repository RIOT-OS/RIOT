/**
 * This example connects to a tcp server. It is assumed to work with RIOT using a tap device
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>

#define REQUESTED_DATA 4031
int backlog = 5;
int port = 2000; 
char *src = "fc00::1";


/* Request for data */
const char request_str[] = "Send me x bytes\n";
typedef struct {
    char msg[sizeof(request_str)];
    uint16_t nByte;
}request_t;

const char data_db[] = 
"\
1,Frank,Arnold,16zXWumSChs8R6wygZipkpHzbFFiTkKu9S,3528804259243022  \
2,Karen,Dunn,15ZWDprbUXUFhagFSFXpANFSgvTwnmxYtK,3531124531543668  \
3,Marie,Martin,1JEZs41VpDrDgy8j2wRgmkJgNSbgShEWrc,4844995724930428  \
4,Matthew,Chapman,17VkNah7Xqdasxu6ZcpHLNHoJtt3mYF3Vb,3528316994302773  \
5,Denise,Dean,1C8sTzTNUKWmFfefqMsKCMkjiSxAmg2wFw,633110552284760357  \
6,Rebecca,Ramirez,1Ahg6AVND3Chbgkf4RqtjrYdKhp66uidLe,3545649307679415  \
7,Dorothy,Jordan,136EutDy62XkTjZxmKa9MYbgw3qBcVhMRE,6375022962188098  \
8,Debra,Pierce,1GBc8fRcqa27vQKCLSg6je4rPVvCrvowpT,372301966017339  \
9,Martha,Williams,17S8XacqdT42End3dcUwQkP9dVR4Pk9nL9,676273534149365988  \
10,Henry,Hanson,13SjpfVBCWfq3huRXr2sFqtTM9iphJdUQP,56022447335705694  \
11,Amy,Foster,1MhjQAUXNYSu24jJLGCg1oU1LHxpTegswF,4936332764025526824  \
12,Judith,Murray,1LLXkYy5pw72gWsxanHnNkZg7KpgCRkAF7,3541531852547094  \
13,Alan,Hunt,17P3n3fgwWdje7riqbCUam1QU5wKmturrC,3584933423866805  \
14,Patricia,Henry,15khr7YnuxTJTca66TVA1Mi8ZyQTosgzWk,6759115613753664041  \
15,Barbara,Lane,15nu9jKEF63Rj1f9nFa2z9PL8wiz2ufZWW,3562349445931889  \
16,Maria,Watkins,1KJwSttMXViH52HSJq3zA1gJZ47z4BKAYS,3531681705657244  \
17,Judy,Mason,14hB49yg2McxAPKASzGcTwukkWmVoKZPzW,30071568392406  \
18,Judy,Ward,1GEYjpytgqz5Xi82LkcpgXm9jEmdqCT8d8,374288734142770  \
19,Kathleen,Campbell,15RS47kjm5A7XbFuJ7181CkCAqdsPXSdWe,4405113120511328  \
20,Bonnie,Cole,1FQaQNMF4tgTJ5UrVZjGB4k8xfPjPBsKVE,4911089591125680  \
21,Edward,Larson,1PRPzK9NKsirq2uMZuQQ4JGfhZ8kcgMJ6j,5524212620207099  \
22,Johnny,Simpson,165Q5TkF6Xuyab3VYFLuF8zQajkP1jXJte,3589007880571284  \
23,Andrew,Garcia,1LBoVcvodEjQQ1iPujnqM2GZfm3pfenbrx,3557017705588056  \
24,Antonio,Lee,17uGeVvh5b6ybnxxYMPWoVqi9m4wQiw4bN,3552293486903237  \
25,William,Roberts,1KE8DgiskjzsSwsS9gvYSxTQSHbxBQTvQe,5100139077157943  \
26,Katherine,Stevens,1AGzKd3H3p9tUJZYBPvubgfi4rAP8iaMFN,3573631943941545  \
27,Carolyn,Montgomery,19XAos358x7xvdH75rWf9XdvQKDRkMr6oG,4911191904557722  \
28,Walter,Miller,1Pv8iSubmnBhtRWQ7iEegyLq1XYyiu4K4v,3558805770652891  \
29,Andrea,Freeman,1KzjPB4f5akgzkusoHeJ5tQh1p9VDMCPR4,3550432622688789  \
30,Lawrence,Baker,12Pjj1S3eTiPttCJBwkJSnv7MxFLS5f1Ak,5602231696655234  \
31,George,Taylor,13dBxiVGnFfpNcShENe8R2RVGHzSKJra8q,201713176492457  \
32,Laura,Price,1Do359pNtVVUb7jhsUvKSRnqgXRghHrKfe,3552949137140467  \
33,Lois,Gardner,15JCQaq34v6dQVtFRVdFUZH9yEM9Ebpig9,4917852594430686  \
34,Harold,Evans,17Fcbeupv75gUYxfrzmbaFkwZ6yfq1jtFD,5100145849540670  \
35,Frank,Allen,1NSZDYJpyyW3mKv1AZpi5kx4GwkQsCHqcL,3532383635319271  \
36,Ralph,Martinez,1BwnsYwstFAHr3ibNxU9YQFiF6WCwuLb8w,67719967076001586  \
37,James,Phillips,151zS9bpfCCQXmp2Rq6P8D8aU9fmk8N7k2,4913493671955974  \
38,Howard,Stanley,139XHCti1QkGbxHvGuhb9fheAVn7G5Dhh7,3548761446737983  \
39,Justin,Spencer,1MnmitQhPnHAUec18pdod1Xr3PLKATMgvX,6333954816167640  \
40,Rachel,Elliott,17UzmyQ5LYzy85vRv1YMWBG13yxtZ2zcri,4405315294554394  \
41,Kathleen,Butler,13qXR8j17BMz8XCzhuv9EEjDpV8akrKKEb,5602234507341119  \
42,Earl,West,181cZbh3KCE4XVAV3vefJo71Z2MB6u3Jge,5602229041663655  \
43,Philip,Stanley,1DSe9E6WxVqWCYM1UShQkTp3FmfLB1MJhS,30179566677585  \
44,Henry,Fox,1KRSiVLHAprUcWZDeWsDPm9odhoCaKh234,3566070996023324  \
45,Lori,Hill,1GYaWBdn2fULanoaeF6YD8vDpmcrTgEXuv,4936362128712284118  \
46,Diane,Allen,19FtVkBa34ZadwSJwTriU32nupmG6JPyNe,6761454039663724  \
47,Joe,Ramirez,15fiU13gfdHrTwb6NZhueeP6iyzXtrDqX3,352824919008717  \
48,Lois,Russell,16PSzYrsuLafPvJdeDk4bhvmEewhHg972d,4911582495876531  \
49,Amanda,Foster,13hBS1pUvB5gTJeLouWKMw96zMUiHtx3iA,3547637532785402  \
50,Catherine,Sanchez,1Ph2zApgaTDFZexqdMY6FsxuaPTLd68UqH,201827297625991  \
51,Alice,Stanley,16NYpiRRNu4S3wdVvMFLBtoV3o24tETp1F,3538797922387481  \
52,Marie,Clark,18hsZBLm7jrDkymXR63m5Yr5vSXDagAMDL,3552860807653152  \
53,Jose,Bowman,1GdVnKDYCyxezwVHEqv5zeFiMmEHd42UtZ,3562971961343783  \
54,Brian,Gibson,1FMSWoBwrwGQrce2kowmjo62xcrfx8FaGs,4844372573233207  \
55,Teresa,Dean,1EqesxpoD4gFMEU1G7VGJP8AWk5DpdAKgF,3582077269634083  \
56,Amy,Foster,16Z3D1eTXB5p39BK96X7zFdf9zn5XkLNYh,201775460749348  \
57,Julia,Lawson,17t71ogbWKYiACQeRJJ7Dej4VPhuN88iMd,3581115788573555  \
58,Ryan,Dixon,1HTYXb6G9gZs3DGFQnvnZ6JpmJc3DsAeJC,3588513871552002  \
";

/* RIOT internet checksum function, used for verification */
uint16_t inet_csum_slice(uint16_t sum, const char *buf, uint16_t len, size_t accum_len)
{
    uint32_t csum = sum;

    if (len == 0)
        return csum;

    if (accum_len & 1) {      
        csum += *buf;         
        buf++;
        len--;
        accum_len++;
    }

    for (int i = 0; i < (len >> 1); buf += 2, i++) {
        csum += (*buf << 8) + *(buf + 1);
    }

    if ((accum_len + len) & 1)
        csum += (*buf << 8);

    while (csum >> 16) {
        uint16_t carry = csum >> 16;
        csum = (csum & 0xffff) + carry;
    }
    return csum;
}


int main(int argc, char **argv){
    int sock = 0;
    int peer = 0;
    int rcv = 0;

    request_t req;


    /* Fill Address Info */
    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    inet_pton(AF_INET6, src, &addr.sin6_addr);
    addr.sin6_flowinfo = 0;
 

    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Setup Socket failed\n");
        return -1;
    }

    if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        printf("Failed to bind\n");
        return -1;
    }

    listen(sock, backlog);  
    peer = accept(sock, NULL, NULL);
    printf("Peer accepted\n");

    /* Wait for request */
    rcv = 0;
    while((rcv += read(peer, &req + rcv, sizeof(req) - rcv)) < sizeof(req)){};

    /* Verify request */
    if(strncmp(req.msg, request_str, sizeof(req.msg)) == 0){
        printf("Received a valid request, requesting %d bytes\n", req.nByte);
        printf("Checksum for bytes to transmit: %d\n", inet_csum_slice(0, (void *)data_db, req.nByte, 0));
    }

    rcv = 0;
    while((rcv += write(peer, data_db + rcv, req.nByte - rcv)) < req.nByte){};

    close(peer);
    close(sock);

    return 0;
}
