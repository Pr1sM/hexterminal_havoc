//
//  room.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "room.h"
#include "../point/point.h"
#include "../logger/logger.h"

#define MIN_WIDTH 4
#define MIN_HEIGHT 3
#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

room_t* room_construct(int x, int y, int width, int height) {
    room_t* r = (room_t*)malloc(sizeof(room_t));
    
    if(x < 1 || x >= DUNGEON_WIDTH - 1) {
        logger.w("Invalid x coord (%d) passed in, defaulting to 0", x);
        x = 0;
    }
    
    if(y < 1 || y >= DUNGEON_HEIGHT - 1) {
        logger.w("Invalid y coord (%d) passed in, defaulting to 0", y);
        y = 0;
    }
    
    if(width < MIN_WIDTH) {
        logger.w("Invalid width given (%d), should be at least %d.  Defaulting to minimum width", width, MIN_WIDTH);
        width = MIN_WIDTH;
    }
    
    if(height < MIN_HEIGHT) {
        logger.w("Invalid width given (%d), should be at least %d.  Defaulting to minimum width", height, MIN_HEIGHT);
        height = MIN_HEIGHT;
    }
    
    point_t* location = pointAPI.construct(x, y);
    r->location = location;
    r->width = width;
    r->height = height;
    return r;
}

void room_destruct(room_t* room) {
    free(room->location);
    free(room);
}

int room_is_overlap(room_t* r1, room_t* r2) {
    int small_x, large_x, x_diff;
    int small_y, large_y, y_diff;
    int min_x_dim = (r1->width + r2->width + 1);
    int min_y_dim = (r1->height + r2->height + 1);
    
    small_x = r1->location->x < r2->location->x ? r1->location->x : r2->location->x;
    large_x = (r1->location->x + r1->width) > (r2->location->x + r2->width) ? (r1->location->x + r1->width) : (r2->location->x + r2->width);
    x_diff = large_x - small_x;
    
    small_y = r1->location->y < r2->location->y ? r1->location->y : r2->location->y;
    large_y = (r1->location->y + r1->width) > (r2->location->y + r2->width) ? (r1->location->y + r1->width) : (r2->location->y + r2->width);
    y_diff = large_y - small_y;
    
    if(y_diff <= min_y_dim && x_diff <= min_x_dim) {
        return 1;
    }
    
    return 0;
}

room_namespace const roomAPI = {
    room_construct,
    room_destruct,
    room_is_overlap
};
