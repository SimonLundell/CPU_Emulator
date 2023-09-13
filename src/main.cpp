#include <stdio.h>
#include <stdlib.h>

struct CPU
{
	using Byte = unsigned char;
	using Word = unsigned short;

	Word PC; // Program counter
	Word SP; // Stack Pointer

	Byte A, X, Y; // Registers

	Byte C : 1; // Status flag
	Byte Z : 1; // Status flag
	Byte I : 1; // Status flag
	Byte D : 1; // Status flag
	Byte B : 1; // Status flag
	Byte V : 1; // Status flag
	Byte N : 1; // Status flag
};

int main()
{
	CPU cpu;
	return 0;
}
