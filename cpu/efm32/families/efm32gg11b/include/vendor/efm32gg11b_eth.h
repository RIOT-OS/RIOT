/***************************************************************************//**
 * @file
 * @brief EFM32GG11B_ETH register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @defgroup EFM32GG11B_ETH ETH
 * @{
 * @brief EFM32GG11B_ETH Register Declaration
 ******************************************************************************/
/** ETH Register Declaration */
typedef struct {
  __IOM uint32_t NETWORKCTRL;         /**< Network control register  */
  __IOM uint32_t NETWORKCFG;          /**< Network configuration register  */
  __IM uint32_t  NETWORKSTATUS;       /**< Network status register   */

  uint32_t       RESERVED0[1U];       /**< Reserved for future use **/
  __IOM uint32_t DMACFG;              /**< DMA Configuration Register  */
  __IOM uint32_t TXSTATUS;            /**< Transmit status register  */
  __IOM uint32_t RXQPTR;              /**< Start address of the receive buffer queue  */
  __IOM uint32_t TXQPTR;              /**< Start address of the transmit buffer queue  */
  __IOM uint32_t RXSTATUS;            /**< Receive status register  */
  __IOM uint32_t IFCR;                /**< Interrupt status register  */
  __IOM uint32_t IENS;                /**< Interrupt Enable Register  */
  __IOM uint32_t IENC;                /**< Interrupt Disable Register  */
  __IOM uint32_t IENRO;               /**< Interrupt mask register  */
  __IOM uint32_t PHYMNGMNT;           /**< PHY management register  */
  __IM uint32_t  RXPAUSEQUANT;        /**< Received Pause Quantum Register  */
  __IOM uint32_t TXPAUSEQUANT;        /**< Transmit Pause Quantum Register  */
  __IOM uint32_t PBUFTXCUTTHRU;       /**< TX Partial Store and Forward  */
  __IOM uint32_t PBUFRXCUTTHRU;       /**< RX Partial Store and Forward  */
  __IOM uint32_t JUMBOMAXLEN;         /**< Maximum Jumbo Frame Size.  */

  uint32_t       RESERVED1[4U];       /**< Reserved for future use **/
  __IOM uint32_t IMOD;                /**< Interrupt moderation register  */
  __IOM uint32_t SYSWAKETIME;         /**< System wake time  */
  uint32_t       RESERVED2[7U];       /**< Reserved for future use **/
  __IOM uint32_t HASHBOTTOM;          /**< Hash Register Bottom [31:0]  */
  __IOM uint32_t HASHTOP;             /**< Hash Register Top [63:32]  */
  __IOM uint32_t SPECADDR1BOTTOM;     /**< Specific Address 1 Bottom  */
  __IOM uint32_t SPECADDR1TOP;        /**< Specific Address 1 Top  */
  __IOM uint32_t SPECADDR2BOTTOM;     /**< Specific Address 2 Bottom  */
  __IOM uint32_t SPECADDR2TOP;        /**< Specific Address 2 Top  */
  __IOM uint32_t SPECADDR3BOTTOM;     /**< Specific Address 3 Bottom  */
  __IOM uint32_t SPECADDR3TOP;        /**< Specific Address 3 Top  */
  __IOM uint32_t SPECADDR4BOTTOM;     /**< Specific Address 4 Bottom  */
  __IOM uint32_t SPECADDR4TOP;        /**< Specific Address 4 Top  */
  __IOM uint32_t SPECTYPE1;           /**< Type ID Match 1  */
  __IOM uint32_t SPECTYPE2;           /**< Type ID Match 2  */
  __IOM uint32_t SPECTYPE3;           /**< Type ID Match 3  */
  __IOM uint32_t SPECTYPE4;           /**< Type ID Match 4  */
  __IOM uint32_t WOLREG;              /**< Wake on LAN Register  */
  __IOM uint32_t STRETCHRATIO;        /**< IPG stretch register  */
  __IOM uint32_t STACKEDVLAN;         /**< Stacked VLAN Register  */
  __IOM uint32_t TXPFCPAUSE;          /**< Transmit PFC Pause Register   */
  __IOM uint32_t MASKADD1BOTTOM;      /**< Specific Address Mask 1 Bottom 31:0  */
  __IOM uint32_t MASKADD1TOP;         /**< Specific Address Mask 1 Top 47:32  */

  uint32_t       RESERVED3[1U];       /**< Reserved for future use **/
  __IOM uint32_t RXPTPUNICAST;        /**< PTP RX unicast IP destination address   */
  __IOM uint32_t TXPTPUNICAST;        /**< PTP TX unicast IP destination address   */
  __IOM uint32_t TSUNSECCMP;          /**< TSU timer comparison value nanoseconds   */
  __IOM uint32_t TSUSECCMP;           /**< TSU timer comparison value seconds [31:0]  */
  __IOM uint32_t TSUMSBSECCMP;        /**< TSU timer comparison value seconds [47:32]  */
  __IM uint32_t  TSUPTPTXMSBSEC;      /**< PTP Event Frame Transmitted Seconds Register 47:32  */
  __IM uint32_t  TSUPTPRXMSBSEC;      /**< PTP Event Frame Received Seconds Register 47:32  */
  __IM uint32_t  TSUPEERTXMSBSEC;     /**< PTP Peer Event Frame Transmitted Seconds Register 47:32  */
  __IM uint32_t  TSUPEERRXMSBSEC;     /**< PTP Peer Event Frame Received Seconds Register 47:32  */

  uint32_t       RESERVED4[2U];       /**< Reserved for future use **/
  __IOM uint32_t OCTETSTXEDBOTTOM;    /**< Octets transmitted 31:0  */
  __IOM uint32_t OCTETSTXEDTOP;       /**< Octets Transmitted 47:32  */
  __IOM uint32_t FRAMESTXEDOK;        /**< Frames Transmitted  */
  __IOM uint32_t BROADCASTTXED;       /**< Broadcast Frames Transmitted  */
  __IOM uint32_t MULTICASTTXED;       /**< Multicast Frames Transmitted  */
  __IOM uint32_t PFRAMESTXED;         /**< Pause Frames Transmitted  */
  __IOM uint32_t FRAMESTXED64;        /**< 64 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED65;        /**< 65 to 127 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED128;       /**< 128 to 255 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED256;       /**< 256 to 511 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED512;       /**< 512 to 1023 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED1024;      /**< 1024 to 1518 Byte Frames Transmitted  */
  __IOM uint32_t FRAMESTXED1519;      /**< Greater Than 1518 Byte Frames Transmitted  */
  __IOM uint32_t TXUNDERRUNS;         /**< Transmit Under Runs  */
  __IOM uint32_t SINGLECOLS;          /**< Single Collision Frames  */
  __IOM uint32_t MULTICOLS;           /**< Multiple Collision Frames  */
  __IOM uint32_t EXCESSCOLS;          /**< Excessive Collisions  */
  __IOM uint32_t LATECOLS;            /**< Late Collisions  */
  __IOM uint32_t DEFERREDFRAMES;      /**< Deferred Transmission Frames  */
  __IOM uint32_t CRSERRS;             /**< Carrier Sense Errors  */
  __IOM uint32_t OCTETSRXEDBOTTOM;    /**< Octets Received 31:0  */
  __IOM uint32_t OCTETSRXEDTOP;       /**< Octets Received 47:32  */
  __IOM uint32_t FRAMESRXEDOK;        /**< Frames Received  */
  __IOM uint32_t BROADCASTRXED;       /**< Broadcast Frames Received  */
  __IOM uint32_t MULTICASTRXED;       /**< Multicast Frames Received  */
  __IOM uint32_t PFRAMESRXED;         /**< Pause Frames Received  */
  __IOM uint32_t FRAMESRXED64;        /**< 64 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED65;        /**< 65 to 127 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED128;       /**< 128 to 255 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED256;       /**< 256 to 511 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED512;       /**< 512 to 1023 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED1024;      /**< 1024 to 1518 Byte Frames Received  */
  __IOM uint32_t FRAMESRXED1519;      /**< 1519 to maximum Byte Frames Received  */
  __IOM uint32_t UNDERSIZEFRAMES;     /**< Undersized Frames Received  */
  __IOM uint32_t EXCESSIVERXLEN;      /**< Oversize Frames Received  */
  __IOM uint32_t RXJABBERS;           /**< Jabbers Received  */
  __IOM uint32_t FCSERRS;             /**< Frame Check Sequence Errors  */
  __IOM uint32_t RXLENERRS;           /**< Length Field Frame Errors  */
  __IOM uint32_t RXSYMBOLERRS;        /**< Receive Symbol Errors  */
  __IOM uint32_t ALIGNERRS;           /**< Alignment Errors  */
  __IOM uint32_t RXRESOURCEERRS;      /**< Receive Resource Errors  */
  __IOM uint32_t RXOVERRUNS;          /**< Receive Overruns  */
  __IOM uint32_t RXIPCKERRS;          /**< IP Header Checksum Errors  */
  __IOM uint32_t RXTCPCKERRS;         /**< TCP Checksum Errors  */
  __IOM uint32_t RXUDPCKERRS;         /**< UDP Checksum Errors  */
  __IOM uint32_t AUTOFLUSHEDPKTS;     /**< Receive DMA Flushed Packets  */
  uint32_t       RESERVED5[1U];       /**< Reserved for future use **/
  __IOM uint32_t TSUTIMERINCRSUBNSEC; /**< 1588 Timer Increment Register subscript nsec  */
  __IOM uint32_t TSUTIMERMSBSEC;      /**< 1588 Timer Seconds Register 47:32  */

  uint32_t       RESERVED6[3U];       /**< Reserved for future use **/
  __IOM uint32_t TSUTIMERSEC;         /**< 1588 Timer Seconds Register 31:0  */
  __IOM uint32_t TSUTIMERNSEC;        /**< 1588 Timer Nanoseconds Register  */
  __IOM uint32_t TSUTIMERADJUST;      /**< This register returns all zeroes when read.  */
  __IOM uint32_t TSUTIMERINCR;        /**< 1588 Timer Increment Register  */
  __IM uint32_t  TSUPTPTXSEC;         /**< PTP Event Frame Transmitted Seconds Register 31:0  */
  __IM uint32_t  TSUPTPTXNSEC;        /**< PTP Event Frame Transmitted Nanoseconds Register  */
  __IM uint32_t  TSUPTPRXSEC;         /**< PTP Event Frame Received Seconds Register 31:0  */
  __IM uint32_t  TSUPTPRXNSEC;        /**< PTP Event Frame Received Nanoseconds Register  */
  __IM uint32_t  TSUPEERTXSEC;        /**< PTP Peer Event Frame Transmitted Seconds Register 31:0  */
  __IM uint32_t  TSUPEERTXNSEC;       /**< PTP Peer Event Frame Transmitted Nanoseconds Register  */
  __IM uint32_t  TSUPEERRXSEC;        /**< PTP Peer Event Frame Received Seconds Register 31:0  */
  __IM uint32_t  TSUPEERRXNSEC;       /**< PTP Peer Event Frame Received Nanoseconds Register  */

  uint32_t       RESERVED7[24U];      /**< Reserved for future use **/
  __IOM uint32_t TXPAUSEQUANT1;       /**< Transmit Pause Quantum Register 1  */
  __IOM uint32_t TXPAUSEQUANT2;       /**< Transmit Pause Quantum Register 2  */
  __IOM uint32_t TXPAUSEQUANT3;       /**< Transmit Pause Quantum Register 3  */
  uint32_t       RESERVED8[1U];       /**< Reserved for future use **/
  __IOM uint32_t RXLPI;               /**< Received LPI transitions  */
  __IOM uint32_t RXLPITIME;           /**< Received LPI time  */
  __IOM uint32_t TXLPI;               /**< Transmit LPI transitions  */
  __IOM uint32_t TXLPITIME;           /**< Transmit LPI time  */

  uint32_t       RESERVED9[147U];     /**< Reserved for future use **/
  __IOM uint32_t TXBDCTRL;            /**< TX BD control register  */
  __IOM uint32_t RXBDCTRL;            /**< RX BD control register  */

  uint32_t       RESERVED10[459U];    /**< Reserved for future use **/
  __IOM uint32_t ROUTEPEN;            /**< I/O Route Enable Register   */
  __IOM uint32_t ROUTELOC0;           /**< I/O Route Location Register 0  */
  uint32_t       RESERVED11[1U];      /**< Reserved for future use **/
  __IOM uint32_t ROUTELOC1;           /**< I/O Route Location Register 1  */
  __IOM uint32_t CTRL;                /**< Ethernet control register  */
} ETH_TypeDef;                        /** @} */

/***************************************************************************//**
 * @addtogroup EFM32GG11B_ETH
 * @{
 * @defgroup EFM32GG11B_ETH_BitFields  ETH Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for ETH NETWORKCTRL */
#define _ETH_NETWORKCTRL_RESETVALUE                      0x00000000UL                                     /**< Default value for ETH_NETWORKCTRL */
#define _ETH_NETWORKCTRL_MASK                            0x035F9FFEUL                                     /**< Mask for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_LOOPBACKLOCAL                    (0x1UL << 1)                                     /**< Loopback local */
#define _ETH_NETWORKCTRL_LOOPBACKLOCAL_SHIFT             1                                                /**< Shift value for ETH_LOOPBACKLOCAL */
#define _ETH_NETWORKCTRL_LOOPBACKLOCAL_MASK              0x2UL                                            /**< Bit mask for ETH_LOOPBACKLOCAL */
#define _ETH_NETWORKCTRL_LOOPBACKLOCAL_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_LOOPBACKLOCAL_DEFAULT            (_ETH_NETWORKCTRL_LOOPBACKLOCAL_DEFAULT << 1)    /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ENBRX                            (0x1UL << 2)                                     /**< Receive enable */
#define _ETH_NETWORKCTRL_ENBRX_SHIFT                     2                                                /**< Shift value for ETH_ENBRX */
#define _ETH_NETWORKCTRL_ENBRX_MASK                      0x4UL                                            /**< Bit mask for ETH_ENBRX */
#define _ETH_NETWORKCTRL_ENBRX_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ENBRX_DEFAULT                    (_ETH_NETWORKCTRL_ENBRX_DEFAULT << 2)            /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ENBTX                            (0x1UL << 3)                                     /**< Transmit enable */
#define _ETH_NETWORKCTRL_ENBTX_SHIFT                     3                                                /**< Shift value for ETH_ENBTX */
#define _ETH_NETWORKCTRL_ENBTX_MASK                      0x8UL                                            /**< Bit mask for ETH_ENBTX */
#define _ETH_NETWORKCTRL_ENBTX_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ENBTX_DEFAULT                    (_ETH_NETWORKCTRL_ENBTX_DEFAULT << 3)            /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_MANPORTEN                        (0x1UL << 4)                                     /**< Management port enable */
#define _ETH_NETWORKCTRL_MANPORTEN_SHIFT                 4                                                /**< Shift value for ETH_MANPORTEN */
#define _ETH_NETWORKCTRL_MANPORTEN_MASK                  0x10UL                                           /**< Bit mask for ETH_MANPORTEN */
#define _ETH_NETWORKCTRL_MANPORTEN_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_MANPORTEN_DEFAULT                (_ETH_NETWORKCTRL_MANPORTEN_DEFAULT << 4)        /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_CLRALLSTATSREGS                  (0x1UL << 5)                                     /**< Clear statistics registers */
#define _ETH_NETWORKCTRL_CLRALLSTATSREGS_SHIFT           5                                                /**< Shift value for ETH_CLRALLSTATSREGS */
#define _ETH_NETWORKCTRL_CLRALLSTATSREGS_MASK            0x20UL                                           /**< Bit mask for ETH_CLRALLSTATSREGS */
#define _ETH_NETWORKCTRL_CLRALLSTATSREGS_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_CLRALLSTATSREGS_DEFAULT          (_ETH_NETWORKCTRL_CLRALLSTATSREGS_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_INCALLSTATSREGS                  (0x1UL << 6)                                     /**< Incremental statistics registers */
#define _ETH_NETWORKCTRL_INCALLSTATSREGS_SHIFT           6                                                /**< Shift value for ETH_INCALLSTATSREGS */
#define _ETH_NETWORKCTRL_INCALLSTATSREGS_MASK            0x40UL                                           /**< Bit mask for ETH_INCALLSTATSREGS */
#define _ETH_NETWORKCTRL_INCALLSTATSREGS_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_INCALLSTATSREGS_DEFAULT          (_ETH_NETWORKCTRL_INCALLSTATSREGS_DEFAULT << 6)  /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STATSWREN                        (0x1UL << 7)                                     /**< Write enable for statistics registers */
#define _ETH_NETWORKCTRL_STATSWREN_SHIFT                 7                                                /**< Shift value for ETH_STATSWREN */
#define _ETH_NETWORKCTRL_STATSWREN_MASK                  0x80UL                                           /**< Bit mask for ETH_STATSWREN */
#define _ETH_NETWORKCTRL_STATSWREN_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STATSWREN_DEFAULT                (_ETH_NETWORKCTRL_STATSWREN_DEFAULT << 7)        /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_BACKPRESSURE                     (0x1UL << 8)                                     /**< Back pressure will force collisions on all received frames */
#define _ETH_NETWORKCTRL_BACKPRESSURE_SHIFT              8                                                /**< Shift value for ETH_BACKPRESSURE */
#define _ETH_NETWORKCTRL_BACKPRESSURE_MASK               0x100UL                                          /**< Bit mask for ETH_BACKPRESSURE */
#define _ETH_NETWORKCTRL_BACKPRESSURE_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_BACKPRESSURE_DEFAULT             (_ETH_NETWORKCTRL_BACKPRESSURE_DEFAULT << 8)     /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXSTRT                           (0x1UL << 9)                                     /**< Start transmission */
#define _ETH_NETWORKCTRL_TXSTRT_SHIFT                    9                                                /**< Shift value for ETH_TXSTRT */
#define _ETH_NETWORKCTRL_TXSTRT_MASK                     0x200UL                                          /**< Bit mask for ETH_TXSTRT */
#define _ETH_NETWORKCTRL_TXSTRT_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXSTRT_DEFAULT                   (_ETH_NETWORKCTRL_TXSTRT_DEFAULT << 9)           /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXHALT                           (0x1UL << 10)                                    /**< Transmit halt */
#define _ETH_NETWORKCTRL_TXHALT_SHIFT                    10                                               /**< Shift value for ETH_TXHALT */
#define _ETH_NETWORKCTRL_TXHALT_MASK                     0x400UL                                          /**< Bit mask for ETH_TXHALT */
#define _ETH_NETWORKCTRL_TXHALT_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXHALT_DEFAULT                   (_ETH_NETWORKCTRL_TXHALT_DEFAULT << 10)          /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFRMREQ                        (0x1UL << 11)                                    /**< Transmit pause frame  */
#define _ETH_NETWORKCTRL_TXPFRMREQ_SHIFT                 11                                               /**< Shift value for ETH_TXPFRMREQ */
#define _ETH_NETWORKCTRL_TXPFRMREQ_MASK                  0x800UL                                          /**< Bit mask for ETH_TXPFRMREQ */
#define _ETH_NETWORKCTRL_TXPFRMREQ_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFRMREQ_DEFAULT                (_ETH_NETWORKCTRL_TXPFRMREQ_DEFAULT << 11)       /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFRMZERO                       (0x1UL << 12)                                    /**< Transmit zero quantum pause frame  */
#define _ETH_NETWORKCTRL_TXPFRMZERO_SHIFT                12                                               /**< Shift value for ETH_TXPFRMZERO */
#define _ETH_NETWORKCTRL_TXPFRMZERO_MASK                 0x1000UL                                         /**< Bit mask for ETH_TXPFRMZERO */
#define _ETH_NETWORKCTRL_TXPFRMZERO_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFRMZERO_DEFAULT               (_ETH_NETWORKCTRL_TXPFRMZERO_DEFAULT << 12)      /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STORERXTS                        (0x1UL << 15)                                    /**< Store receive time stamp to memory. */
#define _ETH_NETWORKCTRL_STORERXTS_SHIFT                 15                                               /**< Shift value for ETH_STORERXTS */
#define _ETH_NETWORKCTRL_STORERXTS_MASK                  0x8000UL                                         /**< Bit mask for ETH_STORERXTS */
#define _ETH_NETWORKCTRL_STORERXTS_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STORERXTS_DEFAULT                (_ETH_NETWORKCTRL_STORERXTS_DEFAULT << 15)       /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PFCENB                           (0x1UL << 16)                                    /**< Enable PFC Priority Based Pause Reception capabilities. */
#define _ETH_NETWORKCTRL_PFCENB_SHIFT                    16                                               /**< Shift value for ETH_PFCENB */
#define _ETH_NETWORKCTRL_PFCENB_MASK                     0x10000UL                                        /**< Bit mask for ETH_PFCENB */
#define _ETH_NETWORKCTRL_PFCENB_DEFAULT                  0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PFCENB_DEFAULT                   (_ETH_NETWORKCTRL_PFCENB_DEFAULT << 16)          /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFCPRIORPFRM                   (0x1UL << 17)                                    /**< Write a one to transmit PFC priority based pause frame. */
#define _ETH_NETWORKCTRL_TXPFCPRIORPFRM_SHIFT            17                                               /**< Shift value for ETH_TXPFCPRIORPFRM */
#define _ETH_NETWORKCTRL_TXPFCPRIORPFRM_MASK             0x20000UL                                        /**< Bit mask for ETH_TXPFCPRIORPFRM */
#define _ETH_NETWORKCTRL_TXPFCPRIORPFRM_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXPFCPRIORPFRM_DEFAULT           (_ETH_NETWORKCTRL_TXPFCPRIORPFRM_DEFAULT << 17)  /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_FLUSHRXPKT                       (0x1UL << 18)                                    /**< Flush the next packet from the external RX DPRAM. */
#define _ETH_NETWORKCTRL_FLUSHRXPKT_SHIFT                18                                               /**< Shift value for ETH_FLUSHRXPKT */
#define _ETH_NETWORKCTRL_FLUSHRXPKT_MASK                 0x40000UL                                        /**< Bit mask for ETH_FLUSHRXPKT */
#define _ETH_NETWORKCTRL_FLUSHRXPKT_DEFAULT              0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_FLUSHRXPKT_DEFAULT               (_ETH_NETWORKCTRL_FLUSHRXPKT_DEFAULT << 18)      /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXLPIEN                          (0x1UL << 19)                                    /**< Enable LPI transmission when set LPI (low power idle) is immediately transmitted. */
#define _ETH_NETWORKCTRL_TXLPIEN_SHIFT                   19                                               /**< Shift value for ETH_TXLPIEN */
#define _ETH_NETWORKCTRL_TXLPIEN_MASK                    0x80000UL                                        /**< Bit mask for ETH_TXLPIEN */
#define _ETH_NETWORKCTRL_TXLPIEN_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_TXLPIEN_DEFAULT                  (_ETH_NETWORKCTRL_TXLPIEN_DEFAULT << 19)         /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PTPUNICASTEN                     (0x1UL << 20)                                    /**< Enable detection of unicast PTP unicast frames. */
#define _ETH_NETWORKCTRL_PTPUNICASTEN_SHIFT              20                                               /**< Shift value for ETH_PTPUNICASTEN */
#define _ETH_NETWORKCTRL_PTPUNICASTEN_MASK               0x100000UL                                       /**< Bit mask for ETH_PTPUNICASTEN */
#define _ETH_NETWORKCTRL_PTPUNICASTEN_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PTPUNICASTEN_DEFAULT             (_ETH_NETWORKCTRL_PTPUNICASTEN_DEFAULT << 20)    /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STOREUDPOFFSET                   (0x1UL << 22)                                    /**< Store UDP / TCP offset to memory. */
#define _ETH_NETWORKCTRL_STOREUDPOFFSET_SHIFT            22                                               /**< Shift value for ETH_STOREUDPOFFSET */
#define _ETH_NETWORKCTRL_STOREUDPOFFSET_MASK             0x400000UL                                       /**< Bit mask for ETH_STOREUDPOFFSET */
#define _ETH_NETWORKCTRL_STOREUDPOFFSET_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_STOREUDPOFFSET_DEFAULT           (_ETH_NETWORKCTRL_STOREUDPOFFSET_DEFAULT << 22)  /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ONESTEPSYNCMODE                  (0x1UL << 24)                                    /**< 1588 One Step Sync Mode.  */
#define _ETH_NETWORKCTRL_ONESTEPSYNCMODE_SHIFT           24                                               /**< Shift value for ETH_ONESTEPSYNCMODE */
#define _ETH_NETWORKCTRL_ONESTEPSYNCMODE_MASK            0x1000000UL                                      /**< Bit mask for ETH_ONESTEPSYNCMODE */
#define _ETH_NETWORKCTRL_ONESTEPSYNCMODE_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_ONESTEPSYNCMODE_DEFAULT          (_ETH_NETWORKCTRL_ONESTEPSYNCMODE_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PFCCTRL                          (0x1UL << 25)                                    /**< Enable multiple PFC pause quantums, one per pause priority */
#define _ETH_NETWORKCTRL_PFCCTRL_SHIFT                   25                                               /**< Shift value for ETH_PFCCTRL */
#define _ETH_NETWORKCTRL_PFCCTRL_MASK                    0x2000000UL                                      /**< Bit mask for ETH_PFCCTRL */
#define _ETH_NETWORKCTRL_PFCCTRL_DEFAULT                 0x00000000UL                                     /**< Mode DEFAULT for ETH_NETWORKCTRL */
#define ETH_NETWORKCTRL_PFCCTRL_DEFAULT                  (_ETH_NETWORKCTRL_PFCCTRL_DEFAULT << 25)         /**< Shifted mode DEFAULT for ETH_NETWORKCTRL */

