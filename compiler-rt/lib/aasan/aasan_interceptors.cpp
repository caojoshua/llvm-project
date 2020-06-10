#include "aasan_interceptors.h"
#include "interception/interception.h"

using namespace __sanitizer;

INTERCEPTOR(void *, malloc, uptr size)
{
  //write(1, "Hooked malloc!\n", internal_strlen("Hooked malloc!\n"));
  void * addr = REAL(malloc)(size);
//   MemAllocYaml yaml(addr, size);
//   llvm::yaml::Output yout(llvm::outs());
//   yout << yaml;
  return addr;
}

using namespace __aasan;

void initializeInterceptors()
{
  INTERCEPT_FUNCTION(malloc);
}

static void __aasan::aasan_init()
{
  initializeInterceptors();
}

#if SANITIZER_CAN_USE_PREINIT_ARRAY
__attribute__((section(".preinit_array"), used)) static void (*__aasan_init_ptr)() = aasan_init;
#endif
