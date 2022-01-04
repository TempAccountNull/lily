#include "init.h"
int main() { 
	Global::hModule = GetModuleHandleA(0);
	return Init(); 
}