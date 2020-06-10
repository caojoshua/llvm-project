#include "llvm/Aasan/AasanYaml.h"

//using namespace llvm;

StringRef AasanRecord::Tag = "";
StringRef MemAllocRecord::Tag = "MemAlloc";
StringRef PointerDefRecord::Tag = "PtrDef";

AasanRecord::RecordKind AasanRecord::getKind() const {
  return Kind;
}

bool MemAllocRecord::classof(const AasanRecord *R) {
  return R->getKind() == AasanRecord::MemAllocKind;
}

bool PointerDefRecord::classof(const AasanRecord *R) {
  return R->getKind() == AasanRecord::PointerDefKind;
}

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

void MappingTraits<AasanRecord *>::mapping(IO &Io,
                                               AasanRecord *&Record) {
  if (Io.outputting()) {
    if (MemAllocRecord *MemAlloc = dyn_cast<MemAllocRecord>(Record)) {
      MappingTraits<MemAllocRecord *>::mapping(Io, MemAlloc);
    }
    if (PointerDefRecord *PointerDef = dyn_cast<PointerDefRecord>(Record)) {
      MappingTraits<PointerDefRecord *>::mapping(Io, PointerDef);
    }
  }

  // inputting
  else {
    if (Io.mapTag(MemAllocRecord::Tag)) {
      MemAllocRecord *MemAlloc = new MemAllocRecord;
      MappingTraits<MemAllocRecord *>::mapping(Io, MemAlloc);
      Record = MemAlloc;
    } else if (Io.mapTag(PointerDefRecord::Tag)) {
      PointerDefRecord *PointerDef = new PointerDefRecord;
      MappingTraits<PointerDefRecord *>::mapping(Io, PointerDef);
      Record = PointerDef;
    }
  }
}

void MappingTraits<MemAllocRecord *>::mapping(IO &Io, MemAllocRecord *&Record) {
  Io.mapTag(MemAllocRecord::Tag, true);
  Io.mapRequired("addr", Record->Addr);
  Io.mapRequired("size", Record->Size);
}

void MappingTraits<PointerDefRecord *>::mapping(IO &Io,
                                              PointerDefRecord *&Record) {
  Io.mapTag(PointerDefRecord::Tag, true);
  Io.mapRequired("pointer ID", Record->PointerID);
  Io.mapRequired("addr", Record->Addr);
}

void MappingTraits<StoreRecord *>::mapping(IO &Io, StoreRecord *&Record) {
  Io.mapRequired("Value", Record->Value);
  Io.mapRequired("addr", Record->Addr);
}
