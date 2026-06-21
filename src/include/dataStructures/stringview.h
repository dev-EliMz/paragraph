#ifndef DSA_STRINGVIEW_H
#define DSA_STRINGVIEW_H

#include <stddef.h>
#include <string.h>

typedef struct dsa_stringview DsaStringView;

struct dsa_stringview {
    char *text;
    size_t size;
};

int dsa_str_equals(const DsaStringView *strPtrA, const DsaStringView *strPtrB) {
    if (strPtrA == strPtrB) return 1;
    if (strPtrA->size != strPtrB->size) return 0;

    return memcmp(strPtrA->text, strPtrB->text, strPtrA->size) == 0;
}

#define DSA_STRING(ptr) \
    ((DsaStringView){ .text = (ptr), .size = strlen(ptr) })

#endif
