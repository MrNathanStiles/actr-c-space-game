#include "actrwasm.h"
#include "actrcanvas.h"
#include "actrformat.h"
#include "actrhashtable.h"
#include "actrquadtree.h"
#include "actrprng.h"
#include "spacegame.h"
#include "actrlist.h"
#include "actrfetch.h"

struct MyState *state;
void draw_station(struct MyObjectStation *station)
{
    actr_canvas2d_fill_style(0, 255, 0, 100);
    actr_canvas2d_fill_rect(
        station->object.position.x - state->player.object.position.x + actr_ui_state->canvas_size.w / 2,
        station->object.position.y - state->player.object.position.y + actr_ui_state->canvas_size.h / 2,
        STATION_SIZE, STATION_SIZE);
}
void draw_asteroid(struct MyObjectAsteroid *asteroid)
{
    struct ActrPointD pt;
    struct ActrPointD start;
    unsigned char r, g, b, a;

    actr_unpack_bytes(OreColor[asteroid->ore], &r, &g, &b, &a);
    actr_canvas2d_fill_style(r, g, b, a);

    asteroid->object.rotation += 0.001;

    int x = asteroid->object.position.x - state->player.object.position.x + actr_ui_state->canvas_size.w / 2;
    int y = asteroid->object.position.y - state->player.object.position.y + actr_ui_state->canvas_size.h / 2;

    /*
    drawass(
        x, y,
        state->player.object.position.x, state->player.object.position.y,
        asteroid->object.position.x, asteroid->object.position.y,
        actrState->canvasSize.w / 2,
        actrState->canvasSize.h / 2
    );
    //*/

    // asteroid->object.mass = 10;
    // actr_canvas2d_fill_style(255, 255, 255, 100);
    ///*

    struct ActrFormatState *format = actr_format("%s x %s");
    actr_format_str(format, OreNames[asteroid->ore]);
    actr_format_int(format, asteroid->object.mass);
    char *text = actr_format_close(format);
    actr_canvas2d_fill_text(x - asteroid->scale, y - asteroid->scale - 5, text);
    actr_free(text);

    //*/

    actr_canvas2d_begin_path();
    pt.x = -0.70710676237;
    pt.y = -0.70710676237;
    rotate_point(&pt, asteroid->object.cos, asteroid->object.sin);
    start = pt;
    actr_canvas2d_moveto(x + pt.x * asteroid->scale, y + pt.y * asteroid->scale);

    pt.x = 0.70710676237;
    pt.y = -0.70710676237;
    rotate_point(&pt, asteroid->object.cos, asteroid->object.sin);
    actr_canvas2d_lineto(x + pt.x * asteroid->scale, y + pt.y * asteroid->scale);

    pt.x = 0.70710676237;
    pt.y = 0.70710676237;
    rotate_point(&pt, asteroid->object.cos, asteroid->object.sin);
    actr_canvas2d_lineto(x + pt.x * asteroid->scale, y + pt.y * asteroid->scale);

    pt.x = -0.70710676237;
    pt.y = 0.70710676237;
    rotate_point(&pt, asteroid->object.cos, asteroid->object.sin);
    actr_canvas2d_lineto(x + pt.x * asteroid->scale, y + pt.y * asteroid->scale);

    actr_canvas2d_lineto(x + start.x * asteroid->scale, y + start.y * asteroid->scale);

    actr_canvas2d_fill();
    // actr_canvas2d_stroke();
}

