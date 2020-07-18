#include "llvm/ObjectYAML/YAML.h"

using namespace llvm;
using yaml::IO;
using yaml::MappingTraits;
using yaml::ScalarTraits;

// base class for all records, allowing dynamic typing in yaml IO
struct AasanRecord {
public:
  enum RecordKind { MemAllocKind, PointerDefKind };

private:
  const RecordKind Kind;

public:
  static StringRef Tag;
  AasanRecord(RecordKind K) : Kind(K) {}
  virtual ~AasanRecord() {}
  RecordKind getKind() const;
};

struct MemAllocRecord : public AasanRecord {
public:
  static bool classof(const AasanRecord *R);
  MemAllocRecord() : MemAllocRecord(0, 0) {}
  MemAllocRecord(void *Addr, __uint64_t Size)
      : AasanRecord(AasanRecord::MemAllocKind), Addr((long) Addr), Size(Size) {}
  static StringRef Tag;
  long Addr;
  __uint64_t Size;
};

struct PointerDefRecord : public AasanRecord {
public:
  static bool classof(const AasanRecord *R);
  PointerDefRecord() : PointerDefRecord(0, 0) {}
  PointerDefRecord(unsigned long int PointerId, void *Addr)
      : AasanRecord(AasanRecord::PointerDefKind), PointerID(PointerId),
        Addr((long) Addr) {}
  static StringRef Tag;
  unsigned long int PointerID;
  long Addr;
};

// we probably dont need this
struct StoreRecord {
  StoreRecord(void *Value, void *Addr) : Value(Value), Addr(Addr) {}
  void *Value;
  void *Addr;
};


LLVM_YAML_IS_SEQUENCE_VECTOR(AasanRecord *)

template <> struct yaml::MappingTraits<AasanRecord *> {
  static void mapping(IO &Io, AasanRecord *&Record);
};

template <> struct yaml::MappingTraits<MemAllocRecord *> {
  static void mapping(IO &Io, MemAllocRecord *&Record);
};

template <> struct yaml::MappingTraits<PointerDefRecord *> {
  static void mapping(IO &Io, PointerDefRecord *&Record);
};

template <> struct yaml::MappingTraits<StoreRecord *> {
  static void mapping(IO &Io, StoreRecord *&Record);
};
