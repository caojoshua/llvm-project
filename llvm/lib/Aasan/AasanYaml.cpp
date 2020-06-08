#include "llvm/Aasan/AasanYaml.h"

using namespace llvm;

// define a ScalarTrait for pointers because there is no default
void ScalarTraits<void *>::output(void *const &Value, void *,
                                  llvm::raw_ostream &out) {
  out << Value;
}

StringRef ScalarTraits<void *>::input(StringRef scalar, void *, void *&Value) {
  // TODO
  return "bar";
}

yaml::QuotingType ScalarTraits<void *>::mustQuote(StringRef) {
  return QuotingType::None;
}

void MappingTraits<MemAllocRecord>::mapping(IO &Io, MemAllocRecord &Record) {
  Io.mapRequired("addr", Record.Addr);
  Io.mapRequired("size", Record.Size);
}

void MappingTraits<PointerDefRecord>::mapping(IO &Io,
                                              PointerDefRecord &Record) {
  Io.mapRequired("pointer ID", Record.PointerID);
  Io.mapRequired("addr", Record.Addr);
}

void MappingTraits<StoreRecord>::mapping(IO &Io, StoreRecord &Record) {
  Io.mapRequired("Value", Record.Value);
  Io.mapRequired("addr", Record.Addr);
}
