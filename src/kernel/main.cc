#include <stddef.h>
#include <stdint.h>

#include "hw/uart.hh"

size_t strlen(const char* str)
{
  size_t ret = 0;
  while ( str[ret] != 0 )
    ret++;
  return ret;
}

void uart_write(const unsigned char* buffer, size_t size)
{
  for ( size_t i = 0; i < size; i++ )
    kernel::hw::uart::uart0().write(buffer[i]);
}

void uart_puts(const char* str)
{
  uart_write((const unsigned char*) str, strlen(str));
}

extern "C" void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags) {
  (void) r0;
  (void) r1;
  (void) atags;

  kernel::hw::uart::init();
  uart_puts("Hello, kernel World!\r\n");

  const auto & uart = kernel::hw::uart::uart0();
  while ( true )
    uart.write(uart.read());
}