/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdebug
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_auto_init_priorities Priority order of modules in the auto-init sequence
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Auto-initialization priorities
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef AUTO_INIT_PRIORITIES_H
#define AUTO_INIT_PRIORITIES_H

#include "debug.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set to 1 if a debug message should be printed
 *        before the init function of a module is called
 */
#ifndef CONFIG_AUTO_INIT_DEBUG
#define CONFIG_AUTO_INIT_DEBUG 0
#endif

#if !defined(CONCAT) || defined(DOXYGEN)
/**
 * @brief   @ref CONCAT helper macro
 * @internal
 */
#define CONCAT_HELP(a, b) a ## b
/**
 * @brief   Classic concat macro
 * @internal
 */
#define CONCAT(a, b) CONCAT_HELP(a, b)
#endif

/**
 * @brief   Auto init function type
 */
typedef void (*auto_init_fn)(void);

/**
 * @brief   Construct the global priority of a subgroup or module of @p group
 *
 * @experimental
 *
 * @param   group       Supergroup priority of the new group or module
 *                      priority to be constructed
 * @param   prio        Priority of the new subgroup or module within
 *                      the supergroup, one of AUTO_INIT_PRIO_x
 */
#define AUTO_INIT_PRIO_ADD(group, prio)                   \
    CONCAT(group, CONCAT(prio, prio))

/**
 * @brief   Construct the global priority of a group to be inserted
 *          after @p group
 *
 * @experimental
 *
 * @param   group       Group priority before the new group or module priority
 *                      to be constructed
 * @param   prio        Priority of the new group or module after @p group
 *                      , one of AUTO_INIT_PRIO_x
 */
#define AUTO_INIT_PRIO_ADD_AFTER_GROUP(group, prio)             \
    CONCAT(group, CONCAT(AUTO_INIT_PRIO_AFTER, prio))

/**
 * @brief   Construct the global priority of a group to be inserted
 *          before @p group
 *
 * @experimental
 *
 * @param   group       Group priority after the new group or module priority
 *                      to be constructed
 * @param   prio        Priority of the new group or module before @p group
 *                      , one of AUTO_INIT_PRIO_x
 */
#define AUTO_INIT_PRIO_ADD_BEFORE_GROUP(group, prio)            \
    CONCAT(group, CONCAT(AUTO_INIT_PRIO_BEFORE, prio))

/**
 * @brief   Construct the global priority of a module or a group
 *          to be inserted after @p module
 *
 * @experimental
 *
 * @param   group       Priority of the group @p module is in
 * @param   module      Priority of the module within @p group, before the new group or module
 * @param   prio        Priority of the new group or module after @p module
 *                      , one of AUTO_INIT_PRIO_x
 */
#define AUTO_INIT_PRIO_ADD_AFTER_MODULE(group, module, prio)    \
    CONCAT(group, CONCAT(module, CONCAT(AUTO_INIT_PRIO_AFTER, prio)))

/**
 * @brief   Construct the global priority of a module or group
 *          to be inserted before @p module
 *
 * @experimental
 *
 * @param   group       Priority of the group @p module is in
 * @param   module      Priority of the module within @p group, after the new group or module
 * @param   prio        Priority of the new group or module before @p module
 *                      , one of AUTO_INIT_PRIO_x
 */
#define AUTO_INIT_PRIO_ADD_BEFORE_MODULE(group, module, prio)    \
    CONCAT(group, CONCAT(module, CONCAT(AUTO_INIT_PRIO_BEFORE, prio)))

/**
 * @name    Numeric group priority prefixes of top level groups
 * @experimental
 * @{
 */
