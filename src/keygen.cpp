#include "ben_afk.hpp"

//http://burtleburtle.net/bob/hash/doobs.html
unsigned long gen_seed(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void gen_random(char *s) {
	static const char	alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"#$%&'()*+,-./\\:;<=>?@[]_^`{|}~";
	unsigned long		seed = gen_seed(clock(), time(NULL), getpid());

	srand(seed);
	for (int i = 0; i < KEYLEN; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
}

void	keygen()
{
	char	key[KEYLEN + 1] = {0};

	printf("Your keys: \n");
	gen_random(key);
	printf("\tpublic:  %s\n", key);
	gen_random(key);
	printf("\tprivate: %s\n", key);
	exit(0);
}