void draw_ship(struct MyObjectShip *ship, int thrusting, int shooting)
{
    struct ActrPointD pt;
    struct ActrPointD nose;

    double sin = ship->object.sin;
    double cos = ship->object.cos;

    long long x = actr_ui_state->canvas_size.w / 2;
    long long y = actr_ui_state->canvas_size.h / 2;

    actr_canvas2d_fill_style(255, 255, 255, 100);
    actr_canvas2d_begin_path();
    actr_canvas2d_moveto(x, y);

    // start at bottom left point
    pt.x = -10;
    pt.y = 10;
    rotate_point(&pt, cos, sin);
    actr_canvas2d_lineto(x + pt.x, y + pt.y);

    // save starting point

    // line to front/tip/nose of ship
    nose.x = 0;
    nose.y = -10;
    rotate_point(&nose, cos, sin);
    actr_canvas2d_lineto(x + nose.x, y + nose.y);

    // line to bottom right
    pt.x = 10;
    pt.y = 10;
    rotate_point(&pt, cos, sin);
    actr_canvas2d_lineto(x + pt.x, y + pt.y);

    // close the path
    actr_canvas2d_lineto(x, y);
    actr_canvas2d_fill();

    if (thrusting)
    {
        pt.x = 0;
        pt.y = 10;
        rotate_point(&pt, cos, sin);
        actr_canvas2d_stroke_style(255, 0, 0, 100);
        actr_canvas2d_begin_path();
        actr_canvas2d_moveto(x, y);
        actr_canvas2d_lineto(x + pt.x, y + pt.y);
        actr_canvas2d_stroke();
    }

    if (shooting)
    {
        pt.x = 0;
        pt.y = -35;
        rotate_point(&pt, cos, sin);
        actr_canvas2d_stroke_style(255, 0, 255, 100);
        actr_canvas2d_begin_path();
        actr_canvas2d_moveto(x + nose.x, y + nose.y);
        actr_canvas2d_lineto(x + pt.x, y + pt.y);
        actr_canvas2d_stroke();
    }
    if (state->target.x != 0 || state->target.y != 0)
    {

        struct ActrPointD displacement = actr_sub(&state->target, &state->player.object.position);

        struct ActrPointD direction = actr_normalize(&displacement);
        if (0 == actr_isnan(direction.x))
        {
            actr_canvas2d_stroke_style(0, 255, 0, 100);
            actr_canvas2d_begin_path();
            actr_canvas2d_moveto(
                actr_ui_state->canvas_size.w / 2 + direction.x * 50,
                actr_ui_state->canvas_size.h / 2 + direction.y * 50);
            actr_canvas2d_lineto(
                actr_ui_state->canvas_size.w / 2 + direction.x * 60,
                actr_ui_state->canvas_size.h / 2 + direction.y * 60);
            actr_canvas2d_stroke();

            struct ActrFormatState *format = actr_format("%s");
            actr_format_int(format, actr_distance(&state->target, &state->player.object.position));
            char *text = actr_format_close(format);
            int len = actr_strlen(text);

            int padx = 0;
            int pady = 10;
            if (direction.y < 0)
            {
                pady = 0;
            }
            if (direction.x < 0)
            {
                padx = -9 * len;
            }

            actr_canvas2d_fill_text(
                actr_ui_state->canvas_size.w / 2 + direction.x * 60 + padx,
                actr_ui_state->canvas_size.h / 2 + direction.y * 60 + pady,
                text);

            actr_free(text);
        }
    }
}

void push_message(char *text)
{
    struct MyMessage *message = actr_malloc(sizeof(struct MyMessage));
    actr_heap_string(&message->text, text);
    state->messages = actr_list(state->messages, message);
}

void free_message(struct MyMessage *message)
{
    actr_free(message->text);
    actr_free(message);
}

void draw_messages()
{
    struct ActrList *list = state->messages;
    struct ActrList *last = 0;
    struct ActrList *temp = 0;
    struct MyMessage *message;
    int top = actr_ui_state->canvas_size.h - 40;
    actr_canvas2d_fill_style(255, 255, 255, 100);
    while (list)
    {
        message = (struct MyMessage *)list->item;
        actr_canvas2d_fill_text(5, top, message->text);
        top -= 20;
        message->age++;
        if (message->age > 60 * 10)
        {
            temp = list->next;
            if (last == 0)
                state->messages = temp;
            else
                last->next = temp;

            free_message(message);
            actr_free(list);
            list = temp;
            continue;
        }
        last = list;
        list = list->next;
    }
}
void draw_waypoint(struct MyObjectWaypoint *waypoint)
{
    actr_canvas2d_fill_style(255, 0, 255, 100);
    actr_canvas2d_fill_text(
        waypoint->object.position.x - state->player.object.position.x + actr_ui_state->canvas_size.w / 2,
        waypoint->object.position.y - state->player.object.position.y + actr_ui_state->canvas_size.h / 2,
        waypoint->name);
}
void draw_view()
{
    long long x = state->player.object.position.x - actr_ui_state->canvas_size.w / 2;
    long long y = state->player.object.position.y - actr_ui_state->canvas_size.h / 2;

    query_view();
    actr_canvas2d_fill_style(255, 255, 255, 10);
    for (int i = 0; i < state->result->count; i++)
    {
        struct ActrQuadTreeLeaf *leaf = state->result->head[i];
        struct MyObject *object = leaf->item;
        switch (object->type)
        {
        case MyObjectTypeShip:
            break;
        case MyObjectTypeAsteroid:
            draw_asteroid((struct MyObjectAsteroid *)object);
            break;
        case MyObjectTypeStation:
            draw_station((struct MyObjectStation *)object);
            break;
        case MyObjectTypeWaypoint:
            draw_waypoint((struct MyObjectWaypoint *)object);
        }
        // actr_canvas2d_fill_text(leaf->bounds.point.x - x, leaf->bounds.point.y - y, leaf->item);
        // actr_canvas2d_fill_rect(leaf->bounds.point.x - x, leaf->bounds.point.y - y, leaf->bounds.size.w, leaf->bounds.size.h);
    }
    state->result->count = 0;
}

