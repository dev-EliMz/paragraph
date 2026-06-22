#ifndef DSA_GEOMETRY_H
#define DSA_GEOMETRY_H

typedef struct rect DsaRect;
typedef struct vect DsaVect;

struct vect {
    int x;
    int y;
};

struct rect {
    DsaVect origin;
    DsaVect size;
};

DsaVect dsa_vect_add(DsaVect a, DsaVect b);
DsaVect dsa_vect_sub(DsaVect a, DsaVect b);
DsaVect dsa_vect_scalar_mul(DsaVect vect, int value);
DsaVect dsa_vect_scalar_div(DsaVect vect, int value);
int dsa_vect_equals(DsaVect a, DsaVect b);
DsaVect dsa_vect_min(DsaVect a, DsaVect b);
DsaVect dsa_vect_max(DsaVect a, DsaVect b);
//int dsa_vect_length_squared(DsaVect vect);
int dsa_vect_abs_distance(DsaVect a, DsaVect b);

#include <stdlib.h>

DsaVect dsa_vect_add(DsaVect a, DsaVect b) {
    return (DsaVect) {(a.x + b.x), (a.y + b.y)};
}

DsaVect dsa_vect_sub(DsaVect a, DsaVect b) {
    return (DsaVect) {(a.x - b.x), (a.y - b.y)};
}

DsaVect dsa_vect_scalar_mul(DsaVect vect, int value) {
    return (DsaVect) {
        (vect.x * value), 
        (vect.y * value)
    };
}

DsaVect dsa_vect_scalar_div(DsaVect vect, int value) {
    return (DsaVect) {
        (vect.x / value),
        (vect.y / value)
    };
}

int dsa_vect_equals(DsaVect a, DsaVect b) {
    return a.x == b.x && a.y == b.y;
}

DsaVect dsa_vect_min(DsaVect a, DsaVect b) {
    return (DsaVect) {
        (a.x < b.x ? a.x : b.x),
        (a.y < b.y ? a.y : b.y)
    };
}

DsaVect dsa_vect_max(DsaVect a, DsaVect b) {
    return (DsaVect) {
        (a.x > b.x ? a.x : b.x),
        (a.y > b.y ? a.y : b.y)
    };
}

int dsa_vect_abs_distance(DsaVect a, DsaVect b) {
    return abs(a.x - b.x + a.y - b.y);
}

#endif
