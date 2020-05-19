
#include "interception/interception.h"
#include "sanitizer_common/sanitizer_linux.h" 

extern "C" {
	// can define hooks here
}
void *aasan_Malloc(__sanitizer::uptr size);
void aasan_InitInterceptors(); 

namespace __aasan {
  static void aasan_init();
  void InitializeInterceptors();
}