struct ActrPointD fromgrid(struct ActrPoint32 point)
{
    struct ActrPointD result;
    result.x = point.x * GRID_SIZE;
    result.y = point.y * GRID_SIZE;
    return result;
}
void init_area(struct ActrPoint32 grid)
{
    int index = 0;
    struct ActrPoint32 test;
    struct ActrPointD point;
    struct ActrQuadTreeBounds bounds;
    struct ActrQuadTreeLeaf *leaf;
    bounds.size.h = GRID_SIZE;
    bounds.size.w = GRID_SIZE;

    for (long long y = grid.y - 1; y < grid.y + 2; y++)
    {
        for (long long x = grid.x - 1; x < grid.x + 2; x++)
        {
            test.x = x;
            test.y = y;
            point = fromgrid(test);
            bounds.point.x = point.x;
            bounds.point.y = point.y;
            // stone 2500kg per m3
            actr_quad_tree_query(state->tree, &bounds, state->result);
            if (state->result->count == 0)
            {
                for (int i = 0; i < 16; i++)
                {
                    long long x = actr_prng() * GRID_SIZE;
                    long long y = actr_prng() * GRID_SIZE;
                    float r = actr_prng();
                    float mass = 10 + r * r * r * 1000000;
                    float rotation = actr_prng() * PI;
                    float scale = actr_sqrt(mass * MASS_SCALE);

                    if (scale < 5)
                    {
                        scale = 5;
                    }

                    int ld = scale + 1;
                    if (x <= ld)
                    {
                        x += (ld - x + 1);
                    }
                    else if (x + ld >= GRID_SIZE)
                    {
                        x -= (x + ld - GRID_SIZE + 1);
                    }
                    if (y <= ld)
                    {
                        y += (ld - x + 1);
                    }
                    else if (y + ld >= GRID_SIZE)
                    {
                        y -= (y + ld - GRID_SIZE + 1);
                    }

                    struct MyObjectAsteroid *asteroid = init_asteroid(random_ore(), point.x + x, point.y + y, rotation, mass, scale);

                    leaf = actr_quad_tree_leaf(point.x + x - ld, point.y + y - ld, ld * 2.0, ld * 2.0, asteroid);
                    actr_quad_tree_insert(state->tree, leaf);
                }
            }
            else
            {
                state->result->count = 0;
            }
        }
    }
}

struct MyObjectAsteroid *init_asteroid(enum MyOre ore, double x, double y, float rotation, float mass, float scale)
{
    struct MyObjectAsteroid *asteroid = actr_malloc(sizeof(struct MyObjectAsteroid));
    init_object((struct MyObject *)asteroid, MyObjectTypeAsteroid, x, y, rotation, mass);
    asteroid->ore = ore;
    asteroid->object.cos = actr_cos(asteroid->object.rotation);
    asteroid->object.sin = actr_sin(asteroid->object.rotation);
    asteroid->object.identity = state->identity++;
    asteroid->scale = scale;

    return asteroid;
}

struct MyObjectStation *init_station(double x, double y)
{
    struct MyObjectStation *station = actr_malloc(sizeof(struct MyObjectStation));
    init_object(&station->object, MyObjectTypeStation, x, y, 0, 999999);
    return station;
}

