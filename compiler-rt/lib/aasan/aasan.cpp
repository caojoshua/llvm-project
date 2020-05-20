
#include "aasan.h"
#include <stdio.h>
#include <unistd.h>

using namespace __sanitizer;

INTERCEPTOR(void*, malloc, uptr size)
{
  write(1, "Hooked malloc!\n", internal_strlen("Hooked malloc!\n"));
  return REAL(malloc)(size);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void MemAllocHook(void* addr, __int64_t size)
{
  printf("memory allocation\n\tstart address: %p\n\tsize in bytes: %lu\n", addr,
      size);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void PointerDefHook(void* value, void* addr)
{
  printf("pointer definition\n\tLHS pointer addr (this should log pointerID): %p\n\tload from(null if ptr=malloc-like): %p\n", value, addr);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void StoreHook(void* value, void* addr)
{
  printf("store\n\tvalue to store: %p\n\tstore into: %p\n", value, addr);
}

using namespace __aasan;

void initializeInterceptors()
{
  write(1, "init intercetors\n", internal_strlen("init_interceptors"));
  INTERCEPT_FUNCTION(malloc);
}

static void __aasan::aasan_init()
{
  write(1, "aasan_init\n", internal_strlen("aasan_init\n"));
  initializeInterceptors();
}

#if SANITIZER_CAN_USE_PREINIT_ARRAY
__attribute__((section(".preinit_array"), used)) static void (*__aasan_init_ptr)() = aasan_init;
#endif
