#include "sanitizer_common/sanitizer_internal_defs.h"
#include "llvm/Aasan/AasanYaml.h"
#include <stdio.h>


static std::vector<AasanRecord *> AasanRecords;

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void MemAllocHook(void* addr, __int64_t size)
{ 
  MemAllocRecord *record = new MemAllocRecord(addr, size);
  AasanRecords.push_back(record);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void PointerDefHook(unsigned long int PointerID, void* addr)
{
  PointerDefRecord *record = new PointerDefRecord(PointerID, addr);
  AasanRecords.push_back(record);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void StoreHook(void* value, void* addr)
{
  // TODO
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void WriteRecordsHook()
{
  llvm::yaml::Output yout(llvm::outs());
  yout << AasanRecords;
  for (AasanRecord *Record : AasanRecords) {
    delete Record;
  }
}