#define AUTO_INIT_PRIO_GP_CPU                               101     /**< CPU group priority */
#define AUTO_INIT_PRIO_GP_BOARD                             102     /**< Board group priority */
#define AUTO_INIT_PRIO_GP_CORE                              103     /**< Core group priority */
#define AUTO_INIT_PRIO_GP_DRIVERS                           104     /**< Drivers group priority */
#define AUTO_INIT_PRIO_GP_FILESYSTEM                        105     /**< Filesystem group priority */
#define AUTO_INIT_PRIO_GP_NETWORK                           106     /**< Network group priority */
#define AUTO_INIT_PRIO_GP_TEST                              109     /**< Test group priority */
/** @} */

/**
 * @name    Priority levels from highest to lowest used for subgrouping
 *          and enumeration
 * @experimental
 * @{
 */
#define AUTO_INIT_PRIO_BEFORE                               00  /**< Insert something before */
#define AUTO_INIT_PRIO_1                                    01  /**< 1 */
#define AUTO_INIT_PRIO_2                                    02  /**< 2 */
#define AUTO_INIT_PRIO_3                                    03  /**< 3 */
#define AUTO_INIT_PRIO_4                                    04  /**< 4 */
#define AUTO_INIT_PRIO_5                                    05  /**< 5 */
#define AUTO_INIT_PRIO_6                                    06  /**< 6 */
#define AUTO_INIT_PRIO_7                                    07  /**< 7 */
#define AUTO_INIT_PRIO_8                                    08  /**< 8 */
#define AUTO_INIT_PRIO_9                                    09  /**< 9 */
#define AUTO_INIT_PRIO_10                                   10  /**< 10 */
#define AUTO_INIT_PRIO_11                                   11  /**< 11 */
#define AUTO_INIT_PRIO_12                                   12  /**< 12 */
#define AUTO_INIT_PRIO_13                                   13  /**< 13 */
#define AUTO_INIT_PRIO_14                                   14  /**< 14 */
#define AUTO_INIT_PRIO_15                                   15  /**< 15 */
#define AUTO_INIT_PRIO_16                                   16  /**< 16 */
#define AUTO_INIT_PRIO_17                                   17  /**< 17 */
#define AUTO_INIT_PRIO_18                                   18  /**< 18 */
#define AUTO_INIT_PRIO_19                                   19  /**< 19 */
#define AUTO_INIT_PRIO_20                                   20  /**< 20 */
#define AUTO_INIT_PRIO_21                                   21  /**< 21 */
#define AUTO_INIT_PRIO_22                                   22  /**< 22 */
#define AUTO_INIT_PRIO_23                                   23  /**< 23 */
#define AUTO_INIT_PRIO_24                                   24  /**< 24 */
#define AUTO_INIT_PRIO_25                                   25  /**< 25 */
#define AUTO_INIT_PRIO_26                                   26  /**< 26 */
#define AUTO_INIT_PRIO_27                                   27  /**< 27 */
#define AUTO_INIT_PRIO_28                                   28  /**< 28 */
#define AUTO_INIT_PRIO_29                                   29  /**< 29 */
#define AUTO_INIT_PRIO_30                                   30  /**< 30 */
#define AUTO_INIT_PRIO_31                                   31  /**< 31 */
#define AUTO_INIT_PRIO_32                                   32  /**< 32 */
#define AUTO_INIT_PRIO_33                                   33  /**< 33 */
#define AUTO_INIT_PRIO_34                                   34  /**< 34 */
#define AUTO_INIT_PRIO_35                                   35  /**< 35 */
#define AUTO_INIT_PRIO_36                                   36  /**< 36 */
#define AUTO_INIT_PRIO_37                                   37  /**< 37 */
#define AUTO_INIT_PRIO_38                                   38  /**< 38 */
#define AUTO_INIT_PRIO_39                                   39  /**< 39 */
#define AUTO_INIT_PRIO_40                                   40  /**< 40 */
#define AUTO_INIT_PRIO_41                                   41  /**< 41 */
#define AUTO_INIT_PRIO_42                                   42  /**< 42 */
#define AUTO_INIT_PRIO_43                                   43  /**< 43 */
#define AUTO_INIT_PRIO_44                                   44  /**< 44 */
#define AUTO_INIT_PRIO_45                                   45  /**< 45 */
#define AUTO_INIT_PRIO_46                                   46  /**< 46 */
#define AUTO_INIT_PRIO_47                                   47  /**< 47 */
#define AUTO_INIT_PRIO_48                                   48  /**< 48 */
#define AUTO_INIT_PRIO_49                                   49  /**< 49 */
#define AUTO_INIT_PRIO_50                                   50  /**< 50 */
#define AUTO_INIT_PRIO_51                                   51  /**< 51 */
#define AUTO_INIT_PRIO_52                                   52  /**< 52 */
#define AUTO_INIT_PRIO_53                                   53  /**< 53 */
#define AUTO_INIT_PRIO_54                                   54  /**< 54 */
#define AUTO_INIT_PRIO_55                                   55  /**< 55 */
#define AUTO_INIT_PRIO_56                                   56  /**< 56 */
#define AUTO_INIT_PRIO_57                                   57  /**< 57 */
#define AUTO_INIT_PRIO_58                                   58  /**< 58 */
#define AUTO_INIT_PRIO_59                                   59  /**< 59 */
#define AUTO_INIT_PRIO_60                                   60  /**< 60 */
#define AUTO_INIT_PRIO_61                                   61  /**< 61 */
#define AUTO_INIT_PRIO_62                                   62  /**< 62 */
#define AUTO_INIT_PRIO_63                                   63  /**< 63 */
#define AUTO_INIT_PRIO_64                                   64  /**< 64 */
#define AUTO_INIT_PRIO_65                                   65  /**< 65 */
#define AUTO_INIT_PRIO_66                                   66  /**< 66 */
#define AUTO_INIT_PRIO_67                                   67  /**< 67 */
#define AUTO_INIT_PRIO_68                                   68  /**< 68 */
#define AUTO_INIT_PRIO_69                                   69  /**< 69 */
#define AUTO_INIT_PRIO_70                                   70  /**< 70 */
#define AUTO_INIT_PRIO_71                                   71  /**< 71 */
#define AUTO_INIT_PRIO_72                                   72  /**< 72 */
#define AUTO_INIT_PRIO_73                                   73  /**< 73 */
#define AUTO_INIT_PRIO_74                                   74  /**< 74 */
#define AUTO_INIT_PRIO_75                                   75  /**< 75 */
#define AUTO_INIT_PRIO_76                                   76  /**< 76 */
#define AUTO_INIT_PRIO_77                                   77  /**< 77 */
#define AUTO_INIT_PRIO_78                                   78  /**< 78 */
#define AUTO_INIT_PRIO_79                                   79  /**< 79 */
#define AUTO_INIT_PRIO_80                                   80  /**< 80 */
#define AUTO_INIT_PRIO_81                                   81  /**< 81 */
#define AUTO_INIT_PRIO_82                                   82  /**< 82 */
#define AUTO_INIT_PRIO_83                                   83  /**< 83 */
#define AUTO_INIT_PRIO_84                                   84  /**< 84 */
#define AUTO_INIT_PRIO_85                                   85  /**< 85 */
#define AUTO_INIT_PRIO_86                                   86  /**< 86 */
#define AUTO_INIT_PRIO_87                                   87  /**< 87 */
#define AUTO_INIT_PRIO_88                                   88  /**< 88 */
#define AUTO_INIT_PRIO_89                                   89  /**< 89 */
#define AUTO_INIT_PRIO_90                                   90  /**< 90 */
#define AUTO_INIT_PRIO_91                                   91  /**< 91 */
#define AUTO_INIT_PRIO_92                                   92  /**< 92 */
#define AUTO_INIT_PRIO_93                                   93  /**< 93 */
#define AUTO_INIT_PRIO_94                                   94  /**< 94 */
#define AUTO_INIT_PRIO_95                                   95  /**< 95 */
#define AUTO_INIT_PRIO_96                                   96  /**< 96 */
#define AUTO_INIT_PRIO_97                                   97  /**< 97 */
#define AUTO_INIT_PRIO_98                                   98  /**< 98 */
#define AUTO_INIT_PRIO_AFTER                                99  /**< Insert something after */
/** @} */

