//
//  point.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "point.h"

point_t* construct(int x, int y) {
    point_t* point = (point_t*)malloc(sizeof(point_t));
    point->x = x;
    point->y = y;
    return point;
}

int destruct(point_t* point) {
    free(point);
    return 0;
}

point_namespace const point = { construct, destruct };