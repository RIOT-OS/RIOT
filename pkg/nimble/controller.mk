MODULE = nimble_controller

SRC += ble_ll_sched.c
SRC += ble_ll_xcvr.c
SRC += ble_ll_whitelist.c
SRC += ble_ll_ctrl.c
SRC += ble_ll_hci.c
SRC += ble_ll_supp_cmd.c
SRC += ble_ll_adv.c
SRC += ble_ll_conn.c
SRC += ble_ll_resolv.c
SRC += ble_ll_conn_hci.c
SRC += ble_ll_rand.c
SRC += ble_ll.c
SRC += ble_ll_scan.c
SRC += ble_ll_dtm.c
SRC += ble_ll_hci_ev.c

include $(RIOTBASE)/Makefile.base