/**
 * @name    Priorities of RIOT auto-init groups, which must be within one top level group
 * @experimental
 * @{
 */
#define AUTO_INIT_PRIO_GP_TIMERS                            AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_1)        /**< Timers group priority */
#define AUTO_INIT_PRIO_GP_RNG                               AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_2)        /**< RNG group priority */
#define AUTO_INIT_PRIO_GP_SCHEDULING                        AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_3)        /**< Scheduling group priority */
#define AUTO_INIT_PRIO_GP_EVENTS_AND_MESSAGING              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_4)        /**< Events and messaging group priority */
#define AUTO_INIT_PRIO_GP_SECURITY                          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_5)        /**< Security group priority */
#define AUTO_INIT_PRIO_GP_BUS                               AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_6)        /**< Bus group priority */
#define AUTO_INIT_PRIO_GP_STORAGE                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_DRIVERS, AUTO_INIT_PRIO_1)     /**< Storage group priority */
#define AUTO_INIT_PRIO_GP_SCREENS                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_DRIVERS, AUTO_INIT_PRIO_2)     /**< Screens group priority */
#define AUTO_INIT_PRIO_GP_MULTIMEDIA                        AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_DRIVERS, AUTO_INIT_PRIO_3)     /**< Multimedia group priority */
#define AUTO_INIT_PRIO_GP_SENSORS_AND_ACTUATORS             AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_DRIVERS, AUTO_INIT_PRIO_4)     /**< Sensors and Actuators group priority */
#define AUTO_INIT_PRIO_GP_INTERFACES                        AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_DRIVERS, AUTO_INIT_PRIO_5)     /**< Interfaces group priority */
#define AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC                AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK, AUTO_INIT_PRIO_1)     /**< GNRC network stack group priority */
#define AUTO_INIT_PRIO_GP_NETWORK_STACK_LWIP                AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK, AUTO_INIT_PRIO_2)     /**< LWIP network stack group priority */
#define AUTO_INIT_PRIO_GP_NETWORK_STACK_OPENTHREAD          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK, AUTO_INIT_PRIO_3)     /**< Openthread network stack group priority */
#define AUTO_INIT_PRIO_GP_NETWORK_STACK_OPENWSN             AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK, AUTO_INIT_PRIO_4)     /**< Openwsn network stack group priority */
#define AUTO_INIT_PRIO_GP_NETWORK_APPLICATION               AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK, AUTO_INIT_PRIO_5)     /**< Network applications group priority */
/** @} */