/* Bit fields for ETH NETWORKCFG */
#define _ETH_NETWORKCFG_RESETVALUE                       0x00080000UL                                         /**< Default value for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MASK                             0x779FF1FFUL                                         /**< Mask for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_SPEED                             (0x1UL << 0)                                         /**< Speed */
#define _ETH_NETWORKCFG_SPEED_SHIFT                      0                                                    /**< Shift value for ETH_SPEED */
#define _ETH_NETWORKCFG_SPEED_MASK                       0x1UL                                                /**< Bit mask for ETH_SPEED */
#define _ETH_NETWORKCFG_SPEED_DEFAULT                    0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_SPEED_DEFAULT                     (_ETH_NETWORKCFG_SPEED_DEFAULT << 0)                 /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_FULLDUPLEX                        (0x1UL << 1)                                         /**< Full duplex */
#define _ETH_NETWORKCFG_FULLDUPLEX_SHIFT                 1                                                    /**< Shift value for ETH_FULLDUPLEX */
#define _ETH_NETWORKCFG_FULLDUPLEX_MASK                  0x2UL                                                /**< Bit mask for ETH_FULLDUPLEX */
#define _ETH_NETWORKCFG_FULLDUPLEX_DEFAULT               0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_FULLDUPLEX_DEFAULT                (_ETH_NETWORKCFG_FULLDUPLEX_DEFAULT << 1)            /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_DISCRDNONVLANFRAMES               (0x1UL << 2)                                         /**< Discard non-VLAN frames  */
#define _ETH_NETWORKCFG_DISCRDNONVLANFRAMES_SHIFT        2                                                    /**< Shift value for ETH_DISCRDNONVLANFRAMES */
#define _ETH_NETWORKCFG_DISCRDNONVLANFRAMES_MASK         0x4UL                                                /**< Bit mask for ETH_DISCRDNONVLANFRAMES */
#define _ETH_NETWORKCFG_DISCRDNONVLANFRAMES_DEFAULT      0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_DISCRDNONVLANFRAMES_DEFAULT       (_ETH_NETWORKCFG_DISCRDNONVLANFRAMES_DEFAULT << 2)   /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_JUMBOFRAMES                       (0x1UL << 3)                                         /**< Jumbo frames enable */
#define _ETH_NETWORKCFG_JUMBOFRAMES_SHIFT                3                                                    /**< Shift value for ETH_JUMBOFRAMES */
#define _ETH_NETWORKCFG_JUMBOFRAMES_MASK                 0x8UL                                                /**< Bit mask for ETH_JUMBOFRAMES */
#define _ETH_NETWORKCFG_JUMBOFRAMES_DEFAULT              0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_JUMBOFRAMES_DEFAULT               (_ETH_NETWORKCFG_JUMBOFRAMES_DEFAULT << 3)           /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_COPYALLFRAMES                     (0x1UL << 4)                                         /**< Copy all frames */
#define _ETH_NETWORKCFG_COPYALLFRAMES_SHIFT              4                                                    /**< Shift value for ETH_COPYALLFRAMES */
#define _ETH_NETWORKCFG_COPYALLFRAMES_MASK               0x10UL                                               /**< Bit mask for ETH_COPYALLFRAMES */
#define _ETH_NETWORKCFG_COPYALLFRAMES_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_COPYALLFRAMES_DEFAULT             (_ETH_NETWORKCFG_COPYALLFRAMES_DEFAULT << 4)         /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_NOBROADCAST                       (0x1UL << 5)                                         /**< No broadcast */
#define _ETH_NETWORKCFG_NOBROADCAST_SHIFT                5                                                    /**< Shift value for ETH_NOBROADCAST */
#define _ETH_NETWORKCFG_NOBROADCAST_MASK                 0x20UL                                               /**< Bit mask for ETH_NOBROADCAST */
#define _ETH_NETWORKCFG_NOBROADCAST_DEFAULT              0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_NOBROADCAST_DEFAULT               (_ETH_NETWORKCFG_NOBROADCAST_DEFAULT << 5)           /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MULTICASTHASHEN                   (0x1UL << 6)                                         /**< Multicast hash enable */
#define _ETH_NETWORKCFG_MULTICASTHASHEN_SHIFT            6                                                    /**< Shift value for ETH_MULTICASTHASHEN */
#define _ETH_NETWORKCFG_MULTICASTHASHEN_MASK             0x40UL                                               /**< Bit mask for ETH_MULTICASTHASHEN */
#define _ETH_NETWORKCFG_MULTICASTHASHEN_DEFAULT          0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MULTICASTHASHEN_DEFAULT           (_ETH_NETWORKCFG_MULTICASTHASHEN_DEFAULT << 6)       /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_UNICASTHASHEN                     (0x1UL << 7)                                         /**< Unicast hash enable */
#define _ETH_NETWORKCFG_UNICASTHASHEN_SHIFT              7                                                    /**< Shift value for ETH_UNICASTHASHEN */
#define _ETH_NETWORKCFG_UNICASTHASHEN_MASK               0x80UL                                               /**< Bit mask for ETH_UNICASTHASHEN */
#define _ETH_NETWORKCFG_UNICASTHASHEN_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_UNICASTHASHEN_DEFAULT             (_ETH_NETWORKCFG_UNICASTHASHEN_DEFAULT << 7)         /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RX1536BYTEFRAMES                  (0x1UL << 8)                                         /**< Receive 1536 byte frames */
#define _ETH_NETWORKCFG_RX1536BYTEFRAMES_SHIFT           8                                                    /**< Shift value for ETH_RX1536BYTEFRAMES */
#define _ETH_NETWORKCFG_RX1536BYTEFRAMES_MASK            0x100UL                                              /**< Bit mask for ETH_RX1536BYTEFRAMES */
#define _ETH_NETWORKCFG_RX1536BYTEFRAMES_DEFAULT         0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RX1536BYTEFRAMES_DEFAULT          (_ETH_NETWORKCFG_RX1536BYTEFRAMES_DEFAULT << 8)      /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RETRYTEST                         (0x1UL << 12)                                        /**< Retry test */
#define _ETH_NETWORKCFG_RETRYTEST_SHIFT                  12                                                   /**< Shift value for ETH_RETRYTEST */
#define _ETH_NETWORKCFG_RETRYTEST_MASK                   0x1000UL                                             /**< Bit mask for ETH_RETRYTEST */
#define _ETH_NETWORKCFG_RETRYTEST_DEFAULT                0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RETRYTEST_DEFAULT                 (_ETH_NETWORKCFG_RETRYTEST_DEFAULT << 12)            /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_PAUSEEN                           (0x1UL << 13)                                        /**< Pause enable */
#define _ETH_NETWORKCFG_PAUSEEN_SHIFT                    13                                                   /**< Shift value for ETH_PAUSEEN */
#define _ETH_NETWORKCFG_PAUSEEN_MASK                     0x2000UL                                             /**< Bit mask for ETH_PAUSEEN */
#define _ETH_NETWORKCFG_PAUSEEN_DEFAULT                  0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_PAUSEEN_DEFAULT                   (_ETH_NETWORKCFG_PAUSEEN_DEFAULT << 13)              /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_RXBUFFOFFSET_SHIFT               14                                                   /**< Shift value for ETH_RXBUFFOFFSET */
#define _ETH_NETWORKCFG_RXBUFFOFFSET_MASK                0xC000UL                                             /**< Bit mask for ETH_RXBUFFOFFSET */
#define _ETH_NETWORKCFG_RXBUFFOFFSET_DEFAULT             0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RXBUFFOFFSET_DEFAULT              (_ETH_NETWORKCFG_RXBUFFOFFSET_DEFAULT << 14)         /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_LENFIELDERRFRMDISCRD              (0x1UL << 16)                                        /**< Length field error frame discard */
#define _ETH_NETWORKCFG_LENFIELDERRFRMDISCRD_SHIFT       16                                                   /**< Shift value for ETH_LENFIELDERRFRMDISCRD */
#define _ETH_NETWORKCFG_LENFIELDERRFRMDISCRD_MASK        0x10000UL                                            /**< Bit mask for ETH_LENFIELDERRFRMDISCRD */
#define _ETH_NETWORKCFG_LENFIELDERRFRMDISCRD_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_LENFIELDERRFRMDISCRD_DEFAULT      (_ETH_NETWORKCFG_LENFIELDERRFRMDISCRD_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_FCSREMOVE                         (0x1UL << 17)                                        /**< FCS remove */
#define _ETH_NETWORKCFG_FCSREMOVE_SHIFT                  17                                                   /**< Shift value for ETH_FCSREMOVE */
#define _ETH_NETWORKCFG_FCSREMOVE_MASK                   0x20000UL                                            /**< Bit mask for ETH_FCSREMOVE */
#define _ETH_NETWORKCFG_FCSREMOVE_DEFAULT                0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_FCSREMOVE_DEFAULT                 (_ETH_NETWORKCFG_FCSREMOVE_DEFAULT << 17)            /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_SHIFT                  18                                                   /**< Shift value for ETH_MDCCLKDIV */
#define _ETH_NETWORKCFG_MDCCLKDIV_MASK                   0x1C0000UL                                           /**< Bit mask for ETH_MDCCLKDIV */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY8                 0x00000000UL                                         /**< Mode DIVBY8 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY16                0x00000001UL                                         /**< Mode DIVBY16 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DEFAULT                0x00000002UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY32                0x00000002UL                                         /**< Mode DIVBY32 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY48                0x00000003UL                                         /**< Mode DIVBY48 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY64                0x00000004UL                                         /**< Mode DIVBY64 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY96                0x00000005UL                                         /**< Mode DIVBY96 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY128               0x00000006UL                                         /**< Mode DIVBY128 for ETH_NETWORKCFG */
#define _ETH_NETWORKCFG_MDCCLKDIV_DIVBY224               0x00000007UL                                         /**< Mode DIVBY224 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY8                  (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY8 << 18)             /**< Shifted mode DIVBY8 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY16                 (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY16 << 18)            /**< Shifted mode DIVBY16 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DEFAULT                 (_ETH_NETWORKCFG_MDCCLKDIV_DEFAULT << 18)            /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY32                 (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY32 << 18)            /**< Shifted mode DIVBY32 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY48                 (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY48 << 18)            /**< Shifted mode DIVBY48 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY64                 (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY64 << 18)            /**< Shifted mode DIVBY64 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY96                 (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY96 << 18)            /**< Shifted mode DIVBY96 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY128                (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY128 << 18)           /**< Shifted mode DIVBY128 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_MDCCLKDIV_DIVBY224                (_ETH_NETWORKCFG_MDCCLKDIV_DIVBY224 << 18)           /**< Shifted mode DIVBY224 for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_DISCOPYOFPFRAMES                  (0x1UL << 23)                                        /**< Disable copy of pause frames */
#define _ETH_NETWORKCFG_DISCOPYOFPFRAMES_SHIFT           23                                                   /**< Shift value for ETH_DISCOPYOFPFRAMES */
#define _ETH_NETWORKCFG_DISCOPYOFPFRAMES_MASK            0x800000UL                                           /**< Bit mask for ETH_DISCOPYOFPFRAMES */
#define _ETH_NETWORKCFG_DISCOPYOFPFRAMES_DEFAULT         0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_DISCOPYOFPFRAMES_DEFAULT          (_ETH_NETWORKCFG_DISCOPYOFPFRAMES_DEFAULT << 23)     /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RXCHKSUMOFFLOADEN                 (0x1UL << 24)                                        /**< Receive checksum offload enable */
#define _ETH_NETWORKCFG_RXCHKSUMOFFLOADEN_SHIFT          24                                                   /**< Shift value for ETH_RXCHKSUMOFFLOADEN */
#define _ETH_NETWORKCFG_RXCHKSUMOFFLOADEN_MASK           0x1000000UL                                          /**< Bit mask for ETH_RXCHKSUMOFFLOADEN */
#define _ETH_NETWORKCFG_RXCHKSUMOFFLOADEN_DEFAULT        0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_RXCHKSUMOFFLOADEN_DEFAULT         (_ETH_NETWORKCFG_RXCHKSUMOFFLOADEN_DEFAULT << 24)    /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_ENHALFDUPLEXRX                    (0x1UL << 25)                                        /**< Enable frames to be received in half-duplex mode while transmitting. */
#define _ETH_NETWORKCFG_ENHALFDUPLEXRX_SHIFT             25                                                   /**< Shift value for ETH_ENHALFDUPLEXRX */
#define _ETH_NETWORKCFG_ENHALFDUPLEXRX_MASK              0x2000000UL                                          /**< Bit mask for ETH_ENHALFDUPLEXRX */
#define _ETH_NETWORKCFG_ENHALFDUPLEXRX_DEFAULT           0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_ENHALFDUPLEXRX_DEFAULT            (_ETH_NETWORKCFG_ENHALFDUPLEXRX_DEFAULT << 25)       /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IGNORERXFCS                       (0x1UL << 26)                                        /**< Ignore RX FCS */
#define _ETH_NETWORKCFG_IGNORERXFCS_SHIFT                26                                                   /**< Shift value for ETH_IGNORERXFCS */
#define _ETH_NETWORKCFG_IGNORERXFCS_MASK                 0x4000000UL                                          /**< Bit mask for ETH_IGNORERXFCS */
#define _ETH_NETWORKCFG_IGNORERXFCS_DEFAULT              0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IGNORERXFCS_DEFAULT               (_ETH_NETWORKCFG_IGNORERXFCS_DEFAULT << 26)          /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IPGSTRTCHEN                       (0x1UL << 28)                                        /**< IPG stretch enable */
#define _ETH_NETWORKCFG_IPGSTRTCHEN_SHIFT                28                                                   /**< Shift value for ETH_IPGSTRTCHEN */
#define _ETH_NETWORKCFG_IPGSTRTCHEN_MASK                 0x10000000UL                                         /**< Bit mask for ETH_IPGSTRTCHEN */
#define _ETH_NETWORKCFG_IPGSTRTCHEN_DEFAULT              0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IPGSTRTCHEN_DEFAULT               (_ETH_NETWORKCFG_IPGSTRTCHEN_DEFAULT << 28)          /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_NSPCHANGE                         (0x1UL << 29)                                        /**< Receive bad preamble. */
#define _ETH_NETWORKCFG_NSPCHANGE_SHIFT                  29                                                   /**< Shift value for ETH_NSPCHANGE */
#define _ETH_NETWORKCFG_NSPCHANGE_MASK                   0x20000000UL                                         /**< Bit mask for ETH_NSPCHANGE */
#define _ETH_NETWORKCFG_NSPCHANGE_DEFAULT                0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_NSPCHANGE_DEFAULT                 (_ETH_NETWORKCFG_NSPCHANGE_DEFAULT << 29)            /**< Shifted mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IGNOREIPGRXER                     (0x1UL << 30)                                        /**< Ignore IPG rx_er. */
#define _ETH_NETWORKCFG_IGNOREIPGRXER_SHIFT              30                                                   /**< Shift value for ETH_IGNOREIPGRXER */
#define _ETH_NETWORKCFG_IGNOREIPGRXER_MASK               0x40000000UL                                         /**< Bit mask for ETH_IGNOREIPGRXER */
#define _ETH_NETWORKCFG_IGNOREIPGRXER_DEFAULT            0x00000000UL                                         /**< Mode DEFAULT for ETH_NETWORKCFG */
#define ETH_NETWORKCFG_IGNOREIPGRXER_DEFAULT             (_ETH_NETWORKCFG_IGNOREIPGRXER_DEFAULT << 30)        /**< Shifted mode DEFAULT for ETH_NETWORKCFG */

/* Bit fields for ETH NETWORKSTATUS */
#define _ETH_NETWORKSTATUS_RESETVALUE                    0x00000004UL                                   /**< Default value for ETH_NETWORKSTATUS */
#define _ETH_NETWORKSTATUS_MASK                          0x000000C6UL                                   /**< Mask for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_MDIOIN                         (0x1UL << 1)                                   /**< Returns status of the mdio_in pin. */
#define _ETH_NETWORKSTATUS_MDIOIN_SHIFT                  1                                              /**< Shift value for ETH_MDIOIN */
#define _ETH_NETWORKSTATUS_MDIOIN_MASK                   0x2UL                                          /**< Bit mask for ETH_MDIOIN */
#define _ETH_NETWORKSTATUS_MDIOIN_DEFAULT                0x00000000UL                                   /**< Mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_MDIOIN_DEFAULT                 (_ETH_NETWORKSTATUS_MDIOIN_DEFAULT << 1)       /**< Shifted mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_MANDONE                        (0x1UL << 2)                                   /**< The PHY management logic is idle (i.e. has completed). */
#define _ETH_NETWORKSTATUS_MANDONE_SHIFT                 2                                              /**< Shift value for ETH_MANDONE */
#define _ETH_NETWORKSTATUS_MANDONE_MASK                  0x4UL                                          /**< Bit mask for ETH_MANDONE */
#define _ETH_NETWORKSTATUS_MANDONE_DEFAULT               0x00000001UL                                   /**< Mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_MANDONE_DEFAULT                (_ETH_NETWORKSTATUS_MANDONE_DEFAULT << 2)      /**< Shifted mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_PFCNEGOTIATE                   (0x1UL << 6)                                   /**< Set when PFC Priority Based Pause has been negotiated. */
#define _ETH_NETWORKSTATUS_PFCNEGOTIATE_SHIFT            6                                              /**< Shift value for ETH_PFCNEGOTIATE */
#define _ETH_NETWORKSTATUS_PFCNEGOTIATE_MASK             0x40UL                                         /**< Bit mask for ETH_PFCNEGOTIATE */
#define _ETH_NETWORKSTATUS_PFCNEGOTIATE_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_PFCNEGOTIATE_DEFAULT           (_ETH_NETWORKSTATUS_PFCNEGOTIATE_DEFAULT << 6) /**< Shifted mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_LPIINDICATE                    (0x1UL << 7)                                   /**< LPI Indication */
#define _ETH_NETWORKSTATUS_LPIINDICATE_SHIFT             7                                              /**< Shift value for ETH_LPIINDICATE */
#define _ETH_NETWORKSTATUS_LPIINDICATE_MASK              0x80UL                                         /**< Bit mask for ETH_LPIINDICATE */
#define _ETH_NETWORKSTATUS_LPIINDICATE_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for ETH_NETWORKSTATUS */
#define ETH_NETWORKSTATUS_LPIINDICATE_DEFAULT            (_ETH_NETWORKSTATUS_LPIINDICATE_DEFAULT << 7)  /**< Shifted mode DEFAULT for ETH_NETWORKSTATUS */

/* Bit fields for ETH DMACFG */
#define _ETH_DMACFG_RESETVALUE                           0x00020704UL                                  /**< Default value for ETH_DMACFG */
#define _ETH_DMACFG_MASK                                 0x37FF1F3FUL                                  /**< Mask for ETH_DMACFG */
#define _ETH_DMACFG_AMBABRSTLEN_SHIFT                    0                                             /**< Shift value for ETH_AMBABRSTLEN */
#define _ETH_DMACFG_AMBABRSTLEN_MASK                     0x1FUL                                        /**< Bit mask for ETH_AMBABRSTLEN */
#define _ETH_DMACFG_AMBABRSTLEN_DEFAULT                  0x00000004UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_AMBABRSTLEN_DEFAULT                   (_ETH_DMACFG_AMBABRSTLEN_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_HDRDATASPLITEN                        (0x1UL << 5)                                  /**< Enable header data Splitting. */
#define _ETH_DMACFG_HDRDATASPLITEN_SHIFT                 5                                             /**< Shift value for ETH_HDRDATASPLITEN */
#define _ETH_DMACFG_HDRDATASPLITEN_MASK                  0x20UL                                        /**< Bit mask for ETH_HDRDATASPLITEN */
#define _ETH_DMACFG_HDRDATASPLITEN_DEFAULT               0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_HDRDATASPLITEN_DEFAULT                (_ETH_DMACFG_HDRDATASPLITEN_DEFAULT << 5)     /**< Shifted mode DEFAULT for ETH_DMACFG */
#define _ETH_DMACFG_RXPBUFSIZE_SHIFT                     8                                             /**< Shift value for ETH_RXPBUFSIZE */
#define _ETH_DMACFG_RXPBUFSIZE_MASK                      0x300UL                                       /**< Bit mask for ETH_RXPBUFSIZE */
#define _ETH_DMACFG_RXPBUFSIZE_SIZE0                     0x00000000UL                                  /**< Mode SIZE0 for ETH_DMACFG */
#define _ETH_DMACFG_RXPBUFSIZE_SIZE1                     0x00000001UL                                  /**< Mode SIZE1 for ETH_DMACFG */
#define _ETH_DMACFG_RXPBUFSIZE_SIZE2                     0x00000002UL                                  /**< Mode SIZE2 for ETH_DMACFG */
#define _ETH_DMACFG_RXPBUFSIZE_DEFAULT                   0x00000003UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define _ETH_DMACFG_RXPBUFSIZE_SIZE3                     0x00000003UL                                  /**< Mode SIZE3 for ETH_DMACFG */
#define ETH_DMACFG_RXPBUFSIZE_SIZE0                      (_ETH_DMACFG_RXPBUFSIZE_SIZE0 << 8)           /**< Shifted mode SIZE0 for ETH_DMACFG */
#define ETH_DMACFG_RXPBUFSIZE_SIZE1                      (_ETH_DMACFG_RXPBUFSIZE_SIZE1 << 8)           /**< Shifted mode SIZE1 for ETH_DMACFG */
#define ETH_DMACFG_RXPBUFSIZE_SIZE2                      (_ETH_DMACFG_RXPBUFSIZE_SIZE2 << 8)           /**< Shifted mode SIZE2 for ETH_DMACFG */
#define ETH_DMACFG_RXPBUFSIZE_DEFAULT                    (_ETH_DMACFG_RXPBUFSIZE_DEFAULT << 8)         /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_RXPBUFSIZE_SIZE3                      (_ETH_DMACFG_RXPBUFSIZE_SIZE3 << 8)           /**< Shifted mode SIZE3 for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFSIZE                            (0x1UL << 10)                                 /**< Transmitter packet buffer memory size select. */
#define _ETH_DMACFG_TXPBUFSIZE_SHIFT                     10                                            /**< Shift value for ETH_TXPBUFSIZE */
#define _ETH_DMACFG_TXPBUFSIZE_MASK                      0x400UL                                       /**< Bit mask for ETH_TXPBUFSIZE */
#define _ETH_DMACFG_TXPBUFSIZE_SIZE0                     0x00000000UL                                  /**< Mode SIZE0 for ETH_DMACFG */
#define _ETH_DMACFG_TXPBUFSIZE_DEFAULT                   0x00000001UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define _ETH_DMACFG_TXPBUFSIZE_SIZE1                     0x00000001UL                                  /**< Mode SIZE1 for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFSIZE_SIZE0                      (_ETH_DMACFG_TXPBUFSIZE_SIZE0 << 10)          /**< Shifted mode SIZE0 for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFSIZE_DEFAULT                    (_ETH_DMACFG_TXPBUFSIZE_DEFAULT << 10)        /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFSIZE_SIZE1                      (_ETH_DMACFG_TXPBUFSIZE_SIZE1 << 10)          /**< Shifted mode SIZE1 for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFTCPEN                           (0x1UL << 11)                                 /**< Transmitter IP, TCP and UDP checksum generation offload enable */
#define _ETH_DMACFG_TXPBUFTCPEN_SHIFT                    11                                            /**< Shift value for ETH_TXPBUFTCPEN */
#define _ETH_DMACFG_TXPBUFTCPEN_MASK                     0x800UL                                       /**< Bit mask for ETH_TXPBUFTCPEN */
#define _ETH_DMACFG_TXPBUFTCPEN_DEFAULT                  0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_TXPBUFTCPEN_DEFAULT                   (_ETH_DMACFG_TXPBUFTCPEN_DEFAULT << 11)       /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_INFLASTDBUFSIZEEN                     (0x1UL << 12)                                 /**< Forces the DMA */
#define _ETH_DMACFG_INFLASTDBUFSIZEEN_SHIFT              12                                            /**< Shift value for ETH_INFLASTDBUFSIZEEN */
#define _ETH_DMACFG_INFLASTDBUFSIZEEN_MASK               0x1000UL                                      /**< Bit mask for ETH_INFLASTDBUFSIZEEN */
#define _ETH_DMACFG_INFLASTDBUFSIZEEN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_INFLASTDBUFSIZEEN_DEFAULT             (_ETH_DMACFG_INFLASTDBUFSIZEEN_DEFAULT << 12) /**< Shifted mode DEFAULT for ETH_DMACFG */
#define _ETH_DMACFG_RXBUFSIZE_SHIFT                      16                                            /**< Shift value for ETH_RXBUFSIZE */
#define _ETH_DMACFG_RXBUFSIZE_MASK                       0xFF0000UL                                    /**< Bit mask for ETH_RXBUFSIZE */
#define _ETH_DMACFG_RXBUFSIZE_DEFAULT                    0x00000002UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_RXBUFSIZE_DEFAULT                     (_ETH_DMACFG_RXBUFSIZE_DEFAULT << 16)         /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCDISCARDONERR                       (0x1UL << 24)                                 /**< Auto Discard RX pkts during lack of resource. */
#define _ETH_DMACFG_FRCDISCARDONERR_SHIFT                24                                            /**< Shift value for ETH_FRCDISCARDONERR */
#define _ETH_DMACFG_FRCDISCARDONERR_MASK                 0x1000000UL                                   /**< Bit mask for ETH_FRCDISCARDONERR */
#define _ETH_DMACFG_FRCDISCARDONERR_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCDISCARDONERR_DEFAULT               (_ETH_DMACFG_FRCDISCARDONERR_DEFAULT << 24)   /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCMAXAMBABRSTRX                      (0x1UL << 25)                                 /**< Force max length bursts on RX. */
#define _ETH_DMACFG_FRCMAXAMBABRSTRX_SHIFT               25                                            /**< Shift value for ETH_FRCMAXAMBABRSTRX */
#define _ETH_DMACFG_FRCMAXAMBABRSTRX_MASK                0x2000000UL                                   /**< Bit mask for ETH_FRCMAXAMBABRSTRX */
#define _ETH_DMACFG_FRCMAXAMBABRSTRX_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCMAXAMBABRSTRX_DEFAULT              (_ETH_DMACFG_FRCMAXAMBABRSTRX_DEFAULT << 25)  /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCMAXAMBABRSTTX                      (0x1UL << 26)                                 /**< Force max length bursts on TX. */
#define _ETH_DMACFG_FRCMAXAMBABRSTTX_SHIFT               26                                            /**< Shift value for ETH_FRCMAXAMBABRSTTX */
#define _ETH_DMACFG_FRCMAXAMBABRSTTX_MASK                0x4000000UL                                   /**< Bit mask for ETH_FRCMAXAMBABRSTTX */
#define _ETH_DMACFG_FRCMAXAMBABRSTTX_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_FRCMAXAMBABRSTTX_DEFAULT              (_ETH_DMACFG_FRCMAXAMBABRSTTX_DEFAULT << 26)  /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_RXBDEXTNDMODEEN                       (0x1UL << 28)                                 /**< Enable RX extended BD mode. */
#define _ETH_DMACFG_RXBDEXTNDMODEEN_SHIFT                28                                            /**< Shift value for ETH_RXBDEXTNDMODEEN */
#define _ETH_DMACFG_RXBDEXTNDMODEEN_MASK                 0x10000000UL                                  /**< Bit mask for ETH_RXBDEXTNDMODEEN */
#define _ETH_DMACFG_RXBDEXTNDMODEEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_RXBDEXTNDMODEEN_DEFAULT               (_ETH_DMACFG_RXBDEXTNDMODEEN_DEFAULT << 28)   /**< Shifted mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_TXBDEXTENDMODEEN                      (0x1UL << 29)                                 /**< Enable TX extended BD mode. */
#define _ETH_DMACFG_TXBDEXTENDMODEEN_SHIFT               29                                            /**< Shift value for ETH_TXBDEXTENDMODEEN */
#define _ETH_DMACFG_TXBDEXTENDMODEEN_MASK                0x20000000UL                                  /**< Bit mask for ETH_TXBDEXTENDMODEEN */
#define _ETH_DMACFG_TXBDEXTENDMODEEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for ETH_DMACFG */
#define ETH_DMACFG_TXBDEXTENDMODEEN_DEFAULT              (_ETH_DMACFG_TXBDEXTENDMODEEN_DEFAULT << 29)  /**< Shifted mode DEFAULT for ETH_DMACFG */

/* Bit fields for ETH TXSTATUS */
#define _ETH_TXSTATUS_RESETVALUE                         0x00000000UL                              /**< Default value for ETH_TXSTATUS */
#define _ETH_TXSTATUS_MASK                               0x000001FFUL                              /**< Mask for ETH_TXSTATUS */
#define ETH_TXSTATUS_USEDBITREAD                         (0x1UL << 0)                              /**< Used bit read */
#define _ETH_TXSTATUS_USEDBITREAD_SHIFT                  0                                         /**< Shift value for ETH_USEDBITREAD */
#define _ETH_TXSTATUS_USEDBITREAD_MASK                   0x1UL                                     /**< Bit mask for ETH_USEDBITREAD */
#define _ETH_TXSTATUS_USEDBITREAD_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_USEDBITREAD_DEFAULT                 (_ETH_TXSTATUS_USEDBITREAD_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_COLOCCRD                            (0x1UL << 1)                              /**< Collision occurred */
#define _ETH_TXSTATUS_COLOCCRD_SHIFT                     1                                         /**< Shift value for ETH_COLOCCRD */
#define _ETH_TXSTATUS_COLOCCRD_MASK                      0x2UL                                     /**< Bit mask for ETH_COLOCCRD */
#define _ETH_TXSTATUS_COLOCCRD_DEFAULT                   0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_COLOCCRD_DEFAULT                    (_ETH_TXSTATUS_COLOCCRD_DEFAULT << 1)     /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_RETRYLMTEXCD                        (0x1UL << 2)                              /**< Retry limit exceeded */
#define _ETH_TXSTATUS_RETRYLMTEXCD_SHIFT                 2                                         /**< Shift value for ETH_RETRYLMTEXCD */
#define _ETH_TXSTATUS_RETRYLMTEXCD_MASK                  0x4UL                                     /**< Bit mask for ETH_RETRYLMTEXCD */
#define _ETH_TXSTATUS_RETRYLMTEXCD_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_RETRYLMTEXCD_DEFAULT                (_ETH_TXSTATUS_RETRYLMTEXCD_DEFAULT << 2) /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXGO                                (0x1UL << 3)                              /**< Transmit go */
#define _ETH_TXSTATUS_TXGO_SHIFT                         3                                         /**< Shift value for ETH_TXGO */
#define _ETH_TXSTATUS_TXGO_MASK                          0x8UL                                     /**< Bit mask for ETH_TXGO */
#define _ETH_TXSTATUS_TXGO_DEFAULT                       0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXGO_DEFAULT                        (_ETH_TXSTATUS_TXGO_DEFAULT << 3)         /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_AMBAERR                             (0x1UL << 4)                              /**< Transmit frame corruption due to AMBA (AHB) errors. */
#define _ETH_TXSTATUS_AMBAERR_SHIFT                      4                                         /**< Shift value for ETH_AMBAERR */
#define _ETH_TXSTATUS_AMBAERR_MASK                       0x10UL                                    /**< Bit mask for ETH_AMBAERR */
#define _ETH_TXSTATUS_AMBAERR_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_AMBAERR_DEFAULT                     (_ETH_TXSTATUS_AMBAERR_DEFAULT << 4)      /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXCMPLT                             (0x1UL << 5)                              /**< Transmit complete */
#define _ETH_TXSTATUS_TXCMPLT_SHIFT                      5                                         /**< Shift value for ETH_TXCMPLT */
#define _ETH_TXSTATUS_TXCMPLT_MASK                       0x20UL                                    /**< Bit mask for ETH_TXCMPLT */
#define _ETH_TXSTATUS_TXCMPLT_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXCMPLT_DEFAULT                     (_ETH_TXSTATUS_TXCMPLT_DEFAULT << 5)      /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXUNDERRUN                          (0x1UL << 6)                              /**< Transmit under run */
#define _ETH_TXSTATUS_TXUNDERRUN_SHIFT                   6                                         /**< Shift value for ETH_TXUNDERRUN */
#define _ETH_TXSTATUS_TXUNDERRUN_MASK                    0x40UL                                    /**< Bit mask for ETH_TXUNDERRUN */
#define _ETH_TXSTATUS_TXUNDERRUN_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_TXUNDERRUN_DEFAULT                  (_ETH_TXSTATUS_TXUNDERRUN_DEFAULT << 6)   /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_LATECOLOCCRD                        (0x1UL << 7)                              /**< Late collision occurred */
#define _ETH_TXSTATUS_LATECOLOCCRD_SHIFT                 7                                         /**< Shift value for ETH_LATECOLOCCRD */
#define _ETH_TXSTATUS_LATECOLOCCRD_MASK                  0x80UL                                    /**< Bit mask for ETH_LATECOLOCCRD */
#define _ETH_TXSTATUS_LATECOLOCCRD_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_LATECOLOCCRD_DEFAULT                (_ETH_TXSTATUS_LATECOLOCCRD_DEFAULT << 7) /**< Shifted mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_RESPNOTOK                           (0x1UL << 8)                              /**< bresp/hresp not OK */
#define _ETH_TXSTATUS_RESPNOTOK_SHIFT                    8                                         /**< Shift value for ETH_RESPNOTOK */
#define _ETH_TXSTATUS_RESPNOTOK_MASK                     0x100UL                                   /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_TXSTATUS_RESPNOTOK_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for ETH_TXSTATUS */
#define ETH_TXSTATUS_RESPNOTOK_DEFAULT                   (_ETH_TXSTATUS_RESPNOTOK_DEFAULT << 8)    /**< Shifted mode DEFAULT for ETH_TXSTATUS */

/* Bit fields for ETH RXQPTR */
#define _ETH_RXQPTR_RESETVALUE                           0x00000000UL                         /**< Default value for ETH_RXQPTR */
#define _ETH_RXQPTR_MASK                                 0xFFFFFFFCUL                         /**< Mask for ETH_RXQPTR */
#define _ETH_RXQPTR_DMARXQPTR_SHIFT                      2                                    /**< Shift value for ETH_DMARXQPTR */
#define _ETH_RXQPTR_DMARXQPTR_MASK                       0xFFFFFFFCUL                         /**< Bit mask for ETH_DMARXQPTR */
#define _ETH_RXQPTR_DMARXQPTR_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_RXQPTR */
#define ETH_RXQPTR_DMARXQPTR_DEFAULT                     (_ETH_RXQPTR_DMARXQPTR_DEFAULT << 2) /**< Shifted mode DEFAULT for ETH_RXQPTR */

/* Bit fields for ETH TXQPTR */
#define _ETH_TXQPTR_RESETVALUE                           0x00000000UL                         /**< Default value for ETH_TXQPTR */
#define _ETH_TXQPTR_MASK                                 0xFFFFFFFCUL                         /**< Mask for ETH_TXQPTR */
#define _ETH_TXQPTR_DMATXQPTR_SHIFT                      2                                    /**< Shift value for ETH_DMATXQPTR */
#define _ETH_TXQPTR_DMATXQPTR_MASK                       0xFFFFFFFCUL                         /**< Bit mask for ETH_DMATXQPTR */
#define _ETH_TXQPTR_DMATXQPTR_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_TXQPTR */
#define ETH_TXQPTR_DMATXQPTR_DEFAULT                     (_ETH_TXQPTR_DMATXQPTR_DEFAULT << 2) /**< Shifted mode DEFAULT for ETH_TXQPTR */

