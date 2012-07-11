#include "stdio.h"
#include "DLLInjector.h"

int main(int argc, char* argv[]) {
	if (Inject("ZumaHook.dll", "ZumasRevenge.exe")) {
		printf("Injection done\n");
	} else {
		printf("Injection error\n");
	}
	return 0;
}