/**
 * @name Priorities of RIOT auto-init modules
 * @experimental
 * @{
 */
#define AUTO_INIT_PRIO_ZTIMER                               AUTO_INIT_PRIO_1    /**< Module priority of ztimer within its group */
#define AUTO_INIT_PRIO_XTIMER                               AUTO_INIT_PRIO_2    /**< Module priority of xtimer within its group */
#define AUTO_INIT_PRIO_MOD_ZTIMER                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_TIMERS, AUTO_INIT_PRIO_ZTIMER) /**< Priority of ztimer */
#define AUTO_INIT_PRIO_MOD_XTIMER                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_TIMERS, AUTO_INIT_PRIO_XTIMER) /**< Priority of xtimer */

#define AUTO_INIT_PRIO_RANDOM                               AUTO_INIT_PRIO_1    /**< Module priority of random within its group */
#define AUTO_INIT_PRIO_MOD_RANDOM                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_RNG, AUTO_INIT_PRIO_RANDOM)    /**< Priority of random */

#define AUTO_INIT_PRIO_SCHEDSTATISTICS                      AUTO_INIT_PRIO_1    /**< Module priority of schedstatistics within its group */
#define AUTO_INIT_PRIO_MOD_SCHEDSTATISTICS                  AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SCHEDULING, AUTO_INIT_PRIO_SCHEDSTATISTICS)    /**< Priority of schedstatistics */