/* Bit fields for ETH RXSTATUS */
#define _ETH_RXSTATUS_RESETVALUE                         0x00000000UL                              /**< Default value for ETH_RXSTATUS */
#define _ETH_RXSTATUS_MASK                               0x0000000FUL                              /**< Mask for ETH_RXSTATUS */
#define ETH_RXSTATUS_BUFFNOTAVAIL                        (0x1UL << 0)                              /**< Buffer not available */
#define _ETH_RXSTATUS_BUFFNOTAVAIL_SHIFT                 0                                         /**< Shift value for ETH_BUFFNOTAVAIL */
#define _ETH_RXSTATUS_BUFFNOTAVAIL_MASK                  0x1UL                                     /**< Bit mask for ETH_BUFFNOTAVAIL */
#define _ETH_RXSTATUS_BUFFNOTAVAIL_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_BUFFNOTAVAIL_DEFAULT                (_ETH_RXSTATUS_BUFFNOTAVAIL_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_FRMRX                               (0x1UL << 1)                              /**< Frame received */
#define _ETH_RXSTATUS_FRMRX_SHIFT                        1                                         /**< Shift value for ETH_FRMRX */
#define _ETH_RXSTATUS_FRMRX_MASK                         0x2UL                                     /**< Bit mask for ETH_FRMRX */
#define _ETH_RXSTATUS_FRMRX_DEFAULT                      0x00000000UL                              /**< Mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_FRMRX_DEFAULT                       (_ETH_RXSTATUS_FRMRX_DEFAULT << 1)        /**< Shifted mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_RXOVERRUN                           (0x1UL << 2)                              /**< Receive overrun */
#define _ETH_RXSTATUS_RXOVERRUN_SHIFT                    2                                         /**< Shift value for ETH_RXOVERRUN */
#define _ETH_RXSTATUS_RXOVERRUN_MASK                     0x4UL                                     /**< Bit mask for ETH_RXOVERRUN */
#define _ETH_RXSTATUS_RXOVERRUN_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_RXOVERRUN_DEFAULT                   (_ETH_RXSTATUS_RXOVERRUN_DEFAULT << 2)    /**< Shifted mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_RESPNOTOK                           (0x1UL << 3)                              /**< bresp/hresp not OK */
#define _ETH_RXSTATUS_RESPNOTOK_SHIFT                    3                                         /**< Shift value for ETH_RESPNOTOK */
#define _ETH_RXSTATUS_RESPNOTOK_MASK                     0x8UL                                     /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_RXSTATUS_RESPNOTOK_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for ETH_RXSTATUS */
#define ETH_RXSTATUS_RESPNOTOK_DEFAULT                   (_ETH_RXSTATUS_RESPNOTOK_DEFAULT << 3)    /**< Shifted mode DEFAULT for ETH_RXSTATUS */

/* Bit fields for ETH IFCR */
#define _ETH_IFCR_RESETVALUE                             0x00000000UL                               /**< Default value for ETH_IFCR */
#define _ETH_IFCR_MASK                                   0x3FFC7CFFUL                               /**< Mask for ETH_IFCR */
#define ETH_IFCR_MNGMNTDONE                              (0x1UL << 0)                               /**< Management frame sent */
#define _ETH_IFCR_MNGMNTDONE_SHIFT                       0                                          /**< Shift value for ETH_MNGMNTDONE */
#define _ETH_IFCR_MNGMNTDONE_MASK                        0x1UL                                      /**< Bit mask for ETH_MNGMNTDONE */
#define _ETH_IFCR_MNGMNTDONE_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_MNGMNTDONE_DEFAULT                      (_ETH_IFCR_MNGMNTDONE_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXCMPLT                                 (0x1UL << 1)                               /**< Receive complete */
#define _ETH_IFCR_RXCMPLT_SHIFT                          1                                          /**< Shift value for ETH_RXCMPLT */
#define _ETH_IFCR_RXCMPLT_MASK                           0x2UL                                      /**< Bit mask for ETH_RXCMPLT */
#define _ETH_IFCR_RXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXCMPLT_DEFAULT                         (_ETH_IFCR_RXCMPLT_DEFAULT << 1)           /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXUSEDBITREAD                           (0x1UL << 2)                               /**< RX used bit read */
#define _ETH_IFCR_RXUSEDBITREAD_SHIFT                    2                                          /**< Shift value for ETH_RXUSEDBITREAD */
#define _ETH_IFCR_RXUSEDBITREAD_MASK                     0x4UL                                      /**< Bit mask for ETH_RXUSEDBITREAD */
#define _ETH_IFCR_RXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXUSEDBITREAD_DEFAULT                   (_ETH_IFCR_RXUSEDBITREAD_DEFAULT << 2)     /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXUSEDBITREAD                           (0x1UL << 3)                               /**< TX used bit read */
#define _ETH_IFCR_TXUSEDBITREAD_SHIFT                    3                                          /**< Shift value for ETH_TXUSEDBITREAD */
#define _ETH_IFCR_TXUSEDBITREAD_MASK                     0x8UL                                      /**< Bit mask for ETH_TXUSEDBITREAD */
#define _ETH_IFCR_TXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXUSEDBITREAD_DEFAULT                   (_ETH_IFCR_TXUSEDBITREAD_DEFAULT << 3)     /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXUNDERRUN                              (0x1UL << 4)                               /**< Transmit under run */
#define _ETH_IFCR_TXUNDERRUN_SHIFT                       4                                          /**< Shift value for ETH_TXUNDERRUN */
#define _ETH_IFCR_TXUNDERRUN_MASK                        0x10UL                                     /**< Bit mask for ETH_TXUNDERRUN */
#define _ETH_IFCR_TXUNDERRUN_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXUNDERRUN_DEFAULT                      (_ETH_IFCR_TXUNDERRUN_DEFAULT << 4)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RTRYLMTORLATECOL                        (0x1UL << 5)                               /**< Retry limit exceeded or late collision */
#define _ETH_IFCR_RTRYLMTORLATECOL_SHIFT                 5                                          /**< Shift value for ETH_RTRYLMTORLATECOL */
#define _ETH_IFCR_RTRYLMTORLATECOL_MASK                  0x20UL                                     /**< Bit mask for ETH_RTRYLMTORLATECOL */
#define _ETH_IFCR_RTRYLMTORLATECOL_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RTRYLMTORLATECOL_DEFAULT                (_ETH_IFCR_RTRYLMTORLATECOL_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_AMBAERR                                 (0x1UL << 6)                               /**< Transmit frame corruption due to AMBA (AHB) error. */
#define _ETH_IFCR_AMBAERR_SHIFT                          6                                          /**< Shift value for ETH_AMBAERR */
#define _ETH_IFCR_AMBAERR_MASK                           0x40UL                                     /**< Bit mask for ETH_AMBAERR */
#define _ETH_IFCR_AMBAERR_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_AMBAERR_DEFAULT                         (_ETH_IFCR_AMBAERR_DEFAULT << 6)           /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXCMPLT                                 (0x1UL << 7)                               /**< Transmit complete */
#define _ETH_IFCR_TXCMPLT_SHIFT                          7                                          /**< Shift value for ETH_TXCMPLT */
#define _ETH_IFCR_TXCMPLT_MASK                           0x80UL                                     /**< Bit mask for ETH_TXCMPLT */
#define _ETH_IFCR_TXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TXCMPLT_DEFAULT                         (_ETH_IFCR_TXCMPLT_DEFAULT << 7)           /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXOVERRUN                               (0x1UL << 10)                              /**< Receive overrun */
#define _ETH_IFCR_RXOVERRUN_SHIFT                        10                                         /**< Shift value for ETH_RXOVERRUN */
#define _ETH_IFCR_RXOVERRUN_MASK                         0x400UL                                    /**< Bit mask for ETH_RXOVERRUN */
#define _ETH_IFCR_RXOVERRUN_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXOVERRUN_DEFAULT                       (_ETH_IFCR_RXOVERRUN_DEFAULT << 10)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RESPNOTOK                               (0x1UL << 11)                              /**< Hresp not OK */
#define _ETH_IFCR_RESPNOTOK_SHIFT                        11                                         /**< Shift value for ETH_RESPNOTOK */
#define _ETH_IFCR_RESPNOTOK_MASK                         0x800UL                                    /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_IFCR_RESPNOTOK_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RESPNOTOK_DEFAULT                       (_ETH_IFCR_RESPNOTOK_DEFAULT << 11)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_NONZEROPFRMQUANT                        (0x1UL << 12)                              /**< Pause frame with non-zero pause quantum received */
#define _ETH_IFCR_NONZEROPFRMQUANT_SHIFT                 12                                         /**< Shift value for ETH_NONZEROPFRMQUANT */
#define _ETH_IFCR_NONZEROPFRMQUANT_MASK                  0x1000UL                                   /**< Bit mask for ETH_NONZEROPFRMQUANT */
#define _ETH_IFCR_NONZEROPFRMQUANT_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_NONZEROPFRMQUANT_DEFAULT                (_ETH_IFCR_NONZEROPFRMQUANT_DEFAULT << 12) /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PAUSETIMEZERO                           (0x1UL << 13)                              /**< Pause Time zero */
#define _ETH_IFCR_PAUSETIMEZERO_SHIFT                    13                                         /**< Shift value for ETH_PAUSETIMEZERO */
#define _ETH_IFCR_PAUSETIMEZERO_MASK                     0x2000UL                                   /**< Bit mask for ETH_PAUSETIMEZERO */
#define _ETH_IFCR_PAUSETIMEZERO_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PAUSETIMEZERO_DEFAULT                   (_ETH_IFCR_PAUSETIMEZERO_DEFAULT << 13)    /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PFRMTX                                  (0x1UL << 14)                              /**< Pause frame transmitted  */
#define _ETH_IFCR_PFRMTX_SHIFT                           14                                         /**< Shift value for ETH_PFRMTX */
#define _ETH_IFCR_PFRMTX_MASK                            0x4000UL                                   /**< Bit mask for ETH_PFRMTX */
#define _ETH_IFCR_PFRMTX_DEFAULT                         0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PFRMTX_DEFAULT                          (_ETH_IFCR_PFRMTX_DEFAULT << 14)           /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPDLYREQFRMRX                          (0x1UL << 18)                              /**< PTP delay_req frame received */
#define _ETH_IFCR_PTPDLYREQFRMRX_SHIFT                   18                                         /**< Shift value for ETH_PTPDLYREQFRMRX */
#define _ETH_IFCR_PTPDLYREQFRMRX_MASK                    0x40000UL                                  /**< Bit mask for ETH_PTPDLYREQFRMRX */
#define _ETH_IFCR_PTPDLYREQFRMRX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPDLYREQFRMRX_DEFAULT                  (_ETH_IFCR_PTPDLYREQFRMRX_DEFAULT << 18)   /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPSYNCFRMRX                            (0x1UL << 19)                              /**< PTP sync frame received */
#define _ETH_IFCR_PTPSYNCFRMRX_SHIFT                     19                                         /**< Shift value for ETH_PTPSYNCFRMRX */
#define _ETH_IFCR_PTPSYNCFRMRX_MASK                      0x80000UL                                  /**< Bit mask for ETH_PTPSYNCFRMRX */
#define _ETH_IFCR_PTPSYNCFRMRX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPSYNCFRMRX_DEFAULT                    (_ETH_IFCR_PTPSYNCFRMRX_DEFAULT << 19)     /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPDLYREQFRMTX                          (0x1UL << 20)                              /**< PTP delay_req frame transmitted  */
#define _ETH_IFCR_PTPDLYREQFRMTX_SHIFT                   20                                         /**< Shift value for ETH_PTPDLYREQFRMTX */
#define _ETH_IFCR_PTPDLYREQFRMTX_MASK                    0x100000UL                                 /**< Bit mask for ETH_PTPDLYREQFRMTX */
#define _ETH_IFCR_PTPDLYREQFRMTX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPDLYREQFRMTX_DEFAULT                  (_ETH_IFCR_PTPDLYREQFRMTX_DEFAULT << 20)   /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPSYNCFRMTX                            (0x1UL << 21)                              /**< PTP sync frame transmitted */
#define _ETH_IFCR_PTPSYNCFRMTX_SHIFT                     21                                         /**< Shift value for ETH_PTPSYNCFRMTX */
#define _ETH_IFCR_PTPSYNCFRMTX_MASK                      0x200000UL                                 /**< Bit mask for ETH_PTPSYNCFRMTX */
#define _ETH_IFCR_PTPSYNCFRMTX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPSYNCFRMTX_DEFAULT                    (_ETH_IFCR_PTPSYNCFRMTX_DEFAULT << 21)     /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYREQFRMRX                         (0x1UL << 22)                              /**< PTP pdelay_req frame received */
#define _ETH_IFCR_PTPPDLYREQFRMRX_SHIFT                  22                                         /**< Shift value for ETH_PTPPDLYREQFRMRX */
#define _ETH_IFCR_PTPPDLYREQFRMRX_MASK                   0x400000UL                                 /**< Bit mask for ETH_PTPPDLYREQFRMRX */
#define _ETH_IFCR_PTPPDLYREQFRMRX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYREQFRMRX_DEFAULT                 (_ETH_IFCR_PTPPDLYREQFRMRX_DEFAULT << 22)  /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYRESPFRMRX                        (0x1UL << 23)                              /**< PTP pdelay_resp frame received  */
#define _ETH_IFCR_PTPPDLYRESPFRMRX_SHIFT                 23                                         /**< Shift value for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IFCR_PTPPDLYRESPFRMRX_MASK                  0x800000UL                                 /**< Bit mask for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IFCR_PTPPDLYRESPFRMRX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYRESPFRMRX_DEFAULT                (_ETH_IFCR_PTPPDLYRESPFRMRX_DEFAULT << 23) /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYREQFRMTX                         (0x1UL << 24)                              /**< PTP pdelay_req frame transmitted  */
#define _ETH_IFCR_PTPPDLYREQFRMTX_SHIFT                  24                                         /**< Shift value for ETH_PTPPDLYREQFRMTX */
#define _ETH_IFCR_PTPPDLYREQFRMTX_MASK                   0x1000000UL                                /**< Bit mask for ETH_PTPPDLYREQFRMTX */
#define _ETH_IFCR_PTPPDLYREQFRMTX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYREQFRMTX_DEFAULT                 (_ETH_IFCR_PTPPDLYREQFRMTX_DEFAULT << 24)  /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYRESPFRMTX                        (0x1UL << 25)                              /**< PTP pdelay_resp frame transmitted */
#define _ETH_IFCR_PTPPDLYRESPFRMTX_SHIFT                 25                                         /**< Shift value for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IFCR_PTPPDLYRESPFRMTX_MASK                  0x2000000UL                                /**< Bit mask for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IFCR_PTPPDLYRESPFRMTX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_PTPPDLYRESPFRMTX_DEFAULT                (_ETH_IFCR_PTPPDLYRESPFRMTX_DEFAULT << 25) /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TSUSECREGINCR                           (0x1UL << 26)                              /**< TSU seconds register increment */
#define _ETH_IFCR_TSUSECREGINCR_SHIFT                    26                                         /**< Shift value for ETH_TSUSECREGINCR */
#define _ETH_IFCR_TSUSECREGINCR_MASK                     0x4000000UL                                /**< Bit mask for ETH_TSUSECREGINCR */
#define _ETH_IFCR_TSUSECREGINCR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TSUSECREGINCR_DEFAULT                   (_ETH_IFCR_TSUSECREGINCR_DEFAULT << 26)    /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXLPIINDC                               (0x1UL << 27)                              /**< Receive LPI indication status bit change */
#define _ETH_IFCR_RXLPIINDC_SHIFT                        27                                         /**< Shift value for ETH_RXLPIINDC */
#define _ETH_IFCR_RXLPIINDC_MASK                         0x8000000UL                                /**< Bit mask for ETH_RXLPIINDC */
#define _ETH_IFCR_RXLPIINDC_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_RXLPIINDC_DEFAULT                       (_ETH_IFCR_RXLPIINDC_DEFAULT << 27)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_WOLEVNTRX                               (0x1UL << 28)                              /**< WOL event received interrupt. */
#define _ETH_IFCR_WOLEVNTRX_SHIFT                        28                                         /**< Shift value for ETH_WOLEVNTRX */
#define _ETH_IFCR_WOLEVNTRX_MASK                         0x10000000UL                               /**< Bit mask for ETH_WOLEVNTRX */
#define _ETH_IFCR_WOLEVNTRX_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_WOLEVNTRX_DEFAULT                       (_ETH_IFCR_WOLEVNTRX_DEFAULT << 28)        /**< Shifted mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TSUTIMERCOMP                            (0x1UL << 29)                              /**< TSU timer comparison interrupt. */
#define _ETH_IFCR_TSUTIMERCOMP_SHIFT                     29                                         /**< Shift value for ETH_TSUTIMERCOMP */
#define _ETH_IFCR_TSUTIMERCOMP_MASK                      0x20000000UL                               /**< Bit mask for ETH_TSUTIMERCOMP */
#define _ETH_IFCR_TSUTIMERCOMP_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IFCR */
#define ETH_IFCR_TSUTIMERCOMP_DEFAULT                    (_ETH_IFCR_TSUTIMERCOMP_DEFAULT << 29)     /**< Shifted mode DEFAULT for ETH_IFCR */

/* Bit fields for ETH IENS */
#define _ETH_IENS_RESETVALUE                             0x00000000UL                               /**< Default value for ETH_IENS */
#define _ETH_IENS_MASK                                   0x3FFC7CFFUL                               /**< Mask for ETH_IENS */
#define ETH_IENS_MNGMNTDONE                              (0x1UL << 0)                               /**< Enable management done interrupt */
#define _ETH_IENS_MNGMNTDONE_SHIFT                       0                                          /**< Shift value for ETH_MNGMNTDONE */
#define _ETH_IENS_MNGMNTDONE_MASK                        0x1UL                                      /**< Bit mask for ETH_MNGMNTDONE */
#define _ETH_IENS_MNGMNTDONE_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_MNGMNTDONE_DEFAULT                      (_ETH_IENS_MNGMNTDONE_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXCMPLT                                 (0x1UL << 1)                               /**< Enable receive complete interrupt */
#define _ETH_IENS_RXCMPLT_SHIFT                          1                                          /**< Shift value for ETH_RXCMPLT */
#define _ETH_IENS_RXCMPLT_MASK                           0x2UL                                      /**< Bit mask for ETH_RXCMPLT */
#define _ETH_IENS_RXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXCMPLT_DEFAULT                         (_ETH_IENS_RXCMPLT_DEFAULT << 1)           /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXUSEDBITREAD                           (0x1UL << 2)                               /**< Enable receive used bit read interrupt */
#define _ETH_IENS_RXUSEDBITREAD_SHIFT                    2                                          /**< Shift value for ETH_RXUSEDBITREAD */
#define _ETH_IENS_RXUSEDBITREAD_MASK                     0x4UL                                      /**< Bit mask for ETH_RXUSEDBITREAD */
#define _ETH_IENS_RXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXUSEDBITREAD_DEFAULT                   (_ETH_IENS_RXUSEDBITREAD_DEFAULT << 2)     /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXUSEDBITREAD                           (0x1UL << 3)                               /**< Enable transmit used bit read interrupt */
#define _ETH_IENS_TXUSEDBITREAD_SHIFT                    3                                          /**< Shift value for ETH_TXUSEDBITREAD */
#define _ETH_IENS_TXUSEDBITREAD_MASK                     0x8UL                                      /**< Bit mask for ETH_TXUSEDBITREAD */
#define _ETH_IENS_TXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXUSEDBITREAD_DEFAULT                   (_ETH_IENS_TXUSEDBITREAD_DEFAULT << 3)     /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXUNDERRUN                              (0x1UL << 4)                               /**< Enable transmit buffer under run interrupt */
#define _ETH_IENS_TXUNDERRUN_SHIFT                       4                                          /**< Shift value for ETH_TXUNDERRUN */
#define _ETH_IENS_TXUNDERRUN_MASK                        0x10UL                                     /**< Bit mask for ETH_TXUNDERRUN */
#define _ETH_IENS_TXUNDERRUN_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXUNDERRUN_DEFAULT                      (_ETH_IENS_TXUNDERRUN_DEFAULT << 4)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RTRYLMTORLATECOL                        (0x1UL << 5)                               /**< Enable retry limit exceeded or late collision interrupt */
#define _ETH_IENS_RTRYLMTORLATECOL_SHIFT                 5                                          /**< Shift value for ETH_RTRYLMTORLATECOL */
#define _ETH_IENS_RTRYLMTORLATECOL_MASK                  0x20UL                                     /**< Bit mask for ETH_RTRYLMTORLATECOL */
#define _ETH_IENS_RTRYLMTORLATECOL_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RTRYLMTORLATECOL_DEFAULT                (_ETH_IENS_RTRYLMTORLATECOL_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_AMBAERR                                 (0x1UL << 6)                               /**< Enable transmit frame corruption due to AMBA (AHB) error interrupt */
#define _ETH_IENS_AMBAERR_SHIFT                          6                                          /**< Shift value for ETH_AMBAERR */
#define _ETH_IENS_AMBAERR_MASK                           0x40UL                                     /**< Bit mask for ETH_AMBAERR */
#define _ETH_IENS_AMBAERR_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_AMBAERR_DEFAULT                         (_ETH_IENS_AMBAERR_DEFAULT << 6)           /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXCMPLT                                 (0x1UL << 7)                               /**< Enable transmit complete interrupt */
#define _ETH_IENS_TXCMPLT_SHIFT                          7                                          /**< Shift value for ETH_TXCMPLT */
#define _ETH_IENS_TXCMPLT_MASK                           0x80UL                                     /**< Bit mask for ETH_TXCMPLT */
#define _ETH_IENS_TXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_TXCMPLT_DEFAULT                         (_ETH_IENS_TXCMPLT_DEFAULT << 7)           /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXOVERRUN                               (0x1UL << 10)                              /**< Enable receive overrun interrupt */
#define _ETH_IENS_RXOVERRUN_SHIFT                        10                                         /**< Shift value for ETH_RXOVERRUN */
#define _ETH_IENS_RXOVERRUN_MASK                         0x400UL                                    /**< Bit mask for ETH_RXOVERRUN */
#define _ETH_IENS_RXOVERRUN_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXOVERRUN_DEFAULT                       (_ETH_IENS_RXOVERRUN_DEFAULT << 10)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RESPNOTOK                               (0x1UL << 11)                              /**< Enable bresp/hresp not OK interrupt */
#define _ETH_IENS_RESPNOTOK_SHIFT                        11                                         /**< Shift value for ETH_RESPNOTOK */
#define _ETH_IENS_RESPNOTOK_MASK                         0x800UL                                    /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_IENS_RESPNOTOK_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RESPNOTOK_DEFAULT                       (_ETH_IENS_RESPNOTOK_DEFAULT << 11)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_NONZEROPFRMQUANT                        (0x1UL << 12)                              /**< Enable pause frame with non-zero pause quantum interrupt */
#define _ETH_IENS_NONZEROPFRMQUANT_SHIFT                 12                                         /**< Shift value for ETH_NONZEROPFRMQUANT */
#define _ETH_IENS_NONZEROPFRMQUANT_MASK                  0x1000UL                                   /**< Bit mask for ETH_NONZEROPFRMQUANT */
#define _ETH_IENS_NONZEROPFRMQUANT_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_NONZEROPFRMQUANT_DEFAULT                (_ETH_IENS_NONZEROPFRMQUANT_DEFAULT << 12) /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PAUSETIMEZERO                           (0x1UL << 13)                              /**< Enable pause time zero interrupt */
#define _ETH_IENS_PAUSETIMEZERO_SHIFT                    13                                         /**< Shift value for ETH_PAUSETIMEZERO */
#define _ETH_IENS_PAUSETIMEZERO_MASK                     0x2000UL                                   /**< Bit mask for ETH_PAUSETIMEZERO */
#define _ETH_IENS_PAUSETIMEZERO_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PAUSETIMEZERO_DEFAULT                   (_ETH_IENS_PAUSETIMEZERO_DEFAULT << 13)    /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PFRMTX                                  (0x1UL << 14)                              /**< Enable pause frame transmitted interrupt */
#define _ETH_IENS_PFRMTX_SHIFT                           14                                         /**< Shift value for ETH_PFRMTX */
#define _ETH_IENS_PFRMTX_MASK                            0x4000UL                                   /**< Bit mask for ETH_PFRMTX */
#define _ETH_IENS_PFRMTX_DEFAULT                         0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PFRMTX_DEFAULT                          (_ETH_IENS_PFRMTX_DEFAULT << 14)           /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPDLYREQFRMRX                          (0x1UL << 18)                              /**< Enable PTP delay_req frame received interrupt */
#define _ETH_IENS_PTPDLYREQFRMRX_SHIFT                   18                                         /**< Shift value for ETH_PTPDLYREQFRMRX */
#define _ETH_IENS_PTPDLYREQFRMRX_MASK                    0x40000UL                                  /**< Bit mask for ETH_PTPDLYREQFRMRX */
#define _ETH_IENS_PTPDLYREQFRMRX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPDLYREQFRMRX_DEFAULT                  (_ETH_IENS_PTPDLYREQFRMRX_DEFAULT << 18)   /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPSYNCFRMRX                            (0x1UL << 19)                              /**< Enable PTP sync frame received interrupt */
#define _ETH_IENS_PTPSYNCFRMRX_SHIFT                     19                                         /**< Shift value for ETH_PTPSYNCFRMRX */
#define _ETH_IENS_PTPSYNCFRMRX_MASK                      0x80000UL                                  /**< Bit mask for ETH_PTPSYNCFRMRX */
#define _ETH_IENS_PTPSYNCFRMRX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPSYNCFRMRX_DEFAULT                    (_ETH_IENS_PTPSYNCFRMRX_DEFAULT << 19)     /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPDLYREQFRMTX                          (0x1UL << 20)                              /**< Enable PTP delay_req frame transmitted interrupt  */
#define _ETH_IENS_PTPDLYREQFRMTX_SHIFT                   20                                         /**< Shift value for ETH_PTPDLYREQFRMTX */
#define _ETH_IENS_PTPDLYREQFRMTX_MASK                    0x100000UL                                 /**< Bit mask for ETH_PTPDLYREQFRMTX */
#define _ETH_IENS_PTPDLYREQFRMTX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPDLYREQFRMTX_DEFAULT                  (_ETH_IENS_PTPDLYREQFRMTX_DEFAULT << 20)   /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPSYNCFRMTX                            (0x1UL << 21)                              /**< Enable PTP sync frame transmitted interrupt  */
#define _ETH_IENS_PTPSYNCFRMTX_SHIFT                     21                                         /**< Shift value for ETH_PTPSYNCFRMTX */
#define _ETH_IENS_PTPSYNCFRMTX_MASK                      0x200000UL                                 /**< Bit mask for ETH_PTPSYNCFRMTX */
#define _ETH_IENS_PTPSYNCFRMTX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPSYNCFRMTX_DEFAULT                    (_ETH_IENS_PTPSYNCFRMTX_DEFAULT << 21)     /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYREQFRMRX                         (0x1UL << 22)                              /**< Enable PTP pdelay_req frame received interrupt */
#define _ETH_IENS_PTPPDLYREQFRMRX_SHIFT                  22                                         /**< Shift value for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENS_PTPPDLYREQFRMRX_MASK                   0x400000UL                                 /**< Bit mask for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENS_PTPPDLYREQFRMRX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYREQFRMRX_DEFAULT                 (_ETH_IENS_PTPPDLYREQFRMRX_DEFAULT << 22)  /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYRESPFRMRX                        (0x1UL << 23)                              /**< Enable PTP pdelay_resp frame received interrupt */
#define _ETH_IENS_PTPPDLYRESPFRMRX_SHIFT                 23                                         /**< Shift value for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENS_PTPPDLYRESPFRMRX_MASK                  0x800000UL                                 /**< Bit mask for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENS_PTPPDLYRESPFRMRX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYRESPFRMRX_DEFAULT                (_ETH_IENS_PTPPDLYRESPFRMRX_DEFAULT << 23) /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYREQFRMTX                         (0x1UL << 24)                              /**< Enable PTP pdelay_req frame transmitted interrupt */
#define _ETH_IENS_PTPPDLYREQFRMTX_SHIFT                  24                                         /**< Shift value for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENS_PTPPDLYREQFRMTX_MASK                   0x1000000UL                                /**< Bit mask for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENS_PTPPDLYREQFRMTX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYREQFRMTX_DEFAULT                 (_ETH_IENS_PTPPDLYREQFRMTX_DEFAULT << 24)  /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYRESPFRMTX                        (0x1UL << 25)                              /**< Enable PTP pdelay_resp frame transmitted interrupt */
#define _ETH_IENS_PTPPDLYRESPFRMTX_SHIFT                 25                                         /**< Shift value for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENS_PTPPDLYRESPFRMTX_MASK                  0x2000000UL                                /**< Bit mask for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENS_PTPPDLYRESPFRMTX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_PTPPDLYRESPFRMTX_DEFAULT                (_ETH_IENS_PTPPDLYRESPFRMTX_DEFAULT << 25) /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_TSUSECREGINCR                           (0x1UL << 26)                              /**< Enable TSU seconds register increment interrupt */
#define _ETH_IENS_TSUSECREGINCR_SHIFT                    26                                         /**< Shift value for ETH_TSUSECREGINCR */
#define _ETH_IENS_TSUSECREGINCR_MASK                     0x4000000UL                                /**< Bit mask for ETH_TSUSECREGINCR */
#define _ETH_IENS_TSUSECREGINCR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_TSUSECREGINCR_DEFAULT                   (_ETH_IENS_TSUSECREGINCR_DEFAULT << 26)    /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXLPIINDC                               (0x1UL << 27)                              /**< Enable RX LPI indication interrupt */
#define _ETH_IENS_RXLPIINDC_SHIFT                        27                                         /**< Shift value for ETH_RXLPIINDC */
#define _ETH_IENS_RXLPIINDC_MASK                         0x8000000UL                                /**< Bit mask for ETH_RXLPIINDC */
#define _ETH_IENS_RXLPIINDC_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_RXLPIINDC_DEFAULT                       (_ETH_IENS_RXLPIINDC_DEFAULT << 27)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_WOLEVNTRX                               (0x1UL << 28)                              /**< Enable WOL event received interrupt */
#define _ETH_IENS_WOLEVNTRX_SHIFT                        28                                         /**< Shift value for ETH_WOLEVNTRX */
#define _ETH_IENS_WOLEVNTRX_MASK                         0x10000000UL                               /**< Bit mask for ETH_WOLEVNTRX */
#define _ETH_IENS_WOLEVNTRX_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_WOLEVNTRX_DEFAULT                       (_ETH_IENS_WOLEVNTRX_DEFAULT << 28)        /**< Shifted mode DEFAULT for ETH_IENS */
#define ETH_IENS_TSUTIMERCOMP                            (0x1UL << 29)                              /**< Enable TSU timer comparison interrupt. */
#define _ETH_IENS_TSUTIMERCOMP_SHIFT                     29                                         /**< Shift value for ETH_TSUTIMERCOMP */
#define _ETH_IENS_TSUTIMERCOMP_MASK                      0x20000000UL                               /**< Bit mask for ETH_TSUTIMERCOMP */
#define _ETH_IENS_TSUTIMERCOMP_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENS */
#define ETH_IENS_TSUTIMERCOMP_DEFAULT                    (_ETH_IENS_TSUTIMERCOMP_DEFAULT << 29)     /**< Shifted mode DEFAULT for ETH_IENS */

