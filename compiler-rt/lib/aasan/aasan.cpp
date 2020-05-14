
#include "aasan.h"
#include <stdio.h>

using namespace __sanitizer;

INTERCEPTOR(void *, malloc, uptr size) {
	printf("malloc intercepted!\n");
	return REAL(malloc)(size);
}

void aasan_InitInterceptors() {
	INTERCEPT_FUNCTION(malloc);
}