#define AUTO_INIT_PRIO_EVENT_THREAD                         AUTO_INIT_PRIO_1    /**< Module priority of event thread within its group */
#define AUTO_INIT_PRIO_SYS_BUS                              AUTO_INIT_PRIO_2    /**< Module priority of sys bus within its group */
#define AUTO_INIT_PRIO_MOD_EVENT_THREAD                     AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_EVENTS_AND_MESSAGING, AUTO_INIT_PRIO_EVENT_THREAD) /**< Priority of event thread */
#define AUTO_INIT_PRIO_MOD_SYS_BUS                          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_EVENTS_AND_MESSAGING, AUTO_INIT_PRIO_SYS_BUS)      /**< Priority of sys bus */

#define AUTO_INIT_PRIO_MOD_MYNEWT                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_98)   /**< Priority of mynewt */

#define AUTO_INIT_PRIO_SOCK_DTLS                            AUTO_INIT_PRIO_1    /**< Module priority of DTLS sockets within its group */
#define AUTO_INIT_PRIO_DSM                                  AUTO_INIT_PRIO_2    /**< Module priority of DSM within its group */
#define AUTO_INIT_PRIO_CRYPTOAUTHLIB                        AUTO_INIT_PRIO_3    /**< Module priority of crypthauthlib within its group */
#define AUTO_INIT_PRIO_MOD_SOCK_DTLS                        AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SECURITY, AUTO_INIT_PRIO_SOCK_DTLS)        /**< Priority of DTLS sockets */
#define AUTO_INIT_PRIO_MOD_DSM                              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SECURITY, AUTO_INIT_PRIO_DSM)              /**< Priority of DSM */
#define AUTO_INIT_PRIO_MOD_CRYPTOAUTHLIB                    AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SECURITY, AUTO_INIT_PRIO_CRYPTOAUTHLIB)    /**< Priority of cryptauthlib */

#define AUTO_INIT_PRIO_USB                                  AUTO_INIT_PRIO_1    /**< Module priority of USB within its group */
#define AUTO_INIT_PRIO_CAN                                  AUTO_INIT_PRIO_2    /**< Module priority of CAN within its group */
#define AUTO_INIT_PRIO_MOD_USB                              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_BUS, AUTO_INIT_PRIO_USB)   /**< Priority of USB */
#define AUTO_INIT_PRIO_MOD_CAN                              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_BUS, AUTO_INIT_PRIO_CAN)   /**< Priority of CAN */

#define AUTO_INIT_PRIO_MCI                                  AUTO_INIT_PRIO_1    /**< Module priority of MCI within its group */
#define AUTO_INIT_PRIO_MOD_MCI                              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_STORAGE, AUTO_INIT_PRIO_MOD_MCI)   /**< Priority of MSI */

#define AUTO_INIT_PRIO_DEVFS                                AUTO_INIT_PRIO_1    /**< Module priority of DEVFS within its group */
#define AUTO_INIT_PRIO_MOD_DEVFS                            AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_FILESYSTEM, AUTO_INIT_PRIO_DEVFS)  /**< Priority of DEVFS */