/* Bit fields for ETH IENC */
#define _ETH_IENC_RESETVALUE                             0x00000000UL                               /**< Default value for ETH_IENC */
#define _ETH_IENC_MASK                                   0x3FFC7CFFUL                               /**< Mask for ETH_IENC */
#define ETH_IENC_MNGMNTDONE                              (0x1UL << 0)                               /**< Disable management done interrupt */
#define _ETH_IENC_MNGMNTDONE_SHIFT                       0                                          /**< Shift value for ETH_MNGMNTDONE */
#define _ETH_IENC_MNGMNTDONE_MASK                        0x1UL                                      /**< Bit mask for ETH_MNGMNTDONE */
#define _ETH_IENC_MNGMNTDONE_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_MNGMNTDONE_DEFAULT                      (_ETH_IENC_MNGMNTDONE_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXCMPLT                                 (0x1UL << 1)                               /**< Disable receive complete interrupt */
#define _ETH_IENC_RXCMPLT_SHIFT                          1                                          /**< Shift value for ETH_RXCMPLT */
#define _ETH_IENC_RXCMPLT_MASK                           0x2UL                                      /**< Bit mask for ETH_RXCMPLT */
#define _ETH_IENC_RXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXCMPLT_DEFAULT                         (_ETH_IENC_RXCMPLT_DEFAULT << 1)           /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXUSEDBITREAD                           (0x1UL << 2)                               /**< Disable receive used bit read interrupt */
#define _ETH_IENC_RXUSEDBITREAD_SHIFT                    2                                          /**< Shift value for ETH_RXUSEDBITREAD */
#define _ETH_IENC_RXUSEDBITREAD_MASK                     0x4UL                                      /**< Bit mask for ETH_RXUSEDBITREAD */
#define _ETH_IENC_RXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXUSEDBITREAD_DEFAULT                   (_ETH_IENC_RXUSEDBITREAD_DEFAULT << 2)     /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXUSEDBITREAD                           (0x1UL << 3)                               /**< Disable transmit used bit read interrupt */
#define _ETH_IENC_TXUSEDBITREAD_SHIFT                    3                                          /**< Shift value for ETH_TXUSEDBITREAD */
#define _ETH_IENC_TXUSEDBITREAD_MASK                     0x8UL                                      /**< Bit mask for ETH_TXUSEDBITREAD */
#define _ETH_IENC_TXUSEDBITREAD_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXUSEDBITREAD_DEFAULT                   (_ETH_IENC_TXUSEDBITREAD_DEFAULT << 3)     /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXUNDERRUN                              (0x1UL << 4)                               /**< Disable transmit buffer under run interrupt */
#define _ETH_IENC_TXUNDERRUN_SHIFT                       4                                          /**< Shift value for ETH_TXUNDERRUN */
#define _ETH_IENC_TXUNDERRUN_MASK                        0x10UL                                     /**< Bit mask for ETH_TXUNDERRUN */
#define _ETH_IENC_TXUNDERRUN_DEFAULT                     0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXUNDERRUN_DEFAULT                      (_ETH_IENC_TXUNDERRUN_DEFAULT << 4)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RTRYLMTORLATECOL                        (0x1UL << 5)                               /**< Disable retry limit exceeded or late collision interrupt */
#define _ETH_IENC_RTRYLMTORLATECOL_SHIFT                 5                                          /**< Shift value for ETH_RTRYLMTORLATECOL */
#define _ETH_IENC_RTRYLMTORLATECOL_MASK                  0x20UL                                     /**< Bit mask for ETH_RTRYLMTORLATECOL */
#define _ETH_IENC_RTRYLMTORLATECOL_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RTRYLMTORLATECOL_DEFAULT                (_ETH_IENC_RTRYLMTORLATECOL_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_AMBAERR                                 (0x1UL << 6)                               /**< Disable transmit frame corruption due to AMBA (AHB) error interrupt */
#define _ETH_IENC_AMBAERR_SHIFT                          6                                          /**< Shift value for ETH_AMBAERR */
#define _ETH_IENC_AMBAERR_MASK                           0x40UL                                     /**< Bit mask for ETH_AMBAERR */
#define _ETH_IENC_AMBAERR_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_AMBAERR_DEFAULT                         (_ETH_IENC_AMBAERR_DEFAULT << 6)           /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXCMPLT                                 (0x1UL << 7)                               /**< Disable transmit complete interrupt */
#define _ETH_IENC_TXCMPLT_SHIFT                          7                                          /**< Shift value for ETH_TXCMPLT */
#define _ETH_IENC_TXCMPLT_MASK                           0x80UL                                     /**< Bit mask for ETH_TXCMPLT */
#define _ETH_IENC_TXCMPLT_DEFAULT                        0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_TXCMPLT_DEFAULT                         (_ETH_IENC_TXCMPLT_DEFAULT << 7)           /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXOVERRUN                               (0x1UL << 10)                              /**< Disable receive overrun interrupt */
#define _ETH_IENC_RXOVERRUN_SHIFT                        10                                         /**< Shift value for ETH_RXOVERRUN */
#define _ETH_IENC_RXOVERRUN_MASK                         0x400UL                                    /**< Bit mask for ETH_RXOVERRUN */
#define _ETH_IENC_RXOVERRUN_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXOVERRUN_DEFAULT                       (_ETH_IENC_RXOVERRUN_DEFAULT << 10)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RESPNOTOK                               (0x1UL << 11)                              /**< Disable bresp/hresp not OK interrupt */
#define _ETH_IENC_RESPNOTOK_SHIFT                        11                                         /**< Shift value for ETH_RESPNOTOK */
#define _ETH_IENC_RESPNOTOK_MASK                         0x800UL                                    /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_IENC_RESPNOTOK_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RESPNOTOK_DEFAULT                       (_ETH_IENC_RESPNOTOK_DEFAULT << 11)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_NONZEROPFRMQUANT                        (0x1UL << 12)                              /**< Disable pause frame with non-zero pause quantum interrupt */
#define _ETH_IENC_NONZEROPFRMQUANT_SHIFT                 12                                         /**< Shift value for ETH_NONZEROPFRMQUANT */
#define _ETH_IENC_NONZEROPFRMQUANT_MASK                  0x1000UL                                   /**< Bit mask for ETH_NONZEROPFRMQUANT */
#define _ETH_IENC_NONZEROPFRMQUANT_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_NONZEROPFRMQUANT_DEFAULT                (_ETH_IENC_NONZEROPFRMQUANT_DEFAULT << 12) /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PAUSETIMEZERO                           (0x1UL << 13)                              /**< Disable pause time zero interrupt */
#define _ETH_IENC_PAUSETIMEZERO_SHIFT                    13                                         /**< Shift value for ETH_PAUSETIMEZERO */
#define _ETH_IENC_PAUSETIMEZERO_MASK                     0x2000UL                                   /**< Bit mask for ETH_PAUSETIMEZERO */
#define _ETH_IENC_PAUSETIMEZERO_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PAUSETIMEZERO_DEFAULT                   (_ETH_IENC_PAUSETIMEZERO_DEFAULT << 13)    /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PFRMTX                                  (0x1UL << 14)                              /**< Disable pause frame transmitted interrupt */
#define _ETH_IENC_PFRMTX_SHIFT                           14                                         /**< Shift value for ETH_PFRMTX */
#define _ETH_IENC_PFRMTX_MASK                            0x4000UL                                   /**< Bit mask for ETH_PFRMTX */
#define _ETH_IENC_PFRMTX_DEFAULT                         0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PFRMTX_DEFAULT                          (_ETH_IENC_PFRMTX_DEFAULT << 14)           /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPDLYREQFRMRX                          (0x1UL << 18)                              /**< Disable PTP delay_req frame received interrupt */
#define _ETH_IENC_PTPDLYREQFRMRX_SHIFT                   18                                         /**< Shift value for ETH_PTPDLYREQFRMRX */
#define _ETH_IENC_PTPDLYREQFRMRX_MASK                    0x40000UL                                  /**< Bit mask for ETH_PTPDLYREQFRMRX */
#define _ETH_IENC_PTPDLYREQFRMRX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPDLYREQFRMRX_DEFAULT                  (_ETH_IENC_PTPDLYREQFRMRX_DEFAULT << 18)   /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPSYNCFRMRX                            (0x1UL << 19)                              /**< Disable PTP sync frame received interrupt */
#define _ETH_IENC_PTPSYNCFRMRX_SHIFT                     19                                         /**< Shift value for ETH_PTPSYNCFRMRX */
#define _ETH_IENC_PTPSYNCFRMRX_MASK                      0x80000UL                                  /**< Bit mask for ETH_PTPSYNCFRMRX */
#define _ETH_IENC_PTPSYNCFRMRX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPSYNCFRMRX_DEFAULT                    (_ETH_IENC_PTPSYNCFRMRX_DEFAULT << 19)     /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPDLYREQFRMTX                          (0x1UL << 20)                              /**< Disable PTP delay_req frame transmitted interrupt  */
#define _ETH_IENC_PTPDLYREQFRMTX_SHIFT                   20                                         /**< Shift value for ETH_PTPDLYREQFRMTX */
#define _ETH_IENC_PTPDLYREQFRMTX_MASK                    0x100000UL                                 /**< Bit mask for ETH_PTPDLYREQFRMTX */
#define _ETH_IENC_PTPDLYREQFRMTX_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPDLYREQFRMTX_DEFAULT                  (_ETH_IENC_PTPDLYREQFRMTX_DEFAULT << 20)   /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPSYNCFRMTX                            (0x1UL << 21)                              /**< Disable PTP sync frame transmitted interrupt  */
#define _ETH_IENC_PTPSYNCFRMTX_SHIFT                     21                                         /**< Shift value for ETH_PTPSYNCFRMTX */
#define _ETH_IENC_PTPSYNCFRMTX_MASK                      0x200000UL                                 /**< Bit mask for ETH_PTPSYNCFRMTX */
#define _ETH_IENC_PTPSYNCFRMTX_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPSYNCFRMTX_DEFAULT                    (_ETH_IENC_PTPSYNCFRMTX_DEFAULT << 21)     /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYREQFRMRX                         (0x1UL << 22)                              /**< Disable PTP pdelay_req frame received interrupt */
#define _ETH_IENC_PTPPDLYREQFRMRX_SHIFT                  22                                         /**< Shift value for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENC_PTPPDLYREQFRMRX_MASK                   0x400000UL                                 /**< Bit mask for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENC_PTPPDLYREQFRMRX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYREQFRMRX_DEFAULT                 (_ETH_IENC_PTPPDLYREQFRMRX_DEFAULT << 22)  /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYRESPFRMRX                        (0x1UL << 23)                              /**< Disable PTP pdelay_resp frame received interrupt */
#define _ETH_IENC_PTPPDLYRESPFRMRX_SHIFT                 23                                         /**< Shift value for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENC_PTPPDLYRESPFRMRX_MASK                  0x800000UL                                 /**< Bit mask for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENC_PTPPDLYRESPFRMRX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYRESPFRMRX_DEFAULT                (_ETH_IENC_PTPPDLYRESPFRMRX_DEFAULT << 23) /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYREQFRMTX                         (0x1UL << 24)                              /**< Disable PTP pdelay_req frame transmitted interrupt */
#define _ETH_IENC_PTPPDLYREQFRMTX_SHIFT                  24                                         /**< Shift value for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENC_PTPPDLYREQFRMTX_MASK                   0x1000000UL                                /**< Bit mask for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENC_PTPPDLYREQFRMTX_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYREQFRMTX_DEFAULT                 (_ETH_IENC_PTPPDLYREQFRMTX_DEFAULT << 24)  /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYRESPFRMTX                        (0x1UL << 25)                              /**< Disable PTP pdelay_resp frame transmitted interrupt */
#define _ETH_IENC_PTPPDLYRESPFRMTX_SHIFT                 25                                         /**< Shift value for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENC_PTPPDLYRESPFRMTX_MASK                  0x2000000UL                                /**< Bit mask for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENC_PTPPDLYRESPFRMTX_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_PTPPDLYRESPFRMTX_DEFAULT                (_ETH_IENC_PTPPDLYRESPFRMTX_DEFAULT << 25) /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_TSUSECREGINCR                           (0x1UL << 26)                              /**< Disable TSU seconds register increment interrupt */
#define _ETH_IENC_TSUSECREGINCR_SHIFT                    26                                         /**< Shift value for ETH_TSUSECREGINCR */
#define _ETH_IENC_TSUSECREGINCR_MASK                     0x4000000UL                                /**< Bit mask for ETH_TSUSECREGINCR */
#define _ETH_IENC_TSUSECREGINCR_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_TSUSECREGINCR_DEFAULT                   (_ETH_IENC_TSUSECREGINCR_DEFAULT << 26)    /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXLPIINDC                               (0x1UL << 27)                              /**< Disable RX LPI indication interrupt */
#define _ETH_IENC_RXLPIINDC_SHIFT                        27                                         /**< Shift value for ETH_RXLPIINDC */
#define _ETH_IENC_RXLPIINDC_MASK                         0x8000000UL                                /**< Bit mask for ETH_RXLPIINDC */
#define _ETH_IENC_RXLPIINDC_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_RXLPIINDC_DEFAULT                       (_ETH_IENC_RXLPIINDC_DEFAULT << 27)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_WOLEVNTRX                               (0x1UL << 28)                              /**< Disable WOL event received interrupt */
#define _ETH_IENC_WOLEVNTRX_SHIFT                        28                                         /**< Shift value for ETH_WOLEVNTRX */
#define _ETH_IENC_WOLEVNTRX_MASK                         0x10000000UL                               /**< Bit mask for ETH_WOLEVNTRX */
#define _ETH_IENC_WOLEVNTRX_DEFAULT                      0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_WOLEVNTRX_DEFAULT                       (_ETH_IENC_WOLEVNTRX_DEFAULT << 28)        /**< Shifted mode DEFAULT for ETH_IENC */
#define ETH_IENC_TSUTIMERCOMP                            (0x1UL << 29)                              /**< Disable TSU timer comparison interrupt. */
#define _ETH_IENC_TSUTIMERCOMP_SHIFT                     29                                         /**< Shift value for ETH_TSUTIMERCOMP */
#define _ETH_IENC_TSUTIMERCOMP_MASK                      0x20000000UL                               /**< Bit mask for ETH_TSUTIMERCOMP */
#define _ETH_IENC_TSUTIMERCOMP_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_IENC */
#define ETH_IENC_TSUTIMERCOMP_DEFAULT                    (_ETH_IENC_TSUTIMERCOMP_DEFAULT << 29)     /**< Shifted mode DEFAULT for ETH_IENC */

/* Bit fields for ETH IENRO */
#define _ETH_IENRO_RESETVALUE                            0x3FFC7DFFUL                                /**< Default value for ETH_IENRO */
#define _ETH_IENRO_MASK                                  0x3FFC7DFFUL                                /**< Mask for ETH_IENRO */
#define ETH_IENRO_MNGMNTDONE                             (0x1UL << 0)                                /**< management done interrupt mask */
#define _ETH_IENRO_MNGMNTDONE_SHIFT                      0                                           /**< Shift value for ETH_MNGMNTDONE */
#define _ETH_IENRO_MNGMNTDONE_MASK                       0x1UL                                       /**< Bit mask for ETH_MNGMNTDONE */
#define _ETH_IENRO_MNGMNTDONE_DEFAULT                    0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_MNGMNTDONE_DEFAULT                     (_ETH_IENRO_MNGMNTDONE_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXCMPLT                                (0x1UL << 1)                                /**< receive complete interrupt mask */
#define _ETH_IENRO_RXCMPLT_SHIFT                         1                                           /**< Shift value for ETH_RXCMPLT */
#define _ETH_IENRO_RXCMPLT_MASK                          0x2UL                                       /**< Bit mask for ETH_RXCMPLT */
#define _ETH_IENRO_RXCMPLT_DEFAULT                       0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXCMPLT_DEFAULT                        (_ETH_IENRO_RXCMPLT_DEFAULT << 1)           /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXUSEDBITREAD                          (0x1UL << 2)                                /**< receive used bit read interrupt mask */
#define _ETH_IENRO_RXUSEDBITREAD_SHIFT                   2                                           /**< Shift value for ETH_RXUSEDBITREAD */
#define _ETH_IENRO_RXUSEDBITREAD_MASK                    0x4UL                                       /**< Bit mask for ETH_RXUSEDBITREAD */
#define _ETH_IENRO_RXUSEDBITREAD_DEFAULT                 0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXUSEDBITREAD_DEFAULT                  (_ETH_IENRO_RXUSEDBITREAD_DEFAULT << 2)     /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXUSEDBITREAD                          (0x1UL << 3)                                /**< transmit used bit read interrupt mask */
#define _ETH_IENRO_TXUSEDBITREAD_SHIFT                   3                                           /**< Shift value for ETH_TXUSEDBITREAD */
#define _ETH_IENRO_TXUSEDBITREAD_MASK                    0x8UL                                       /**< Bit mask for ETH_TXUSEDBITREAD */
#define _ETH_IENRO_TXUSEDBITREAD_DEFAULT                 0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXUSEDBITREAD_DEFAULT                  (_ETH_IENRO_TXUSEDBITREAD_DEFAULT << 3)     /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXUNDERRUN                             (0x1UL << 4)                                /**< transmit buffer under run interrupt mask */
#define _ETH_IENRO_TXUNDERRUN_SHIFT                      4                                           /**< Shift value for ETH_TXUNDERRUN */
#define _ETH_IENRO_TXUNDERRUN_MASK                       0x10UL                                      /**< Bit mask for ETH_TXUNDERRUN */
#define _ETH_IENRO_TXUNDERRUN_DEFAULT                    0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXUNDERRUN_DEFAULT                     (_ETH_IENRO_TXUNDERRUN_DEFAULT << 4)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RTRYLMTORLATECOL                       (0x1UL << 5)                                /**< Retry limit exceeded or late collision (gigabit mode only) interrupt mask */
#define _ETH_IENRO_RTRYLMTORLATECOL_SHIFT                5                                           /**< Shift value for ETH_RTRYLMTORLATECOL */
#define _ETH_IENRO_RTRYLMTORLATECOL_MASK                 0x20UL                                      /**< Bit mask for ETH_RTRYLMTORLATECOL */
#define _ETH_IENRO_RTRYLMTORLATECOL_DEFAULT              0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RTRYLMTORLATECOL_DEFAULT               (_ETH_IENRO_RTRYLMTORLATECOL_DEFAULT << 5)  /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_AMBAERR                                (0x1UL << 6)                                /**< Transmit frame corruption due to AMBA (AHB) error interrupt mask */
#define _ETH_IENRO_AMBAERR_SHIFT                         6                                           /**< Shift value for ETH_AMBAERR */
#define _ETH_IENRO_AMBAERR_MASK                          0x40UL                                      /**< Bit mask for ETH_AMBAERR */
#define _ETH_IENRO_AMBAERR_DEFAULT                       0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_AMBAERR_DEFAULT                        (_ETH_IENRO_AMBAERR_DEFAULT << 6)           /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXCMPLT                                (0x1UL << 7)                                /**< Transmit complete interrupt mask */
#define _ETH_IENRO_TXCMPLT_SHIFT                         7                                           /**< Shift value for ETH_TXCMPLT */
#define _ETH_IENRO_TXCMPLT_MASK                          0x80UL                                      /**< Bit mask for ETH_TXCMPLT */
#define _ETH_IENRO_TXCMPLT_DEFAULT                       0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TXCMPLT_DEFAULT                        (_ETH_IENRO_TXCMPLT_DEFAULT << 7)           /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_UNUSED                                 (0x1UL << 8)                                /**< Unused */
#define _ETH_IENRO_UNUSED_SHIFT                          8                                           /**< Shift value for ETH_UNUSED */
#define _ETH_IENRO_UNUSED_MASK                           0x100UL                                     /**< Bit mask for ETH_UNUSED */
#define _ETH_IENRO_UNUSED_DEFAULT                        0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_UNUSED_DEFAULT                         (_ETH_IENRO_UNUSED_DEFAULT << 8)            /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXOVERRUN                              (0x1UL << 10)                               /**< Receive overrun interrupt mask */
#define _ETH_IENRO_RXOVERRUN_SHIFT                       10                                          /**< Shift value for ETH_RXOVERRUN */
#define _ETH_IENRO_RXOVERRUN_MASK                        0x400UL                                     /**< Bit mask for ETH_RXOVERRUN */
#define _ETH_IENRO_RXOVERRUN_DEFAULT                     0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXOVERRUN_DEFAULT                      (_ETH_IENRO_RXOVERRUN_DEFAULT << 10)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RESPNOTOK                              (0x1UL << 11)                               /**< bresp/hresp not OK interrupt mask */
#define _ETH_IENRO_RESPNOTOK_SHIFT                       11                                          /**< Shift value for ETH_RESPNOTOK */
#define _ETH_IENRO_RESPNOTOK_MASK                        0x800UL                                     /**< Bit mask for ETH_RESPNOTOK */
#define _ETH_IENRO_RESPNOTOK_DEFAULT                     0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RESPNOTOK_DEFAULT                      (_ETH_IENRO_RESPNOTOK_DEFAULT << 11)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_NONZEROPFRMQUANT                       (0x1UL << 12)                               /**< Pause frame with non-zero pause quantum interrupt mask */
#define _ETH_IENRO_NONZEROPFRMQUANT_SHIFT                12                                          /**< Shift value for ETH_NONZEROPFRMQUANT */
#define _ETH_IENRO_NONZEROPFRMQUANT_MASK                 0x1000UL                                    /**< Bit mask for ETH_NONZEROPFRMQUANT */
#define _ETH_IENRO_NONZEROPFRMQUANT_DEFAULT              0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_NONZEROPFRMQUANT_DEFAULT               (_ETH_IENRO_NONZEROPFRMQUANT_DEFAULT << 12) /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PAUSETIMEZERO                          (0x1UL << 13)                               /**< pause time zero interrupt mask */
#define _ETH_IENRO_PAUSETIMEZERO_SHIFT                   13                                          /**< Shift value for ETH_PAUSETIMEZERO */
#define _ETH_IENRO_PAUSETIMEZERO_MASK                    0x2000UL                                    /**< Bit mask for ETH_PAUSETIMEZERO */
#define _ETH_IENRO_PAUSETIMEZERO_DEFAULT                 0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PAUSETIMEZERO_DEFAULT                  (_ETH_IENRO_PAUSETIMEZERO_DEFAULT << 13)    /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PFRMTX                                 (0x1UL << 14)                               /**< pause frame transmitted interrupt mask */
#define _ETH_IENRO_PFRMTX_SHIFT                          14                                          /**< Shift value for ETH_PFRMTX */
#define _ETH_IENRO_PFRMTX_MASK                           0x4000UL                                    /**< Bit mask for ETH_PFRMTX */
#define _ETH_IENRO_PFRMTX_DEFAULT                        0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PFRMTX_DEFAULT                         (_ETH_IENRO_PFRMTX_DEFAULT << 14)           /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPDLYREQFRMRX                         (0x1UL << 18)                               /**< PTP delay_req frame received mask */
#define _ETH_IENRO_PTPDLYREQFRMRX_SHIFT                  18                                          /**< Shift value for ETH_PTPDLYREQFRMRX */
#define _ETH_IENRO_PTPDLYREQFRMRX_MASK                   0x40000UL                                   /**< Bit mask for ETH_PTPDLYREQFRMRX */
#define _ETH_IENRO_PTPDLYREQFRMRX_DEFAULT                0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPDLYREQFRMRX_DEFAULT                 (_ETH_IENRO_PTPDLYREQFRMRX_DEFAULT << 18)   /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPSYNCFRMRX                           (0x1UL << 19)                               /**< PTP sync frame received mask */
#define _ETH_IENRO_PTPSYNCFRMRX_SHIFT                    19                                          /**< Shift value for ETH_PTPSYNCFRMRX */
#define _ETH_IENRO_PTPSYNCFRMRX_MASK                     0x80000UL                                   /**< Bit mask for ETH_PTPSYNCFRMRX */
#define _ETH_IENRO_PTPSYNCFRMRX_DEFAULT                  0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPSYNCFRMRX_DEFAULT                   (_ETH_IENRO_PTPSYNCFRMRX_DEFAULT << 19)     /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPDLYREQFRMTX                         (0x1UL << 20)                               /**< PTP delay_req frame transmitted mask */
#define _ETH_IENRO_PTPDLYREQFRMTX_SHIFT                  20                                          /**< Shift value for ETH_PTPDLYREQFRMTX */
#define _ETH_IENRO_PTPDLYREQFRMTX_MASK                   0x100000UL                                  /**< Bit mask for ETH_PTPDLYREQFRMTX */
#define _ETH_IENRO_PTPDLYREQFRMTX_DEFAULT                0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPDLYREQFRMTX_DEFAULT                 (_ETH_IENRO_PTPDLYREQFRMTX_DEFAULT << 20)   /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPSYNCFRMTX                           (0x1UL << 21)                               /**< PTP sync frame transmitted mask */
#define _ETH_IENRO_PTPSYNCFRMTX_SHIFT                    21                                          /**< Shift value for ETH_PTPSYNCFRMTX */
#define _ETH_IENRO_PTPSYNCFRMTX_MASK                     0x200000UL                                  /**< Bit mask for ETH_PTPSYNCFRMTX */
#define _ETH_IENRO_PTPSYNCFRMTX_DEFAULT                  0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPSYNCFRMTX_DEFAULT                   (_ETH_IENRO_PTPSYNCFRMTX_DEFAULT << 21)     /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYREQFRMRX                        (0x1UL << 22)                               /**< PTP pdelay_req frame received mask */
#define _ETH_IENRO_PTPPDLYREQFRMRX_SHIFT                 22                                          /**< Shift value for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENRO_PTPPDLYREQFRMRX_MASK                  0x400000UL                                  /**< Bit mask for ETH_PTPPDLYREQFRMRX */
#define _ETH_IENRO_PTPPDLYREQFRMRX_DEFAULT               0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYREQFRMRX_DEFAULT                (_ETH_IENRO_PTPPDLYREQFRMRX_DEFAULT << 22)  /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYRESPFRMRX                       (0x1UL << 23)                               /**< PTP pdelay_resp frame received mask */
#define _ETH_IENRO_PTPPDLYRESPFRMRX_SHIFT                23                                          /**< Shift value for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENRO_PTPPDLYRESPFRMRX_MASK                 0x800000UL                                  /**< Bit mask for ETH_PTPPDLYRESPFRMRX */
#define _ETH_IENRO_PTPPDLYRESPFRMRX_DEFAULT              0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYRESPFRMRX_DEFAULT               (_ETH_IENRO_PTPPDLYRESPFRMRX_DEFAULT << 23) /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYREQFRMTX                        (0x1UL << 24)                               /**< PTP pdelay_req frame transmitted mask */
#define _ETH_IENRO_PTPPDLYREQFRMTX_SHIFT                 24                                          /**< Shift value for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENRO_PTPPDLYREQFRMTX_MASK                  0x1000000UL                                 /**< Bit mask for ETH_PTPPDLYREQFRMTX */
#define _ETH_IENRO_PTPPDLYREQFRMTX_DEFAULT               0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYREQFRMTX_DEFAULT                (_ETH_IENRO_PTPPDLYREQFRMTX_DEFAULT << 24)  /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYRESPFRMTX                       (0x1UL << 25)                               /**< PTP pdelay_resp frame transmitted mask */
#define _ETH_IENRO_PTPPDLYRESPFRMTX_SHIFT                25                                          /**< Shift value for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENRO_PTPPDLYRESPFRMTX_MASK                 0x2000000UL                                 /**< Bit mask for ETH_PTPPDLYRESPFRMTX */
#define _ETH_IENRO_PTPPDLYRESPFRMTX_DEFAULT              0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_PTPPDLYRESPFRMTX_DEFAULT               (_ETH_IENRO_PTPPDLYRESPFRMTX_DEFAULT << 25) /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TSUSECREGINCR                          (0x1UL << 26)                               /**< TSU seconds register increment mask */
#define _ETH_IENRO_TSUSECREGINCR_SHIFT                   26                                          /**< Shift value for ETH_TSUSECREGINCR */
#define _ETH_IENRO_TSUSECREGINCR_MASK                    0x4000000UL                                 /**< Bit mask for ETH_TSUSECREGINCR */
#define _ETH_IENRO_TSUSECREGINCR_DEFAULT                 0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TSUSECREGINCR_DEFAULT                  (_ETH_IENRO_TSUSECREGINCR_DEFAULT << 26)    /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXLPIINDC                              (0x1UL << 27)                               /**< RX LPI indication mask */
#define _ETH_IENRO_RXLPIINDC_SHIFT                       27                                          /**< Shift value for ETH_RXLPIINDC */
#define _ETH_IENRO_RXLPIINDC_MASK                        0x8000000UL                                 /**< Bit mask for ETH_RXLPIINDC */
#define _ETH_IENRO_RXLPIINDC_DEFAULT                     0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_RXLPIINDC_DEFAULT                      (_ETH_IENRO_RXLPIINDC_DEFAULT << 27)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_WOLEVNTRX                              (0x1UL << 28)                               /**< WOL event received mask */
#define _ETH_IENRO_WOLEVNTRX_SHIFT                       28                                          /**< Shift value for ETH_WOLEVNTRX */
#define _ETH_IENRO_WOLEVNTRX_MASK                        0x10000000UL                                /**< Bit mask for ETH_WOLEVNTRX */
#define _ETH_IENRO_WOLEVNTRX_DEFAULT                     0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_WOLEVNTRX_DEFAULT                      (_ETH_IENRO_WOLEVNTRX_DEFAULT << 28)        /**< Shifted mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TSUTIMERCOMP                           (0x1UL << 29)                               /**< TSU timer comparison interrupt mask. */
#define _ETH_IENRO_TSUTIMERCOMP_SHIFT                    29                                          /**< Shift value for ETH_TSUTIMERCOMP */
#define _ETH_IENRO_TSUTIMERCOMP_MASK                     0x20000000UL                                /**< Bit mask for ETH_TSUTIMERCOMP */
#define _ETH_IENRO_TSUTIMERCOMP_DEFAULT                  0x00000001UL                                /**< Mode DEFAULT for ETH_IENRO */
#define ETH_IENRO_TSUTIMERCOMP_DEFAULT                   (_ETH_IENRO_TSUTIMERCOMP_DEFAULT << 29)     /**< Shifted mode DEFAULT for ETH_IENRO */

