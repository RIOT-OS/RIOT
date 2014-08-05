      /*
       * Copyright (C) 2014 Hamburg University of Applied Sciences
       *
       * This file is subject to the terms and conditions of the GNU Lesser General
       * Public License. See the file LICENSE in the top level directory for more
       * details.
       */

      /**
       * @ingroup     driver_periph
       * @{
       *
       * @file        spi.c
       * @brief       Low-level SPI driver implementation
       *
       * @author      Maxime Blanloeil <maxime.blanloeil@phelma.grenoble-inp.fr>
       *
       * @}
       */

      #include "cpu.h"
      #include "periph/gpio.h"
      #include "periph_conf.h"
      #include "periph/spi.h"
      #include "sam3x8e.h"

      typedef struct {
          char (*cb)(char data);
      } spi_state_t;

      static inline void irq_handler(spi_t dev);

      static spi_state_t config[SPI_NUMOF];

      static char cb_delay = 0xb8;

      static char byte_begin = 0xab;

      int spi_poweron(spi_t dev){

          switch (dev) {
      #if SPI_0_EN
              case SPI_0:
                  SPI_0_CLKEN(); /* Enable peripheral clock for SPI0 */
                  SPI_0_PORT_CLKEN();/* Enable peripheral clock for PIOA */
                  break;
      #endif
      #if SPI_1_EN
              case SPI_1:
                  SPI_1_CLKEN();
                  break;
      #endif

          }
          return 1;
      }

      int spi_poweroff(spi_t dev){

      switch (dev) {
      #if SPI_0_EN
              case SPI_0:
              	while( !(SPI_0_DEV->SPI_SR & SPI_SR_SPIENS) );/* wait until no more busy */
              	SPI_0_CLKDIS();
                  break;
      #endif
      #if SPI_1_EN
              case SPI_1:
              	while( !(SPI_1_DEV->SPI_SR & SPI_SR_SPIENS) );/* wait until no more busy */
              	SPI_1_CLKDIS();
                  break;
      #endif

          }

          return 1;
      }

      int spi_init_master(spi_t dev, spi_conf_t conf, uint32_t speed){

      	Pio *port = 0;
      	Spi *spi_port = 0;

          switch (dev) {
      #if SPI_0_EN
              case SPI_0:
      			port = SPI_0_PORT; 
      			spi_port = SPI_0_DEV;
                  /************************* PIO-Init *************************/
            /* For details on the following operations, please read the SAM3X documentation 
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. PIO description is given from
            the page 641 to the page 696 */
            /* Output mode */
           /* port->PIO_OER |= SPI_0_MISO_PIN;
            port->PIO_OER |= SPI_0_MOSI_PIN;
            port->PIO_OER |= SPI_0_SCK_PIN;
            port->PIO_OER |= SPI_0_NSS_PIN;*/

            /* Push-pull configuration */
            port->PIO_MDER &= ~SPI_0_MISO_PIN; 
            port->PIO_MDDR |= SPI_0_MISO_PIN;
            port->PIO_MDER &= ~SPI_0_MOSI_PIN; 
            port->PIO_MDDR |= SPI_0_MOSI_PIN;
            port->PIO_MDER &= ~SPI_0_SCK_PIN; 
            port->PIO_MDDR |= SPI_0_SCK_PIN;
            port->PIO_MDER &= ~SPI_0_NSS_PIN; 
            port->PIO_MDDR |= SPI_0_NSS_PIN;

            /* With pull-up resistors */
            port->PIO_PUDR &= ~SPI_0_MISO_PIN; 
            port->PIO_PUER |= SPI_0_MISO_PIN;
            port->PIO_PUDR &= ~SPI_0_MOSI_PIN; 
            port->PIO_PUER |= SPI_0_MOSI_PIN;
            port->PIO_PUDR &= ~SPI_0_SCK_PIN; 
            port->PIO_PUER |= SPI_0_SCK_PIN;
            port->PIO_PUDR &= ~SPI_0_NSS_PIN; 
            port->PIO_PUER |= SPI_0_NSS_PIN; 

            /* Clear output */
            port->PIO_SODR &= ~SPI_0_MISO_PIN; 
            port->PIO_CODR |= SPI_0_MISO_PIN;
            port->PIO_SODR &= ~SPI_0_MOSI_PIN; 
            port->PIO_CODR |= SPI_0_MOSI_PIN;
            port->PIO_SODR &= ~SPI_0_SCK_PIN; 
            port->PIO_CODR |= SPI_0_SCK_PIN;
            port->PIO_SODR &= ~SPI_0_NSS_PIN; 
            port->PIO_CODR |= SPI_0_NSS_PIN; 

            /************************* Configure PIOs to SPI0 *************************/
            /* For details on the following operations, please read the SAM3X documentation 
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. Peripherals description is given from
            the page 47 to the page 56 */

            /* Peripheral Function Selection */
            port->PIO_PER &= ~SPI_0_MISO_PIN; 
            port->PIO_PDR |= SPI_0_MISO_PIN;
            port->PIO_PER &= ~SPI_0_MOSI_PIN; 
            port->PIO_PDR |= SPI_0_MOSI_PIN;
            port->PIO_PER &= ~SPI_0_SCK_PIN; 
            port->PIO_PDR |= SPI_0_SCK_PIN;
            port->PIO_PER &= ~SPI_0_NSS_PIN; 
            port->PIO_PDR |= SPI_0_NSS_PIN;

            /* Periphal A */
            port->PIO_ABSR &= ~SPI_0_MISO_PIN;
            port->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            port->PIO_ABSR &= ~SPI_0_SCK_PIN;
            port->PIO_ABSR &= ~SPI_0_NSS_PIN;
            /* Chip Select Register */
            spi_port->SPI_CSR[0] = 0;
            spi_port->SPI_CSR[0] |= SPI_CSR_SCBR(speed);
            spi_port->SPI_CSR[0] |= SPI_CSR_DLYBCT(0x3);
            switch(conf){
              case SPI_CONF_FIRST_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;
             case SPI_CONF_SECOND_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;
             case SPI_CONF_FIRST_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;
             case SPI_CONF_SECOND_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

          }
      #endif
            break;
      		}

          /************************* SPI-Init *************************/	
          /* Control Register */
          spi_port->SPI_CR |= SPI_CR_SPIEN; 
          /* Mode Register */
          spi_port->SPI_MR = 0;
          spi_port->SPI_MR |= SPI_MR_MSTR;
          spi_port->SPI_MR &= ~SPI_MR_PS;
          spi_port->SPI_MR &= ~SPI_MR_PCS(0);
          /* Enable SPI interrupts */
          spi_port->SPI_IER = 0;
          spi_port->SPI_IER |= SPI_IER_RDRF;
          spi_port->SPI_IER |= SPI_IER_TDRE;
          spi_port->SPI_IER |= SPI_IER_TXEMPTY;
          spi_port->SPI_IER |= SPI_IER_OVRES;
          spi_port->SPI_IER |= SPI_IER_NSSR;

        return 1;

      }

    int spi_init_slave( spi_t dev, spi_conf_t conf, char (*cb)(char data) ){

        Pio *port = 0;
        Spi *spi_port = 0;

          switch (dev) {
      #if SPI_0_EN
              case SPI_0:
            port = SPI_0_PORT; 
            spi_port = SPI_0_DEV;
            NVIC_SetPriority(SPI_0_IRQ, SPI_0_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_0_IRQ);

            /************************* PIO-Init *************************/
            /* For details on the following operations, please read the SAM3X documentation 
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. PIO description is given from
            the page 641 to the page 696 */
            /* Output mode */
            port->PIO_OER |= SPI_0_MISO_PIN;

            /* Push-pull configuration */
            port->PIO_MDER &= ~SPI_0_MISO_PIN; 
            port->PIO_MDDR |= SPI_0_MISO_PIN;
            port->PIO_MDER &= ~SPI_0_MOSI_PIN; 
            port->PIO_MDDR |= SPI_0_MOSI_PIN;
            port->PIO_MDER &= ~SPI_0_SCK_PIN; 
            port->PIO_MDDR |= SPI_0_SCK_PIN;
            port->PIO_MDER &= ~SPI_0_NSS_PIN; 
            port->PIO_MDDR |= SPI_0_NSS_PIN;

            /* With pull-up resistors */
            port->PIO_PUDR &= ~SPI_0_MISO_PIN; 
            port->PIO_PUER |= SPI_0_MISO_PIN;
            port->PIO_PUDR &= ~SPI_0_MOSI_PIN; 
            port->PIO_PUER |= SPI_0_MOSI_PIN;
            port->PIO_PUDR &= ~SPI_0_SCK_PIN; 
            port->PIO_PUER |= SPI_0_SCK_PIN;
            port->PIO_PUDR &= ~SPI_0_NSS_PIN; 
            port->PIO_PUER |= SPI_0_NSS_PIN; 

            /* Clear output */
            port->PIO_SODR &= ~SPI_0_MISO_PIN; 
            port->PIO_CODR |= SPI_0_MISO_PIN;
            port->PIO_SODR &= ~SPI_0_MOSI_PIN; 
            port->PIO_CODR |= SPI_0_MOSI_PIN;
            port->PIO_SODR &= ~SPI_0_SCK_PIN; 
            port->PIO_CODR |= SPI_0_SCK_PIN;
            port->PIO_SODR &= ~SPI_0_NSS_PIN; 
            port->PIO_CODR |= SPI_0_NSS_PIN; 

            /* Enable interrupt */
            port->PIO_IER |= SPI_0_NSS_PIN; 
            port->PIO_IDR &= ~SPI_0_NSS_PIN; 

            /************************* Configure PIOs to SPI0 *************************/
            /* For details on the following operations, please read the SAM3X documentation 
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. Peripherals description is given from
            the page 47 to the page 56 */

            /* Peripheral Function Selection */
            port->PIO_PER &= ~SPI_0_MISO_PIN; 
            port->PIO_PDR |= SPI_0_MISO_PIN;
            port->PIO_PER &= ~SPI_0_MOSI_PIN; 
            port->PIO_PDR |= SPI_0_MOSI_PIN;
            port->PIO_PER &= ~SPI_0_SCK_PIN; 
            port->PIO_PDR |= SPI_0_SCK_PIN;
            port->PIO_PER &= ~SPI_0_NSS_PIN; 
            port->PIO_PDR |= SPI_0_NSS_PIN;

            /* Periphal A */
            port->PIO_ABSR &= ~SPI_0_MISO_PIN;
            port->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            port->PIO_ABSR &= ~SPI_0_SCK_PIN;
            port->PIO_ABSR &= ~SPI_0_NSS_PIN;

            /* Chip Select Register */
            spi_port->SPI_CSR[0] = 0;
            switch(conf){
              case SPI_CONF_FIRST_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;
             case SPI_CONF_SECOND_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;
             case SPI_CONF_FIRST_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;
             case SPI_CONF_SECOND_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

          }

      #endif
          break;
      }

           /************************* SPI-Init *************************/ 
          /* Control Register */
          spi_port->SPI_CR |= SPI_CR_SPIEN; 
          /* Mode Register */
          spi_port->SPI_MR = 0;
          /* Enable SPI interrupts */
          spi_port->SPI_IER = 0;
          spi_port->SPI_IER |= SPI_IER_RDRF;
          spi_port->SPI_IER |= SPI_IER_OVRES;
          /* set callback */
          config[dev].cb = cb;

        return 0;

      }
        
      int spi_transfer_byte(spi_t dev, char out, char *in){

          Spi * spi_port = 0;

          switch (dev) {
      #if SPI_0_EN
              case SPI_0:
                  spi_port = SPI_0_DEV;
                  break;
      #endif
      #if SPI_1_EN
              case SPI_1:
                  spi_port = SPI_1_DEV;
                  break;
      #endif
           } 

           while ( !(spi_port->SPI_SR & SPI_SR_TDRE ) ); 
           spi_port->SPI_TDR = SPI_TDR_TD(out);


           while ( !(spi_port->SPI_SR & SPI_SR_RDRF)); 
           *in = spi_port->SPI_RDR & SPI_RDR_RD_Msk;

           return 1;
      
      }


  int spi_transfer_bytes(spi_t dev, char *out, char *in, int length)
  {

      int i, trans_ret, trans_bytes = 0;
      char in_temp;

      for (i = 0; i < length; i++) {

          if (out) {
              trans_ret = spi_transfer_byte(dev, out[i], &in_temp);
          }
          else {
              trans_ret = spi_transfer_byte(dev, 0, &in_temp);
          }

          if (trans_ret < 0) {
              return -1;
          }

          if (in) {
              in[i] = in_temp;
          }

          trans_bytes++;
      }

      return trans_bytes++;
  }

  void spi_transmission_begin(spi_t dev, char reset_val)
  {

      Spi *spi_port = 0;

      switch (dev) {
  #if SPI_0_EN
          case SPI_0:
              spi_port = SPI_0_DEV;
              break;
  #endif

  #if SPI_1_EN
          case SPI_1:
              spi_port = SPI_1_DEV;
              break;
  #endif

      }

      spi_port->SPI_TDR = SPI_TDR_TD(reset_val);
  }

    static inline void irq_handler(spi_t dev)
    {

      char cb = 0;
      Spi *spi_port = 0;

    switch (dev) {
    #if SPI_0_EN
        case SPI_0:
          spi_port = SPI_0_DEV;
          break;
    #endif
    #if SPI_1_EN
        case SPI_1:
          spi_port = SPI_1_DEV;
          break;
    #endif
        }

        while ( !(spi_port->SPI_SR & SPI_SR_TDRE));
        spi_port->SPI_TDR = cb_delay;

        while ( !(spi_port->SPI_SR & SPI_SR_RDRF)); 
        cb = spi_port->SPI_RDR & SPI_RDR_RD_Msk;

          config[dev].cb(cb);
          /* return byte of callback is transferred to master in next transmission cycle */
          cb_delay = cb;
    }

    static inline void irq_handler_begin(spi_t dev)
  {
      printf("irq_handler_begin");
      spi_transmission_begin(dev, byte_begin);
  }


    __attribute__((naked))
    void isr_spi0(void)
    {

      ISR_ENTER();
      printf("csr: %x\n", SPI_0_DEV->SPI_CSR[0]);
      irq_handler(SPI_0);
      ISR_EXIT();
    }

    #if SPI_0_EN
  __attribute__((naked)) void isr_pioa(void)
  {
      ISR_ENTER();
      if (PIOA->PIO_ISR & PIO_PA28A_SPI0_NPCS0){
          irq_handler_begin(SPI_0);
      }
      ISR_EXIT();
  }
  #endif

  __attribute__((naked)) void isr_pioa(void)
{
    ISR_ENTER();
    uint32_t status = PIOA->PIO_ISR;
#ifdef GPIO_A0_MAP
    if (status & (1 << 0)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A1_MAP
    if (status & (1 << 1)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A2_MAP
    if (status & (1 << 2)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A3_MAP
    if (status & (1 << 3)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A4_MAP
    if (status & (1 << 4)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A5_MAP
    if (status & (1 << 5)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A6_MAP
    if (status & (1 << 6)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A7_MAP
    if (status & (1 << 7)) {
        puts ("nan mais allo");
      }
#endif
#ifdef GPIO_A8_MAP
    if (status & (1 << 8)) {
         puts ("nan mais allo");
       }
#endif
#ifdef GPIO_A9_MAP
    if (status & (1 << 9)) {
         puts ("nan mais allo");
       }
#endif
#ifdef GPIO_A10_MAP
    if (status & (1 << 10)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A11_MAP
    if (status & (1 << 11)) {
         puts ("nan mais allo");
       }
#endif
#ifdef GPIO_A12_MAP
    if (status & (1 << 12)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A13_MAP
    if (status & (1 << 13)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A14_MAP
    if (status & (1 << 14)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A15_MAP
    if (status & (1 << 15)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A16_MAP
    if (status & (1 << 16)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A17_MAP
    if (status & (1 << 17)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A18_MAP
    if (status & (1 << 18)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A19_MAP
    if (status & (1 << 19)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A20_MAP
    if (status & (1 << 20)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A21_MAP
    if (status & (1 << 21)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A22_MAP
    if (status & (1 << 22)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A23_MAP
    if (status & (1 << 23)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A24_MAP
    if (status & (1 << 24)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A25_MAP
    if (status & (1 << 25)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A26_MAP
    if (status & (1 << 26)) {
         puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A27_MAP
    if (status & (1 << 27)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A28_MAP
    if (status & (1 << 28)) {
        puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A29_MAP
    if (status & (1 << 29)) {
       puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A30_MAP
    if (status & (1 << 30)) {
       puts ("nan mais allo");
    }
#endif
#ifdef GPIO_A31_MAP
    if (status & (1 << 31)) {
         puts ("nan mais allo");
    }
#endif
    ISR_EXIT();
}





