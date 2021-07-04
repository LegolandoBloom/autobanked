struct hitscan
{
    int x;
    //how many spaces are empty until collision(REAL X)
    int xempty;
    int width;
    int y;
    //how many spaces are empty until collision(REAL Y)
    int yempty;
    int height;
    int spritids[4];
    int ground;
    int bkcoll;
};

//all information about an enemy is stored in a single struct
struct gumbas
{
    int tile;    
    int HP;
    //enemy's location is stored in this struct as a square to use for collision
    struct hitscan hitbox;
    int xspeed;
    int yspeed;
    int dir;
    int hitcounter;
    //relation to player
    int relation;
};

struct bkobject
{
    int x;
    int width;
    int y;
    int height;
};