#include <stdio.h>
#include <gb/gb.h>

#include "structs.h"
#include "Inv_Anim.c"

#include "Dudeb.c"
#include "enemytiles.c"

#include "bktiles.c"
#include "bkmap.c"

#include "test.c"

#include <gb/font.h>

#include "lvl2-1.h"
#include "lvl2-2.h"
#include "lvl2-3.h"


#define GRAV 2 
#define P_ACCEL 2 
#define Y_RAGD 4 
#define X_RAGD 4


void level2(void) BANKED;
int maps2Count = 3;
char *maps2[3] = {lvl21, lvl22, lvl23};

int attcounter = 0, speedmax = 4, xspeed = 0, yspeed = 0, counter = 0, dir = 1, xlast, ylast;
// 0 is right 1 is left
//the desired x and y. If theres no background collision these become the new player.x player.y
//if player is colliding with enemy, -1 means touching from left, 1 from right, 0 no touch
int touching = 0, pragdoll_counter = 0;
//- 1 left 1 right
int flingdir;
int invul = 0;
int bkgXScroll = 0, bkgYScroll = 0;
int bkgX, drawn;



struct hitscan player;
struct hitscan pscan;

struct bkobject ground;
struct bkobject obj1;
struct bkobject obj2;

void performantdelay(int cycles);
int setBetween32(int n);

int checkcoll(struct hitscan *sq1, struct hitscan *sq2);
//Background stuff
void setup_bkobject(struct bkobject *name, int x, int width, int y, int height);
void regBlock(struct bkobject *name);
void draw(int mapCount, char **maps);

//Player stuff
void plyan(int zero, int one, int two, int three);
void setupplayer(void);
void moveplayer(int x, int y);
void playergrav(void);
void pragdoll(void);
void basicmove(void);
void lastDir(void);
void setpscan(int x, int width, int y, int height);
void invulnerable(void);
void normalatt(void);
//Level stuff
//Enemy stuff
void setupgumba(struct gumbas *gumba, int HP, int tile, int x, int y, int dir);
void gumbamove(struct gumbas *gumba, int jX, int jY, int aispeed, int xmin, int xmax, int ymin);

int bpress;

void main(void)
{
    //font_t min_font;

    //font_init();
    //min_font = font_load(font_min);
    //font_set(min_font);

    set_bkg_data(0, 6, bktiles);
    set_bkg_tiles(0, 0, 20, 18, bkmap);
    setup_bkobject(&obj1, 40, 24, 120, 24);
    setup_bkobject(&obj2, 152, 24, 120, 32);
    setup_bkobject(&ground, 0, 1200, 144, 24);
    SHOW_BKG;
    DISPLAY_ON;
    move_bkg(0, 0);
    bkgX = 0;
    drawn = 0;

    set_sprite_data(0, 33, dudeb);
    setupplayer();
    moveplayer(player.x, player.y);
    xlast = player.x;
    ylast = player.y;

    set_sprite_data(33, 2, enemy);
    set_sprite_data(35, 5, invl);
    //gumba gumba1;
    //setupgumba(&gumba1, 3, 9, 120, 130, 0);
    //gumba gumba2;
    //setupgumba(&gumba2, 3, 10, 50, 100, 1);    
        
    SHOW_SPRITES;
    while(1)
    {          
        if(joypad() & J_B)
        {
            if(attcounter == 0 && bpress == 0)
            {
                attcounter = 1;
            }
            bpress = 1;
        }
        else
        {
            bpress = 0;
        }  
        if(pragdoll_counter == 0)
        {
            basicmove();            
            playergrav();
            normalatt();
        }
        else if (invul == 0)
        {
            pragdoll();
            playergrav();
            attcounter = 0;
        }
        
        
        if (invul > 0)
        {
            invulnerable();
        }
        
        level2();     
        
        //adress of the enemy struct, tile number, the speeds it will move when attacked jX-jY, and ai move speed
        //gumbamove(&gumba1, 4, -3, 1, 120, 140, 130);
        //gumbamove(&gumba2, 4, -3, 1, 50, 70, 100);
        
        if (player.x < 100)
        {
            if (player.x > 40)
            {
                xlast = player.x;
            }
            else
            {
                if(bkgX > 0)
                {
                    bkgXScroll = player.x - xlast; 
                    bkgX += bkgXScroll;
                    //PROBLEMATIC !!!!!!!
                    //CHANGE LATER !!!!!
                    player.x = 40;   
                    xlast = player.x;
                }
                else if(player.x > 0)
                {
                    xlast = player.x;
                }  
               
            }
        }
        else
        {           
            bkgXScroll = player.x - xlast;
            bkgX += bkgXScroll;
            if (bkgX > 256)
            {
            } 
            //PROBLEMATIC !!!!!!!
            //CHANGE LATER !!!!!
            player.x = 100;   
            xlast = player.x;

        }
        ylast = player.y;
        //MOVE PLAYER to the location determined as a result of this cycle
        moveplayer(xlast, ylast);
        draw(maps2Count, maps2);
        scroll_bkg(bkgXScroll, bkgYScroll);
        //default timer: 4
        performantdelay(4);
    }
}




