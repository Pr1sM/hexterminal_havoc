//
//  corridor.c
//  cs_327
//
//  Created by Srinivas Dhanwada on 9/20/16.
//  Copyright © 2016 dhanwada. All rights reserved.
//

#include <stdlib.h>
#include <limits.h>

#include "corridor.h"
#include "dungeon.h"
#include "../dijkstra/dijkstra.h"
#include "../graph/graph.h"
#include "../tile/tile.h"
#include "../point/point.h"
#include "../logger/logger.h"
#include "../util/util.h"

static void place_path(graph_t* g, dungeon_t* d, point_t* b);

static void check_room_intercept(dungeon_t* d, point_t* point) {
    if(d->room_size < 0) {
        logger.w("Check room called before rooms have been generated!");
        return;
    }
    room_t* r;
    int i;
    for(i = 0; i < d->room_size; i++) {
        r = d->rooms[i];
        if(r->contains(r, point) && !r->connected) {
            r->connected = 1;
            logger.t("Point (%d, %d) connects room %d", point->x, point->y, i);
            return;
        }
    }
}

static graph_t* construct(dungeon_t* d, int invert) {
    logger.d("Constructing Graph for pathfinding...");
    graph_t* g = (graph_t*)calloc(1, sizeof(graph_t));
    g->point_to_index = point_to_index;
    
    // add all edges to graph
    // just use adjacent tiles for now
    int i, j, k, x, y;
    int coord_adj[] = {-1, 0, 1, 0, -1};
    
    for(i = 1; i < DUNGEON_HEIGHT - 1; i++) {
        for(j = 1; j < DUNGEON_WIDTH - 1; j++) {
            tile_t* t = d->tiles[i][j];
            for(k = 0; k < 4; k++) {
                // check if we are in range
                y = i + coord_adj[k];
                x = j + coord_adj[k + 1];
                if(x < 1 || x >= DUNGEON_WIDTH - 1 || y < 1 || y >= DUNGEON_HEIGHT - 1) {
                    continue;
                }
                tile_t* dest = d->tiles[y][x];
                
                int weight = invert ? ROCK_MAX-dest->rock_hardness : dest->rock_hardness;
                graphAPI.add_edge(g, t->location, dest->location, weight);
            }
        }
    }
    
    logger.d("Final Vertex Count: %d", g->len);
    logger.d("Final Edge Count: %d", g->edge_count);
    logger.d("Graph for pathfinding constructed");
    return g;
}

static void destruct(graph_t* g) {
    logger.d("Destructing graph for pathfinding...");
    int i;
    for(i = 0; i < g->size; i++) {
        if(g->vertices[i] != NULL) {
            graphAPI.free_vertex(g->vertices[i]);
        }
    }
    free(g->vertices);
    free(g);
    logger.d("Graph for pathfinding destructed");
}

static void pathfind(graph_t* g, dungeon_t* d, point_t* start, point_t* end) {
    dijkstraAPI.dijkstra(g, start, end);
    place_path(g, d, end);
}

static void place_path(graph_t* g, dungeon_t* d, point_t* b) {
    int n;
    vertex_t* v;
    tile_t* tile;
    point_t p;
    v = g->vertices[point_to_index(b)];
    if(v->dist == INT_MAX) {
        logger.e("no path! exiting program");
        exit(1);
    }
    for(n = 1; v->dist; n++) {
        index_to_point(v->index, &p);
        
        tile = d->tiles[p.y][p.x];
        if(tile->content == tc_ROCK) {
            tile->update_content(tile, tc_PATH);
        } else if(tile->content == tc_ROOM) {
            check_room_intercept(d, &p);
        }
        v = g->vertices[v->prev];
        if(v == NULL) break;
    }
}

corridor_namespace const corridorAPI = {
    construct,
    destruct,
    pathfind
};
