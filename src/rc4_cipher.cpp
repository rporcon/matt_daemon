#include "ben_afk.hpp"

unsigned char	s[256];

void	swap(unsigned char *a, unsigned char *b)
{
	unsigned char temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

// key max len: 256 bytes
void	rc4_init(const unsigned char *key, int keylen)
{
	unsigned char	t[256];
	int				tmp = 0;

	for (int i = 0; i < 256; i++) {
		s[i] = i;
		t[i] = key[i % keylen];
	}
	for (int i = 0; i < 256; i++) {
		tmp = (tmp + s[i] + t[i]) % 256;
		swap(&s[i], &s[tmp]);
	}
}

void	rc4(const unsigned char *key, int keylen, char *data, size_t data_len)
{
	rc4_init(key, keylen);
	unsigned long t1 = 0;
	unsigned long t2 = 0;
	unsigned char val;

	for (size_t i = 0; i < data_len; i++) {
		t1 = (t1 + 1) % 256;
		t2 = (t2 + s[t1]) % 256;
		swap(&s[t1], &s[t2]);
		val = (s[t1] + s[t2]) % 256;
		*data = *data ^ val;
		data++;
	}
}