void performantdelay(int cycles)
{
    for(int i = 0; i < cycles; i++)
    {
        wait_vbl_done();
    }
}

int setBetween32(int n)
{
    if(n > 32)
    {
        n -= 32;
    }
    else if(n < 0)
    {
        n += 32;
    }
    return n;
}

void draw(int mapCount, char **maps)
{
    int diff = bkgX/8 - drawn;
    if (diff > 0)
    {
        for(int i = 0; i < mapCount - 1; i++)
        {
            if(bkgX > 260 * i && bkgX < 260 * (i + 1))
            {
                set_bkg_submap(setBetween32(drawn + 20), 0, diff, 18, maps[i + 1], 32);
            }
        }
        if(bkgX > (mapCount - 1) * 260)
        {
            set_bkg_submap(setBetween32(drawn + 20), 0, diff, 18, maps[0], 32);
        }
    }
    else if (diff < 0)
    {
        for(int i = 0; i < mapCount; i++)
        {
            if(bkgX > 260 * i && bkgX < 260 * (i + 1))
            {
                set_bkg_submap(setBetween32(drawn + diff), 0, -diff, 18, maps[i], 32);
            }
        }  
    }
    drawn += diff;  
       
}

//the main collision detection function
int checkcoll(struct hitscan *sq1, struct hitscan *sq2)
{
//EITHER sq1's x[0] is between sq2's x's OR sq1's x[1] is between sq2's x's OR (sq1 x0 is smaller AND sq1 x1 is bigger) // meaning sq1 contains sq2 x's inside
    if ((sq1->x + sq1->xempty >= sq2->x + sq2->xempty && sq1->x + sq1->xempty <= sq2->x + sq2->xempty + sq2->width) || 
    (sq1->x + sq1->xempty + sq1->width >= sq2->x + sq2->xempty && sq1->x + sq1->xempty + sq1->width <= sq2->x + sq2->xempty + sq2->width) || 
    (sq1->x + sq1->xempty <= sq2->x + sq2->xempty && sq1->x + sq1->xempty + sq1->width >= sq2->x + sq2->xempty + sq2->width) == 1)
    {
// 1 = passed both, 0 = passed x but failed y        
        return (sq1->y + sq1->yempty >= sq2->y + sq2->yempty && sq1->y + sq1->yempty <= sq2->y + sq2->yempty + sq2->height) || 
        (sq1->y + sq1->yempty + sq1->height >= sq2->y + sq2->yempty && sq1->y + sq1->yempty + sq1->height <= sq2->y + sq2->yempty + sq2->height) || 
        (sq1->y + sq1->yempty <= sq2->y + sq2->yempty && sq1->y + sq1->yempty + sq1->height >= sq2->y + sq2->yempty + sq2->height);
    }
//returns 2 if passes x critera but fails y    
    return 2;
}




void setup_bkobject(struct bkobject *name, int x, int width, int y, int height)
{
    name->x = x;
    name->width = width;
    name->y = y;
    name->height = height;
}

