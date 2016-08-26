#include <stdio.h>

int main(int argc, char *argv[])
{
	static const char *defaultName = "data";
	const char *name = defaultName;
	if (argc == 2)
		name = argv[1];

	printf("const unsigned char %s[] = {\n", name);

	unsigned int cnt = 0;
loop:
	printf("\t");
	unsigned int i = 16;
	while (i--) {
		int c = getchar();
		if (c == -1) {
			if (i != 15)
				putchar('\n');
			printf("};\n");
			printf("\nextern const unsigned char %s[%u];\n", name, cnt);
			return 0;
		}
		printf("0x%02x, ", (unsigned char)c);
		cnt++;
	}
	putchar('\n');
	goto loop;

	return 1;
}