void build_station(struct ActrPointD *point)
{
    struct ActrQuadTreeLeaf *leaf;
    struct MyObject *object;
    struct ActrQuadTreeBounds area;
    
    double minDist2 = (STATION_SIZE * 2) * (STATION_SIZE * 2);
    
    area.point.x = point->x - GRID_SIZE;
    area.point.y = point->y - GRID_SIZE;
    area.size.w = GRID_SIZE * 2;
    area.size.h = GRID_SIZE * 2;
    
    actr_quad_tree_query(state->tree, &area, state->result);
    
    for (int i = 0; i < state->result->count; i++)
    {

        leaf = state->result->head[i];
        object = leaf->item;
        if (object->type == MyObjectTypeWaypoint)
        {
            continue;
        }
        if (object->type == MyObjectTypeStation)
        {
            state->result->count = 0;
            push_message("Other station too close");
            return;
        }

        struct ActrPointD center = actr_quad_tree_bounds_center(&leaf->bounds);
        double d2 = actr_distance2(&center, point);
        if (d2 < minDist2)
        {
            push_message("Other object too close.");
            return;
        }
    }
    state->result->count = 0;
    double x = point->x - STATION_SIZE / 2;
    double y = point->y - STATION_SIZE / 2;
    struct MyObjectStation *station = init_station(x, y);
    actr_quad_tree_insert(state->tree, actr_quad_tree_leaf(x, y, STATION_SIZE, STATION_SIZE, station));
    push_message("New Station Built");
}

void init_object(struct MyObject *object, enum MyObjectType type, double x, double y, float rotation, float mass)
{
    object->type = type;
    object->position.x = x;
    object->position.y = y;
    object->velocity.x = 0;
    object->velocity.y = 0;
    object->rotation = rotation;
    object->mass = mass;
}

// a1 is line1 start, a2 is line1 end, b1 is line2 start, b2 is line2 end
int lines_intersect(struct ActrPointF a1, struct ActrPointF a2, struct ActrPointF b1, struct ActrPointF b2)
{
    // https://stackoverflow.com/a/3746601
    struct ActrPointF intersection;
    struct ActrPointF c;
    struct ActrPointF b;
    struct ActrPointF d;

    intersection.x = 0;
    intersection.y = 0;

    b.x = a2.x - a1.x;
    b.y = a2.y - a1.y;

    d.x = b2.x - b1.x;
    d.y = b2.y - b1.y;

    float bDotDPerp = b.x * d.y - b.y * d.x;

    // if b dot d == 0, it means the lines are parallel so have infinite intersection points
    if (bDotDPerp == 0)
    {
        return 0;
    }

    c.x = b1.x - a1.x;
    c.y = b1.y - a1.y;
    float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
    if (t < 0 || t > 1)
    {
        return 0;
    }

    float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
    if (u < 0 || u > 1)
    {
        return 0;
    }

    return 1;
}

void query_view()
{
    struct ActrQuadTreeBounds bounds;
    bounds.point.x = state->player.object.position.x - actr_ui_state->canvas_size.w / 2;
    bounds.point.y = state->player.object.position.y - actr_ui_state->canvas_size.h / 2;
    bounds.size.w = actr_ui_state->canvas_size.w;
    bounds.size.h = actr_ui_state->canvas_size.h;
    actr_quad_tree_query(state->tree, &bounds, state->result);
}

enum MyOre random_ore()
{
    float v = actr_prng();
    for (int i = 0; i < MyOreEnd; i++)
    {
        if (v < OreProbability[i])
            return i;
    }
    return MyOreStone;
}

void rotate_point(struct ActrPointD *point, double cos, double sin)
{
    double x = point->x * cos - point->y * sin;
    double y = point->x * sin + point->y * cos;
    point->x = x;
    point->y = -y;
}

struct ActrPoint32 togrid(struct ActrPointD point)
{
    struct ActrPoint32 result;
    result.x = (point.x < 0 ? (point.x - GRID_SIZE) : point.x) / GRID_SIZE;
    result.y = (point.y < 0 ? (point.y - GRID_SIZE) : point.y) / GRID_SIZE;
    return result;
}

double wrapN(double value, double N)
{
    if (value > N)
    {
        return value - N;
    }
    else if (value < -N)
    {
        return value + N;
    }
    return value;
}

double wrapPI(double value)
{
    return wrapN(value, PI);
}

double wrapTAU(double value)
{
    return wrapN(value, TAU);
}

double wrapPITAU(double value)
{
    if (value > PI)
    {
        return value - TAU;
    }
    else if (value < -PI)
    {
        return value + TAU;
    }
    return value;
}

