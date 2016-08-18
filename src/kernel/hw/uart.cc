#include <stddef.h>
#include <stdint.h>

#include "uart.hh"
#include "mmio.hh"
#include "delay.hh"

namespace kernel {
  namespace hw {

    static uart uart0_impl(
#ifdef QEMU
        0x101f1000
#else
        0x20201000
#endif
      );

    enum
    {
        // The GPIO registers base address.
        GPIO_BASE = 0x20200000,

        // The offsets for reach register.

        // Controls actuation of pull up/down to ALL GPIO pins.
        GPPUD = (GPIO_BASE + 0x94),

        // Controls actuation of pull up/down for specific GPIO pin.
        GPPUDCLK0 = (GPIO_BASE + 0x98),
    };

    void uart::init() {

      uart0_impl.disable();

      // TODO: move to gpio
      // Setup the GPIO pin 14 && 15.

      // Disable pull up/down for all GPIO pins & delay for 150 cycles.
      kernel::hw::mmio::write(GPPUD, 0x00000000);
      kernel::hw::delay(150);

      // Disable pull up/down for pin 14,15 & delay for 150 cycles.
      kernel::hw::mmio::write(GPPUDCLK0, (1 << 14) | (1 << 15));
      kernel::hw::delay(150);

      // Write 0 to GPPUDCLK0 to make it take effect.
      kernel::hw::mmio::write(GPPUDCLK0, 0x00000000);

      uart0_impl.clear_interupts();
      uart0_impl.configure();
      uart0_impl.mask_interupts();
      uart0_impl.enable();
    }

    const uart & uart::uart0() {
      return uart0_impl;
    }

    uart::uart(const uint32_t &base)
      : dr    (base + 0x00),
        rsrecr(base + 0x04),
        fr    (base + 0x18),
        ilpr  (base + 0x20),
        ibrd  (base + 0x24),
        fbrd  (base + 0x28),
        lcrh  (base + 0x2C),
        cr    (base + 0x30),
        ifls  (base + 0x34),
        imsc  (base + 0x38),
        ris   (base + 0x3C),
        mis   (base + 0x40),
        icr   (base + 0x44),
        dmacr (base + 0x48),
        itcr  (base + 0x80),
        itip  (base + 0x84),
        itop  (base + 0x88),
        tdr   (base + 0x8C) {
    }

    void uart::write(const uint8_t &byte) const {
      // Wait for UART to become ready to transmit.
      while ( kernel::hw::mmio::read(fr) & (1 << 5) );
      kernel::hw::mmio::write(dr, byte);
    }

    uint8_t uart::read() const {
      // Wait for UART to have recieved something.
      while ( kernel::hw::mmio::read(fr) & (1 << 4) );
      return kernel::hw::mmio::read(dr);
    }

    void uart::disable() const {
      kernel::hw::mmio::write(cr, 0x00000000);
    }

    void uart::enable() const {
      // Enable receive & transfer part of UART.
      kernel::hw::mmio::write(cr, (1 << 0) | (1 << 8) | (1 << 9));

    }

    void uart::clear_interupts() const {
      kernel::hw::mmio::write(icr, 0x7FF);
    }

    void uart::mask_interupts() const {
      // Mask all interrupts.
      kernel::hw::mmio::write(imsc, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
                             (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
    }

    void uart::configure() const {

      // Set integer & fractional part of baud rate.
      // Divider = UART_CLOCK/(16 * Baud)
      // Fraction part register = (Fractional part * 64) + 0.5
      // UART_CLOCK = 3000000; Baud = 115200.

      // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
      // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
      kernel::hw::mmio::write(ibrd, 1);
      kernel::hw::mmio::write(fbrd, 40);

      // Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
      kernel::hw::mmio::write(lcrh, (1 << 4) | (1 << 5) | (1 << 6));
    }

  }
}