#define AUTO_INIT_PRIO_GNRC_PKTBUF                          AUTO_INIT_PRIO_1    /**< Module priority of GNRC pktbuf within its group */
#define AUTO_INIT_PRIO_GNRC_NETIF                           AUTO_INIT_PRIO_2    /**< Module priority of GNRC netif within its group */
#define AUTO_INIT_PRIO_SKALD                                AUTO_INIT_PRIO_3    /**< Module priority of skald within its group */
#define AUTO_INIT_PRIO_NIMBLE                               AUTO_INIT_PRIO_4    /**< Module priority of nimble within its group */
#define AUTO_INIT_PRIO_LORAMAC                              AUTO_INIT_PRIO_5    /**< Module priority of loramac within its group */
#define AUTO_INIT_PRIO_UWB_CORE                             AUTO_INIT_PRIO_6    /**< Module priority of UWB core within its group */
#define AUTO_INIT_PRIO_MOD_GNRC_PKTBUF                      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_GNRC_PKTBUF)    /**< Priority of GNRC pktbuf */
#define AUTO_INIT_PRIO_MOD_GNRC_NETIF                       AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_GNRC_NETIF)     /**< Priority of GNRC netif */
#define AUTO_INIT_PRIO_MOD_SKALD                            AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_SKALD)          /**< Priority of skald */
#define AUTO_INIT_PRIO_MOD_NIMBLE                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_NIMBLE)         /**< Priority of nimble */
#define AUTO_INIT_PRIO_MOD_LORAMAC                          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_LORAMAC)        /**< Priority of loramac */
#define AUTO_INIT_PRIO_MOD_UWB_CORE                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_INTERFACES, AUTO_INIT_PRIO_UWB_CORE)       /**< Priority of UWB core */

#define AUTO_INIT_PRIO_GNRC_IPV6                            AUTO_INIT_PRIO_1    /**< Module priority of GNRC IPV6 within its group */
#define AUTO_INIT_PRIO_GNRC_SIXLOWPAN                       AUTO_INIT_PRIO_2    /**< Module priority of GNRC sixlowpan within its group */
#define AUTO_INIT_PRIO_GNRC_RPL                             AUTO_INIT_PRIO_3    /**< Module priority of GNRC RPL within its group */
#define AUTO_INIT_PRIO_GNRC_IPV6_NIB                        AUTO_INIT_PRIO_4    /**< Module priority of GNRC IPV6 NIB within its group */
#define AUTO_INIT_PRIO_GNRC_UDP                             AUTO_INIT_PRIO_5    /**< Module priority of GNRC UDP within its group */
#define AUTO_INIT_PRIO_GNRC_TCP                             AUTO_INIT_PRIO_6    /**< Module priority of GNRC TCP within its group */
#define AUTO_INIT_PRIO_GNRC_UHCPC                           AUTO_INIT_PRIO_7    /**< Module priority of GNRC UHCPC within its group */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6                        AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_IPV6)      /**< Priority of GNRC IPV6 */
#define AUTO_INIT_PRIO_MOD_GNRC_SIXLOWPAN                   AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_SIXLOWPAN) /**< Priority of GNRC sixlowpan */
#define AUTO_INIT_PRIO_MOD_GNRC_RPL                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_RPL)       /**< Priority of GNRC RPL */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6_NIB                    AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_IPV6_NIB)  /**< Priority of GNRC IPV6 NIB */
#define AUTO_INIT_PRIO_MOD_GNRC_UDP                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_UDP)       /**< Priority of GNRC UDP */
#define AUTO_INIT_PRIO_MOD_GNRC_TCP                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_TCP)       /**< Priority of GNRC TCP */
#define AUTO_INIT_PRIO_MOD_GNRC_UHCPC                       AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_GNRC, AUTO_INIT_PRIO_GNRC_UHCPC)     /**< Priority of GNRC UHCPC */

#define AUTO_INIT_PRIO_LWIP                                 AUTO_INIT_PRIO_1    /**< Module priority of LWIP within its group */
#define AUTO_INIT_PRIO_MOD_LWIP                             AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_LWIP, AUTO_INIT_PRIO_LWIP)   /**< Priority of LWIP */

#define AUTO_INIT_PRIO_OPENTHREAD                           AUTO_INIT_PRIO_1    /**< Module priority of openthread within its group */
#define AUTO_INIT_PRIO_MOD_OPENTHREAD                       AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_OPENTHREAD, AUTO_INIT_PRIO_OPENTHREAD)   /**< Priority of openthread */

