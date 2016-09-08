//
//  dungeon.h
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/5/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#ifndef dungeon_h
#define dungeon_h

#include "../tile/tile.h"
#include "../point/point.h"

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

extern tile_t*** _dungeon_array;

typedef struct dungeon_namespace {
    void (*const construct)();
    void (*const destruct)();
    void (*const generate_terrain)();
    void (*const place_rooms)();
    void (*const pathfind)();
    void (*const print)();
    int  (*const connect_room)(point_t* p);
} dungeon_namespace;
extern dungeon_namespace const dungeonAPI;

#endif /* dungeon_h */
