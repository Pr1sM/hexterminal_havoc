//
//  tile.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/2/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "tile.h"
#include "../env/env.h"

#define BORDER_CHAR_DEBUG '%'
#define BORDER_CHAR_NORM ' '
#define BORDER_CHAR (DEBUG_MODE ? BORDER_CHAR_DEBUG : BORDER_CHAR_NORM)
#define DEFAULT_CHAR_NORM ' '
#define DEFAULT_CHAR_DEBUG '?'
#define DEFAULT_CHAR (DEBUG_MODE ? DEFAULT_CHAR_DEBUG : DEFAULT_CHAR_NORM)
#define ROCK_CHAR ' '
#define ROOM_CHAR '.'
#define PATH_CHAR '#'


tile_t* tile_construct(uint8_t x, uint8_t y) {
    tile_t* t = (tile_t*)malloc(sizeof(tile_t));
    point_t* location = pointAPI.construct(x, y);
    t->location = location;
    t->content = tc_UNSET;
    t->rock_hardness = 0;
    t->changes = (tile_t*)malloc(sizeof(tile_t));
    return t;
}

void tile_destruct(tile_t* tile) {
    free(tile->changes);
    free(tile->location);
    free(tile);
}

void tile_update_hardness(tile_t* tile, uint8_t value) {
    tile->rock_hardness = value;
    tile->changes->rock_hardness = value;
}

void tile_update_content(tile_t* tile, tile_content value) {
    tile->content = value;
    tile->changes->content = value;
}

void tile_propose_update_hardness(tile_t* tile, uint8_t value) {
    tile->changes->rock_hardness = value;
}

void tile_propose_update_content(tile_t* tile, tile_content value) {
    tile->changes->content = value;
}

void tile_commit_updates(tile_t* tile) {
    tile->rock_hardness = tile->changes->rock_hardness;
    tile->content = tile->changes->content;
}

int tile_are_changes_proposed(tile_t* tile) {
    return (tile->rock_hardness != tile->changes->rock_hardness) ||
           (tile->content != tile->changes->content);
}

char tile_char_for_content(tile_t* tile) {
    return tile->content == tc_BORDER ? BORDER_CHAR :
           tile->content == tc_ROCK   ? ROCK_CHAR   :
           tile->content == tc_ROOM   ? ROOM_CHAR   :
           tile->content == tc_PATH   ? PATH_CHAR   : DEFAULT_CHAR ;
}

void import_tile(tile_t* tile, unsigned char value, int room) {
    tileAPI.propose_update_hardness(tile, value);
    if(value == 255) {
        tileAPI.propose_update_content(tile, tc_BORDER);
    } else if(value == 0) {
        tileAPI.propose_update_content(tile, tc_PATH);
    } else {
        tileAPI.propose_update_content(tile, tc_ROCK);
    }
    
    if(room) {
        tileAPI.propose_update_content(tile, tc_ROOM);
    }
    
    tileAPI.commit_updates(tile);
}

tile_namespace const tileAPI = {
    tile_construct,
    tile_destruct,
    tile_update_hardness,
    tile_update_content,
    tile_propose_update_hardness,
    tile_propose_update_content,
    tile_commit_updates,
    tile_are_changes_proposed,
    tile_char_for_content,
    import_tile
};