/* Bit fields for ETH PHYMNGMNT */
#define _ETH_PHYMNGMNT_RESETVALUE                        0x00000000UL                             /**< Default value for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_MASK                              0xFFFFFFFFUL                             /**< Mask for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_PHYRWDATA_SHIFT                   0                                        /**< Shift value for ETH_PHYRWDATA */
#define _ETH_PHYMNGMNT_PHYRWDATA_MASK                    0xFFFFUL                                 /**< Bit mask for ETH_PHYRWDATA */
#define _ETH_PHYMNGMNT_PHYRWDATA_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_PHYRWDATA_DEFAULT                  (_ETH_PHYMNGMNT_PHYRWDATA_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_WRITE10_SHIFT                     16                                       /**< Shift value for ETH_WRITE10 */
#define _ETH_PHYMNGMNT_WRITE10_MASK                      0x30000UL                                /**< Bit mask for ETH_WRITE10 */
#define _ETH_PHYMNGMNT_WRITE10_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_WRITE10_DEFAULT                    (_ETH_PHYMNGMNT_WRITE10_DEFAULT << 16)   /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_REGADDR_SHIFT                     18                                       /**< Shift value for ETH_REGADDR */
#define _ETH_PHYMNGMNT_REGADDR_MASK                      0x7C0000UL                               /**< Bit mask for ETH_REGADDR */
#define _ETH_PHYMNGMNT_REGADDR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_REGADDR_DEFAULT                    (_ETH_PHYMNGMNT_REGADDR_DEFAULT << 18)   /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_PHYADDR_SHIFT                     23                                       /**< Shift value for ETH_PHYADDR */
#define _ETH_PHYMNGMNT_PHYADDR_MASK                      0xF800000UL                              /**< Bit mask for ETH_PHYADDR */
#define _ETH_PHYMNGMNT_PHYADDR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_PHYADDR_DEFAULT                    (_ETH_PHYMNGMNT_PHYADDR_DEFAULT << 23)   /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define _ETH_PHYMNGMNT_OPERATION_SHIFT                   28                                       /**< Shift value for ETH_OPERATION */
#define _ETH_PHYMNGMNT_OPERATION_MASK                    0x30000000UL                             /**< Bit mask for ETH_OPERATION */
#define _ETH_PHYMNGMNT_OPERATION_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_OPERATION_DEFAULT                  (_ETH_PHYMNGMNT_OPERATION_DEFAULT << 28) /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_WRITE1                             (0x1UL << 30)                            /**< Must be written to 1 for a valid Clause 22 frame and to 0 for a valid Clause 45 frame. */
#define _ETH_PHYMNGMNT_WRITE1_SHIFT                      30                                       /**< Shift value for ETH_WRITE1 */
#define _ETH_PHYMNGMNT_WRITE1_MASK                       0x40000000UL                             /**< Bit mask for ETH_WRITE1 */
#define _ETH_PHYMNGMNT_WRITE1_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_WRITE1_DEFAULT                     (_ETH_PHYMNGMNT_WRITE1_DEFAULT << 30)    /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_WRITE0                             (0x1UL << 31)                            /**< Must be written with 0. */
#define _ETH_PHYMNGMNT_WRITE0_SHIFT                      31                                       /**< Shift value for ETH_WRITE0 */
#define _ETH_PHYMNGMNT_WRITE0_MASK                       0x80000000UL                             /**< Bit mask for ETH_WRITE0 */
#define _ETH_PHYMNGMNT_WRITE0_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for ETH_PHYMNGMNT */
#define ETH_PHYMNGMNT_WRITE0_DEFAULT                     (_ETH_PHYMNGMNT_WRITE0_DEFAULT << 31)    /**< Shifted mode DEFAULT for ETH_PHYMNGMNT */

/* Bit fields for ETH RXPAUSEQUANT */
#define _ETH_RXPAUSEQUANT_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_RXPAUSEQUANT */
#define _ETH_RXPAUSEQUANT_MASK                           0x0000FFFFUL                           /**< Mask for ETH_RXPAUSEQUANT */
#define _ETH_RXPAUSEQUANT_QUANT_SHIFT                    0                                      /**< Shift value for ETH_QUANT */
#define _ETH_RXPAUSEQUANT_QUANT_MASK                     0xFFFFUL                               /**< Bit mask for ETH_QUANT */
#define _ETH_RXPAUSEQUANT_QUANT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_RXPAUSEQUANT */
#define ETH_RXPAUSEQUANT_QUANT_DEFAULT                   (_ETH_RXPAUSEQUANT_QUANT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXPAUSEQUANT */

/* Bit fields for ETH TXPAUSEQUANT */
#define _ETH_TXPAUSEQUANT_RESETVALUE                     0xFFFFFFFFUL                              /**< Default value for ETH_TXPAUSEQUANT */
#define _ETH_TXPAUSEQUANT_MASK                           0xFFFFFFFFUL                              /**< Mask for ETH_TXPAUSEQUANT */
#define _ETH_TXPAUSEQUANT_QUANT_SHIFT                    0                                         /**< Shift value for ETH_QUANT */
#define _ETH_TXPAUSEQUANT_QUANT_MASK                     0xFFFFUL                                  /**< Bit mask for ETH_QUANT */
#define _ETH_TXPAUSEQUANT_QUANT_DEFAULT                  0x0000FFFFUL                              /**< Mode DEFAULT for ETH_TXPAUSEQUANT */
#define ETH_TXPAUSEQUANT_QUANT_DEFAULT                   (_ETH_TXPAUSEQUANT_QUANT_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT */
#define _ETH_TXPAUSEQUANT_QUANTP1_SHIFT                  16                                        /**< Shift value for ETH_QUANTP1 */
#define _ETH_TXPAUSEQUANT_QUANTP1_MASK                   0xFFFF0000UL                              /**< Bit mask for ETH_QUANTP1 */
#define _ETH_TXPAUSEQUANT_QUANTP1_DEFAULT                0x0000FFFFUL                              /**< Mode DEFAULT for ETH_TXPAUSEQUANT */
#define ETH_TXPAUSEQUANT_QUANTP1_DEFAULT                 (_ETH_TXPAUSEQUANT_QUANTP1_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT */

/* Bit fields for ETH PBUFTXCUTTHRU */
#define _ETH_PBUFTXCUTTHRU_RESETVALUE                    0x000003FFUL                                      /**< Default value for ETH_PBUFTXCUTTHRU */
#define _ETH_PBUFTXCUTTHRU_MASK                          0x800003FFUL                                      /**< Mask for ETH_PBUFTXCUTTHRU */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRUTHR_SHIFT         0                                                 /**< Shift value for ETH_DMATXCUTTHRUTHR */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRUTHR_MASK          0x3FFUL                                           /**< Bit mask for ETH_DMATXCUTTHRUTHR */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRUTHR_DEFAULT       0x000003FFUL                                      /**< Mode DEFAULT for ETH_PBUFTXCUTTHRU */
#define ETH_PBUFTXCUTTHRU_DMATXCUTTHRUTHR_DEFAULT        (_ETH_PBUFTXCUTTHRU_DMATXCUTTHRUTHR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_PBUFTXCUTTHRU */
#define ETH_PBUFTXCUTTHRU_DMATXCUTTHRU                   (0x1UL << 31)                                     /**< Enable TX partial store and forward operation */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRU_SHIFT            31                                                /**< Shift value for ETH_DMATXCUTTHRU */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRU_MASK             0x80000000UL                                      /**< Bit mask for ETH_DMATXCUTTHRU */
#define _ETH_PBUFTXCUTTHRU_DMATXCUTTHRU_DEFAULT          0x00000000UL                                      /**< Mode DEFAULT for ETH_PBUFTXCUTTHRU */
#define ETH_PBUFTXCUTTHRU_DMATXCUTTHRU_DEFAULT           (_ETH_PBUFTXCUTTHRU_DMATXCUTTHRU_DEFAULT << 31)   /**< Shifted mode DEFAULT for ETH_PBUFTXCUTTHRU */

/* Bit fields for ETH PBUFRXCUTTHRU */
#define _ETH_PBUFRXCUTTHRU_RESETVALUE                    0x000003FFUL                                      /**< Default value for ETH_PBUFRXCUTTHRU */
#define _ETH_PBUFRXCUTTHRU_MASK                          0x800003FFUL                                      /**< Mask for ETH_PBUFRXCUTTHRU */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRUTHR_SHIFT         0                                                 /**< Shift value for ETH_DMARXCUTTHRUTHR */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRUTHR_MASK          0x3FFUL                                           /**< Bit mask for ETH_DMARXCUTTHRUTHR */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRUTHR_DEFAULT       0x000003FFUL                                      /**< Mode DEFAULT for ETH_PBUFRXCUTTHRU */
#define ETH_PBUFRXCUTTHRU_DMARXCUTTHRUTHR_DEFAULT        (_ETH_PBUFRXCUTTHRU_DMARXCUTTHRUTHR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_PBUFRXCUTTHRU */
#define ETH_PBUFRXCUTTHRU_DMARXCUTTHRU                   (0x1UL << 31)                                     /**< Enable RX partial store and forward operation */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRU_SHIFT            31                                                /**< Shift value for ETH_DMARXCUTTHRU */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRU_MASK             0x80000000UL                                      /**< Bit mask for ETH_DMARXCUTTHRU */
#define _ETH_PBUFRXCUTTHRU_DMARXCUTTHRU_DEFAULT          0x00000000UL                                      /**< Mode DEFAULT for ETH_PBUFRXCUTTHRU */
#define ETH_PBUFRXCUTTHRU_DMARXCUTTHRU_DEFAULT           (_ETH_PBUFRXCUTTHRU_DMARXCUTTHRU_DEFAULT << 31)   /**< Shifted mode DEFAULT for ETH_PBUFRXCUTTHRU */

/* Bit fields for ETH JUMBOMAXLEN */
#define _ETH_JUMBOMAXLEN_RESETVALUE                      0x00002800UL                                /**< Default value for ETH_JUMBOMAXLEN */
#define _ETH_JUMBOMAXLEN_MASK                            0x00003FFFUL                                /**< Mask for ETH_JUMBOMAXLEN */
#define _ETH_JUMBOMAXLEN_JUMBOMAXLEN_SHIFT               0                                           /**< Shift value for ETH_JUMBOMAXLEN */
#define _ETH_JUMBOMAXLEN_JUMBOMAXLEN_MASK                0x3FFFUL                                    /**< Bit mask for ETH_JUMBOMAXLEN */
#define _ETH_JUMBOMAXLEN_JUMBOMAXLEN_DEFAULT             0x00002800UL                                /**< Mode DEFAULT for ETH_JUMBOMAXLEN */
#define ETH_JUMBOMAXLEN_JUMBOMAXLEN_DEFAULT              (_ETH_JUMBOMAXLEN_JUMBOMAXLEN_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_JUMBOMAXLEN */

/* Bit fields for ETH IMOD */
#define _ETH_IMOD_RESETVALUE                             0x00000000UL                       /**< Default value for ETH_IMOD */
#define _ETH_IMOD_MASK                                   0x00FF00FFUL                       /**< Mask for ETH_IMOD */
#define _ETH_IMOD_RXINTMOD_SHIFT                         0                                  /**< Shift value for ETH_RXINTMOD */
#define _ETH_IMOD_RXINTMOD_MASK                          0xFFUL                             /**< Bit mask for ETH_RXINTMOD */
#define _ETH_IMOD_RXINTMOD_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for ETH_IMOD */
#define ETH_IMOD_RXINTMOD_DEFAULT                        (_ETH_IMOD_RXINTMOD_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_IMOD */
#define _ETH_IMOD_TXINTMOD_SHIFT                         16                                 /**< Shift value for ETH_TXINTMOD */
#define _ETH_IMOD_TXINTMOD_MASK                          0xFF0000UL                         /**< Bit mask for ETH_TXINTMOD */
#define _ETH_IMOD_TXINTMOD_DEFAULT                       0x00000000UL                       /**< Mode DEFAULT for ETH_IMOD */
#define ETH_IMOD_TXINTMOD_DEFAULT                        (_ETH_IMOD_TXINTMOD_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_IMOD */

/* Bit fields for ETH SYSWAKETIME */
#define _ETH_SYSWAKETIME_RESETVALUE                      0x00000000UL                                /**< Default value for ETH_SYSWAKETIME */
#define _ETH_SYSWAKETIME_MASK                            0x0000FFFFUL                                /**< Mask for ETH_SYSWAKETIME */
#define _ETH_SYSWAKETIME_SYSWAKETIME_SHIFT               0                                           /**< Shift value for ETH_SYSWAKETIME */
#define _ETH_SYSWAKETIME_SYSWAKETIME_MASK                0xFFFFUL                                    /**< Bit mask for ETH_SYSWAKETIME */
#define _ETH_SYSWAKETIME_SYSWAKETIME_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for ETH_SYSWAKETIME */
#define ETH_SYSWAKETIME_SYSWAKETIME_DEFAULT              (_ETH_SYSWAKETIME_SYSWAKETIME_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SYSWAKETIME */

/* Bit fields for ETH HASHBOTTOM */
#define _ETH_HASHBOTTOM_RESETVALUE                       0x00000000UL                        /**< Default value for ETH_HASHBOTTOM */
#define _ETH_HASHBOTTOM_MASK                             0xFFFFFFFFUL                        /**< Mask for ETH_HASHBOTTOM */
#define _ETH_HASHBOTTOM_ADDR_SHIFT                       0                                   /**< Shift value for ETH_ADDR */
#define _ETH_HASHBOTTOM_ADDR_MASK                        0xFFFFFFFFUL                        /**< Bit mask for ETH_ADDR */
#define _ETH_HASHBOTTOM_ADDR_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for ETH_HASHBOTTOM */
#define ETH_HASHBOTTOM_ADDR_DEFAULT                      (_ETH_HASHBOTTOM_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_HASHBOTTOM */

/* Bit fields for ETH HASHTOP */
#define _ETH_HASHTOP_RESETVALUE                          0x00000000UL                     /**< Default value for ETH_HASHTOP */
#define _ETH_HASHTOP_MASK                                0xFFFFFFFFUL                     /**< Mask for ETH_HASHTOP */
#define _ETH_HASHTOP_ADDR_SHIFT                          0                                /**< Shift value for ETH_ADDR */
#define _ETH_HASHTOP_ADDR_MASK                           0xFFFFFFFFUL                     /**< Bit mask for ETH_ADDR */
#define _ETH_HASHTOP_ADDR_DEFAULT                        0x00000000UL                     /**< Mode DEFAULT for ETH_HASHTOP */
#define ETH_HASHTOP_ADDR_DEFAULT                         (_ETH_HASHTOP_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_HASHTOP */

/* Bit fields for ETH SPECADDR1BOTTOM */
#define _ETH_SPECADDR1BOTTOM_RESETVALUE                  0x00000000UL                             /**< Default value for ETH_SPECADDR1BOTTOM */
#define _ETH_SPECADDR1BOTTOM_MASK                        0xFFFFFFFFUL                             /**< Mask for ETH_SPECADDR1BOTTOM */
#define _ETH_SPECADDR1BOTTOM_ADDR_SHIFT                  0                                        /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR1BOTTOM_ADDR_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR1BOTTOM_ADDR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_SPECADDR1BOTTOM */
#define ETH_SPECADDR1BOTTOM_ADDR_DEFAULT                 (_ETH_SPECADDR1BOTTOM_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SPECADDR1BOTTOM */

/* Bit fields for ETH SPECADDR1TOP */
#define _ETH_SPECADDR1TOP_RESETVALUE                     0x00000000UL                                 /**< Default value for ETH_SPECADDR1TOP */
#define _ETH_SPECADDR1TOP_MASK                           0x0001FFFFUL                                 /**< Mask for ETH_SPECADDR1TOP */
#define _ETH_SPECADDR1TOP_ADDR_SHIFT                     0                                            /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR1TOP_ADDR_MASK                      0xFFFFUL                                     /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR1TOP_ADDR_DEFAULT                   0x00000000UL                                 /**< Mode DEFAULT for ETH_SPECADDR1TOP */
#define ETH_SPECADDR1TOP_ADDR_DEFAULT                    (_ETH_SPECADDR1TOP_ADDR_DEFAULT << 0)        /**< Shifted mode DEFAULT for ETH_SPECADDR1TOP */
#define ETH_SPECADDR1TOP_FILTERTYPE                      (0x1UL << 16)                                /**< MAC SA or DA selection */
#define _ETH_SPECADDR1TOP_FILTERTYPE_SHIFT               16                                           /**< Shift value for ETH_FILTERTYPE */
#define _ETH_SPECADDR1TOP_FILTERTYPE_MASK                0x10000UL                                    /**< Bit mask for ETH_FILTERTYPE */
#define _ETH_SPECADDR1TOP_FILTERTYPE_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for ETH_SPECADDR1TOP */
#define _ETH_SPECADDR1TOP_FILTERTYPE_DA                  0x00000000UL                                 /**< Mode DA for ETH_SPECADDR1TOP */
#define _ETH_SPECADDR1TOP_FILTERTYPE_SA                  0x00000001UL                                 /**< Mode SA for ETH_SPECADDR1TOP */
#define ETH_SPECADDR1TOP_FILTERTYPE_DEFAULT              (_ETH_SPECADDR1TOP_FILTERTYPE_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_SPECADDR1TOP */
#define ETH_SPECADDR1TOP_FILTERTYPE_DA                   (_ETH_SPECADDR1TOP_FILTERTYPE_DA << 16)      /**< Shifted mode DA for ETH_SPECADDR1TOP */
#define ETH_SPECADDR1TOP_FILTERTYPE_SA                   (_ETH_SPECADDR1TOP_FILTERTYPE_SA << 16)      /**< Shifted mode SA for ETH_SPECADDR1TOP */

/* Bit fields for ETH SPECADDR2BOTTOM */
#define _ETH_SPECADDR2BOTTOM_RESETVALUE                  0x00000000UL                             /**< Default value for ETH_SPECADDR2BOTTOM */
#define _ETH_SPECADDR2BOTTOM_MASK                        0xFFFFFFFFUL                             /**< Mask for ETH_SPECADDR2BOTTOM */
#define _ETH_SPECADDR2BOTTOM_ADDR_SHIFT                  0                                        /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR2BOTTOM_ADDR_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR2BOTTOM_ADDR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_SPECADDR2BOTTOM */
#define ETH_SPECADDR2BOTTOM_ADDR_DEFAULT                 (_ETH_SPECADDR2BOTTOM_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SPECADDR2BOTTOM */

/* Bit fields for ETH SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_RESETVALUE                     0x00000000UL                                     /**< Default value for ETH_SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_MASK                           0x3F01FFFFUL                                     /**< Mask for ETH_SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_ADDR_SHIFT                     0                                                /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR2TOP_ADDR_MASK                      0xFFFFUL                                         /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR2TOP_ADDR_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_ADDR_DEFAULT                    (_ETH_SPECADDR2TOP_ADDR_DEFAULT << 0)            /**< Shifted mode DEFAULT for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_FILTERTYPE                      (0x1UL << 16)                                    /**< MAC SA or DA selection */
#define _ETH_SPECADDR2TOP_FILTERTYPE_SHIFT               16                                               /**< Shift value for ETH_FILTERTYPE */
#define _ETH_SPECADDR2TOP_FILTERTYPE_MASK                0x10000UL                                        /**< Bit mask for ETH_FILTERTYPE */
#define _ETH_SPECADDR2TOP_FILTERTYPE_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_FILTERTYPE_DA                  0x00000000UL                                     /**< Mode DA for ETH_SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_FILTERTYPE_SA                  0x00000001UL                                     /**< Mode SA for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_FILTERTYPE_DEFAULT              (_ETH_SPECADDR2TOP_FILTERTYPE_DEFAULT << 16)     /**< Shifted mode DEFAULT for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_FILTERTYPE_DA                   (_ETH_SPECADDR2TOP_FILTERTYPE_DA << 16)          /**< Shifted mode DA for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_FILTERTYPE_SA                   (_ETH_SPECADDR2TOP_FILTERTYPE_SA << 16)          /**< Shifted mode SA for ETH_SPECADDR2TOP */
#define _ETH_SPECADDR2TOP_FILTERBYTEMASK_SHIFT           24                                               /**< Shift value for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR2TOP_FILTERBYTEMASK_MASK            0x3F000000UL                                     /**< Bit mask for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR2TOP_FILTERBYTEMASK_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR2TOP */
#define ETH_SPECADDR2TOP_FILTERBYTEMASK_DEFAULT          (_ETH_SPECADDR2TOP_FILTERBYTEMASK_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_SPECADDR2TOP */

/* Bit fields for ETH SPECADDR3BOTTOM */
#define _ETH_SPECADDR3BOTTOM_RESETVALUE                  0x00000000UL                             /**< Default value for ETH_SPECADDR3BOTTOM */
#define _ETH_SPECADDR3BOTTOM_MASK                        0xFFFFFFFFUL                             /**< Mask for ETH_SPECADDR3BOTTOM */
#define _ETH_SPECADDR3BOTTOM_ADDR_SHIFT                  0                                        /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR3BOTTOM_ADDR_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR3BOTTOM_ADDR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_SPECADDR3BOTTOM */
#define ETH_SPECADDR3BOTTOM_ADDR_DEFAULT                 (_ETH_SPECADDR3BOTTOM_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SPECADDR3BOTTOM */

/* Bit fields for ETH SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_RESETVALUE                     0x00000000UL                                     /**< Default value for ETH_SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_MASK                           0x3F01FFFFUL                                     /**< Mask for ETH_SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_ADDR_SHIFT                     0                                                /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR3TOP_ADDR_MASK                      0xFFFFUL                                         /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR3TOP_ADDR_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_ADDR_DEFAULT                    (_ETH_SPECADDR3TOP_ADDR_DEFAULT << 0)            /**< Shifted mode DEFAULT for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_FILTERTYPE                      (0x1UL << 16)                                    /**< MAC SA or DA selection */
#define _ETH_SPECADDR3TOP_FILTERTYPE_SHIFT               16                                               /**< Shift value for ETH_FILTERTYPE */
#define _ETH_SPECADDR3TOP_FILTERTYPE_MASK                0x10000UL                                        /**< Bit mask for ETH_FILTERTYPE */
#define _ETH_SPECADDR3TOP_FILTERTYPE_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_FILTERTYPE_DA                  0x00000000UL                                     /**< Mode DA for ETH_SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_FILTERTYPE_SA                  0x00000001UL                                     /**< Mode SA for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_FILTERTYPE_DEFAULT              (_ETH_SPECADDR3TOP_FILTERTYPE_DEFAULT << 16)     /**< Shifted mode DEFAULT for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_FILTERTYPE_DA                   (_ETH_SPECADDR3TOP_FILTERTYPE_DA << 16)          /**< Shifted mode DA for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_FILTERTYPE_SA                   (_ETH_SPECADDR3TOP_FILTERTYPE_SA << 16)          /**< Shifted mode SA for ETH_SPECADDR3TOP */
#define _ETH_SPECADDR3TOP_FILTERBYTEMASK_SHIFT           24                                               /**< Shift value for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR3TOP_FILTERBYTEMASK_MASK            0x3F000000UL                                     /**< Bit mask for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR3TOP_FILTERBYTEMASK_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR3TOP */
#define ETH_SPECADDR3TOP_FILTERBYTEMASK_DEFAULT          (_ETH_SPECADDR3TOP_FILTERBYTEMASK_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_SPECADDR3TOP */

/* Bit fields for ETH SPECADDR4BOTTOM */
#define _ETH_SPECADDR4BOTTOM_RESETVALUE                  0x00000000UL                             /**< Default value for ETH_SPECADDR4BOTTOM */
#define _ETH_SPECADDR4BOTTOM_MASK                        0xFFFFFFFFUL                             /**< Mask for ETH_SPECADDR4BOTTOM */
#define _ETH_SPECADDR4BOTTOM_ADDR_SHIFT                  0                                        /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR4BOTTOM_ADDR_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR4BOTTOM_ADDR_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_SPECADDR4BOTTOM */
#define ETH_SPECADDR4BOTTOM_ADDR_DEFAULT                 (_ETH_SPECADDR4BOTTOM_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SPECADDR4BOTTOM */

/* Bit fields for ETH SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_RESETVALUE                     0x00000000UL                                     /**< Default value for ETH_SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_MASK                           0x3F01FFFFUL                                     /**< Mask for ETH_SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_ADDR_SHIFT                     0                                                /**< Shift value for ETH_ADDR */
#define _ETH_SPECADDR4TOP_ADDR_MASK                      0xFFFFUL                                         /**< Bit mask for ETH_ADDR */
#define _ETH_SPECADDR4TOP_ADDR_DEFAULT                   0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_ADDR_DEFAULT                    (_ETH_SPECADDR4TOP_ADDR_DEFAULT << 0)            /**< Shifted mode DEFAULT for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_FILTERTYPE                      (0x1UL << 16)                                    /**< MAC SA or DA selection */
#define _ETH_SPECADDR4TOP_FILTERTYPE_SHIFT               16                                               /**< Shift value for ETH_FILTERTYPE */
#define _ETH_SPECADDR4TOP_FILTERTYPE_MASK                0x10000UL                                        /**< Bit mask for ETH_FILTERTYPE */
#define _ETH_SPECADDR4TOP_FILTERTYPE_DEFAULT             0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_FILTERTYPE_DA                  0x00000000UL                                     /**< Mode DA for ETH_SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_FILTERTYPE_SA                  0x00000001UL                                     /**< Mode SA for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_FILTERTYPE_DEFAULT              (_ETH_SPECADDR4TOP_FILTERTYPE_DEFAULT << 16)     /**< Shifted mode DEFAULT for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_FILTERTYPE_DA                   (_ETH_SPECADDR4TOP_FILTERTYPE_DA << 16)          /**< Shifted mode DA for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_FILTERTYPE_SA                   (_ETH_SPECADDR4TOP_FILTERTYPE_SA << 16)          /**< Shifted mode SA for ETH_SPECADDR4TOP */
#define _ETH_SPECADDR4TOP_FILTERBYTEMASK_SHIFT           24                                               /**< Shift value for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR4TOP_FILTERBYTEMASK_MASK            0x3F000000UL                                     /**< Bit mask for ETH_FILTERBYTEMASK */
#define _ETH_SPECADDR4TOP_FILTERBYTEMASK_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for ETH_SPECADDR4TOP */
#define ETH_SPECADDR4TOP_FILTERBYTEMASK_DEFAULT          (_ETH_SPECADDR4TOP_FILTERBYTEMASK_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_SPECADDR4TOP */

/* Bit fields for ETH SPECTYPE1 */
#define _ETH_SPECTYPE1_RESETVALUE                        0x00000000UL                           /**< Default value for ETH_SPECTYPE1 */
#define _ETH_SPECTYPE1_MASK                              0x8000FFFFUL                           /**< Mask for ETH_SPECTYPE1 */
#define _ETH_SPECTYPE1_MATCH_SHIFT                       0                                      /**< Shift value for ETH_MATCH */
#define _ETH_SPECTYPE1_MATCH_MASK                        0xFFFFUL                               /**< Bit mask for ETH_MATCH */
#define _ETH_SPECTYPE1_MATCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE1 */
#define ETH_SPECTYPE1_MATCH_DEFAULT                      (_ETH_SPECTYPE1_MATCH_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETH_SPECTYPE1 */
#define ETH_SPECTYPE1_ENBCOPY                            (0x1UL << 31)                          /**< Enable copying of type ID match 1 matched frames. */
#define _ETH_SPECTYPE1_ENBCOPY_SHIFT                     31                                     /**< Shift value for ETH_ENBCOPY */
#define _ETH_SPECTYPE1_ENBCOPY_MASK                      0x80000000UL                           /**< Bit mask for ETH_ENBCOPY */
#define _ETH_SPECTYPE1_ENBCOPY_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE1 */
#define ETH_SPECTYPE1_ENBCOPY_DEFAULT                    (_ETH_SPECTYPE1_ENBCOPY_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_SPECTYPE1 */

/* Bit fields for ETH SPECTYPE2 */
#define _ETH_SPECTYPE2_RESETVALUE                        0x00000000UL                           /**< Default value for ETH_SPECTYPE2 */
#define _ETH_SPECTYPE2_MASK                              0x8000FFFFUL                           /**< Mask for ETH_SPECTYPE2 */
#define _ETH_SPECTYPE2_MATCH_SHIFT                       0                                      /**< Shift value for ETH_MATCH */
#define _ETH_SPECTYPE2_MATCH_MASK                        0xFFFFUL                               /**< Bit mask for ETH_MATCH */
#define _ETH_SPECTYPE2_MATCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE2 */
#define ETH_SPECTYPE2_MATCH_DEFAULT                      (_ETH_SPECTYPE2_MATCH_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETH_SPECTYPE2 */
#define ETH_SPECTYPE2_ENBCOPY                            (0x1UL << 31)                          /**< Enable copying of type ID match 2 matched frames. */
#define _ETH_SPECTYPE2_ENBCOPY_SHIFT                     31                                     /**< Shift value for ETH_ENBCOPY */
#define _ETH_SPECTYPE2_ENBCOPY_MASK                      0x80000000UL                           /**< Bit mask for ETH_ENBCOPY */
#define _ETH_SPECTYPE2_ENBCOPY_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE2 */
#define ETH_SPECTYPE2_ENBCOPY_DEFAULT                    (_ETH_SPECTYPE2_ENBCOPY_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_SPECTYPE2 */

/* Bit fields for ETH SPECTYPE3 */
#define _ETH_SPECTYPE3_RESETVALUE                        0x00000000UL                           /**< Default value for ETH_SPECTYPE3 */
#define _ETH_SPECTYPE3_MASK                              0x8000FFFFUL                           /**< Mask for ETH_SPECTYPE3 */
#define _ETH_SPECTYPE3_MATCH_SHIFT                       0                                      /**< Shift value for ETH_MATCH */
#define _ETH_SPECTYPE3_MATCH_MASK                        0xFFFFUL                               /**< Bit mask for ETH_MATCH */
#define _ETH_SPECTYPE3_MATCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE3 */
#define ETH_SPECTYPE3_MATCH_DEFAULT                      (_ETH_SPECTYPE3_MATCH_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETH_SPECTYPE3 */
#define ETH_SPECTYPE3_ENBCOPY                            (0x1UL << 31)                          /**< Enable copying of type ID match 3 matched frames. */
#define _ETH_SPECTYPE3_ENBCOPY_SHIFT                     31                                     /**< Shift value for ETH_ENBCOPY */
#define _ETH_SPECTYPE3_ENBCOPY_MASK                      0x80000000UL                           /**< Bit mask for ETH_ENBCOPY */
#define _ETH_SPECTYPE3_ENBCOPY_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE3 */
#define ETH_SPECTYPE3_ENBCOPY_DEFAULT                    (_ETH_SPECTYPE3_ENBCOPY_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_SPECTYPE3 */

/* Bit fields for ETH SPECTYPE4 */
#define _ETH_SPECTYPE4_RESETVALUE                        0x00000000UL                           /**< Default value for ETH_SPECTYPE4 */
#define _ETH_SPECTYPE4_MASK                              0x8000FFFFUL                           /**< Mask for ETH_SPECTYPE4 */
#define _ETH_SPECTYPE4_MATCH_SHIFT                       0                                      /**< Shift value for ETH_MATCH */
#define _ETH_SPECTYPE4_MATCH_MASK                        0xFFFFUL                               /**< Bit mask for ETH_MATCH */
#define _ETH_SPECTYPE4_MATCH_DEFAULT                     0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE4 */
#define ETH_SPECTYPE4_MATCH_DEFAULT                      (_ETH_SPECTYPE4_MATCH_DEFAULT << 0)    /**< Shifted mode DEFAULT for ETH_SPECTYPE4 */
#define ETH_SPECTYPE4_ENBCOPY                            (0x1UL << 31)                          /**< Enable copying of type ID match 4 matched frames. */
#define _ETH_SPECTYPE4_ENBCOPY_SHIFT                     31                                     /**< Shift value for ETH_ENBCOPY */
#define _ETH_SPECTYPE4_ENBCOPY_MASK                      0x80000000UL                           /**< Bit mask for ETH_ENBCOPY */
#define _ETH_SPECTYPE4_ENBCOPY_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for ETH_SPECTYPE4 */
#define ETH_SPECTYPE4_ENBCOPY_DEFAULT                    (_ETH_SPECTYPE4_ENBCOPY_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_SPECTYPE4 */