//Regular Block Collision logic
void regBlock(struct bkobject *name)
{
    if ((name->x >= player.x + player.xempty && name->x <= player.x + player.xempty + player.width) || 
    (name->x + name->width >= player.x + player.xempty && name->x + name->width <= player.x + player.xempty + player.width) || 
    (name->x <= player.x + player.xempty && name->x + name->width >= player.x + player.xempty + player.width) == 1)
    {      
        if ((name->y > ylast + player.yempty && name->y <= ylast + player.yempty + player.height) || 
        (name->y + name->height >= ylast + player.yempty && name->y + name->height <= ylast + player.yempty + player.height) || 
        (name->y <= ylast + player.yempty && name->y + name->height >= ylast + player.yempty + player.height) == 1)
        {
            if(xlast < name->x)
            {
                printf("aaaaaaa");
                player.x = name->x - player.xempty - player.width -2;
            }
            else
            {
                printf("bbbbbbbb");
                player.x = name->x + name->width - player.xempty + 2;
            }
        }
    }   
    
    if ((name->x >= xlast + player.xempty && name->x <= xlast + player.xempty + player.width) || 
    (name->x + name->width >= xlast + player.xempty && name->x + name->width <= xlast + player.xempty + player.width) || 
    (name->x <= xlast + player.xempty && name->x + name->width >= xlast + player.xempty + player.width) == 1)
    {     
        if ((name->y >= player.y - player.yempty && name->y - name->height <= player.y - player.yempty) || 
        (name->y >= player.y - player.yempty - player.height && name->y - name->height <= player.y - player.yempty - player.height) || 
        (name->y >= player.y - player.yempty && name->y - name->height <= player.y - player.yempty - player.height) == 1)
        {
            player.y = name->y - name->height;
            player.bkcoll = 1;
        }
        //the condition for standing/walking on the object       
        else if(player.y - player.yempty == name->y - name->height)
        {   
            player.bkcoll = 1;
        }
    }
}
 
 void playergrav(void)
 {
     if (player.bkcoll != 1)
        {
            //this condition is necessary to stop character from going back to jump pose during attack          
            if(attcounter == 0)
            {
                plyan(5, 13, 11, 14);                
            }
            yspeed += GRAV;
            player.y += yspeed;                     
        }
        else
        {
            yspeed = 0;
        }
 }

 void pragdoll(void)
 {
     switch(pragdoll_counter)
     {
         case 1:
            yspeed = -Y_RAGD;
            player.y += yspeed;
            flingdir = -touching;
            player.x += X_RAGD * flingdir;
            pragdoll_counter++;
            break;
         default:
            player.x += X_RAGD * flingdir;
            pragdoll_counter++;
            break;
         case 4:
             pragdoll_counter = 0;
             touching = 0;
             invul++;
             break;
     }
 }

  void basicmove(void)
{
        //for jumping   
        if (joypad() & J_A && player.bkcoll == 1)
        {
            player.bkcoll = 0;
            yspeed = -12;
            player.y += yspeed;
        } 
        
        if (joypad() & J_RIGHT)
        {
            //makes it so you cant change your direction once you've already finished the first part of attack animation, but you can still move the other way           
            if (attcounter < 4)
            {
                //used to reset animation counter if it was counting for the other direction previously                
                if (dir == -1)
                {
                    dir = 1;
                    counter = 0;
                }
                if (counter > 2 && counter < 4)
                {
                    plyan(5, 6, 3, 7);
                }
                else if (counter == 4)
                {
                    counter = 0;
                    plyan(1, 2, 3, 4);
                }         
                counter++;
            }
            if (xspeed < speedmax)
                {
                    xspeed += P_ACCEL;
                }    
            
        }
        else if (joypad() & J_LEFT)
        {
            if (attcounter < 4)
            {
                //used to reset animation counter if it was counting for the other direction previously                  
                if (dir == 1)
                {
                    dir = -1;
                    counter = 0;
                }
                if (counter > 2 && counter < 4)
                {
                    plyan(1, 10, 11, 12);
                }
                else if (counter == 4)
                {
                    plyan(1, 8, 3, 9);
                    counter = 0;
                    //set_sprite_tile(0, 3);
                } 
                  
                counter++;
            }
            if (xspeed > -speedmax)
                {
                    xspeed += -P_ACCEL;
                }  
            
        }
        else if (xspeed > 0)
        {
            xspeed += -P_ACCEL;
            //this condition is necessary to stop the character from going to default pose during attack if no button is pressed            
            if (attcounter == 0)
            {
                plyan(1, 2, 3, 4);
                //set_sprite_tile(0, 1);
            }
        }
        else if (xspeed < 0)
        {
            xspeed += P_ACCEL;
            //this condition is necessary to stop the character from going to default pose during attack if no button is pressed            
            if (attcounter == 0)
            {
                plyan(1, 8, 3, 9);
                //set_sprite_tile(0, 3);
            }
        }
        player.x += xspeed;         
        
}


