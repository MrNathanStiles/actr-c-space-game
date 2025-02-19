#ifndef SPACEGAME_H
#define SPACEGAME_H
#include "actrwasm.h"
#include "actrui.h"
#include "actrlist.h"

#define GRID_SIZE 4096
#define MASS_SCALE 0.001
#define STATION_SIZE 128

enum MyOre {
    MyOreStone,
    MyOreIce,
    MyOreIron,
    MyOreCopper,
    MyOreNickel,
    MyOreSilicon,
    MyOreCobalt,
    MyOreSilver,
    MyOreGold,
    MyOrePlatinum,
    MyOreUranium,
    MyOreEnd // used as the end if this ore is displayed in the game that is an error
};

char * OreNames[] = {
    "Stone",
    "Ice",
    "Iron",
    "Copper",
    "Nickel",
    "Silicon",
    "Cobalt",
    "Silver",
    "Gold",
    "Platinum",
    "Uranium",
    "Error"
};

unsigned int OreColor[] = {
    0x765b4664, // stone
    0xb9e8ea64, // ice
    0xb7410e64, // iron 
    0xB8733364, // copper
    0x6C747464, // nickel
    0x80808064, // silicon
    0x0047AB64, // cobalt
    0xC0C0C064, // silver
    0xFFD70064, // gold
    0xE5E4E264, // platinum
    0xE0FF6664, // uranium
    0xff000064, // error
};

float OreProbability[] = {
    0.25, // stone
    0.30, // ice
    0.40, // iron 
    0.45, // copper
    0.50, // nickel
    0.55, // silicon
    0.60, // cobalt
    0.65, // silver
    0.70, // gold
    0.75, // platinum
    0.80, // uranium
};

enum MyInventoryItemType {
    MyInventoryItemTypeOre
};


struct MyInventoryItem {
    enum MyInventoryItemType type;
    unsigned int quantity;
};



enum MyObjectType {
    MyObjectTypeAsteroid,
    MyObjectTypeShip,
    MyObjectTypeStation,
    MyObjectTypeWaypoint,
};

struct MyInventory {
    struct MyInventoryItem ore[MyOreEnd];
};

struct MyObject {
    enum MyObjectType type;
    struct ActrPointD position;
    struct ActrPointD velocity;
    float rotation;
    float mass;
    float sin;
    float cos;
    int identity;
};


struct MyObjectAsteroid { 
    struct MyObject object;
    enum MyOre ore;
    float scale;

};

struct MyObjectShip
{
    struct MyObject object;
    struct MyInventory inventory;
};

struct MyObjectStation
{
    struct MyObject object;
    struct MyInventory inventory;
};

struct MyObjectWaypoint {
    
    struct MyObject object;
    char * name;
    struct ActrQuadTreeLeaf * leaf;
};


enum MyMenuAction {
    MyMenuActionClose,
    MyMenuActionBack,
    MyMenuActionBuildStation,
    MyMenuActionAddWaypoint,
    MyMenuActionTargetWaypoint,
    MyMenuActionRemoveTarget,
    MyMenuActionWaypointMenu,
    MyMenuActionDeleteWaypoint,
};

struct MyMenuItem {
    enum MyMenuAction action;
    char * text;
    void * state;
};

struct MyMenu {
    int key;
    struct ActrVector * items;
    int position;
    struct MyMenu * previous;
};

struct MyMessage {
    int age;
    char * text;
};

struct MyState
{
    struct MyObjectShip player;
    float aspect;
    char keys[256];
    struct ActrQuadTree *tree;
    struct ActrVector *result;
    struct ActrPoint32 lastGrid;
    int paused;
    int identity;
    struct MyMenu * menu;
    struct ActrList * messages;
    struct ActrVector *waypoints;
    struct ActrPointD target;
};


// a1 is line1 start, a2 is line1 end, b1 is line2 start, b2 is line2 end

void drawAsteroid(struct MyObjectAsteroid *asteroid);
void draw_ship(struct MyObjectShip *ship, int thrusting, int shooting);
void drawView();
struct ActrPointD fromgrid(struct ActrPoint32 point);
void init_area(struct ActrPoint32 grid);
struct MyObjectAsteroid *init_asteroid(enum MyOre ore, double x, double y, float rotation, float mass, float distance);
void init_object(struct MyObject * object, enum MyObjectType type, double x, double y, float rotation, float mass);
int lines_intersect(struct ActrPointF a1, struct ActrPointF a2, struct ActrPointF b1, struct ActrPointF b2);
void query_view();
enum MyOre random_ore();
void rotate_point(struct ActrPointD *point, double cos, double sin);
struct ActrPoint32 togrid(struct ActrPointD point);
void update_ship(struct MyObjectShip *ship, double delta, float rotate, float thrust, int shooting);
double wrapN(double value, double N);
double wrapPI(double value);
double wrapPITAU(double value);
double wrapTAU(double value);


#endif