/* Bit fields for ETH WOLREG */
#define _ETH_WOLREG_RESETVALUE                           0x00000000UL                         /**< Default value for ETH_WOLREG */
#define _ETH_WOLREG_MASK                                 0x000FFFFFUL                         /**< Mask for ETH_WOLREG */
#define _ETH_WOLREG_ADDR_SHIFT                           0                                    /**< Shift value for ETH_ADDR */
#define _ETH_WOLREG_ADDR_MASK                            0xFFFFUL                             /**< Bit mask for ETH_ADDR */
#define _ETH_WOLREG_ADDR_DEFAULT                         0x00000000UL                         /**< Mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_ADDR_DEFAULT                          (_ETH_WOLREG_ADDR_DEFAULT << 0)      /**< Shifted mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK0                              (0x1UL << 16)                        /**< Wake on LAN magic packet event enable */
#define _ETH_WOLREG_WOLMASK0_SHIFT                       16                                   /**< Shift value for ETH_WOLMASK0 */
#define _ETH_WOLREG_WOLMASK0_MASK                        0x10000UL                            /**< Bit mask for ETH_WOLMASK0 */
#define _ETH_WOLREG_WOLMASK0_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK0_DEFAULT                      (_ETH_WOLREG_WOLMASK0_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK1                              (0x1UL << 17)                        /**< Wake on LAN ARP request event enable */
#define _ETH_WOLREG_WOLMASK1_SHIFT                       17                                   /**< Shift value for ETH_WOLMASK1 */
#define _ETH_WOLREG_WOLMASK1_MASK                        0x20000UL                            /**< Bit mask for ETH_WOLMASK1 */
#define _ETH_WOLREG_WOLMASK1_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK1_DEFAULT                      (_ETH_WOLREG_WOLMASK1_DEFAULT << 17) /**< Shifted mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK2                              (0x1UL << 18)                        /**< Wake on LAN specific address register 1 event enable */
#define _ETH_WOLREG_WOLMASK2_SHIFT                       18                                   /**< Shift value for ETH_WOLMASK2 */
#define _ETH_WOLREG_WOLMASK2_MASK                        0x40000UL                            /**< Bit mask for ETH_WOLMASK2 */
#define _ETH_WOLREG_WOLMASK2_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK2_DEFAULT                      (_ETH_WOLREG_WOLMASK2_DEFAULT << 18) /**< Shifted mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK3                              (0x1UL << 19)                        /**< Wake on LAN multicast hash event enable */
#define _ETH_WOLREG_WOLMASK3_SHIFT                       19                                   /**< Shift value for ETH_WOLMASK3 */
#define _ETH_WOLREG_WOLMASK3_MASK                        0x80000UL                            /**< Bit mask for ETH_WOLMASK3 */
#define _ETH_WOLREG_WOLMASK3_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for ETH_WOLREG */
#define ETH_WOLREG_WOLMASK3_DEFAULT                      (_ETH_WOLREG_WOLMASK3_DEFAULT << 19) /**< Shifted mode DEFAULT for ETH_WOLREG */

/* Bit fields for ETH STRETCHRATIO */
#define _ETH_STRETCHRATIO_RESETVALUE                     0x00000000UL                                /**< Default value for ETH_STRETCHRATIO */
#define _ETH_STRETCHRATIO_MASK                           0x0000FFFFUL                                /**< Mask for ETH_STRETCHRATIO */
#define _ETH_STRETCHRATIO_IPGSTRETCH_SHIFT               0                                           /**< Shift value for ETH_IPGSTRETCH */
#define _ETH_STRETCHRATIO_IPGSTRETCH_MASK                0xFFFFUL                                    /**< Bit mask for ETH_IPGSTRETCH */
#define _ETH_STRETCHRATIO_IPGSTRETCH_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for ETH_STRETCHRATIO */
#define ETH_STRETCHRATIO_IPGSTRETCH_DEFAULT              (_ETH_STRETCHRATIO_IPGSTRETCH_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_STRETCHRATIO */

/* Bit fields for ETH STACKEDVLAN */
#define _ETH_STACKEDVLAN_RESETVALUE                      0x00000000UL                                   /**< Default value for ETH_STACKEDVLAN */
#define _ETH_STACKEDVLAN_MASK                            0x8000FFFFUL                                   /**< Mask for ETH_STACKEDVLAN */
#define _ETH_STACKEDVLAN_MATCH_SHIFT                     0                                              /**< Shift value for ETH_MATCH */
#define _ETH_STACKEDVLAN_MATCH_MASK                      0xFFFFUL                                       /**< Bit mask for ETH_MATCH */
#define _ETH_STACKEDVLAN_MATCH_DEFAULT                   0x00000000UL                                   /**< Mode DEFAULT for ETH_STACKEDVLAN */
#define ETH_STACKEDVLAN_MATCH_DEFAULT                    (_ETH_STACKEDVLAN_MATCH_DEFAULT << 0)          /**< Shifted mode DEFAULT for ETH_STACKEDVLAN */
#define ETH_STACKEDVLAN_ENBPROCESSING                    (0x1UL << 31)                                  /**< Enable stacked VLAN processing mode */
#define _ETH_STACKEDVLAN_ENBPROCESSING_SHIFT             31                                             /**< Shift value for ETH_ENBPROCESSING */
#define _ETH_STACKEDVLAN_ENBPROCESSING_MASK              0x80000000UL                                   /**< Bit mask for ETH_ENBPROCESSING */
#define _ETH_STACKEDVLAN_ENBPROCESSING_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for ETH_STACKEDVLAN */
#define ETH_STACKEDVLAN_ENBPROCESSING_DEFAULT            (_ETH_STACKEDVLAN_ENBPROCESSING_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_STACKEDVLAN */

/* Bit fields for ETH TXPFCPAUSE */
#define _ETH_TXPFCPAUSE_RESETVALUE                       0x00000000UL                             /**< Default value for ETH_TXPFCPAUSE */
#define _ETH_TXPFCPAUSE_MASK                             0x0000FFFFUL                             /**< Mask for ETH_TXPFCPAUSE */
#define _ETH_TXPFCPAUSE_VECTORENB_SHIFT                  0                                        /**< Shift value for ETH_VECTORENB */
#define _ETH_TXPFCPAUSE_VECTORENB_MASK                   0xFFUL                                   /**< Bit mask for ETH_VECTORENB */
#define _ETH_TXPFCPAUSE_VECTORENB_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_TXPFCPAUSE */
#define ETH_TXPFCPAUSE_VECTORENB_DEFAULT                 (_ETH_TXPFCPAUSE_VECTORENB_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TXPFCPAUSE */
#define _ETH_TXPFCPAUSE_VECTOR_SHIFT                     8                                        /**< Shift value for ETH_VECTOR */
#define _ETH_TXPFCPAUSE_VECTOR_MASK                      0xFF00UL                                 /**< Bit mask for ETH_VECTOR */
#define _ETH_TXPFCPAUSE_VECTOR_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for ETH_TXPFCPAUSE */
#define ETH_TXPFCPAUSE_VECTOR_DEFAULT                    (_ETH_TXPFCPAUSE_VECTOR_DEFAULT << 8)    /**< Shifted mode DEFAULT for ETH_TXPFCPAUSE */

/* Bit fields for ETH MASKADD1BOTTOM */
#define _ETH_MASKADD1BOTTOM_RESETVALUE                   0x00000000UL                                /**< Default value for ETH_MASKADD1BOTTOM */
#define _ETH_MASKADD1BOTTOM_MASK                         0xFFFFFFFFUL                                /**< Mask for ETH_MASKADD1BOTTOM */
#define _ETH_MASKADD1BOTTOM_ADDRMASK_SHIFT               0                                           /**< Shift value for ETH_ADDRMASK */
#define _ETH_MASKADD1BOTTOM_ADDRMASK_MASK                0xFFFFFFFFUL                                /**< Bit mask for ETH_ADDRMASK */
#define _ETH_MASKADD1BOTTOM_ADDRMASK_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for ETH_MASKADD1BOTTOM */
#define ETH_MASKADD1BOTTOM_ADDRMASK_DEFAULT              (_ETH_MASKADD1BOTTOM_ADDRMASK_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_MASKADD1BOTTOM */

/* Bit fields for ETH MASKADD1TOP */
#define _ETH_MASKADD1TOP_RESETVALUE                      0x00000000UL                             /**< Default value for ETH_MASKADD1TOP */
#define _ETH_MASKADD1TOP_MASK                            0x0000FFFFUL                             /**< Mask for ETH_MASKADD1TOP */
#define _ETH_MASKADD1TOP_ADDRMASK_SHIFT                  0                                        /**< Shift value for ETH_ADDRMASK */
#define _ETH_MASKADD1TOP_ADDRMASK_MASK                   0xFFFFUL                                 /**< Bit mask for ETH_ADDRMASK */
#define _ETH_MASKADD1TOP_ADDRMASK_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_MASKADD1TOP */
#define ETH_MASKADD1TOP_ADDRMASK_DEFAULT                 (_ETH_MASKADD1TOP_ADDRMASK_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_MASKADD1TOP */

/* Bit fields for ETH RXPTPUNICAST */
#define _ETH_RXPTPUNICAST_RESETVALUE                     0x00000000UL                          /**< Default value for ETH_RXPTPUNICAST */
#define _ETH_RXPTPUNICAST_MASK                           0xFFFFFFFFUL                          /**< Mask for ETH_RXPTPUNICAST */
#define _ETH_RXPTPUNICAST_ADDR_SHIFT                     0                                     /**< Shift value for ETH_ADDR */
#define _ETH_RXPTPUNICAST_ADDR_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_ADDR */
#define _ETH_RXPTPUNICAST_ADDR_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_RXPTPUNICAST */
#define ETH_RXPTPUNICAST_ADDR_DEFAULT                    (_ETH_RXPTPUNICAST_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXPTPUNICAST */

/* Bit fields for ETH TXPTPUNICAST */
#define _ETH_TXPTPUNICAST_RESETVALUE                     0x00000000UL                          /**< Default value for ETH_TXPTPUNICAST */
#define _ETH_TXPTPUNICAST_MASK                           0xFFFFFFFFUL                          /**< Mask for ETH_TXPTPUNICAST */
#define _ETH_TXPTPUNICAST_ADDR_SHIFT                     0                                     /**< Shift value for ETH_ADDR */
#define _ETH_TXPTPUNICAST_ADDR_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_ADDR */
#define _ETH_TXPTPUNICAST_ADDR_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TXPTPUNICAST */
#define ETH_TXPTPUNICAST_ADDR_DEFAULT                    (_ETH_TXPTPUNICAST_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TXPTPUNICAST */

/* Bit fields for ETH TSUNSECCMP */
#define _ETH_TSUNSECCMP_RESETVALUE                       0x00000000UL                           /**< Default value for ETH_TSUNSECCMP */
#define _ETH_TSUNSECCMP_MASK                             0x003FFFFFUL                           /**< Mask for ETH_TSUNSECCMP */
#define _ETH_TSUNSECCMP_COMPVAL_SHIFT                    0                                      /**< Shift value for ETH_COMPVAL */
#define _ETH_TSUNSECCMP_COMPVAL_MASK                     0x3FFFFFUL                             /**< Bit mask for ETH_COMPVAL */
#define _ETH_TSUNSECCMP_COMPVAL_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUNSECCMP */
#define ETH_TSUNSECCMP_COMPVAL_DEFAULT                   (_ETH_TSUNSECCMP_COMPVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUNSECCMP */

/* Bit fields for ETH TSUSECCMP */
#define _ETH_TSUSECCMP_RESETVALUE                        0x00000000UL                          /**< Default value for ETH_TSUSECCMP */
#define _ETH_TSUSECCMP_MASK                              0xFFFFFFFFUL                          /**< Mask for ETH_TSUSECCMP */
#define _ETH_TSUSECCMP_COMPVAL_SHIFT                     0                                     /**< Shift value for ETH_COMPVAL */
#define _ETH_TSUSECCMP_COMPVAL_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_COMPVAL */
#define _ETH_TSUSECCMP_COMPVAL_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TSUSECCMP */
#define ETH_TSUSECCMP_COMPVAL_DEFAULT                    (_ETH_TSUSECCMP_COMPVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUSECCMP */

/* Bit fields for ETH TSUMSBSECCMP */
#define _ETH_TSUMSBSECCMP_RESETVALUE                     0x00000000UL                             /**< Default value for ETH_TSUMSBSECCMP */
#define _ETH_TSUMSBSECCMP_MASK                           0x0000FFFFUL                             /**< Mask for ETH_TSUMSBSECCMP */
#define _ETH_TSUMSBSECCMP_COMPVAL_SHIFT                  0                                        /**< Shift value for ETH_COMPVAL */
#define _ETH_TSUMSBSECCMP_COMPVAL_MASK                   0xFFFFUL                                 /**< Bit mask for ETH_COMPVAL */
#define _ETH_TSUMSBSECCMP_COMPVAL_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_TSUMSBSECCMP */
#define ETH_TSUMSBSECCMP_COMPVAL_DEFAULT                 (_ETH_TSUMSBSECCMP_COMPVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUMSBSECCMP */

/* Bit fields for ETH TSUPTPTXMSBSEC */
#define _ETH_TSUPTPTXMSBSEC_RESETVALUE                   0x00000000UL                                /**< Default value for ETH_TSUPTPTXMSBSEC */
#define _ETH_TSUPTPTXMSBSEC_MASK                         0x0000FFFFUL                                /**< Mask for ETH_TSUPTPTXMSBSEC */
#define _ETH_TSUPTPTXMSBSEC_TIMERSEC_SHIFT               0                                           /**< Shift value for ETH_TIMERSEC */
#define _ETH_TSUPTPTXMSBSEC_TIMERSEC_MASK                0xFFFFUL                                    /**< Bit mask for ETH_TIMERSEC */
#define _ETH_TSUPTPTXMSBSEC_TIMERSEC_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for ETH_TSUPTPTXMSBSEC */
#define ETH_TSUPTPTXMSBSEC_TIMERSEC_DEFAULT              (_ETH_TSUPTPTXMSBSEC_TIMERSEC_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPTXMSBSEC */

/* Bit fields for ETH TSUPTPRXMSBSEC */
#define _ETH_TSUPTPRXMSBSEC_RESETVALUE                   0x00000000UL                                /**< Default value for ETH_TSUPTPRXMSBSEC */
#define _ETH_TSUPTPRXMSBSEC_MASK                         0x0000FFFFUL                                /**< Mask for ETH_TSUPTPRXMSBSEC */
#define _ETH_TSUPTPRXMSBSEC_TIMERSEC_SHIFT               0                                           /**< Shift value for ETH_TIMERSEC */
#define _ETH_TSUPTPRXMSBSEC_TIMERSEC_MASK                0xFFFFUL                                    /**< Bit mask for ETH_TIMERSEC */
#define _ETH_TSUPTPRXMSBSEC_TIMERSEC_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for ETH_TSUPTPRXMSBSEC */
#define ETH_TSUPTPRXMSBSEC_TIMERSEC_DEFAULT              (_ETH_TSUPTPRXMSBSEC_TIMERSEC_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPRXMSBSEC */

/* Bit fields for ETH TSUPEERTXMSBSEC */
#define _ETH_TSUPEERTXMSBSEC_RESETVALUE                  0x00000000UL                                 /**< Default value for ETH_TSUPEERTXMSBSEC */
#define _ETH_TSUPEERTXMSBSEC_MASK                        0x0000FFFFUL                                 /**< Mask for ETH_TSUPEERTXMSBSEC */
#define _ETH_TSUPEERTXMSBSEC_TIMERSEC_SHIFT              0                                            /**< Shift value for ETH_TIMERSEC */
#define _ETH_TSUPEERTXMSBSEC_TIMERSEC_MASK               0xFFFFUL                                     /**< Bit mask for ETH_TIMERSEC */
#define _ETH_TSUPEERTXMSBSEC_TIMERSEC_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for ETH_TSUPEERTXMSBSEC */
#define ETH_TSUPEERTXMSBSEC_TIMERSEC_DEFAULT             (_ETH_TSUPEERTXMSBSEC_TIMERSEC_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERTXMSBSEC */

/* Bit fields for ETH TSUPEERRXMSBSEC */
#define _ETH_TSUPEERRXMSBSEC_RESETVALUE                  0x00000000UL                                 /**< Default value for ETH_TSUPEERRXMSBSEC */
#define _ETH_TSUPEERRXMSBSEC_MASK                        0x0000FFFFUL                                 /**< Mask for ETH_TSUPEERRXMSBSEC */
#define _ETH_TSUPEERRXMSBSEC_TIMERSEC_SHIFT              0                                            /**< Shift value for ETH_TIMERSEC */
#define _ETH_TSUPEERRXMSBSEC_TIMERSEC_MASK               0xFFFFUL                                     /**< Bit mask for ETH_TIMERSEC */
#define _ETH_TSUPEERRXMSBSEC_TIMERSEC_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for ETH_TSUPEERRXMSBSEC */
#define ETH_TSUPEERRXMSBSEC_TIMERSEC_DEFAULT             (_ETH_TSUPEERRXMSBSEC_TIMERSEC_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERRXMSBSEC */

/* Bit fields for ETH OCTETSTXEDBOTTOM */
#define _ETH_OCTETSTXEDBOTTOM_RESETVALUE                 0x00000000UL                               /**< Default value for ETH_OCTETSTXEDBOTTOM */
#define _ETH_OCTETSTXEDBOTTOM_MASK                       0xFFFFFFFFUL                               /**< Mask for ETH_OCTETSTXEDBOTTOM */
#define _ETH_OCTETSTXEDBOTTOM_COUNT_SHIFT                0                                          /**< Shift value for ETH_COUNT */
#define _ETH_OCTETSTXEDBOTTOM_COUNT_MASK                 0xFFFFFFFFUL                               /**< Bit mask for ETH_COUNT */
#define _ETH_OCTETSTXEDBOTTOM_COUNT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for ETH_OCTETSTXEDBOTTOM */
#define ETH_OCTETSTXEDBOTTOM_COUNT_DEFAULT               (_ETH_OCTETSTXEDBOTTOM_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_OCTETSTXEDBOTTOM */

/* Bit fields for ETH OCTETSTXEDTOP */
#define _ETH_OCTETSTXEDTOP_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_OCTETSTXEDTOP */
#define _ETH_OCTETSTXEDTOP_MASK                          0x0000FFFFUL                            /**< Mask for ETH_OCTETSTXEDTOP */
#define _ETH_OCTETSTXEDTOP_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_OCTETSTXEDTOP_COUNT_MASK                    0xFFFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_OCTETSTXEDTOP_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_OCTETSTXEDTOP */
#define ETH_OCTETSTXEDTOP_COUNT_DEFAULT                  (_ETH_OCTETSTXEDTOP_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_OCTETSTXEDTOP */

/* Bit fields for ETH FRAMESTXEDOK */
#define _ETH_FRAMESTXEDOK_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESTXEDOK */
#define _ETH_FRAMESTXEDOK_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESTXEDOK */
#define _ETH_FRAMESTXEDOK_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXEDOK_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXEDOK_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESTXEDOK */
#define ETH_FRAMESTXEDOK_COUNT_DEFAULT                   (_ETH_FRAMESTXEDOK_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXEDOK */

/* Bit fields for ETH BROADCASTTXED */
#define _ETH_BROADCASTTXED_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_BROADCASTTXED */
#define _ETH_BROADCASTTXED_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_BROADCASTTXED */
#define _ETH_BROADCASTTXED_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_BROADCASTTXED_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_BROADCASTTXED_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_BROADCASTTXED */
#define ETH_BROADCASTTXED_COUNT_DEFAULT                  (_ETH_BROADCASTTXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_BROADCASTTXED */

/* Bit fields for ETH MULTICASTTXED */
#define _ETH_MULTICASTTXED_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_MULTICASTTXED */
#define _ETH_MULTICASTTXED_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_MULTICASTTXED */
#define _ETH_MULTICASTTXED_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_MULTICASTTXED_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_MULTICASTTXED_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_MULTICASTTXED */
#define ETH_MULTICASTTXED_COUNT_DEFAULT                  (_ETH_MULTICASTTXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_MULTICASTTXED */

/* Bit fields for ETH PFRAMESTXED */
#define _ETH_PFRAMESTXED_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_PFRAMESTXED */
#define _ETH_PFRAMESTXED_MASK                            0x0000FFFFUL                          /**< Mask for ETH_PFRAMESTXED */
#define _ETH_PFRAMESTXED_COUNT_SHIFT                     0                                     /**< Shift value for ETH_COUNT */
#define _ETH_PFRAMESTXED_COUNT_MASK                      0xFFFFUL                              /**< Bit mask for ETH_COUNT */
#define _ETH_PFRAMESTXED_COUNT_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_PFRAMESTXED */
#define ETH_PFRAMESTXED_COUNT_DEFAULT                    (_ETH_PFRAMESTXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_PFRAMESTXED */

/* Bit fields for ETH FRAMESTXED64 */
#define _ETH_FRAMESTXED64_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESTXED64 */
#define _ETH_FRAMESTXED64_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESTXED64 */
#define _ETH_FRAMESTXED64_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED64_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED64_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESTXED64 */
#define ETH_FRAMESTXED64_COUNT_DEFAULT                   (_ETH_FRAMESTXED64_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED64 */

/* Bit fields for ETH FRAMESTXED65 */
#define _ETH_FRAMESTXED65_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESTXED65 */
#define _ETH_FRAMESTXED65_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESTXED65 */
#define _ETH_FRAMESTXED65_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED65_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED65_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESTXED65 */
#define ETH_FRAMESTXED65_COUNT_DEFAULT                   (_ETH_FRAMESTXED65_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED65 */

/* Bit fields for ETH FRAMESTXED128 */
#define _ETH_FRAMESTXED128_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESTXED128 */
#define _ETH_FRAMESTXED128_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESTXED128 */
#define _ETH_FRAMESTXED128_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED128_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED128_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESTXED128 */
#define ETH_FRAMESTXED128_COUNT_DEFAULT                  (_ETH_FRAMESTXED128_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED128 */

/* Bit fields for ETH FRAMESTXED256 */
#define _ETH_FRAMESTXED256_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESTXED256 */
#define _ETH_FRAMESTXED256_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESTXED256 */
#define _ETH_FRAMESTXED256_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED256_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED256_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESTXED256 */
#define ETH_FRAMESTXED256_COUNT_DEFAULT                  (_ETH_FRAMESTXED256_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED256 */

/* Bit fields for ETH FRAMESTXED512 */
#define _ETH_FRAMESTXED512_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESTXED512 */
#define _ETH_FRAMESTXED512_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESTXED512 */
#define _ETH_FRAMESTXED512_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED512_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED512_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESTXED512 */
#define ETH_FRAMESTXED512_COUNT_DEFAULT                  (_ETH_FRAMESTXED512_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED512 */

/* Bit fields for ETH FRAMESTXED1024 */
#define _ETH_FRAMESTXED1024_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_FRAMESTXED1024 */
#define _ETH_FRAMESTXED1024_MASK                         0xFFFFFFFFUL                             /**< Mask for ETH_FRAMESTXED1024 */
#define _ETH_FRAMESTXED1024_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED1024_COUNT_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED1024_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_FRAMESTXED1024 */
#define ETH_FRAMESTXED1024_COUNT_DEFAULT                 (_ETH_FRAMESTXED1024_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED1024 */

/* Bit fields for ETH FRAMESTXED1519 */
#define _ETH_FRAMESTXED1519_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_FRAMESTXED1519 */
#define _ETH_FRAMESTXED1519_MASK                         0xFFFFFFFFUL                             /**< Mask for ETH_FRAMESTXED1519 */
#define _ETH_FRAMESTXED1519_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESTXED1519_COUNT_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESTXED1519_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_FRAMESTXED1519 */
#define ETH_FRAMESTXED1519_COUNT_DEFAULT                 (_ETH_FRAMESTXED1519_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESTXED1519 */

/* Bit fields for ETH TXUNDERRUNS */
#define _ETH_TXUNDERRUNS_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_TXUNDERRUNS */
#define _ETH_TXUNDERRUNS_MASK                            0x000003FFUL                          /**< Mask for ETH_TXUNDERRUNS */
#define _ETH_TXUNDERRUNS_COUNT_SHIFT                     0                                     /**< Shift value for ETH_COUNT */
#define _ETH_TXUNDERRUNS_COUNT_MASK                      0x3FFUL                               /**< Bit mask for ETH_COUNT */
#define _ETH_TXUNDERRUNS_COUNT_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TXUNDERRUNS */
#define ETH_TXUNDERRUNS_COUNT_DEFAULT                    (_ETH_TXUNDERRUNS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TXUNDERRUNS */

/* Bit fields for ETH SINGLECOLS */
#define _ETH_SINGLECOLS_RESETVALUE                       0x00000000UL                         /**< Default value for ETH_SINGLECOLS */
#define _ETH_SINGLECOLS_MASK                             0x0003FFFFUL                         /**< Mask for ETH_SINGLECOLS */
#define _ETH_SINGLECOLS_COUNT_SHIFT                      0                                    /**< Shift value for ETH_COUNT */
#define _ETH_SINGLECOLS_COUNT_MASK                       0x3FFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_SINGLECOLS_COUNT_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_SINGLECOLS */
#define ETH_SINGLECOLS_COUNT_DEFAULT                     (_ETH_SINGLECOLS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_SINGLECOLS */

/* Bit fields for ETH MULTICOLS */
#define _ETH_MULTICOLS_RESETVALUE                        0x00000000UL                        /**< Default value for ETH_MULTICOLS */
#define _ETH_MULTICOLS_MASK                              0x0003FFFFUL                        /**< Mask for ETH_MULTICOLS */
#define _ETH_MULTICOLS_COUNT_SHIFT                       0                                   /**< Shift value for ETH_COUNT */
#define _ETH_MULTICOLS_COUNT_MASK                        0x3FFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_MULTICOLS_COUNT_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for ETH_MULTICOLS */
#define ETH_MULTICOLS_COUNT_DEFAULT                      (_ETH_MULTICOLS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_MULTICOLS */

/* Bit fields for ETH EXCESSCOLS */
#define _ETH_EXCESSCOLS_RESETVALUE                       0x00000000UL                         /**< Default value for ETH_EXCESSCOLS */
#define _ETH_EXCESSCOLS_MASK                             0x000003FFUL                         /**< Mask for ETH_EXCESSCOLS */
#define _ETH_EXCESSCOLS_COUNT_SHIFT                      0                                    /**< Shift value for ETH_COUNT */
#define _ETH_EXCESSCOLS_COUNT_MASK                       0x3FFUL                              /**< Bit mask for ETH_COUNT */
#define _ETH_EXCESSCOLS_COUNT_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_EXCESSCOLS */
#define ETH_EXCESSCOLS_COUNT_DEFAULT                     (_ETH_EXCESSCOLS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_EXCESSCOLS */

/* Bit fields for ETH LATECOLS */
#define _ETH_LATECOLS_RESETVALUE                         0x00000000UL                       /**< Default value for ETH_LATECOLS */
#define _ETH_LATECOLS_MASK                               0x000003FFUL                       /**< Mask for ETH_LATECOLS */
#define _ETH_LATECOLS_COUNT_SHIFT                        0                                  /**< Shift value for ETH_COUNT */
#define _ETH_LATECOLS_COUNT_MASK                         0x3FFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_LATECOLS_COUNT_DEFAULT                      0x00000000UL                       /**< Mode DEFAULT for ETH_LATECOLS */
#define ETH_LATECOLS_COUNT_DEFAULT                       (_ETH_LATECOLS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_LATECOLS */

/* Bit fields for ETH DEFERREDFRAMES */
#define _ETH_DEFERREDFRAMES_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_DEFERREDFRAMES */
#define _ETH_DEFERREDFRAMES_MASK                         0x0003FFFFUL                             /**< Mask for ETH_DEFERREDFRAMES */
#define _ETH_DEFERREDFRAMES_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_DEFERREDFRAMES_COUNT_MASK                   0x3FFFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_DEFERREDFRAMES_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_DEFERREDFRAMES */
#define ETH_DEFERREDFRAMES_COUNT_DEFAULT                 (_ETH_DEFERREDFRAMES_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_DEFERREDFRAMES */

/* Bit fields for ETH CRSERRS */
#define _ETH_CRSERRS_RESETVALUE                          0x00000000UL                      /**< Default value for ETH_CRSERRS */
#define _ETH_CRSERRS_MASK                                0x000003FFUL                      /**< Mask for ETH_CRSERRS */
#define _ETH_CRSERRS_COUNT_SHIFT                         0                                 /**< Shift value for ETH_COUNT */
#define _ETH_CRSERRS_COUNT_MASK                          0x3FFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_CRSERRS_COUNT_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for ETH_CRSERRS */
#define ETH_CRSERRS_COUNT_DEFAULT                        (_ETH_CRSERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_CRSERRS */

/* Bit fields for ETH OCTETSRXEDBOTTOM */
#define _ETH_OCTETSRXEDBOTTOM_RESETVALUE                 0x00000000UL                               /**< Default value for ETH_OCTETSRXEDBOTTOM */
#define _ETH_OCTETSRXEDBOTTOM_MASK                       0xFFFFFFFFUL                               /**< Mask for ETH_OCTETSRXEDBOTTOM */
#define _ETH_OCTETSRXEDBOTTOM_COUNT_SHIFT                0                                          /**< Shift value for ETH_COUNT */
#define _ETH_OCTETSRXEDBOTTOM_COUNT_MASK                 0xFFFFFFFFUL                               /**< Bit mask for ETH_COUNT */
#define _ETH_OCTETSRXEDBOTTOM_COUNT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for ETH_OCTETSRXEDBOTTOM */
#define ETH_OCTETSRXEDBOTTOM_COUNT_DEFAULT               (_ETH_OCTETSRXEDBOTTOM_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_OCTETSRXEDBOTTOM */

/* Bit fields for ETH OCTETSRXEDTOP */
#define _ETH_OCTETSRXEDTOP_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_OCTETSRXEDTOP */
#define _ETH_OCTETSRXEDTOP_MASK                          0x0000FFFFUL                            /**< Mask for ETH_OCTETSRXEDTOP */
#define _ETH_OCTETSRXEDTOP_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_OCTETSRXEDTOP_COUNT_MASK                    0xFFFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_OCTETSRXEDTOP_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_OCTETSRXEDTOP */
#define ETH_OCTETSRXEDTOP_COUNT_DEFAULT                  (_ETH_OCTETSRXEDTOP_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_OCTETSRXEDTOP */

