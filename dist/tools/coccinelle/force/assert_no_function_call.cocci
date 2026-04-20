// One might be tempted to write `assert(do_something() == 0);` if one expects
// that `do_something()` does succeed and wants to panic if not. However, with
// `-DNDEBUG` both the check of the return value but also the function call get
// optimized out.
//
// Something like `assert(self_test() == 0);` does make sense if and only if
// `self_test()` is a pure (as in `__attribute__((pure))`) function. We can
// exclude pure functions from this list, as not too many different pure
// functions end up in an `assert()` anyway.
//
// SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
// SPDX-License-Identifier: LGPL-2.1-only
// Confidence: Moderate
// Options:

@@
identifier f != {
	ARRAY_SIZE,
	atomic_load_kernel_pid,
	bitarithm_bits_set,
	block_get_enc_msg_len,
	clist_count,
	CMU_ClockFreqGet,
	dev,
	DEV,
	_does_handle_ack,
	dtls_get_app_data,
	_get_nud_state,
	__get_PRIMASK,
	gnrc_pktbuf_contains,
	gnrc_sixlowpan_frag_rb_entry_empty,
	_gpio_init_mode_check,
	gpio_is_equal,
	gpio_is_valid,
	GPIO_PORT_PIN_NUMOF,
	gpio_port_unpack_addr,
	HAS_ALIGNMENT_OF,
	ieee802154_radio_has_irq_ack_timeout,
	ipv6_addr_is_link_local,
	ipv6_hdr_is,
	irq_is_in,
	IS_ACTIVE,
	is_gpio_port_num_valid,
	_is_set,
	IS_USED,
	KHZ,
	kw41zrf_is_dsm,
	MHZ,
	msg_bus_get_sender_pid,
	msp430_auxiliary_clock_freq,
	netif_iter,
	_nib_onl_get_if,
	PARAMS,
	pid_is_valid,
	PWM_DEV,
	sixlowpan_sfr_is,
	soft_spi_bus_is_valid,
	soft_spi_mode_is_valid,
	SPI_DEV,
	strcmp,
	strncmp,
	thread_getpid,
	unicoap_message_is_request,
	_valid_offset,
	ztimer_is_set
};
expression list args;
expression value;
@@

(
- assert(f(args) == value);
+ /* TODO: replace C23 typeof() with type by hand for portability */
+ typeof(f(args)) tmp = f(args);
+ (void)tmp;
+ assert(tmp == value);
|
- assert(value == f(args));
+ /* TODO: replace C23 typeof() with type by hand for portability */
+ typeof(f(args)) tmp = f(args);
+ (void)tmp;
+ assert(tmp == value);
|
- assert(f(args));
+ bool tmp = f(args);
+ (void)tmp;
+ assert(tmp);
|
- assert(!f(args));
+ bool tmp = f(args);
+ (void)tmp;
+ assert(!tmp);
)
