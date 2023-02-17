#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("ZipJpeg file analyzer v0.01\n\n");

	if (argc < 2) {
		printf("Usage: %s filename");
		return 1;
	}

	printf("Analyzing file: %s\n", argv[1]);

	return 0;
}
