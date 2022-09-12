#ifndef HELLO_HELLPERS_H
#define HELLO_HELLPERS_H



// Helper function that allows to get a decimal order
// of an arbitrary interger value.
// For example:
//  -      1 -> 1;
//  -     10 -> 2;
//  - 123000 -> 6.
static int decimal_order (int n)
{
	int order = 1;

	while (n /= 10) {
		order++;
	}

	return order;
}



// Helmper function that converts usual decimal to a
// fractional decimal order.
// For example (if resolution is 9 for "nano" values):
//  -      1 -> 8;
//  -     10 -> 7;
//  - 123000 -> 3.
// You should think about the result as "a number of leading zeros".
static int fraction_order (int resolution, int decimal_order)
{
	return resolution - decimal_order;
}



// Helper function that removes trailing zeroes.
//  -      1 ->   1;
//  -     10 ->   1;
//  - 123000 -> 123.
static long normalize_fraction (long n)
{
	while (n && !(n % 10) ) {
		n /= 10;
	}

	return n;
}



struct prefix_t {
	char * buffer;
	int capacity;
	int terminator;
};



static int init_prefix (struct prefix_t * prefix, int resolution, void * (* allocator) (size_t) )
{
	if (!prefix) {
		return -EINVAL;
	}

	prefix->capacity = resolution;
	prefix->buffer = allocator (prefix->capacity);
	memset (prefix->buffer, '0', prefix->capacity - 1);
	prefix->terminator = prefix->capacity - 1;
	prefix->buffer [prefix->terminator] = '\0';

	return 0;
}



static char * update_prefix (struct prefix_t * prefix, long n)
{
	int terminator = 0;

	if (!prefix || !prefix->buffer) {
		return NULL;
	}

	terminator = fraction_order (prefix->capacity, decimal_order (n) );

	if (terminator != prefix->terminator) {
		prefix->buffer [prefix->terminator] = '0';
		prefix->terminator = terminator;
		prefix->buffer [prefix->terminator] = '\0';
	}

	return prefix->buffer;
}



static void release_prefix (struct prefix_t * prefix, void (* deallocator) (const void *) )
{
	if (!prefix) {
		return;
	}

	if (prefix->buffer) {
		deallocator (prefix->buffer);
		prefix->buffer = NULL;
	}

	prefix->capacity = 0;
	prefix->terminator = 0;
}

#endif//HELLO_HELLPERS_H