#define AUTO_INIT_PRIO_OPENWSN                              AUTO_INIT_PRIO_1    /**< Module priority of openwsn within its group */
#define AUTO_INIT_PRIO_MOD_OPENWSN                          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_STACK_OPENWSN, AUTO_INIT_PRIO_OPENWSN) /**< Priority of openwsn */

#define AUTO_INIT_PRIO_GCOAP                                AUTO_INIT_PRIO_1    /**< Module priority of gcoap within its group */
#define AUTO_INIT_PRIO_CORD_COMMON                          AUTO_INIT_PRIO_2    /**< Module priority of CORD within its group */
#define AUTO_INIT_PRIO_CORD_EP_STANDALONE                   AUTO_INIT_PRIO_3    /**< Module priority of CORD EP within its group */
#define AUTO_INIT_PRIO_ASYMCUTE                             AUTO_INIT_PRIO_4    /**< Module priority of asymcute within its group */
#define AUTO_INIT_PRIO_NDN                                  AUTO_INIT_PRIO_5    /**< Module priority of NDN within its group */
#define AUTO_INIT_PRIO_DHCPV6_CLIENT                        AUTO_INIT_PRIO_6    /**< Module priority of DHCPV6 client within its group */
#define AUTO_INIT_PRIO_DHCPV6_RELAY                         AUTO_INIT_PRIO_7    /**< Module priority of DHCPV6 relay within its group */
#define AUTO_INIT_PRIO_DHCPV6_CLIENT_SIMPLE_PD              AUTO_INIT_PRIO_8    /**< Module priority of DHCPV6 client simple PD within its group */
#define AUTO_INIT_PRIO_GNRC_IPV6_AUTO_SUBNETS               AUTO_INIT_PRIO_9    /**< Module priority of GNRC IPV6 auto-subnets within its group */
#define AUTO_INIT_PRIO_GNRC_PKTDUMP                         AUTO_INIT_PRIO_10   /**< Module priority of GNRC pktdump within its group */
#define AUTO_INIT_PRIO_SUIT_CONDITIONS                      AUTO_INIT_PRIO_11   /**< Module priority of SUIT conditions within its group */
#define AUTO_INIT_PRIO_MOD_GCOAP                            AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_GCOAP)                     /**< Priority of gcoap */
#define AUTO_INIT_PRIO_MOD_CORD_COMMON                      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_CORD_COMMON)               /**< Priority of CORD */
#define AUTO_INIT_PRIO_MOD_CORD_EP_STANDALONE               AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_CORD_EP_STANDALONE)        /**< Priority of CORD EP */
#define AUTO_INIT_PRIO_MOD_ASYMCUTE                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_ASYMCUTE)                  /**< Priority of asymcute */
#define AUTO_INIT_PRIO_MOD_NDN                              AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_NDN)                       /**< Priority of NDN */
#define AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT                    AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_DHCPV6_CLIENT)             /**< Priority of DHCPV6 client */
#define AUTO_INIT_PRIO_MOD_DHCPV6_RELAY                     AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_DHCPV6_RELAY)              /**< Priority of DHCPV6 relay */
#define AUTO_INIT_PRIO_MOD_DHCPV6_CLIENT_SIMPLE_PD          AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_DHCPV6_CLIENT_SIMPLE_PD)   /**< Priority of DHCPV6 client simple PD */
#define AUTO_INIT_PRIO_MOD_GNRC_IPV6_AUTO_SUBNETS           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_GNRC_IPV6_AUTO_SUBNETS)    /**< Priority of GNRC IPV6 auto-subnets */
#define AUTO_INIT_PRIO_MOD_GNRC_PKTDUMP                     AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_GNRC_PKTDUMP)              /**< Priority of GNRC pktdump */
#define AUTO_INIT_PRIO_MOD_SUIT_CONDITIONS                  AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_NETWORK_APPLICATION, AUTO_INIT_PRIO_SUIT_CONDITIONS)           /**< Priority of SUIT conditions */