void lastDir(void)
{
    if (dir == 1)
    {
        plyan(1, 2, 3, 4);
    }
    else
    {
        plyan(1, 8, 3, 9);
    }
}
void plyan(int zero, int one, int two, int three)
{
   set_sprite_tile(0, zero);
   player.spritids[0] = zero;
   set_sprite_tile(1, one);
   player.spritids[1] = one;
   set_sprite_tile(2, two);
   player.spritids[2] = two;
   set_sprite_tile(3, three);
   player.spritids[3] = three;
}

void setupplayer(void)
 {  
    plyan(1, 2, 3, 4);
    player.x = 50;
    player.xempty = 4;
    player.width = 8;
    player.y = 60;
    player.yempty = 0;
    player.height = 16;
    player.ground = 120;
 }

  void moveplayer(int x, int y)
 {    
    move_sprite(0, x, y);
    move_sprite(1, x, y + 8);
    move_sprite(2, x + 8, y);
    move_sprite(3, x + 8, y + 8);
 }

 //sets hitscan for the player **player hitting the enemy**
void setpscan(int x, int width, int y, int height)
{
    pscan.x = x;
    pscan.width = width;
    pscan.y = y;
    pscan.height = height;
    pscan.xempty = 0;
    pscan.yempty = 0;
}
 


void invulnerable(void)
{
    switch(invul)
    {
        case 1:
            set_sprite_tile(8, 35);
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 2:
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 3:
            set_sprite_tile(8, 36);
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 4:
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 5:
            set_sprite_tile(8, 37);
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 6:
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 7:
            set_sprite_tile(8, 38);
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 8:
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 9:
            set_sprite_tile(8, 39);
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 10:
            move_sprite(8, player.x, player.y);
            invul++;
            break;
        case 11:
            invul = 0;
            set_sprite_tile(8, 0);
            break;   
    }   
}

