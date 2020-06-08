#include "aasan_hooks.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "llvm/Aasan/AasanYaml.h"
#include <stdio.h>

llvm::yaml::Output yout(llvm::outs());

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void MemAllocHook(void* addr, __int64_t size)
{
  MemAllocRecord record(addr, size);
  yout << record;
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void PointerDefHook(unsigned long int PointerID, void* addr)
{
  PointerDefRecord record(PointerID, addr);
  yout << record;
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void StoreHook(void* value, void* addr)
{
  // TODO
}