#define AUTO_INIT_PRIO_SHT1X                                AUTO_INIT_PRIO_1    /**< Module priority of SHT1X within its group */
#define AUTO_INIT_PRIO_SAUL                                 AUTO_INIT_PRIO_2    /**< Module priority of SAUL within its group */
#define AUTO_INIT_PRIO_MOD_SHT1X                            AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SENSORS_AND_ACTUATORS, AUTO_INIT_PRIO_SHT1X)   /**< Priority of SHT1X */
#define AUTO_INIT_PRIO_MOD_SAUL                             AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SENSORS_AND_ACTUATORS, AUTO_INIT_PRIO_SAUL)    /**< Priority of SAUL */

#define AUTO_INIT_PRIO_DFPLAYER                             AUTO_INIT_PRIO_1    /**< Module priority of DFPlayer within its group */
#define AUTO_INIT_PRIO_MOD_DFPLAYER                         AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_MULTIMEDIA, AUTO_INIT_PRIO_DFPLAYER)   /**< Priority of DFPlayer */

#define AUTO_INIT_PRIO_SCREENS                              AUTO_INIT_PRIO_1    /**< Module priority of screens within its group */
#define AUTO_INIT_PRIO_MOD_SCREEN                           AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_SCREENS, AUTO_INIT_PRIO_SCREENS)   /**< Priority of Screens */

#define AUTO_INIT_PRIO_TEST_UTILS_INTERACTIVE_SYNC          AUTO_INIT_PRIO_1    /**< Module priority of test utils interactive sync within its group */
#define AUTO_INIT_PRIO_BENCHMARK_UDP                        AUTO_INIT_PRIO_2    /**< Module priority of UDP benchmarks */
#define AUTO_INIT_PRIO_DUMMY_THREAD                         AUTO_INIT_PRIO_3    /**< Module priority of dummy thread within its group */
#define AUTO_INIT_PRIO_MOD_TEST_UTILS_INTERACTIVE_SYNC      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_TEST, AUTO_INIT_PRIO_TEST_UTILS_INTERACTIVE_SYNC)  /**< Priority of test utils interactive sync */
#define AUTO_INIT_PRIO_MOD_BENCHMARK_UDP                    AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_TEST, AUTO_INIT_PRIO_BENCHMARK_UDP)                /**< Priority of UDP benchmarks */
#define AUTO_INIT_PRIO_MOD_DUMMY_THREAD                     AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_GP_TEST, AUTO_INIT_PRIO_DUMMY_THREAD)                 /**< Priority of dummy thread */
/** @} */

/**
 * @brief   Add a module to the auto-initialization array
 *
 * @param   module      Module name
 * @param   prio        Priority level
 * @param   function    Function to be called on initialization
 *
 * @experimental
 */
#if IS_ACTIVE(CONFIG_AUTO_INIT_DEBUG) || defined(DOXYGEN)
#define AUTO_INIT_MODULE(module, prio, function)                                        \
    AUTO_INIT_MODULE_DEBUG(module, prio, function)
#else
#define AUTO_INIT_MODULE(module, prio, function)                                        \
    XFA_CONST(auto_init_xfa, prio)                                                      \
    auto_init_fn CONCAT(CONCAT(auto_init_xfa, prio), module) = (auto_init_fn)function
#endif

/**
 * @brief   Add a module to the auto-initialization array and print a debug
 *          message when @p function is called
 *
 * @internal
 */
#define AUTO_INIT_MODULE_DEBUG(module, prio, function)                                  \
    static inline void CONCAT(CONCAT(_debug_, prio), function)(void) {                  \
        puts("auto_init: " #module);                                                    \
        function();                                                                     \
    }                                                                                   \
    XFA_CONST(auto_init_xfa, prio)                                                      \
    auto_init_fn CONCAT(CONCAT(auto_init_xfa, prio), module) =                          \
        (auto_init_fn)CONCAT(CONCAT(_debug_, prio), function)

#ifdef __cplusplus
}
#endif

#endif /* AUTO_INIT_PRIORITIES_H */
/** @} */