void normalatt(void)
{
    if (dir == 1)
            {
                //Right attack animation
                switch(attcounter)
                {
                    case 0:
                        break;
                    case 1:
                        //set_sprite_tile(0, 6);
                        plyan(1, 2, 15, 16);
                        attcounter++;
                        break;
                    case 2:
                        //set_sprite_tile(0, 7);
                        plyan(1, 2, 3, 16);
                        set_sprite_tile(4, 17);
                        move_sprite(4, player.x + 8, player.y - 8);
                        attcounter++;
                        break;
                    case 3:
                        set_sprite_tile(4, 32);
                        set_sprite_tile(5, 17);
                        move_sprite(4, player.x + 8, player.y - 8);
                        move_sprite(5, player.x + 8, player.y - 16);
                        attcounter++;
                        break;
                    case 4:
                    //sets hitscan(pscan) area to attack area
                    //ORIGINAL width is 24, but input reduced for game feel
                        setpscan(player.x + 8, 24 - 4, player.y, -16);
                        
                        //set_sprite_tile(0, 9);
                        plyan(1, 2, 18, 19);

                        set_sprite_tile(4, 25);
                        set_sprite_tile(5, 27);
                        set_sprite_tile(6, 26);
                        set_sprite_tile(7, 28);
                        move_sprite(4, player.x + 16, player.y);
                        move_sprite(5, player.x + 24, player.y);
                        move_sprite(6, player.x + 16, player.y + 8);
                        move_sprite(7, player.x + 24, player.y + 8);
                        attcounter++;
                        break;
                    //Locks the character in frame till the last case
                    //sets hitscan(pscan) again in case of movement
                    default:
                        setpscan(player.x + 8, 24 - 4, player.y, -16);
                        move_sprite(4, player.x + 16, player.y);
                        move_sprite(5, player.x + 24, player.y);
                        move_sprite(6, player.x + 16, player.y + 8);
                        move_sprite(7, player.x + 24, player.y + 8);
                        attcounter++;
                        break;
//determines how many cycles player will be locked in last part of animation                    
                    case 7:
                    //after attack ends, sets hitscan area to 0    
                        setpscan(0, 0, 0, 0);
                        
                        //set_sprite_tile(0, 1);
                        plyan(1, 2, 3, 4);
                        
                        set_sprite_tile(4, 0);
                        set_sprite_tile(5, 0);
                        set_sprite_tile(6, 0);
                        set_sprite_tile(7, 0);
                        attcounter = 0;
                        break;
                } 
                
            }
            else if (dir == -1)
            {
                //Left attack animation
                switch(attcounter)
                {
                    case 0:
                        break;
                    case 1:
                        //set_sprite_tile(0, 6);
                        plyan(20, 21, 3, 9);
                        attcounter++;
                        break;
                    case 2:
                        //set_sprite_tile(0, 7);
                        plyan(1, 21, 3, 9);
                        set_sprite_tile(4, 22);
                        move_sprite(4, player.x, player.y - 8);
                        attcounter++;
                        break;
                    case 3:
                        set_sprite_tile(4, 31);
                        set_sprite_tile(5, 22);
                        move_sprite(4, player.x, player.y - 8);
                        move_sprite(5, player.x, player.y - 16);
                        attcounter++;
                        break;
                    case 4:
                    //sets hitscan(pscan) area to attack area
                    //ORIGINAL width is 24, but input reduced for game feel
                        setpscan(player.x, -24 + 4, player.y, -16);
                        
                        //set_sprite_tile(0, 9);
                        plyan(23, 24, 3, 9);

                        set_sprite_tile(4, 25);
                        set_sprite_tile(5, 29);
                        set_sprite_tile(6, 26);
                        set_sprite_tile(7, 30);
                        move_sprite(4, player.x - 8, player.y);
                        move_sprite(5, player.x - 16, player.y);
                        move_sprite(6, player.x - 8, player.y + 8);
                        move_sprite(7, player.x - 16, player.y + 8);
                        attcounter++;
                        break;
                    //Locks the character in frame till the last case
                    //sets hitscan(pscan) again in case of movement
                    default:
                        setpscan(player.x + 8, -24 + 4, player.y, -16);
                        move_sprite(4, player.x - 8, player.y);
                        move_sprite(5, player.x - 16, player.y);
                        move_sprite(6, player.x - 8, player.y + 8);
                        move_sprite(7, player.x - 16, player.y + 8);
                        attcounter++;
                        break;
//determines how many cycles player will be locked in last part of animation                    
                    case 7:
                    //after attack ends, sets hitscan area to 0    
                        setpscan(0, 0, 0, 0);
                        
                        //set_sprite_tile(0, 1);
                        plyan(1, 8, 3, 9);
                        
                        set_sprite_tile(4, 0);
                        set_sprite_tile(5, 0);
                        set_sprite_tile(6, 0);
                        set_sprite_tile(7, 0);
                        attcounter = 0;
                        break;
                } 
            }
}

void setupgumba(struct gumbas *gumba, int HP, int tile, int x, int y, int dir)
{
    gumba->tile = tile;
    gumba->HP = HP;
    gumba->hitbox.x = x;
    gumba->hitbox.y = y;
    gumba->hitbox.width = 8;
    gumba->hitbox.height = 8;
    gumba->hitbox.xempty = 0;
    gumba->hitbox.yempty = 0;

    gumba->xspeed = 0;
    gumba->yspeed = 0;
    gumba->dir = dir;
    gumba->hitcounter = 0;
    set_sprite_tile(tile, 33);
    move_sprite(tile, gumba->hitbox.x, gumba->hitbox.y);
}

