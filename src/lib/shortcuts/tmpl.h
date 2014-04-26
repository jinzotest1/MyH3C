#ifndef MUTIL_TEMPLATE_H
#define MUTIL_TEMPLATE_H (1)

/* Swap function TEMPLATE */

#define TEMPLATE_DEC_SWAP(Type) \
	void swap_##Type(Type *p1, Type *p2);

#define TEMPLATE_SWAP(Type) \
	void swap_##Type(Type *p1, Type *p2) \
{ \
	Type tmp = *p1; \
	*p1 = *p2; \
	*p2 = tmp; \
}

#define TEMPLATE_REVERSE(Type) \
	void reverse_##Type(Type arr[], const size_t nmemb) \
{ \
	for (size_t i = 0; i < nmemb / 2; i++) \
	swap_##Type(arr + i, arr + nmemb - 1 - i); \
}

#define Reverse(arr, len) {             \
	for (size_t i = 0; i < len / 2; i++)  \
	Swap(arr[i], arr[len - 1 - i]);       \
}

#endif
