#include "mhookwrapper.h"

const int kcallOffset = 3;
const int kjmpOffset = 12;
char shellcode[] = "\x60\x54\xE8\x00\x00\x00\x00\x83\xC4\x04\x61\xE9\x00\x00\x00\x00";

long calculateDistance(long hookedFunction, long hook) {
	return hook < hookedFunction ? 
		hookedFunction - hook : hook - hookedFunction;
}

void RegisterHook(long hookedFunction, long hook) {
	long dwDistance;
	char *function = (char *)VirtualAlloc(NULL, sizeof(shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(function, shellcode, sizeof(shellcode));
	int result = Mhook_SetHook(&(PVOID&)hookedFunction, function);
	dwDistance = calculateDistance(hookedFunction, (long)function + kjmpOffset + 4);
	memcpy(function + kjmpOffset, &dwDistance, 4);
	dwDistance = calculateDistance(hook, (long)function + kcallOffset + 4);
	memcpy(function + kcallOffset, &dwDistance, 4);
}

std::ostream &operator<<(std::ostream & stream,  Register t) {    
	stream << t.value;
	return stream;
}