/* Bit fields for ETH FRAMESRXEDOK */
#define _ETH_FRAMESRXEDOK_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESRXEDOK */
#define _ETH_FRAMESRXEDOK_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESRXEDOK */
#define _ETH_FRAMESRXEDOK_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXEDOK_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXEDOK_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESRXEDOK */
#define ETH_FRAMESRXEDOK_COUNT_DEFAULT                   (_ETH_FRAMESRXEDOK_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXEDOK */

/* Bit fields for ETH BROADCASTRXED */
#define _ETH_BROADCASTRXED_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_BROADCASTRXED */
#define _ETH_BROADCASTRXED_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_BROADCASTRXED */
#define _ETH_BROADCASTRXED_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_BROADCASTRXED_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_BROADCASTRXED_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_BROADCASTRXED */
#define ETH_BROADCASTRXED_COUNT_DEFAULT                  (_ETH_BROADCASTRXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_BROADCASTRXED */

/* Bit fields for ETH MULTICASTRXED */
#define _ETH_MULTICASTRXED_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_MULTICASTRXED */
#define _ETH_MULTICASTRXED_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_MULTICASTRXED */
#define _ETH_MULTICASTRXED_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_MULTICASTRXED_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_MULTICASTRXED_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_MULTICASTRXED */
#define ETH_MULTICASTRXED_COUNT_DEFAULT                  (_ETH_MULTICASTRXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_MULTICASTRXED */

/* Bit fields for ETH PFRAMESRXED */
#define _ETH_PFRAMESRXED_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_PFRAMESRXED */
#define _ETH_PFRAMESRXED_MASK                            0x0000FFFFUL                          /**< Mask for ETH_PFRAMESRXED */
#define _ETH_PFRAMESRXED_COUNT_SHIFT                     0                                     /**< Shift value for ETH_COUNT */
#define _ETH_PFRAMESRXED_COUNT_MASK                      0xFFFFUL                              /**< Bit mask for ETH_COUNT */
#define _ETH_PFRAMESRXED_COUNT_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_PFRAMESRXED */
#define ETH_PFRAMESRXED_COUNT_DEFAULT                    (_ETH_PFRAMESRXED_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_PFRAMESRXED */

/* Bit fields for ETH FRAMESRXED64 */
#define _ETH_FRAMESRXED64_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESRXED64 */
#define _ETH_FRAMESRXED64_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESRXED64 */
#define _ETH_FRAMESRXED64_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED64_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED64_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESRXED64 */
#define ETH_FRAMESRXED64_COUNT_DEFAULT                   (_ETH_FRAMESRXED64_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED64 */

/* Bit fields for ETH FRAMESRXED65 */
#define _ETH_FRAMESRXED65_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_FRAMESRXED65 */
#define _ETH_FRAMESRXED65_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_FRAMESRXED65 */
#define _ETH_FRAMESRXED65_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED65_COUNT_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED65_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_FRAMESRXED65 */
#define ETH_FRAMESRXED65_COUNT_DEFAULT                   (_ETH_FRAMESRXED65_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED65 */

/* Bit fields for ETH FRAMESRXED128 */
#define _ETH_FRAMESRXED128_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESRXED128 */
#define _ETH_FRAMESRXED128_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESRXED128 */
#define _ETH_FRAMESRXED128_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED128_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED128_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESRXED128 */
#define ETH_FRAMESRXED128_COUNT_DEFAULT                  (_ETH_FRAMESRXED128_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED128 */

/* Bit fields for ETH FRAMESRXED256 */
#define _ETH_FRAMESRXED256_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESRXED256 */
#define _ETH_FRAMESRXED256_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESRXED256 */
#define _ETH_FRAMESRXED256_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED256_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED256_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESRXED256 */
#define ETH_FRAMESRXED256_COUNT_DEFAULT                  (_ETH_FRAMESRXED256_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED256 */

/* Bit fields for ETH FRAMESRXED512 */
#define _ETH_FRAMESRXED512_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_FRAMESRXED512 */
#define _ETH_FRAMESRXED512_MASK                          0xFFFFFFFFUL                            /**< Mask for ETH_FRAMESRXED512 */
#define _ETH_FRAMESRXED512_COUNT_SHIFT                   0                                       /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED512_COUNT_MASK                    0xFFFFFFFFUL                            /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED512_COUNT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_FRAMESRXED512 */
#define ETH_FRAMESRXED512_COUNT_DEFAULT                  (_ETH_FRAMESRXED512_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED512 */

/* Bit fields for ETH FRAMESRXED1024 */
#define _ETH_FRAMESRXED1024_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_FRAMESRXED1024 */
#define _ETH_FRAMESRXED1024_MASK                         0xFFFFFFFFUL                             /**< Mask for ETH_FRAMESRXED1024 */
#define _ETH_FRAMESRXED1024_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED1024_COUNT_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED1024_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_FRAMESRXED1024 */
#define ETH_FRAMESRXED1024_COUNT_DEFAULT                 (_ETH_FRAMESRXED1024_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED1024 */

/* Bit fields for ETH FRAMESRXED1519 */
#define _ETH_FRAMESRXED1519_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_FRAMESRXED1519 */
#define _ETH_FRAMESRXED1519_MASK                         0xFFFFFFFFUL                             /**< Mask for ETH_FRAMESRXED1519 */
#define _ETH_FRAMESRXED1519_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_FRAMESRXED1519_COUNT_MASK                   0xFFFFFFFFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_FRAMESRXED1519_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_FRAMESRXED1519 */
#define ETH_FRAMESRXED1519_COUNT_DEFAULT                 (_ETH_FRAMESRXED1519_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FRAMESRXED1519 */

/* Bit fields for ETH UNDERSIZEFRAMES */
#define _ETH_UNDERSIZEFRAMES_RESETVALUE                  0x00000000UL                              /**< Default value for ETH_UNDERSIZEFRAMES */
#define _ETH_UNDERSIZEFRAMES_MASK                        0x000003FFUL                              /**< Mask for ETH_UNDERSIZEFRAMES */
#define _ETH_UNDERSIZEFRAMES_COUNT_SHIFT                 0                                         /**< Shift value for ETH_COUNT */
#define _ETH_UNDERSIZEFRAMES_COUNT_MASK                  0x3FFUL                                   /**< Bit mask for ETH_COUNT */
#define _ETH_UNDERSIZEFRAMES_COUNT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_UNDERSIZEFRAMES */
#define ETH_UNDERSIZEFRAMES_COUNT_DEFAULT                (_ETH_UNDERSIZEFRAMES_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_UNDERSIZEFRAMES */

/* Bit fields for ETH EXCESSIVERXLEN */
#define _ETH_EXCESSIVERXLEN_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_EXCESSIVERXLEN */
#define _ETH_EXCESSIVERXLEN_MASK                         0x000003FFUL                             /**< Mask for ETH_EXCESSIVERXLEN */
#define _ETH_EXCESSIVERXLEN_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_EXCESSIVERXLEN_COUNT_MASK                   0x3FFUL                                  /**< Bit mask for ETH_COUNT */
#define _ETH_EXCESSIVERXLEN_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_EXCESSIVERXLEN */
#define ETH_EXCESSIVERXLEN_COUNT_DEFAULT                 (_ETH_EXCESSIVERXLEN_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_EXCESSIVERXLEN */

/* Bit fields for ETH RXJABBERS */
#define _ETH_RXJABBERS_RESETVALUE                        0x00000000UL                        /**< Default value for ETH_RXJABBERS */
#define _ETH_RXJABBERS_MASK                              0x000003FFUL                        /**< Mask for ETH_RXJABBERS */
#define _ETH_RXJABBERS_COUNT_SHIFT                       0                                   /**< Shift value for ETH_COUNT */
#define _ETH_RXJABBERS_COUNT_MASK                        0x3FFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_RXJABBERS_COUNT_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for ETH_RXJABBERS */
#define ETH_RXJABBERS_COUNT_DEFAULT                      (_ETH_RXJABBERS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXJABBERS */

/* Bit fields for ETH FCSERRS */
#define _ETH_FCSERRS_RESETVALUE                          0x00000000UL                      /**< Default value for ETH_FCSERRS */
#define _ETH_FCSERRS_MASK                                0x000003FFUL                      /**< Mask for ETH_FCSERRS */
#define _ETH_FCSERRS_COUNT_SHIFT                         0                                 /**< Shift value for ETH_COUNT */
#define _ETH_FCSERRS_COUNT_MASK                          0x3FFUL                           /**< Bit mask for ETH_COUNT */
#define _ETH_FCSERRS_COUNT_DEFAULT                       0x00000000UL                      /**< Mode DEFAULT for ETH_FCSERRS */
#define ETH_FCSERRS_COUNT_DEFAULT                        (_ETH_FCSERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_FCSERRS */

/* Bit fields for ETH RXLENERRS */
#define _ETH_RXLENERRS_RESETVALUE                        0x00000000UL                        /**< Default value for ETH_RXLENERRS */
#define _ETH_RXLENERRS_MASK                              0x000003FFUL                        /**< Mask for ETH_RXLENERRS */
#define _ETH_RXLENERRS_COUNT_SHIFT                       0                                   /**< Shift value for ETH_COUNT */
#define _ETH_RXLENERRS_COUNT_MASK                        0x3FFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_RXLENERRS_COUNT_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for ETH_RXLENERRS */
#define ETH_RXLENERRS_COUNT_DEFAULT                      (_ETH_RXLENERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXLENERRS */

/* Bit fields for ETH RXSYMBOLERRS */
#define _ETH_RXSYMBOLERRS_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_RXSYMBOLERRS */
#define _ETH_RXSYMBOLERRS_MASK                           0x000003FFUL                           /**< Mask for ETH_RXSYMBOLERRS */
#define _ETH_RXSYMBOLERRS_COUNT_SHIFT                    0                                      /**< Shift value for ETH_COUNT */
#define _ETH_RXSYMBOLERRS_COUNT_MASK                     0x3FFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_RXSYMBOLERRS_COUNT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_RXSYMBOLERRS */
#define ETH_RXSYMBOLERRS_COUNT_DEFAULT                   (_ETH_RXSYMBOLERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXSYMBOLERRS */

/* Bit fields for ETH ALIGNERRS */
#define _ETH_ALIGNERRS_RESETVALUE                        0x00000000UL                        /**< Default value for ETH_ALIGNERRS */
#define _ETH_ALIGNERRS_MASK                              0x000003FFUL                        /**< Mask for ETH_ALIGNERRS */
#define _ETH_ALIGNERRS_COUNT_SHIFT                       0                                   /**< Shift value for ETH_COUNT */
#define _ETH_ALIGNERRS_COUNT_MASK                        0x3FFUL                             /**< Bit mask for ETH_COUNT */
#define _ETH_ALIGNERRS_COUNT_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for ETH_ALIGNERRS */
#define ETH_ALIGNERRS_COUNT_DEFAULT                      (_ETH_ALIGNERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_ALIGNERRS */

/* Bit fields for ETH RXRESOURCEERRS */
#define _ETH_RXRESOURCEERRS_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_RXRESOURCEERRS */
#define _ETH_RXRESOURCEERRS_MASK                         0x0003FFFFUL                             /**< Mask for ETH_RXRESOURCEERRS */
#define _ETH_RXRESOURCEERRS_COUNT_SHIFT                  0                                        /**< Shift value for ETH_COUNT */
#define _ETH_RXRESOURCEERRS_COUNT_MASK                   0x3FFFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_RXRESOURCEERRS_COUNT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_RXRESOURCEERRS */
#define ETH_RXRESOURCEERRS_COUNT_DEFAULT                 (_ETH_RXRESOURCEERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXRESOURCEERRS */

/* Bit fields for ETH RXOVERRUNS */
#define _ETH_RXOVERRUNS_RESETVALUE                       0x00000000UL                         /**< Default value for ETH_RXOVERRUNS */
#define _ETH_RXOVERRUNS_MASK                             0x000003FFUL                         /**< Mask for ETH_RXOVERRUNS */
#define _ETH_RXOVERRUNS_COUNT_SHIFT                      0                                    /**< Shift value for ETH_COUNT */
#define _ETH_RXOVERRUNS_COUNT_MASK                       0x3FFUL                              /**< Bit mask for ETH_COUNT */
#define _ETH_RXOVERRUNS_COUNT_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_RXOVERRUNS */
#define ETH_RXOVERRUNS_COUNT_DEFAULT                     (_ETH_RXOVERRUNS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXOVERRUNS */

/* Bit fields for ETH RXIPCKERRS */
#define _ETH_RXIPCKERRS_RESETVALUE                       0x00000000UL                         /**< Default value for ETH_RXIPCKERRS */
#define _ETH_RXIPCKERRS_MASK                             0x000000FFUL                         /**< Mask for ETH_RXIPCKERRS */
#define _ETH_RXIPCKERRS_COUNT_SHIFT                      0                                    /**< Shift value for ETH_COUNT */
#define _ETH_RXIPCKERRS_COUNT_MASK                       0xFFUL                               /**< Bit mask for ETH_COUNT */
#define _ETH_RXIPCKERRS_COUNT_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for ETH_RXIPCKERRS */
#define ETH_RXIPCKERRS_COUNT_DEFAULT                     (_ETH_RXIPCKERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXIPCKERRS */

/* Bit fields for ETH RXTCPCKERRS */
#define _ETH_RXTCPCKERRS_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_RXTCPCKERRS */
#define _ETH_RXTCPCKERRS_MASK                            0x000000FFUL                          /**< Mask for ETH_RXTCPCKERRS */
#define _ETH_RXTCPCKERRS_COUNT_SHIFT                     0                                     /**< Shift value for ETH_COUNT */
#define _ETH_RXTCPCKERRS_COUNT_MASK                      0xFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_RXTCPCKERRS_COUNT_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_RXTCPCKERRS */
#define ETH_RXTCPCKERRS_COUNT_DEFAULT                    (_ETH_RXTCPCKERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXTCPCKERRS */

/* Bit fields for ETH RXUDPCKERRS */
#define _ETH_RXUDPCKERRS_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_RXUDPCKERRS */
#define _ETH_RXUDPCKERRS_MASK                            0x000000FFUL                          /**< Mask for ETH_RXUDPCKERRS */
#define _ETH_RXUDPCKERRS_COUNT_SHIFT                     0                                     /**< Shift value for ETH_COUNT */
#define _ETH_RXUDPCKERRS_COUNT_MASK                      0xFFUL                                /**< Bit mask for ETH_COUNT */
#define _ETH_RXUDPCKERRS_COUNT_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_RXUDPCKERRS */
#define ETH_RXUDPCKERRS_COUNT_DEFAULT                    (_ETH_RXUDPCKERRS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXUDPCKERRS */

/* Bit fields for ETH AUTOFLUSHEDPKTS */
#define _ETH_AUTOFLUSHEDPKTS_RESETVALUE                  0x00000000UL                              /**< Default value for ETH_AUTOFLUSHEDPKTS */
#define _ETH_AUTOFLUSHEDPKTS_MASK                        0x0000FFFFUL                              /**< Mask for ETH_AUTOFLUSHEDPKTS */
#define _ETH_AUTOFLUSHEDPKTS_COUNT_SHIFT                 0                                         /**< Shift value for ETH_COUNT */
#define _ETH_AUTOFLUSHEDPKTS_COUNT_MASK                  0xFFFFUL                                  /**< Bit mask for ETH_COUNT */
#define _ETH_AUTOFLUSHEDPKTS_COUNT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_AUTOFLUSHEDPKTS */
#define ETH_AUTOFLUSHEDPKTS_COUNT_DEFAULT                (_ETH_AUTOFLUSHEDPKTS_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_AUTOFLUSHEDPKTS */

/* Bit fields for ETH TSUTIMERINCRSUBNSEC */
#define _ETH_TSUTIMERINCRSUBNSEC_RESETVALUE              0x00000000UL                                          /**< Default value for ETH_TSUTIMERINCRSUBNSEC */
#define _ETH_TSUTIMERINCRSUBNSEC_MASK                    0xFF00FFFFUL                                          /**< Mask for ETH_TSUTIMERINCRSUBNSEC */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCR_SHIFT         0                                                     /**< Shift value for ETH_SUBNSINCR */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCR_MASK          0xFFFFUL                                              /**< Bit mask for ETH_SUBNSINCR */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCR_DEFAULT       0x00000000UL                                          /**< Mode DEFAULT for ETH_TSUTIMERINCRSUBNSEC */
#define ETH_TSUTIMERINCRSUBNSEC_SUBNSINCR_DEFAULT        (_ETH_TSUTIMERINCRSUBNSEC_SUBNSINCR_DEFAULT << 0)     /**< Shifted mode DEFAULT for ETH_TSUTIMERINCRSUBNSEC */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCRLSB_SHIFT      24                                                    /**< Shift value for ETH_SUBNSINCRLSB */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCRLSB_MASK       0xFF000000UL                                          /**< Bit mask for ETH_SUBNSINCRLSB */
#define _ETH_TSUTIMERINCRSUBNSEC_SUBNSINCRLSB_DEFAULT    0x00000000UL                                          /**< Mode DEFAULT for ETH_TSUTIMERINCRSUBNSEC */
#define ETH_TSUTIMERINCRSUBNSEC_SUBNSINCRLSB_DEFAULT     (_ETH_TSUTIMERINCRSUBNSEC_SUBNSINCRLSB_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_TSUTIMERINCRSUBNSEC */

/* Bit fields for ETH TSUTIMERMSBSEC */
#define _ETH_TSUTIMERMSBSEC_RESETVALUE                   0x00000000UL                             /**< Default value for ETH_TSUTIMERMSBSEC */
#define _ETH_TSUTIMERMSBSEC_MASK                         0x0000FFFFUL                             /**< Mask for ETH_TSUTIMERMSBSEC */
#define _ETH_TSUTIMERMSBSEC_TIMER_SHIFT                  0                                        /**< Shift value for ETH_TIMER */
#define _ETH_TSUTIMERMSBSEC_TIMER_MASK                   0xFFFFUL                                 /**< Bit mask for ETH_TIMER */
#define _ETH_TSUTIMERMSBSEC_TIMER_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for ETH_TSUTIMERMSBSEC */
#define ETH_TSUTIMERMSBSEC_TIMER_DEFAULT                 (_ETH_TSUTIMERMSBSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUTIMERMSBSEC */

/* Bit fields for ETH TSUTIMERSEC */
#define _ETH_TSUTIMERSEC_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_TSUTIMERSEC */
#define _ETH_TSUTIMERSEC_MASK                            0xFFFFFFFFUL                          /**< Mask for ETH_TSUTIMERSEC */
#define _ETH_TSUTIMERSEC_TIMER_SHIFT                     0                                     /**< Shift value for ETH_TIMER */
#define _ETH_TSUTIMERSEC_TIMER_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_TIMER */
#define _ETH_TSUTIMERSEC_TIMER_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TSUTIMERSEC */
#define ETH_TSUTIMERSEC_TIMER_DEFAULT                    (_ETH_TSUTIMERSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUTIMERSEC */

/* Bit fields for ETH TSUTIMERNSEC */
#define _ETH_TSUTIMERNSEC_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_TSUTIMERNSEC */
#define _ETH_TSUTIMERNSEC_MASK                           0x3FFFFFFFUL                           /**< Mask for ETH_TSUTIMERNSEC */
#define _ETH_TSUTIMERNSEC_TIMER_SHIFT                    0                                      /**< Shift value for ETH_TIMER */
#define _ETH_TSUTIMERNSEC_TIMER_MASK                     0x3FFFFFFFUL                           /**< Bit mask for ETH_TIMER */
#define _ETH_TSUTIMERNSEC_TIMER_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUTIMERNSEC */
#define ETH_TSUTIMERNSEC_TIMER_DEFAULT                   (_ETH_TSUTIMERNSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUTIMERNSEC */

/* Bit fields for ETH TSUTIMERADJUST */
#define _ETH_TSUTIMERADJUST_RESETVALUE                   0x00000000UL                                    /**< Default value for ETH_TSUTIMERADJUST */
#define _ETH_TSUTIMERADJUST_MASK                         0xBFFFFFFFUL                                    /**< Mask for ETH_TSUTIMERADJUST */
#define _ETH_TSUTIMERADJUST_INCREMENTVAL_SHIFT           0                                               /**< Shift value for ETH_INCREMENTVAL */
#define _ETH_TSUTIMERADJUST_INCREMENTVAL_MASK            0x3FFFFFFFUL                                    /**< Bit mask for ETH_INCREMENTVAL */
#define _ETH_TSUTIMERADJUST_INCREMENTVAL_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for ETH_TSUTIMERADJUST */
#define ETH_TSUTIMERADJUST_INCREMENTVAL_DEFAULT          (_ETH_TSUTIMERADJUST_INCREMENTVAL_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUTIMERADJUST */
#define ETH_TSUTIMERADJUST_ADDSUBTRACT                   (0x1UL << 31)                                   /**< Write as one to subtract from the 1588 timer */
#define _ETH_TSUTIMERADJUST_ADDSUBTRACT_SHIFT            31                                              /**< Shift value for ETH_ADDSUBTRACT */
#define _ETH_TSUTIMERADJUST_ADDSUBTRACT_MASK             0x80000000UL                                    /**< Bit mask for ETH_ADDSUBTRACT */
#define _ETH_TSUTIMERADJUST_ADDSUBTRACT_DEFAULT          0x00000000UL                                    /**< Mode DEFAULT for ETH_TSUTIMERADJUST */
#define ETH_TSUTIMERADJUST_ADDSUBTRACT_DEFAULT           (_ETH_TSUTIMERADJUST_ADDSUBTRACT_DEFAULT << 31) /**< Shifted mode DEFAULT for ETH_TSUTIMERADJUST */

/* Bit fields for ETH TSUTIMERINCR */
#define _ETH_TSUTIMERINCR_RESETVALUE                     0x00000000UL                                 /**< Default value for ETH_TSUTIMERINCR */
#define _ETH_TSUTIMERINCR_MASK                           0x00FFFFFFUL                                 /**< Mask for ETH_TSUTIMERINCR */
#define _ETH_TSUTIMERINCR_NSINCREMENT_SHIFT              0                                            /**< Shift value for ETH_NSINCREMENT */
#define _ETH_TSUTIMERINCR_NSINCREMENT_MASK               0xFFUL                                       /**< Bit mask for ETH_NSINCREMENT */
#define _ETH_TSUTIMERINCR_NSINCREMENT_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for ETH_TSUTIMERINCR */
#define ETH_TSUTIMERINCR_NSINCREMENT_DEFAULT             (_ETH_TSUTIMERINCR_NSINCREMENT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUTIMERINCR */
#define _ETH_TSUTIMERINCR_ALTNSINCR_SHIFT                8                                            /**< Shift value for ETH_ALTNSINCR */
#define _ETH_TSUTIMERINCR_ALTNSINCR_MASK                 0xFF00UL                                     /**< Bit mask for ETH_ALTNSINCR */
#define _ETH_TSUTIMERINCR_ALTNSINCR_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for ETH_TSUTIMERINCR */
#define ETH_TSUTIMERINCR_ALTNSINCR_DEFAULT               (_ETH_TSUTIMERINCR_ALTNSINCR_DEFAULT << 8)   /**< Shifted mode DEFAULT for ETH_TSUTIMERINCR */
#define _ETH_TSUTIMERINCR_NUMINCS_SHIFT                  16                                           /**< Shift value for ETH_NUMINCS */
#define _ETH_TSUTIMERINCR_NUMINCS_MASK                   0xFF0000UL                                   /**< Bit mask for ETH_NUMINCS */
#define _ETH_TSUTIMERINCR_NUMINCS_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for ETH_TSUTIMERINCR */
#define ETH_TSUTIMERINCR_NUMINCS_DEFAULT                 (_ETH_TSUTIMERINCR_NUMINCS_DEFAULT << 16)    /**< Shifted mode DEFAULT for ETH_TSUTIMERINCR */

/* Bit fields for ETH TSUPTPTXSEC */
#define _ETH_TSUPTPTXSEC_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_TSUPTPTXSEC */
#define _ETH_TSUPTPTXSEC_MASK                            0xFFFFFFFFUL                          /**< Mask for ETH_TSUPTPTXSEC */
#define _ETH_TSUPTPTXSEC_TIMER_SHIFT                     0                                     /**< Shift value for ETH_TIMER */
#define _ETH_TSUPTPTXSEC_TIMER_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPTPTXSEC_TIMER_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TSUPTPTXSEC */
#define ETH_TSUPTPTXSEC_TIMER_DEFAULT                    (_ETH_TSUPTPTXSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPTXSEC */

/* Bit fields for ETH TSUPTPTXNSEC */
#define _ETH_TSUPTPTXNSEC_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_TSUPTPTXNSEC */
#define _ETH_TSUPTPTXNSEC_MASK                           0x3FFFFFFFUL                           /**< Mask for ETH_TSUPTPTXNSEC */
#define _ETH_TSUPTPTXNSEC_TIMER_SHIFT                    0                                      /**< Shift value for ETH_TIMER */
#define _ETH_TSUPTPTXNSEC_TIMER_MASK                     0x3FFFFFFFUL                           /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPTPTXNSEC_TIMER_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUPTPTXNSEC */
#define ETH_TSUPTPTXNSEC_TIMER_DEFAULT                   (_ETH_TSUPTPTXNSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPTXNSEC */

/* Bit fields for ETH TSUPTPRXSEC */
#define _ETH_TSUPTPRXSEC_RESETVALUE                      0x00000000UL                          /**< Default value for ETH_TSUPTPRXSEC */
#define _ETH_TSUPTPRXSEC_MASK                            0xFFFFFFFFUL                          /**< Mask for ETH_TSUPTPRXSEC */
#define _ETH_TSUPTPRXSEC_TIMER_SHIFT                     0                                     /**< Shift value for ETH_TIMER */
#define _ETH_TSUPTPRXSEC_TIMER_MASK                      0xFFFFFFFFUL                          /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPTPRXSEC_TIMER_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TSUPTPRXSEC */
#define ETH_TSUPTPRXSEC_TIMER_DEFAULT                    (_ETH_TSUPTPRXSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPRXSEC */

/* Bit fields for ETH TSUPTPRXNSEC */
#define _ETH_TSUPTPRXNSEC_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_TSUPTPRXNSEC */
#define _ETH_TSUPTPRXNSEC_MASK                           0x3FFFFFFFUL                           /**< Mask for ETH_TSUPTPRXNSEC */
#define _ETH_TSUPTPRXNSEC_TIMER_SHIFT                    0                                      /**< Shift value for ETH_TIMER */
#define _ETH_TSUPTPRXNSEC_TIMER_MASK                     0x3FFFFFFFUL                           /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPTPRXNSEC_TIMER_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUPTPRXNSEC */
#define ETH_TSUPTPRXNSEC_TIMER_DEFAULT                   (_ETH_TSUPTPRXNSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPTPRXNSEC */

/* Bit fields for ETH TSUPEERTXSEC */
#define _ETH_TSUPEERTXSEC_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_TSUPEERTXSEC */
#define _ETH_TSUPEERTXSEC_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_TSUPEERTXSEC */
#define _ETH_TSUPEERTXSEC_TIMER_SHIFT                    0                                      /**< Shift value for ETH_TIMER */
#define _ETH_TSUPEERTXSEC_TIMER_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPEERTXSEC_TIMER_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUPEERTXSEC */
#define ETH_TSUPEERTXSEC_TIMER_DEFAULT                   (_ETH_TSUPEERTXSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERTXSEC */

/* Bit fields for ETH TSUPEERTXNSEC */
#define _ETH_TSUPEERTXNSEC_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_TSUPEERTXNSEC */
#define _ETH_TSUPEERTXNSEC_MASK                          0x3FFFFFFFUL                            /**< Mask for ETH_TSUPEERTXNSEC */
#define _ETH_TSUPEERTXNSEC_TIMER_SHIFT                   0                                       /**< Shift value for ETH_TIMER */
#define _ETH_TSUPEERTXNSEC_TIMER_MASK                    0x3FFFFFFFUL                            /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPEERTXNSEC_TIMER_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_TSUPEERTXNSEC */
#define ETH_TSUPEERTXNSEC_TIMER_DEFAULT                  (_ETH_TSUPEERTXNSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERTXNSEC */

/* Bit fields for ETH TSUPEERRXSEC */
#define _ETH_TSUPEERRXSEC_RESETVALUE                     0x00000000UL                           /**< Default value for ETH_TSUPEERRXSEC */
#define _ETH_TSUPEERRXSEC_MASK                           0xFFFFFFFFUL                           /**< Mask for ETH_TSUPEERRXSEC */
#define _ETH_TSUPEERRXSEC_TIMER_SHIFT                    0                                      /**< Shift value for ETH_TIMER */
#define _ETH_TSUPEERRXSEC_TIMER_MASK                     0xFFFFFFFFUL                           /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPEERRXSEC_TIMER_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for ETH_TSUPEERRXSEC */
#define ETH_TSUPEERRXSEC_TIMER_DEFAULT                   (_ETH_TSUPEERRXSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERRXSEC */

/* Bit fields for ETH TSUPEERRXNSEC */
#define _ETH_TSUPEERRXNSEC_RESETVALUE                    0x00000000UL                            /**< Default value for ETH_TSUPEERRXNSEC */
#define _ETH_TSUPEERRXNSEC_MASK                          0x3FFFFFFFUL                            /**< Mask for ETH_TSUPEERRXNSEC */
#define _ETH_TSUPEERRXNSEC_TIMER_SHIFT                   0                                       /**< Shift value for ETH_TIMER */
#define _ETH_TSUPEERRXNSEC_TIMER_MASK                    0x3FFFFFFFUL                            /**< Bit mask for ETH_TIMER */
#define _ETH_TSUPEERRXNSEC_TIMER_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_TSUPEERRXNSEC */
#define ETH_TSUPEERRXNSEC_TIMER_DEFAULT                  (_ETH_TSUPEERRXNSEC_TIMER_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TSUPEERRXNSEC */

/* Bit fields for ETH TXPAUSEQUANT1 */
#define _ETH_TXPAUSEQUANT1_RESETVALUE                    0xFFFFFFFFUL                               /**< Default value for ETH_TXPAUSEQUANT1 */
#define _ETH_TXPAUSEQUANT1_MASK                          0xFFFFFFFFUL                               /**< Mask for ETH_TXPAUSEQUANT1 */
#define _ETH_TXPAUSEQUANT1_QUANTP2_SHIFT                 0                                          /**< Shift value for ETH_QUANTP2 */
#define _ETH_TXPAUSEQUANT1_QUANTP2_MASK                  0xFFFFUL                                   /**< Bit mask for ETH_QUANTP2 */
#define _ETH_TXPAUSEQUANT1_QUANTP2_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT1 */
#define ETH_TXPAUSEQUANT1_QUANTP2_DEFAULT                (_ETH_TXPAUSEQUANT1_QUANTP2_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT1 */
#define _ETH_TXPAUSEQUANT1_QUANTP3_SHIFT                 16                                         /**< Shift value for ETH_QUANTP3 */
#define _ETH_TXPAUSEQUANT1_QUANTP3_MASK                  0xFFFF0000UL                               /**< Bit mask for ETH_QUANTP3 */
#define _ETH_TXPAUSEQUANT1_QUANTP3_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT1 */
#define ETH_TXPAUSEQUANT1_QUANTP3_DEFAULT                (_ETH_TXPAUSEQUANT1_QUANTP3_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT1 */

