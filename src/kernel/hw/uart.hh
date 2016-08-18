#ifndef KERNEL_HW_UART_HH
#define KERNEL_HW_UART_HH

#include "kernel/utils/non-copyable.hh"

namespace kernel {
  namespace hw {

    class uart : public utils::non_copyable {
    public:
      static void init();
      static const uart &uart0();

      explicit uart(const uint32_t &base);
      void write(const uint8_t &byte) const;
      uint8_t read() const;

    private:
      void disable() const;
      void enable() const;
      void clear_interupts() const;
      void mask_interupts() const;
      void configure() const;

      uint32_t dr;
      uint32_t rsrecr;
      uint32_t fr;
      uint32_t ilpr;
      uint32_t ibrd;
      uint32_t fbrd;
      uint32_t lcrh;
      uint32_t cr;
      uint32_t ifls;
      uint32_t imsc;
      uint32_t ris;
      uint32_t mis;
      uint32_t icr;
      uint32_t dmacr;
      uint32_t itcr;
      uint32_t itip;
      uint32_t itop;
      uint32_t tdr;
    };

  }
}

#endif // KERNEL_HW_UART_HH