void gumbamove(struct gumbas *gumba, int jX, int jY, int aispeed, int xmin, int xmax, int ymin)
{
                                //HP AND//
                                    //DAMAGE ANIMATION//
    //the condition for when hitcounter is bigger than 0, meaning the enemy has been hit. changes tile, then the enemy hitcounter counts up to a certain point and changes the tile to the og.  
    if (gumba->hitcounter > 0)
    {
        if(gumba->hitcounter < 4)
        {
            gumba->hitcounter++;
        }
        else
        {
    //TEMPORARY MEASURE            
            if (gumba->HP > 0)
            {
                set_sprite_tile(gumba->tile, 33);
            }
            gumba->hitcounter = 0;
        }
    }


                                //REGULAR//
                                    //MOVEMENT//
    if (gumba->dir == 0 && gumba->hitbox.x < xmax && gumba->hitcounter == 0)
    {
        gumba->hitbox.x += aispeed;
        move_sprite(gumba->tile, gumba->hitbox.x, gumba->hitbox.y);
    }
    else if (gumba->dir == 0)
    {
        gumba->dir = 1;
    }
    
    if (gumba->dir == 1 && gumba->hitbox.x > xmin && gumba->hitcounter == 0)
    {
        gumba->hitbox.x += -aispeed;
        move_sprite(gumba->tile, gumba->hitbox.x, gumba->hitbox.y);
    }
    else if (gumba->dir == 1)
    {
        gumba->dir = 0;
    }

                            //RELATION// 
                                //TO PLAYER//
    
    if(gumba->hitbox.x < player.x)
    {
        //-1 means to the left of player
        gumba->relation = -1;
    }
    else
    {
        //1 means to the right of player
        gumba->relation = 1;
    }





                                    //PLAYER//
                                        //ATTACK//
                                            //COLLISION//
    if (checkcoll(&(gumba->hitbox), &pscan) == 1 && gumba->hitcounter == 0)
    {        
    //enemy takes damage, hp reduces by 1, makes invisible if HP already == 0 --> CHANGE LATER           
            if(gumba->HP > 0)
            {
                gumba->HP--;
            }
            else
            {
                set_sprite_tile(gumba->tile, 0);
            }
            gumba->hitcounter++;
            
            // TEMPORARY MEASURE            
            if(gumba->HP > 0)
            {
                set_sprite_tile(gumba->tile, 34);
            }
            //x and y fling speed is set here, also moved once for better responsiveness TODO: MAKE FLING INTO A FUNCTION           
            //xspeed is either + or - based on relation to the player
            gumba->xspeed = jX * gumba->relation;  
            gumba->yspeed = jY;
            gumba->hitbox.x += gumba->xspeed;
            gumba->hitbox.y += gumba->yspeed;
            move_sprite(gumba->tile, gumba->hitbox.x, gumba->hitbox.y);    
    }   

                                    //COLLISION// 
                                        //WITH// 
                                            //PLAYER//
    if (checkcoll(&(gumba->hitbox), &player) == 1 && pragdoll_counter == 0 && invul == 0)
    {
        touching = gumba->relation;
        set_sprite_tile(4, 0);
        set_sprite_tile(5, 0);
        set_sprite_tile(6, 0);
        set_sprite_tile(7, 0);
        pragdoll_counter++;
    }



                //RAGDOLL//
                    //IN THE//
                        //AIR//
    //if in the air, uses corresponding fling speeds to keep flinging until hit ground
    if (gumba->hitbox.y < ymin)
        {            
            gumba->yspeed += GRAV;
            gumba->hitbox.y += gumba->yspeed;
            gumba->hitbox.x += gumba->xspeed;
            move_sprite(gumba->tile, gumba->hitbox.x, gumba->hitbox.y);
        }
    //once hits ground, sets fling speeds to 0
    else    
        {
            gumba->yspeed = 0;
            gumba->xspeed = 0;
        }
}