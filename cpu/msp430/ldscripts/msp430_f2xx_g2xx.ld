/* provide address for USCI register map by taking the address of the first
 * register (as provided by the vendor files) */
PROVIDE(USCI_A0 = UCA0ABCTL);
PROVIDE(USCI_A1 = UCA1ABCTL);
PROVIDE(USCI_B0 = UCB0CTL0);
PROVIDE(USCI_B1 = UCB1CTL0);