[[clang::export_name("actr_init")]]
void actr_init(int w, int h)
{
    actr_ui_init(w, h);
    state = actr_malloc(sizeof(struct MyState));
    state->identity = 1;
    init_object(&state->player.object, MyObjectTypeShip, 0, 0, PI, 3500);
    state->player.object.position.x = 500;
    state->player.object.position.y = 500;
    state->tree = actr_quad_tree_init(1, 0, 0, GRID_SIZE, 0);
    state->result = actr_vector_init(4, 4);
    state->waypoints = actr_vector_init(4, 4);
    struct ActrPoint32 grid;
    grid.x = 0;
    grid.y = 0;
    init_area(grid);
}

[[clang::export_name("actr_resize")]]
void actr_resize(float w, float h)
{
    state->aspect = w / h;
    actr_ui_state->canvas_size.w = w;
    actr_ui_state->canvas_size.h = h;
}

struct MyMenu *menu_init(int key, struct MyMenu *previous)
{
    struct MyMenu *result = actr_malloc(sizeof(struct MyMenu));
    result->previous = previous;
    result->items = actr_vector_init(4, 4);
    result->key = key;
    return result;
}

void menu_add_item(struct MyMenu *menu, enum MyMenuAction action, char *text, void *state)
{
    struct MyMenuItem *item = actr_malloc(sizeof(struct MyMenuItem));
    item->action = action;
    item->state = state;
    actr_heap_string(&item->text, text);
    actr_vector_add(menu->items, item);
}

void menu_free(struct MyMenu *menu)
{
    for (int i = 0; i < menu->items->count; i++)
    {
        struct MyMenuItem *item = menu->items->head[i];
        actr_free(item->text);
        actr_free(item);
    }
    actr_vector_free(menu->items);
    actr_free(menu);
}

void menu_close(struct MyMenu *menu)
{
    if (menu->previous)
    {
        menu_free(menu->previous);
    }
    menu_free(menu);
    state->menu = 0;
}

int check_open_menu(int key)
{
    int open = 0;

    if (state->menu)
    {
        if (state->menu->key != key)
            open = 1;
        menu_close(state->menu);
    }
    else
    {
        open = 1;
    }
    return open;
}

void waypoint_dispose(struct MyObjectWaypoint *waypoint)
{
    actr_quad_tree_remove(waypoint->leaf);
    actr_free(waypoint->leaf);
    actr_free(waypoint->name);
    actr_vector_remove(state->waypoints, actr_vector_find(state->waypoints, waypoint));
    actr_free(waypoint);
}

struct MyObjectWaypoint *init_waypoint(long long x, long long y)
{
    struct MyObjectWaypoint *waypoint = actr_malloc(sizeof(struct MyObjectWaypoint));
    struct ActrQuadTreeLeaf *leaf = actr_quad_tree_leaf(x, y, 1, 1, waypoint);
    waypoint->leaf = leaf;
    actr_quad_tree_insert(state->tree, leaf);
    init_object(&waypoint->object, MyObjectTypeWaypoint, state->player.object.position.x, state->player.object.position.y, 0, 0);
    struct ActrFormatState *format = actr_format("%s.%s");
    actr_format_int(format, x);
    actr_format_int(format, y);
    waypoint->name = actr_format_close(format);
    actr_vector_add(state->waypoints, waypoint);
    return waypoint;
}