/* Bit fields for ETH TXPAUSEQUANT2 */
#define _ETH_TXPAUSEQUANT2_RESETVALUE                    0xFFFFFFFFUL                               /**< Default value for ETH_TXPAUSEQUANT2 */
#define _ETH_TXPAUSEQUANT2_MASK                          0xFFFFFFFFUL                               /**< Mask for ETH_TXPAUSEQUANT2 */
#define _ETH_TXPAUSEQUANT2_QUANTP4_SHIFT                 0                                          /**< Shift value for ETH_QUANTP4 */
#define _ETH_TXPAUSEQUANT2_QUANTP4_MASK                  0xFFFFUL                                   /**< Bit mask for ETH_QUANTP4 */
#define _ETH_TXPAUSEQUANT2_QUANTP4_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT2 */
#define ETH_TXPAUSEQUANT2_QUANTP4_DEFAULT                (_ETH_TXPAUSEQUANT2_QUANTP4_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT2 */
#define _ETH_TXPAUSEQUANT2_QUANTP5_SHIFT                 16                                         /**< Shift value for ETH_QUANTP5 */
#define _ETH_TXPAUSEQUANT2_QUANTP5_MASK                  0xFFFF0000UL                               /**< Bit mask for ETH_QUANTP5 */
#define _ETH_TXPAUSEQUANT2_QUANTP5_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT2 */
#define ETH_TXPAUSEQUANT2_QUANTP5_DEFAULT                (_ETH_TXPAUSEQUANT2_QUANTP5_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT2 */

/* Bit fields for ETH TXPAUSEQUANT3 */
#define _ETH_TXPAUSEQUANT3_RESETVALUE                    0xFFFFFFFFUL                               /**< Default value for ETH_TXPAUSEQUANT3 */
#define _ETH_TXPAUSEQUANT3_MASK                          0xFFFFFFFFUL                               /**< Mask for ETH_TXPAUSEQUANT3 */
#define _ETH_TXPAUSEQUANT3_QUANTP6_SHIFT                 0                                          /**< Shift value for ETH_QUANTP6 */
#define _ETH_TXPAUSEQUANT3_QUANTP6_MASK                  0xFFFFUL                                   /**< Bit mask for ETH_QUANTP6 */
#define _ETH_TXPAUSEQUANT3_QUANTP6_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT3 */
#define ETH_TXPAUSEQUANT3_QUANTP6_DEFAULT                (_ETH_TXPAUSEQUANT3_QUANTP6_DEFAULT << 0)  /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT3 */
#define _ETH_TXPAUSEQUANT3_QUANTP7_SHIFT                 16                                         /**< Shift value for ETH_QUANTP7 */
#define _ETH_TXPAUSEQUANT3_QUANTP7_MASK                  0xFFFF0000UL                               /**< Bit mask for ETH_QUANTP7 */
#define _ETH_TXPAUSEQUANT3_QUANTP7_DEFAULT               0x0000FFFFUL                               /**< Mode DEFAULT for ETH_TXPAUSEQUANT3 */
#define ETH_TXPAUSEQUANT3_QUANTP7_DEFAULT                (_ETH_TXPAUSEQUANT3_QUANTP7_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_TXPAUSEQUANT3 */

/* Bit fields for ETH RXLPI */
#define _ETH_RXLPI_RESETVALUE                            0x00000000UL                    /**< Default value for ETH_RXLPI */
#define _ETH_RXLPI_MASK                                  0x0000FFFFUL                    /**< Mask for ETH_RXLPI */
#define _ETH_RXLPI_COUNT_SHIFT                           0                               /**< Shift value for ETH_COUNT */
#define _ETH_RXLPI_COUNT_MASK                            0xFFFFUL                        /**< Bit mask for ETH_COUNT */
#define _ETH_RXLPI_COUNT_DEFAULT                         0x00000000UL                    /**< Mode DEFAULT for ETH_RXLPI */
#define ETH_RXLPI_COUNT_DEFAULT                          (_ETH_RXLPI_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXLPI */

/* Bit fields for ETH RXLPITIME */
#define _ETH_RXLPITIME_RESETVALUE                        0x00000000UL                          /**< Default value for ETH_RXLPITIME */
#define _ETH_RXLPITIME_MASK                              0x00FFFFFFUL                          /**< Mask for ETH_RXLPITIME */
#define _ETH_RXLPITIME_LPITIME_SHIFT                     0                                     /**< Shift value for ETH_LPITIME */
#define _ETH_RXLPITIME_LPITIME_MASK                      0xFFFFFFUL                            /**< Bit mask for ETH_LPITIME */
#define _ETH_RXLPITIME_LPITIME_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_RXLPITIME */
#define ETH_RXLPITIME_LPITIME_DEFAULT                    (_ETH_RXLPITIME_LPITIME_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_RXLPITIME */

/* Bit fields for ETH TXLPI */
#define _ETH_TXLPI_RESETVALUE                            0x00000000UL                    /**< Default value for ETH_TXLPI */
#define _ETH_TXLPI_MASK                                  0x0000FFFFUL                    /**< Mask for ETH_TXLPI */
#define _ETH_TXLPI_COUNT_SHIFT                           0                               /**< Shift value for ETH_COUNT */
#define _ETH_TXLPI_COUNT_MASK                            0xFFFFUL                        /**< Bit mask for ETH_COUNT */
#define _ETH_TXLPI_COUNT_DEFAULT                         0x00000000UL                    /**< Mode DEFAULT for ETH_TXLPI */
#define ETH_TXLPI_COUNT_DEFAULT                          (_ETH_TXLPI_COUNT_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TXLPI */

/* Bit fields for ETH TXLPITIME */
#define _ETH_TXLPITIME_RESETVALUE                        0x00000000UL                          /**< Default value for ETH_TXLPITIME */
#define _ETH_TXLPITIME_MASK                              0x00FFFFFFUL                          /**< Mask for ETH_TXLPITIME */
#define _ETH_TXLPITIME_LPITIME_SHIFT                     0                                     /**< Shift value for ETH_LPITIME */
#define _ETH_TXLPITIME_LPITIME_MASK                      0xFFFFFFUL                            /**< Bit mask for ETH_LPITIME */
#define _ETH_TXLPITIME_LPITIME_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for ETH_TXLPITIME */
#define ETH_TXLPITIME_LPITIME_DEFAULT                    (_ETH_TXLPITIME_LPITIME_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_TXLPITIME */

/* Bit fields for ETH TXBDCTRL */
#define _ETH_TXBDCTRL_RESETVALUE                         0x00000000UL                            /**< Default value for ETH_TXBDCTRL */
#define _ETH_TXBDCTRL_MASK                               0x00000030UL                            /**< Mask for ETH_TXBDCTRL */
#define _ETH_TXBDCTRL_TXBDTSMODE_SHIFT                   4                                       /**< Shift value for ETH_TXBDTSMODE */
#define _ETH_TXBDCTRL_TXBDTSMODE_MASK                    0x30UL                                  /**< Bit mask for ETH_TXBDTSMODE */
#define _ETH_TXBDCTRL_TXBDTSMODE_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_TXBDCTRL */
#define ETH_TXBDCTRL_TXBDTSMODE_DEFAULT                  (_ETH_TXBDCTRL_TXBDTSMODE_DEFAULT << 4) /**< Shifted mode DEFAULT for ETH_TXBDCTRL */

/* Bit fields for ETH RXBDCTRL */
#define _ETH_RXBDCTRL_RESETVALUE                         0x00000000UL                            /**< Default value for ETH_RXBDCTRL */
#define _ETH_RXBDCTRL_MASK                               0x00000030UL                            /**< Mask for ETH_RXBDCTRL */
#define _ETH_RXBDCTRL_RXBDTSMODE_SHIFT                   4                                       /**< Shift value for ETH_RXBDTSMODE */
#define _ETH_RXBDCTRL_RXBDTSMODE_MASK                    0x30UL                                  /**< Bit mask for ETH_RXBDTSMODE */
#define _ETH_RXBDCTRL_RXBDTSMODE_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for ETH_RXBDCTRL */
#define ETH_RXBDCTRL_RXBDTSMODE_DEFAULT                  (_ETH_RXBDCTRL_RXBDTSMODE_DEFAULT << 4) /**< Shifted mode DEFAULT for ETH_RXBDCTRL */

/* Bit fields for ETH ROUTEPEN */
#define _ETH_ROUTEPEN_RESETVALUE                         0x00000000UL                              /**< Default value for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MASK                               0x0000003FUL                              /**< Mask for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MDIOPEN                             (0x1UL << 0)                              /**< MDIO I/O Enable */
#define _ETH_ROUTEPEN_MDIOPEN_SHIFT                      0                                         /**< Shift value for ETH_MDIOPEN */
#define _ETH_ROUTEPEN_MDIOPEN_MASK                       0x1UL                                     /**< Bit mask for ETH_MDIOPEN */
#define _ETH_ROUTEPEN_MDIOPEN_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MDIOPEN_DISABLE                    0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MDIOPEN_ENABLE                     0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MDIOPEN_DEFAULT                     (_ETH_ROUTEPEN_MDIOPEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MDIOPEN_DISABLE                     (_ETH_ROUTEPEN_MDIOPEN_DISABLE << 0)      /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MDIOPEN_ENABLE                      (_ETH_ROUTEPEN_MDIOPEN_ENABLE << 0)       /**< Shifted mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIITXERPEN                          (0x1UL << 1)                              /**< MII TX ER I/O Enable */
#define _ETH_ROUTEPEN_MIITXERPEN_SHIFT                   1                                         /**< Shift value for ETH_MIITXERPEN */
#define _ETH_ROUTEPEN_MIITXERPEN_MASK                    0x2UL                                     /**< Bit mask for ETH_MIITXERPEN */
#define _ETH_ROUTEPEN_MIITXERPEN_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIITXERPEN_DISABLE                 0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIITXERPEN_ENABLE                  0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIITXERPEN_DEFAULT                  (_ETH_ROUTEPEN_MIITXERPEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIITXERPEN_DISABLE                  (_ETH_ROUTEPEN_MIITXERPEN_DISABLE << 1)   /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIITXERPEN_ENABLE                   (_ETH_ROUTEPEN_MIITXERPEN_ENABLE << 1)    /**< Shifted mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIRXERPEN                          (0x1UL << 2)                              /**< MII TX ER I/O Enable */
#define _ETH_ROUTEPEN_MIIRXERPEN_SHIFT                   2                                         /**< Shift value for ETH_MIIRXERPEN */
#define _ETH_ROUTEPEN_MIIRXERPEN_MASK                    0x4UL                                     /**< Bit mask for ETH_MIIRXERPEN */
#define _ETH_ROUTEPEN_MIIRXERPEN_DEFAULT                 0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIIRXERPEN_DISABLE                 0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIIRXERPEN_ENABLE                  0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIRXERPEN_DEFAULT                  (_ETH_ROUTEPEN_MIIRXERPEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIRXERPEN_DISABLE                  (_ETH_ROUTEPEN_MIIRXERPEN_DISABLE << 2)   /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIRXERPEN_ENABLE                   (_ETH_ROUTEPEN_MIIRXERPEN_ENABLE << 2)    /**< Shifted mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIPEN                              (0x1UL << 3)                              /**< MII I/O Enable */
#define _ETH_ROUTEPEN_MIIPEN_SHIFT                       3                                         /**< Shift value for ETH_MIIPEN */
#define _ETH_ROUTEPEN_MIIPEN_MASK                        0x8UL                                     /**< Bit mask for ETH_MIIPEN */
#define _ETH_ROUTEPEN_MIIPEN_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIIPEN_DISABLE                     0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_MIIPEN_ENABLE                      0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIPEN_DEFAULT                      (_ETH_ROUTEPEN_MIIPEN_DEFAULT << 3)       /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIPEN_DISABLE                      (_ETH_ROUTEPEN_MIIPEN_DISABLE << 3)       /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_MIIPEN_ENABLE                       (_ETH_ROUTEPEN_MIIPEN_ENABLE << 3)        /**< Shifted mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_RMIIPEN                             (0x1UL << 4)                              /**< RMII I/O Enable */
#define _ETH_ROUTEPEN_RMIIPEN_SHIFT                      4                                         /**< Shift value for ETH_RMIIPEN */
#define _ETH_ROUTEPEN_RMIIPEN_MASK                       0x10UL                                    /**< Bit mask for ETH_RMIIPEN */
#define _ETH_ROUTEPEN_RMIIPEN_DEFAULT                    0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_RMIIPEN_DISABLE                    0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_RMIIPEN_ENABLE                     0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_RMIIPEN_DEFAULT                     (_ETH_ROUTEPEN_RMIIPEN_DEFAULT << 4)      /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_RMIIPEN_DISABLE                     (_ETH_ROUTEPEN_RMIIPEN_DISABLE << 4)      /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_RMIIPEN_ENABLE                      (_ETH_ROUTEPEN_RMIIPEN_ENABLE << 4)       /**< Shifted mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_TSUTMRTOGPEN                        (0x1UL << 5)                              /**< TSU_TMR_CNT_SEC Output Enable */
#define _ETH_ROUTEPEN_TSUTMRTOGPEN_SHIFT                 5                                         /**< Shift value for ETH_TSUTMRTOGPEN */
#define _ETH_ROUTEPEN_TSUTMRTOGPEN_MASK                  0x20UL                                    /**< Bit mask for ETH_TSUTMRTOGPEN */
#define _ETH_ROUTEPEN_TSUTMRTOGPEN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_TSUTMRTOGPEN_DISABLE               0x00000000UL                              /**< Mode DISABLE for ETH_ROUTEPEN */
#define _ETH_ROUTEPEN_TSUTMRTOGPEN_ENABLE                0x00000001UL                              /**< Mode ENABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_TSUTMRTOGPEN_DEFAULT                (_ETH_ROUTEPEN_TSUTMRTOGPEN_DEFAULT << 5) /**< Shifted mode DEFAULT for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_TSUTMRTOGPEN_DISABLE                (_ETH_ROUTEPEN_TSUTMRTOGPEN_DISABLE << 5) /**< Shifted mode DISABLE for ETH_ROUTEPEN */
#define ETH_ROUTEPEN_TSUTMRTOGPEN_ENABLE                 (_ETH_ROUTEPEN_TSUTMRTOGPEN_ENABLE << 5)  /**< Shifted mode ENABLE for ETH_ROUTEPEN */

/* Bit fields for ETH ROUTELOC0 */
#define _ETH_ROUTELOC0_RESETVALUE                        0x00000000UL                             /**< Default value for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MASK                              0x03030301UL                             /**< Mask for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIITXLOC_SHIFT                    0                                        /**< Shift value for ETH_MIITXLOC */
#define _ETH_ROUTELOC0_MIITXLOC_MASK                     0x1UL                                    /**< Bit mask for ETH_MIITXLOC */
#define _ETH_ROUTELOC0_MIITXLOC_LOC0                     0x00000000UL                             /**< Mode LOC0 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIITXLOC_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIITXLOC_LOC1                     0x00000001UL                             /**< Mode LOC1 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIITXLOC_LOC0                      (_ETH_ROUTELOC0_MIITXLOC_LOC0 << 0)      /**< Shifted mode LOC0 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIITXLOC_DEFAULT                   (_ETH_ROUTELOC0_MIITXLOC_DEFAULT << 0)   /**< Shifted mode DEFAULT for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIITXLOC_LOC1                      (_ETH_ROUTELOC0_MIITXLOC_LOC1 << 0)      /**< Shifted mode LOC1 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIIRXLOC_SHIFT                    8                                        /**< Shift value for ETH_MIIRXLOC */
#define _ETH_ROUTELOC0_MIIRXLOC_MASK                     0x300UL                                  /**< Bit mask for ETH_MIIRXLOC */
#define _ETH_ROUTELOC0_MIIRXLOC_LOC0                     0x00000000UL                             /**< Mode LOC0 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIIRXLOC_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIIRXLOC_LOC1                     0x00000001UL                             /**< Mode LOC1 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIIRXLOC_LOC2                     0x00000002UL                             /**< Mode LOC2 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIIRXLOC_LOC0                      (_ETH_ROUTELOC0_MIIRXLOC_LOC0 << 8)      /**< Shifted mode LOC0 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIIRXLOC_DEFAULT                   (_ETH_ROUTELOC0_MIIRXLOC_DEFAULT << 8)   /**< Shifted mode DEFAULT for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIIRXLOC_LOC1                      (_ETH_ROUTELOC0_MIIRXLOC_LOC1 << 8)      /**< Shifted mode LOC1 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIIRXLOC_LOC2                      (_ETH_ROUTELOC0_MIIRXLOC_LOC2 << 8)      /**< Shifted mode LOC2 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICRSLOC_SHIFT                   16                                       /**< Shift value for ETH_MIICRSLOC */
#define _ETH_ROUTELOC0_MIICRSLOC_MASK                    0x30000UL                                /**< Bit mask for ETH_MIICRSLOC */
#define _ETH_ROUTELOC0_MIICRSLOC_LOC0                    0x00000000UL                             /**< Mode LOC0 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICRSLOC_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICRSLOC_LOC1                    0x00000001UL                             /**< Mode LOC1 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICRSLOC_LOC2                    0x00000002UL                             /**< Mode LOC2 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICRSLOC_LOC0                     (_ETH_ROUTELOC0_MIICRSLOC_LOC0 << 16)    /**< Shifted mode LOC0 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICRSLOC_DEFAULT                  (_ETH_ROUTELOC0_MIICRSLOC_DEFAULT << 16) /**< Shifted mode DEFAULT for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICRSLOC_LOC1                     (_ETH_ROUTELOC0_MIICRSLOC_LOC1 << 16)    /**< Shifted mode LOC1 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICRSLOC_LOC2                     (_ETH_ROUTELOC0_MIICRSLOC_LOC2 << 16)    /**< Shifted mode LOC2 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICOLLOC_SHIFT                   24                                       /**< Shift value for ETH_MIICOLLOC */
#define _ETH_ROUTELOC0_MIICOLLOC_MASK                    0x3000000UL                              /**< Bit mask for ETH_MIICOLLOC */
#define _ETH_ROUTELOC0_MIICOLLOC_LOC0                    0x00000000UL                             /**< Mode LOC0 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICOLLOC_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICOLLOC_LOC1                    0x00000001UL                             /**< Mode LOC1 for ETH_ROUTELOC0 */
#define _ETH_ROUTELOC0_MIICOLLOC_LOC2                    0x00000002UL                             /**< Mode LOC2 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICOLLOC_LOC0                     (_ETH_ROUTELOC0_MIICOLLOC_LOC0 << 24)    /**< Shifted mode LOC0 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICOLLOC_DEFAULT                  (_ETH_ROUTELOC0_MIICOLLOC_DEFAULT << 24) /**< Shifted mode DEFAULT for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICOLLOC_LOC1                     (_ETH_ROUTELOC0_MIICOLLOC_LOC1 << 24)    /**< Shifted mode LOC1 for ETH_ROUTELOC0 */
#define ETH_ROUTELOC0_MIICOLLOC_LOC2                     (_ETH_ROUTELOC0_MIICOLLOC_LOC2 << 24)    /**< Shifted mode LOC2 for ETH_ROUTELOC0 */

/* Bit fields for ETH ROUTELOC1 */
#define _ETH_ROUTELOC1_RESETVALUE                        0x00000000UL                               /**< Default value for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MASK                              0x01030303UL                               /**< Mask for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_SHIFT                0                                          /**< Shift value for ETH_TSUEXTCLKLOC */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_MASK                 0x3UL                                      /**< Bit mask for ETH_TSUEXTCLKLOC */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_LOC0                 0x00000000UL                               /**< Mode LOC0 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_LOC1                 0x00000001UL                               /**< Mode LOC1 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_LOC2                 0x00000002UL                               /**< Mode LOC2 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUEXTCLKLOC_LOC3                 0x00000003UL                               /**< Mode LOC3 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUEXTCLKLOC_LOC0                  (_ETH_ROUTELOC1_TSUEXTCLKLOC_LOC0 << 0)    /**< Shifted mode LOC0 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUEXTCLKLOC_DEFAULT               (_ETH_ROUTELOC1_TSUEXTCLKLOC_DEFAULT << 0) /**< Shifted mode DEFAULT for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUEXTCLKLOC_LOC1                  (_ETH_ROUTELOC1_TSUEXTCLKLOC_LOC1 << 0)    /**< Shifted mode LOC1 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUEXTCLKLOC_LOC2                  (_ETH_ROUTELOC1_TSUEXTCLKLOC_LOC2 << 0)    /**< Shifted mode LOC2 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUEXTCLKLOC_LOC3                  (_ETH_ROUTELOC1_TSUEXTCLKLOC_LOC3 << 0)    /**< Shifted mode LOC3 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_SHIFT                8                                          /**< Shift value for ETH_TSUTMRTOGLOC */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_MASK                 0x300UL                                    /**< Bit mask for ETH_TSUTMRTOGLOC */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_LOC0                 0x00000000UL                               /**< Mode LOC0 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_LOC1                 0x00000001UL                               /**< Mode LOC1 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_LOC2                 0x00000002UL                               /**< Mode LOC2 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_TSUTMRTOGLOC_LOC3                 0x00000003UL                               /**< Mode LOC3 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUTMRTOGLOC_LOC0                  (_ETH_ROUTELOC1_TSUTMRTOGLOC_LOC0 << 8)    /**< Shifted mode LOC0 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUTMRTOGLOC_DEFAULT               (_ETH_ROUTELOC1_TSUTMRTOGLOC_DEFAULT << 8) /**< Shifted mode DEFAULT for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUTMRTOGLOC_LOC1                  (_ETH_ROUTELOC1_TSUTMRTOGLOC_LOC1 << 8)    /**< Shifted mode LOC1 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUTMRTOGLOC_LOC2                  (_ETH_ROUTELOC1_TSUTMRTOGLOC_LOC2 << 8)    /**< Shifted mode LOC2 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_TSUTMRTOGLOC_LOC3                  (_ETH_ROUTELOC1_TSUTMRTOGLOC_LOC3 << 8)    /**< Shifted mode LOC3 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MDIOLOC_SHIFT                     16                                         /**< Shift value for ETH_MDIOLOC */
#define _ETH_ROUTELOC1_MDIOLOC_MASK                      0x30000UL                                  /**< Bit mask for ETH_MDIOLOC */
#define _ETH_ROUTELOC1_MDIOLOC_LOC0                      0x00000000UL                               /**< Mode LOC0 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MDIOLOC_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MDIOLOC_LOC1                      0x00000001UL                               /**< Mode LOC1 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MDIOLOC_LOC2                      0x00000002UL                               /**< Mode LOC2 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_MDIOLOC_LOC3                      0x00000003UL                               /**< Mode LOC3 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_MDIOLOC_LOC0                       (_ETH_ROUTELOC1_MDIOLOC_LOC0 << 16)        /**< Shifted mode LOC0 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_MDIOLOC_DEFAULT                    (_ETH_ROUTELOC1_MDIOLOC_DEFAULT << 16)     /**< Shifted mode DEFAULT for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_MDIOLOC_LOC1                       (_ETH_ROUTELOC1_MDIOLOC_LOC1 << 16)        /**< Shifted mode LOC1 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_MDIOLOC_LOC2                       (_ETH_ROUTELOC1_MDIOLOC_LOC2 << 16)        /**< Shifted mode LOC2 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_MDIOLOC_LOC3                       (_ETH_ROUTELOC1_MDIOLOC_LOC3 << 16)        /**< Shifted mode LOC3 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_RMIILOC_SHIFT                     24                                         /**< Shift value for ETH_RMIILOC */
#define _ETH_ROUTELOC1_RMIILOC_MASK                      0x1000000UL                                /**< Bit mask for ETH_RMIILOC */
#define _ETH_ROUTELOC1_RMIILOC_LOC0                      0x00000000UL                               /**< Mode LOC0 for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_RMIILOC_DEFAULT                   0x00000000UL                               /**< Mode DEFAULT for ETH_ROUTELOC1 */
#define _ETH_ROUTELOC1_RMIILOC_LOC1                      0x00000001UL                               /**< Mode LOC1 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_RMIILOC_LOC0                       (_ETH_ROUTELOC1_RMIILOC_LOC0 << 24)        /**< Shifted mode LOC0 for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_RMIILOC_DEFAULT                    (_ETH_ROUTELOC1_RMIILOC_DEFAULT << 24)     /**< Shifted mode DEFAULT for ETH_ROUTELOC1 */
#define ETH_ROUTELOC1_RMIILOC_LOC1                       (_ETH_ROUTELOC1_RMIILOC_LOC1 << 24)        /**< Shifted mode LOC1 for ETH_ROUTELOC1 */

/* Bit fields for ETH CTRL */
#define _ETH_CTRL_RESETVALUE                             0x00000000UL                            /**< Default value for ETH_CTRL */
#define _ETH_CTRL_MASK                                   0x000007F7UL                            /**< Mask for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_SHIFT                        0                                       /**< Shift value for ETH_TSUCLKSEL */
#define _ETH_CTRL_TSUCLKSEL_MASK                         0x7UL                                   /**< Bit mask for ETH_TSUCLKSEL */
#define _ETH_CTRL_TSUCLKSEL_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_NOCLOCK                      0x00000000UL                            /**< Mode NOCLOCK for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_PLL                          0x00000001UL                            /**< Mode PLL for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_RXCLK                        0x00000002UL                            /**< Mode RXCLK for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_REFCLK                       0x00000003UL                            /**< Mode REFCLK for ETH_CTRL */
#define _ETH_CTRL_TSUCLKSEL_TSUEXTCLK                    0x00000004UL                            /**< Mode TSUEXTCLK for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_DEFAULT                       (_ETH_CTRL_TSUCLKSEL_DEFAULT << 0)      /**< Shifted mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_NOCLOCK                       (_ETH_CTRL_TSUCLKSEL_NOCLOCK << 0)      /**< Shifted mode NOCLOCK for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_PLL                           (_ETH_CTRL_TSUCLKSEL_PLL << 0)          /**< Shifted mode PLL for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_RXCLK                         (_ETH_CTRL_TSUCLKSEL_RXCLK << 0)        /**< Shifted mode RXCLK for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_REFCLK                        (_ETH_CTRL_TSUCLKSEL_REFCLK << 0)       /**< Shifted mode REFCLK for ETH_CTRL */
#define ETH_CTRL_TSUCLKSEL_TSUEXTCLK                     (_ETH_CTRL_TSUCLKSEL_TSUEXTCLK << 0)    /**< Shifted mode TSUEXTCLK for ETH_CTRL */
#define _ETH_CTRL_TSUPRESC_SHIFT                         4                                       /**< Shift value for ETH_TSUPRESC */
#define _ETH_CTRL_TSUPRESC_MASK                          0xF0UL                                  /**< Bit mask for ETH_TSUPRESC */
#define _ETH_CTRL_TSUPRESC_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_TSUPRESC_DEFAULT                        (_ETH_CTRL_TSUPRESC_DEFAULT << 4)       /**< Shifted mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_MIISEL                                  (0x1UL << 8)                            /**< MII select signal */
#define _ETH_CTRL_MIISEL_SHIFT                           8                                       /**< Shift value for ETH_MIISEL */
#define _ETH_CTRL_MIISEL_MASK                            0x100UL                                 /**< Bit mask for ETH_MIISEL */
#define _ETH_CTRL_MIISEL_DEFAULT                         0x00000000UL                            /**< Mode DEFAULT for ETH_CTRL */
#define _ETH_CTRL_MIISEL_RMII                            0x00000000UL                            /**< Mode RMII for ETH_CTRL */
#define _ETH_CTRL_MIISEL_MII                             0x00000001UL                            /**< Mode MII for ETH_CTRL */
#define ETH_CTRL_MIISEL_DEFAULT                          (_ETH_CTRL_MIISEL_DEFAULT << 8)         /**< Shifted mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_MIISEL_RMII                             (_ETH_CTRL_MIISEL_RMII << 8)            /**< Shifted mode RMII for ETH_CTRL */
#define ETH_CTRL_MIISEL_MII                              (_ETH_CTRL_MIISEL_MII << 8)             /**< Shifted mode MII for ETH_CTRL */
#define ETH_CTRL_GBLCLKEN                                (0x1UL << 9)                            /**< Global Clock Enable signal for Ethernet clocks tsu_clk, tx_clk, rx_clk and ref_clk */
#define _ETH_CTRL_GBLCLKEN_SHIFT                         9                                       /**< Shift value for ETH_GBLCLKEN */
#define _ETH_CTRL_GBLCLKEN_MASK                          0x200UL                                 /**< Bit mask for ETH_GBLCLKEN */
#define _ETH_CTRL_GBLCLKEN_DEFAULT                       0x00000000UL                            /**< Mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_GBLCLKEN_DEFAULT                        (_ETH_CTRL_GBLCLKEN_DEFAULT << 9)       /**< Shifted mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_TXREFCLKSEL                             (0x1UL << 10)                           /**< REFCLK source select for RMII_TXD and RMII_TX_EN */
#define _ETH_CTRL_TXREFCLKSEL_SHIFT                      10                                      /**< Shift value for ETH_TXREFCLKSEL */
#define _ETH_CTRL_TXREFCLKSEL_MASK                       0x400UL                                 /**< Bit mask for ETH_TXREFCLKSEL */
#define _ETH_CTRL_TXREFCLKSEL_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for ETH_CTRL */
#define _ETH_CTRL_TXREFCLKSEL_REFCLKINT                  0x00000000UL                            /**< Mode REFCLKINT for ETH_CTRL */
#define _ETH_CTRL_TXREFCLKSEL_REFCLKPIN                  0x00000001UL                            /**< Mode REFCLKPIN for ETH_CTRL */
#define ETH_CTRL_TXREFCLKSEL_DEFAULT                     (_ETH_CTRL_TXREFCLKSEL_DEFAULT << 10)   /**< Shifted mode DEFAULT for ETH_CTRL */
#define ETH_CTRL_TXREFCLKSEL_REFCLKINT                   (_ETH_CTRL_TXREFCLKSEL_REFCLKINT << 10) /**< Shifted mode REFCLKINT for ETH_CTRL */
#define ETH_CTRL_TXREFCLKSEL_REFCLKPIN                   (_ETH_CTRL_TXREFCLKSEL_REFCLKPIN << 10) /**< Shifted mode REFCLKPIN for ETH_CTRL */

/** @} */
/** @} End of group EFM32GG11B_ETH */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

