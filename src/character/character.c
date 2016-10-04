//
//  character.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 10/3/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>

#include "character.h"
#include "../point/point.h"
#include "../logger/logger.h"

static void set_position_impl(character_t* self, point_t* p);
static void set_destination_impl(character_t* self, point_t* p);
static void set_point_impl(point_t* c, point_t* p);

static character_t* gPLAYER_CHARACTER = NULL;

static character_t* construct_impl(character_type type, point_t* p) {
    character_t* c = calloc(1, sizeof(character_t));
    point_t* pos;
    
    if(type == PC) {
        c->speed = 10;
        c->attrs = 0;
    } else if(type == NPC) {
        c->speed = (rand() & 0xf) + 5;
        c->attrs = rand() & 0xf;
    } else {
        logger.w("Invalid type passed into character constructor! returning NULL!");
        free(c);
        return NULL;
    }
    c->turn_count = 100 / c->speed;
    
    if(p == NULL) {
        logger.w("NULL point passed into character constructor! returning NULL!");
        free(c);
        return NULL;
    } else {
        pos = pointAPI.construct(p->x, p->y);
        c->position = pos;
    }
    c->destination = NULL;
    
    c->set_position = set_position_impl;
    c->set_destination = set_destination_impl;
    return c;
}

static void destruct_impl(character_t* c) {
    free(c->position);
    free(c->destination);
    free(c);
}

static character_t* get_pc_impl() {
    if(gPLAYER_CHARACTER == NULL) {
        point_t p = {1, 1};
        gPLAYER_CHARACTER = characterAPI.construct(PC, &p);
    }
    return gPLAYER_CHARACTER;
}

static void set_position_impl(character_t* self, point_t* p) {
    set_point_impl(self->position, p);
}

static void set_destination_impl(character_t* self, point_t* p) {
    set_point_impl(self->destination, p);
}

static void set_point_impl(point_t* c, point_t* p) {
    if(p == NULL) {
        logger.w("NULL point passed into set_point! returning without changing!");
        return;
    }
    if(c == NULL) {
        c = pointAPI.construct(p->x, p->y);
    } else {
        c->x = p->x;
        c->y = p->y;
    }
}

const character_namespace characterAPI = {
    construct_impl,
    destruct_impl,
    get_pc_impl
};