[[clang::export_name("actr_key_down")]]
void actr_key_down(int key)
{
    if (key == 98)
    {
        // building
        if (check_open_menu(key))
        {
            state->menu = menu_init(key, 0);
            menu_add_item(state->menu, MyMenuActionBuildStation, "Build Station", 0);
            menu_add_item(state->menu, MyMenuActionClose, "Close", 0);
        }
        return;
    }
    else if (key == 105)
    {
        // inventory
        if (check_open_menu(key))
        {
            state->menu = menu_init(key, 0);
            for (int i = 0; i < MyOreEnd; i++)
            {
                if (state->player.inventory.ore[i].quantity > 0)
                {
                    struct ActrFormatState *format = actr_format("%s %s");
                    actr_format_str(format, OreNames[i]);
                    actr_format_int(format, state->player.inventory.ore[i].quantity);
                    menu_add_item(state->menu, MyMenuActionClose, actr_format_close(format), 0);
                }
            }
            menu_add_item(state->menu, MyMenuActionClose, "Close", 0);
        }
    }
    else if (key == 109)
    {
        // waypoints
        if (check_open_menu(key))
        {
            state->menu = menu_init(key, 0);
            menu_add_item(state->menu, MyMenuActionAddWaypoint, "Add Waypoint", 0);
            menu_add_item(state->menu, MyMenuActionRemoveTarget, "Remove Target", 0);
            struct MyObjectWaypoint *wp;
            struct ActrFormatState *format;
            char *text;
            for (int i = 0; i < state->waypoints->count; i++)
            {
                wp = (struct MyObjectWaypoint *)state->waypoints->head[i];
                format = actr_format("%s.%s");
                actr_format_int(format, wp->object.position.x);
                actr_format_int(format, wp->object.position.y);
                text = actr_format_close(format);
                menu_add_item(state->menu, MyMenuActionWaypointMenu, text, wp);
                actr_free(text);
            }
        }
    }
    else if (state->menu)
    {
        if (key == 1)
        {
            state->menu->position--;
            if (state->menu->position < 0)
            {
                state->menu->position = state->menu->items->count - 1;
            }
        }
        else if (key == 2)
        {
            state->menu->position++;
            if (state->menu->position == state->menu->items->count)
            {
                state->menu->position = 0;
            }
        }
        else if (key == 32)
        {
            struct MyMenuItem *item = state->menu->items->head[state->menu->position];
            struct MyMenu *temp;
            struct MyObjectWaypoint *waypoint;

            switch (item->action)
            {
            case MyMenuActionClose:
                break;
                ;
            case MyMenuActionBuildStation:
                build_station(&state->player.object.position);
                break;
            case MyMenuActionAddWaypoint:
                init_waypoint(state->player.object.position.x, state->player.object.position.y);
                break;
            case MyMenuActionRemoveTarget:
                state->target.x = 0;
                state->target.y = 0;
                break;
            case MyMenuActionDeleteWaypoint:
                waypoint_dispose(item->state);
                break;
            case MyMenuActionWaypointMenu:
                state->menu = menu_init(0, state->menu);
                menu_add_item(state->menu, MyMenuActionTargetWaypoint, "Set Target", item->state);
                menu_add_item(state->menu, MyMenuActionDeleteWaypoint, "Delete Waypoint", item->state);
                menu_add_item(state->menu, MyMenuActionBack, "Back", 0);
                return;
                break;
            case MyMenuActionTargetWaypoint:
                waypoint = item->state;
                state->target.x = waypoint->object.position.x;
                state->target.y = waypoint->object.position.y;
                break;
            case MyMenuActionBack:
                temp = state->menu->previous;
                state->menu->previous = 0;
                menu_close(state->menu);
                state->menu = temp;
                return;
            }
            menu_close(state->menu);
        }
    }
    state->keys[key] = 1;
}

[[clang::export_name("actr_key_up")]]
void actr_key_up(int key)
{
    if (key == 112)
    {
        if (state->paused)
        {
            state->paused = 0;
        }
        else
        {
            state->paused = 1;
        }
    }
    state->keys[key] = 0;
}

void update_ship(struct MyObjectShip *ship, double delta, float rotate, float thrust, int shooting)
{
    // y
    double cos = ship->object.cos = actr_cos(ship->object.rotation);
    // x
    double sin = ship->object.sin = actr_sin(ship->object.rotation);

    float force = 99;

    ship->object.rotation = wrapTAU(ship->object.rotation);

    if (thrust > 0)
    {
        ship->object.velocity.x += sin * delta * force;
        ship->object.velocity.y += cos * delta * force;
    }
    if (rotate != 0)
    {
        ship->object.rotation += delta * (shooting ? 1 : 5) * rotate;
    }

    ship->object.position.x += ship->object.velocity.x * delta;
    ship->object.position.y += ship->object.velocity.y * delta;

    if (shooting == 1)
    {
        struct ActrPointD nose, tip;
        // start of beam
        nose.x = 0;
        nose.y = -10;
        // tip of beam
        tip.x = 0;
        tip.y = -35;

        rotate_point(&nose, cos, sin);
        rotate_point(&tip, cos, sin);

        nose.x += ship->object.position.x;
        nose.y += ship->object.position.y;

        tip.x += ship->object.position.x;
        tip.y += ship->object.position.y;

        state->result->count = 0;
        struct ActrQuadTreeBounds area;

        double w = nose.x - tip.x;
        if (w < 0)
            w = -w;

        double h = nose.y - tip.y;
        if (h < 0)
            h = -h;

        area.point.x = tip.x < nose.x ? tip.x : nose.x;
        area.point.y = tip.y < nose.y ? tip.y : nose.y;
        area.size.w = w;
        area.size.h = h;

        actr_quad_tree_query(state->tree, &area, state->result);
        struct MyObject *object;
        for (int i = 0; i < state->result->count; i++)
        {
            struct ActrQuadTreeLeaf *leaf = state->result->head[i];

            struct MyObjectAsteroid *asteroid = leaf->item;
            if (asteroid->object.type != MyObjectTypeAsteroid)
                continue;
            asteroid->object.mass--;
            if (asteroid->object.mass < 0)
            {
                actr_quad_tree_remove(leaf);
                actr_free(asteroid);
                actr_free(leaf);
            }
            else
            {
                ship->inventory.ore[asteroid->ore].quantity++;
            }
            break;
        }
        state->result->count = 0;
    }
}

