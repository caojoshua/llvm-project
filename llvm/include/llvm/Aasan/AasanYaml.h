#include "llvm/ObjectYAML/YAML.h"

using llvm::yaml::IO;
using llvm::yaml::MappingTraits;
using llvm::yaml::ScalarTraits;

struct MemAllocRecord {
  MemAllocRecord(void *Addr, __uint64_t Size) : Addr(Addr), Size(Size) {}
  void *Addr;
  __uint64_t Size;
};

struct PointerDefRecord {
  PointerDefRecord(unsigned long int PointerId, void *Addr)
      : PointerID(PointerId), Addr(Addr) {}
  unsigned long int PointerID;
  void *Addr;
};

struct StoreRecord {
  StoreRecord(void *Value, void *Addr) : Value(Value), Addr(Addr) {}
  void *Value;
  void *Addr;
};

template <> struct ScalarTraits<void *> {
  static void output(void *const &Value, void *, llvm::raw_ostream &Out);
  static StringRef input(StringRef Scalar, void *, void *&Value);
  static QuotingType mustQuote(StringRef);
};

template <> struct MappingTraits<MemAllocRecord> {
  static void mapping(IO &Io, MemAllocRecord &Record);
};

template <> struct MappingTraits<PointerDefRecord> {
  static void mapping(IO &Io, PointerDefRecord &Record);
};

template <> struct MappingTraits<StoreRecord> {
  static void mapping(IO &Io, StoreRecord &Record);
};
