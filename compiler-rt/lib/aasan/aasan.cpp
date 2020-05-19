
#include "aasan.h"
#include <unistd.h>

using namespace __sanitizer;

INTERCEPTOR(void *, malloc, uptr size) {
  write(1, "Hooked malloc!\n", internal_strlen("Hooked malloc!\n"));
	return REAL(malloc)(size);
}

using namespace __aasan;

void initializeInterceptors() {
  write(1, "init intercetors\n", internal_strlen("init_interceptors"));
  INTERCEPT_FUNCTION(malloc);
}

static void __aasan::aasan_init() {
  write(1, "aasan_init\n", internal_strlen("aasan_init\n"));
  initializeInterceptors();
}

#if SANITIZER_CAN_USE_PREINIT_ARRAY
  __attribute__((section(".preinit_array"), used))
  static void (*__aasan_init_ptr)() = aasan_init;
#endif