void draw_menu()
{
    if (state->menu == 0)
    {
        return;
    }
    int margin = 8;
    int height = 25;
    int top = margin;
    int left = margin;

    for (int i = 0; i < state->menu->items->count; i++)
    {
        struct MyMenuItem *item = state->menu->items->head[i];
        int width = margin + margin + actr_strlen(item->text) * 9;
        if (left + width + margin > actr_ui_state->canvas_size.w)
        {
            left = margin;
            top += height + margin;
        }
        actr_canvas2d_fill_style(128, 128, 128, 50);
        actr_canvas2d_fill_rect(left, top, width, height);
        if (state->menu->position == i)
        {
            actr_canvas2d_stroke_style(255, 255, 255, 100);
            actr_canvas2d_stroke_rect(left, top, width, height);
        }
        actr_canvas2d_fill_style(255, 255, 255, 50);
        actr_canvas2d_fill_text(left + margin, top + height - margin, item->text);
        left += width + margin;
    }
}

[[clang::export_name("actr_step")]]
void actr_step(double delta)
{
    if (state->paused)
        return;

    float length = 50;
    int shoot = 0;
    int thrust = 0;
    int rotate = 0;

    if (state->menu == 0)
    {

        if (state->keys[5])
            thrust = 1;
        if (state->keys[32])
            shoot = 1;

        if (state->keys[1])
            rotate = 1;
        else if (state->keys[2])
            rotate = -1;
        else if (state->keys[6])
        {
            double desired = actr_atan2(-state->player.object.velocity.y, state->player.object.velocity.x) - PI / 2.0;
            rotate = actr_sign(wrapPITAU(desired - state->player.object.rotation));
        }
    }

    update_ship(&state->player, delta, rotate, thrust, shoot);

    actr_canvas2d_fill_style(0, 0, 0, 100);
    actr_canvas2d_fill_rect(-10, -10, actr_ui_state->canvas_size.w + 20, actr_ui_state->canvas_size.h + 20);

    draw_view();
    draw_ship(&state->player, thrust, shoot);

    struct ActrPoint32 tgrid = togrid(state->player.object.position);

    if (tgrid.x != state->lastGrid.x || tgrid.y != state->lastGrid.y)
    {
        state->lastGrid = tgrid;
        init_area(tgrid);
    }

    draw_menu();
    draw_messages();
    // format
    struct ActrFormatState *format = actr_format("pos %s x %s grid %s x %s delta %s");
    // pos
    actr_format_int(format, state->player.object.position.x);
    actr_format_int(format, state->player.object.position.y);
    // grid
    actr_format_int(format, tgrid.x);
    actr_format_int(format, tgrid.y);

    actr_format_float(format, delta * 1000);

    char *text = actr_format_close(format);
    actr_canvas2d_fill_style(255, 255, 255, 100);
    actr_canvas2d_fill_text(5, actr_ui_state->canvas_size.h - 5, text);
    actr_free(text);

    text = actr_memory_report();
    actr_canvas2d_fill_text(5, actr_ui_state->canvas_size.h - 20, text);
    actr_free(text);

    struct ActrPoint64 offset;

    offset.x = (state->player.object.position.x - actr_ui_state->canvas_size.w / 2);
    offset.y = (state->player.object.position.y - actr_ui_state->canvas_size.h / 2);
    // actr_quad_tree_draw(state->tree, offset);
}
