#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include "../include/game.h"


#define ITEM_SWORD          0x00
#define ITEM_SHIELD         0x01
#define ITEM_DOGE           0x02
#define ITEM_CLOAK          0x03
#define ITEM_ARMOR          0x04
#define ITEM_BOW            0x05
#define ITEM_FOOD           0x06
#define ITEM_WATER          0x07
#define ITEM_TENT           0x08
#define ITEM_SPECIAL        0x09

#define ACTION_SPECIAL      0x0
#define ACTION_ATTACK       0x1
#define ACTION_DEFEND       0x2
#define ACTION_HEAL         0x3
#define ACTION_BUY          0x4
#define ACTION_SELL         0x5
#define ACTION_PET          0x6


#define LOCATION_VILLAGE    0x0
#define LOCATION_WILDERNESS 0x1
#define LOCATION_MOUNTAINS  0x2
#define LOCATION_DUNES      0x3
#define LOCATION_WATERFALL  0x4
#define LOCATION_CAVES      0x5
#define LOCATION_MARKET     0x6

#define ENEMIES_BOAR        0x0
#define ENEMIES_DRAGON      0x1
#define ENEMIES_WOLF        0x2
#define ENEMIES_BEAR        0x3
#define ENEMIES_LION        0x4
#define ENEMIES_SNAKE       0x5

#define INITIAL_HEALTH      100
#define INITIAL_COIN        100

#define MAX_STR_LEN         250

const char item_labels[10][MAX_STR_LEN] = {
    "a sword",
    "a shield",
    "a small doge",
    "a stealthy cloak",
    "a suit of armor",
    "a bow and arrow",
    "some food",
    "some water",
    "a camping tent",
    "a special item"
};
const uint32_t item_actions[10] = {
    ACTION_ATTACK,
    ACTION_DEFEND,
    ACTION_PET,
    ACTION_DEFEND,
    ACTION_DEFEND,
    ACTION_ATTACK,
    ACTION_HEAL,
    ACTION_HEAL,
    ACTION_HEAL,
    ACTION_SPECIAL
};

const uint32_t item_expendable[10] = {
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
    1
};

const uint32_t action_weights[10] = {
    200,
    600,
    100,
    75,
    200,
    250,
    10,
    10,
    15,
    0
};

const char item_action_labels[7][MAX_STR_LEN] = {
    "use special",
    "attack",
    "defend",
    "heal",
    "buy",
    "sell",
    "pet"
};

const char location_labels[7][MAX_STR_LEN] = {
    "local village",
    "wilderness",
    "wintery mountains",
    "sandy dunes",
    "mystic waterfalls",
    "caverns of the deep",
    "local market",
};

const char enemy_labels[6][MAX_STR_LEN] = {
    "wild boar",
    "fire-breathing dragon",
    "wild wolf",
    "bear",
    "lion",
    "little snek"
};

const uint32_t enemy_health_vals[6] = {
    80,
    300,
    100,
    150,
    140,
    35
};

const uint32_t enemy_attack_vals[6] = {
    25,
    500,
    150,
    200,
    180,
    90
};


volatile uint32_t inverse_walking_risk = 90000;
volatile uint8_t special_enabled = 0;
volatile uint32_t item_index_counter = 0;

typedef struct _inventory_item
{
    uint32_t item_index;
    uint32_t type;
    char *name;
    struct _inventory_item *next;
} inventory_item_t;

typedef struct _character_context 
{
    uint32_t health;
    uint32_t location;
    uint32_t coins;
    inventory_item_t *bag;
} context_t;

inventory_item_t * create_item(uint32_t item_type)
{
    inventory_item_t * new_item = malloc(sizeof(inventory_item_t));

    if (new_item != NULL) 
    {
        new_item->type = item_type;
        new_item->item_index = item_index_counter;
        new_item->name = NULL;
        new_item->next = NULL;

        item_index_counter+=1;
    }
    return new_item;
}


void display_inventory(inventory_item_t *inventory)
{
    inventory_item_t *inventory_ptr = inventory;
    printf("Your inventory:\n");
    while (inventory_ptr != NULL )
    {
        printf(" - Item %d\n", inventory_ptr->item_index);
        printf("\tType: %s\n", item_labels[inventory_ptr->type]);
        printf("\tAction: %s\n", item_action_labels[item_actions[inventory_ptr->type]]);
        if(inventory_ptr->type == ITEM_DOGE)
        {
            printf("\tName: %s\n",inventory_ptr->name);
        }
        inventory_ptr = inventory_ptr->next;
    }
}

void display_stats(context_t *player)
{
    printf("Your current status:\n");
    printf("\tHealth: %d\n", player->health);
    printf("\tLocation: %s\n", location_labels[player->location]);
    printf("\tCoins: %d\n", player->coins);
}

inventory_item_t * get_item_from_inventory(context_t *player, uint32_t type)
{
    inventory_item_t *result = NULL;
    inventory_item_t *inventory_ptr = player->bag;
    while (inventory_ptr != NULL )
    {
        if ( inventory_ptr->type == type ) 
        {
            result = inventory_ptr;
            break;
        }
        inventory_ptr = inventory_ptr->next;
    }
    return result;
}

uint8_t drop_item_from_bag(context_t *player, uint32_t item_index)
{
    uint8_t success = 0;
    inventory_item_t *inventory_ptr = player->bag;
    while (inventory_ptr != NULL )
    {
        if ( inventory_ptr->item_index == item_index ) 
        {
            free(inventory_ptr); // no memory leaks here :)
            success = 1;
            break;
        }
        inventory_ptr = inventory_ptr->next;
    }
    return success;
}

uint8_t remove_item_from_inventory(context_t *player, uint32_t item_index)
{
    uint8_t success = 0;
    inventory_item_t *inventory_ptr = player->bag;
    inventory_item_t *previous_ptr = NULL;
    while (inventory_ptr != NULL )
    {
        if ( inventory_ptr->item_index == item_index ) 
        {
            if (NULL == previous_ptr)
            {
                player->bag = inventory_ptr->next;
            }
            else 
            {
                previous_ptr->next = inventory_ptr->next;
            }
            success = 1;
            break;
        }
        previous_ptr = inventory_ptr;
        inventory_ptr = inventory_ptr->next;

    }
    return success;
}

void add_item_to_inventory(context_t *player, inventory_item_t *new_item)
{
    if(player->bag == NULL )
    {
        player->bag = new_item;
    } 
    else 
    {
        inventory_item_t *inventory_ptr = player->bag;
        while (inventory_ptr->next != NULL )
        {
            inventory_ptr = inventory_ptr->next;
        }
        inventory_ptr->next = new_item;
    }

}



uint32_t read_until_newline_or_limit(char * buffer, uint32_t limit)
{
    uint32_t index = 0;
    char c = 0;
    do 
    {
        c = getchar();
        if ( '\n' == c)
        {
            break;
        }
        buffer[index] = c;
        index +=1;
    } while (index < limit-1);
    buffer[index+1] = '\0';
    return index+2;
}

void add_item_to_bag(context_t *player, uint32_t item_type)
{
    inventory_item_t * new_item = create_item(item_type);
    if ( item_type == ITEM_DOGE )
    {
        printf("Please enter a name for your doge:");
        char buffer[MAX_STR_LEN];
        memset(buffer,'\0',MAX_STR_LEN);
        uint32_t buff_len = read_until_newline_or_limit((char *)&buffer,MAX_STR_LEN);
        new_item->name = malloc(buff_len+1);
        memcpy(new_item->name,&buffer,buff_len);
        new_item->name[buff_len] = '\0';
    }
    add_item_to_inventory(player,new_item);
}


context_t * create_new_player()
{
    context_t *new_player = calloc(1,sizeof(context_t));

    new_player->health = INITIAL_HEALTH;
    new_player->coins = INITIAL_COIN;
    new_player->location = LOCATION_VILLAGE;
    new_player->bag = NULL;

    return new_player;
}

uint8_t show_menu(char items[][MAX_STR_LEN], uint32_t item_count) // menus can only support 36 items
{
    uint8_t resp = 0;
    uint8_t item_found = 0;
    char selection_chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    uint32_t selection_chars_length = strlen(selection_chars);
    if (selection_chars_length >= item_count)
    {        
        printf("Please make a selection:\n");
        for(uint32_t index = 0; index< item_count; index++ )
        {
            printf("\t- [%c] - %s\n",selection_chars[index],  items[index]);
        }
        printf("Enter your choice: ");
        uint8_t response = (uint8_t) getchar();
        getchar(); // new line
        
        for (uint32_t index = 0; index<selection_chars_length; index++)
        {
            if ( selection_chars[index] == (char) response && index < item_count )
            {
                resp = index;
                item_found = 1;
            } 
        }
        if ( 0 == item_found) 
        {
            resp = show_menu(items, item_count);
        }
    }
    return resp;
}

void seed_random()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand((time_t)ts.tv_nsec);
}

/**
 * @brief Get a random number between min and max (inclusive)
 * 
 * @param min minumum value you want within range
 * @param max maximum value you want within range (inclusive)
 * @return uint32_t 
 */
uint32_t get_random_number(uint32_t min, uint32_t max)
{


    uint32_t return_val = 0;
    if (min <= max)
    {
        return_val = (rand() % (max + 1 - min)) + min;
    }
    return return_val;
}

void print_item_info(uint32_t item_type)
{
    printf("This item, %s, can be used to perform this action: %s.\n", item_labels[item_type], item_action_labels[item_actions[item_type]]);
}

void handle_new_item(context_t *player)
{
    printf("While exploring, you found something!\n");
    uint32_t new_item = get_random_number(0,8);
    print_item_info(new_item);
    char choices [2][MAX_STR_LEN] = 
    {
        "no",
        "yes"
    };
    printf ("Add this to your inventory? ");
    uint32_t add_item = show_menu(choices,2);
    if (add_item == 1) 
    {
        add_item_to_bag(player,new_item);
    }
}

uint32_t count_items(inventory_item_t * inventory)
{
    uint32_t count = 0;
    while ( NULL != inventory )
    {
        count += 1;
        inventory = inventory->next;
    }
    return count;
}

inventory_item_t * select_item_in_inventory(context_t *player)
{
    uint32_t selected_item = 0;
    uint32_t num_items = count_items(player->bag);
    inventory_item_t *items_tracker[num_items];
    inventory_item_t *return_val = NULL;
    if (0 != num_items)
    {
        char choices[num_items][MAX_STR_LEN];
        inventory_item_t *inventory_ptr = player->bag;
        uint32_t index = 0;
        while (index < num_items && inventory_ptr != NULL)
        {
            items_tracker[index] = inventory_ptr;
            snprintf(choices[index],MAX_STR_LEN, "%s", item_labels[inventory_ptr->type]);
            inventory_ptr = inventory_ptr->next;
            index += 1;
        }
        selected_item = show_menu( choices, num_items);     
        return_val = items_tracker[selected_item] ;
    }
    return return_val;
}

uint8_t handle_enemy(context_t *player)
{
    uint8_t game_active = 1;
    uint32_t enemy_type = get_random_number(0,5);
    uint32_t enemy_health = get_random_number(
        enemy_health_vals[enemy_type], 
        enemy_health_vals[enemy_type] + INITIAL_HEALTH
        );
    uint32_t enemy_coin = get_random_number(1,50);

    uint32_t player_attack = 0;
    uint32_t player_defend = 0;

    inventory_item_t *player_item = NULL;
    uint32_t player_item_type = 0;
    //uint32_t player_action = 0;
    uint32_t enemy_attack = get_random_number(
        enemy_attack_vals[enemy_type] -10, 
        enemy_attack_vals[enemy_type] +10
        );

    uint32_t enemy_action = 0;
    // do nothing (0); defend (1); attack (2)

    printf("You have encountered a %s. Its health is %d and it is carrying %d coin.\n",enemy_labels[enemy_type], enemy_health, enemy_coin);

    char choices[2][MAX_STR_LEN] = {
            "run away",
            "use item"
        };


    uint8_t choice = 0;
    
    do
    {
         enemy_action = get_random_number(0,2); 
         player_attack = 0;
         player_defend = 0;
        choice = show_menu( choices,2);
        if ( 0 == choice) 
        {
            uint32_t killed_while_running = get_random_number(0,10);
            if ( 1 == killed_while_running )
            {
                player->health = 0;
                game_active = 0;
                printf("You were killed while fleeing.\n");
            } else 
            {
                printf("You successfully ran away.\n");
            }
            break; 
        } else if ( 1== choice) 
        {
            if ( NULL == player->bag )
            {
                printf("You do not have any items.\n");
            } else 
            {
                player_item = select_item_in_inventory(player);
                player_item_type = player_item->type;
                if (player_item != NULL)
                {
                    if (item_actions[player_item_type] == ACTION_ATTACK)
                    {
                        player_attack = get_random_number(
                            action_weights[player_item_type]-5,
                            action_weights[player_item_type]+5
                            );
                    } 
                    else if (item_actions[player_item_type] == ACTION_DEFEND)
                    {
                        player_defend = get_random_number(
                            action_weights[player_item_type]-5,
                            action_weights[player_item_type]+5
                            );
                    }
                    else if (item_actions[player_item_type] == ACTION_HEAL)
                    {
                        player->health += get_random_number(1,20);
                        printf("Your new health is: %d\n",player->health);
                        if (item_expendable[player_item_type] == 1)
                        {
                            remove_item_from_inventory(player,player_item->item_index);
                        }
                    } 
                    else 
                    {
                        printf("This item does not help you in battle.\n");
                    }
                }
            }
            if (enemy_action == 0 )
            {
                printf("The %s does nothing.\n", enemy_labels[enemy_type]);
            } else if (enemy_action == 1)
            {
                printf("The %s tries to defend itself.\n",enemy_labels[enemy_type]);
            }
            
            if ( enemy_action != 1 && player_attack > 0 ) 
            {
                if ( player_attack > enemy_health)
                {
                    enemy_health = 0;
                } 
                else 
                {
                    enemy_health -= player_attack;
                }
                if ( 0 == enemy_health )
                {
                    printf("You have slain the %s and taken their coin.\n",enemy_labels[enemy_type]);
                    player->coins += enemy_coin;
                }
                else if ( player_attack > 0 )
                {
                    printf("The %s has a new health of %d.\n",enemy_labels[enemy_type],enemy_health);
                }
            } 
            else if (player_attack > 0 )
            {
                printf("The %s successfully defended itself from your attack. Its current health is:%d\n",enemy_labels[enemy_type], enemy_health);
            }

            
            if ( enemy_action == 2 )
            {
                if (player_defend >= enemy_attack) 
                {
                    enemy_attack = 0;
                }
                else
                {
                    enemy_attack -= player_defend;
                }
                if (enemy_attack > player->health )
                {
                    player->health = 0;
                    printf("The %s has killed you.\n", enemy_labels[enemy_type]);
                    game_active = 0;
                    break;
                }
                else 
                {
                    player->health -= enemy_attack;
                    printf("After the %s attacked you, your new health is: %d\n", 
                    enemy_labels[enemy_type], player->health);
                }
                
            } 
 
            
            
        }

    } while ( enemy_health > 0);

    return game_active;
}

uint8_t handle_travel(context_t *player)
{
    uint32_t new_location = get_random_number(0,5);
    uint32_t injury_occurs = get_random_number(0,inverse_walking_risk);
    uint8_t game_active = 1;

    if ( 1 == injury_occurs )
    {
        printf("You slipped and fell while traveling, " 
        "breaking your neck. You have died.\n");
        game_active = 0;
    }

    player->location = new_location;

    return game_active;

}

void buy_item(context_t * player, uint32_t store_prices[],uint32_t player_choice)
{
    uint32_t item_price = store_prices[player_choice];
    if (player->coins >= item_price) 
    {
        player->coins -= item_price;
        add_item_to_bag(player,player_choice);
        printf("New balance: %d\n",player->coins);
    }
    else
    {
        printf("You don't have enough money for that. Requires %d, you have %d\n",item_price,player->coins);        
    }
    
    
}

void sell_item(context_t * player, uint32_t store_prices[])
{
    printf("Selling Item. Please select an item to sell at 1/2 of its value.\n");
    inventory_item_t *item_to_sell = select_item_in_inventory(player);

    if (NULL == item_to_sell)
    {
        printf("You have no items.\n");
        return;
    }

    if ( item_to_sell->type == ITEM_FOOD 
      || item_to_sell->type == ITEM_WATER 
      || item_to_sell->type == ITEM_DOGE )
    {
        printf("You cannot sell that item.\n");
    }
    else
    {

        uint32_t value = store_prices[item_to_sell->type];
        value >>= 1;
        player->coins += value;
        printf("Item sold for %d coins.\n",value);
        remove_item_from_inventory(player,item_to_sell->item_index);
    }
}



void handle_market(context_t *player)
{
       
    uint8_t leave_market = 0;
    uint32_t player_choice  = 0;
    char store_choices[12][MAX_STR_LEN];
    uint32_t store_prices[10];
    store_prices[ITEM_SWORD]   = get_random_number(200,400);
    store_prices[ITEM_SHIELD]  = get_random_number(40,60);
    store_prices[ITEM_DOGE]    = 15;
    store_prices[ITEM_CLOAK]   = get_random_number(10,50);
    store_prices[ITEM_ARMOR]   = get_random_number(65,85);
    store_prices[ITEM_BOW]     = get_random_number(150,300);
    store_prices[ITEM_FOOD]    = get_random_number(2,10);
    store_prices[ITEM_WATER]   = get_random_number(1,5);
    store_prices[ITEM_TENT]    = get_random_number(30,40);
    store_prices[ITEM_SPECIAL] = get_random_number(2000,2500);
    
    snprintf(store_choices[ITEM_SWORD],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_SWORD], store_prices[ITEM_SWORD]);

     
    snprintf(store_choices[ITEM_SHIELD],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_SHIELD],store_prices[ITEM_SHIELD]);

    
    snprintf(store_choices[ITEM_DOGE],MAX_STR_LEN,"%s - donate %d to the local humane society to adopt", item_labels[ITEM_DOGE],store_prices[ITEM_DOGE]);

     
    snprintf(store_choices[ITEM_CLOAK],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_CLOAK],store_prices[ITEM_CLOAK]);

     
    snprintf(store_choices[ITEM_ARMOR],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_ARMOR],store_prices[ITEM_ARMOR]);

     
    snprintf(store_choices[ITEM_BOW],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_BOW],store_prices[ITEM_BOW]);

     
    snprintf(store_choices[ITEM_FOOD],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_FOOD],store_prices[ITEM_FOOD]);

     
    snprintf(store_choices[ITEM_WATER],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_WATER],store_prices[ITEM_WATER]);

     
    snprintf(store_choices[ITEM_TENT],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_TENT], store_prices[ITEM_TENT]);

     
    snprintf(store_choices[ITEM_SPECIAL],MAX_STR_LEN,"%s - %d coins", item_labels[ITEM_SPECIAL],store_prices[ITEM_SPECIAL]);

    snprintf(store_choices[10],MAX_STR_LEN,"sell an item");
    snprintf(store_choices[11],MAX_STR_LEN,"leave the market");

    while (leave_market == 0) 
    {
        player_choice = show_menu(store_choices,12);

        if (10 > player_choice)
        {
            buy_item(player,store_prices,player_choice);
        } 
        else if (10 == player_choice)
        {
            sell_item(player,store_prices);
        }
        else
        {
            leave_market = 1;
        }
        
    }



    return;
}

void use_item(context_t *player)
{
    printf("Selecting item to use. ");
    inventory_item_t *item = select_item_in_inventory(player);
    if (NULL == item)
    {
        printf("You don't have any items!");
        return;
    }
    if (
        !(
            item_actions[item->type] == ACTION_HEAL
        ||  item_actions[item->type] == ACTION_PET
        ||  item_actions[item->type] == ACTION_SPECIAL
        )
    )
    {
        printf("You cannot use that item at this time.\n");
    }
    else 
    {
        if ( item_actions[item->type] == ACTION_HEAL )
        {
            printf("You increase your health with %s.\n",item_labels[item->type]);
            player->health += action_weights[item->type];
        }
        else if (item_actions[item->type] == ACTION_PET)
        {
            printf("You pet your doge. Everything is better.\n");
            player->health += action_weights[item->type];
        }
        else if (item_actions[item->type] == ACTION_SPECIAL)
        {
            special_enabled = 1;
        }

        if ( item_expendable[item->type] == 1)
        {
            remove_item_from_inventory(player,item->item_index);
        }
    }
}

void drop_item(context_t * player)
{
    if (player->bag != NULL)
    {
        printf("Selecting item to drop. ");
        inventory_item_t *item = select_item_in_inventory(player);
        if (1 == drop_item_from_bag(player,item->item_index))
        {
            printf("Item was dropped!\n");
        }
    }
    else
    {
        printf("You don't have any items!");
    }

}

uint8_t handle_explore(context_t *player)
{
    uint8_t game_active = 1;
    printf("Exploring the %s.\n", location_labels[player->location]);



    uint8_t encounter_item = get_random_number(0,1);
    uint8_t encounter_enemy = get_random_number(0,1);


    if (1 == encounter_item)
    {
        handle_new_item(player);
    }

    if(player->location == LOCATION_VILLAGE )
    {
        printf("You have found the market.\n");
        handle_market(player);
    }
    else
    {
    if (1 == encounter_enemy) 
        {
            game_active = handle_enemy(player);
        }
    }
    
    return game_active;
}

uint8_t show_main_menu(context_t *player)
{
    uint8_t active_game = 1;
    char choices[7][MAX_STR_LEN] = {
            "quit",
            "travel",
            "explore location",
            "show items in bag",
            "use item",
            "drop item",
            "show stats"
        };

    uint8_t choice = show_menu( choices, 7);
    
    switch(choice)
    {
        case 0 : // quit
            active_game = 0;
            break;
        
        case 1 : // travel
            active_game = handle_travel(player);
            break;

        case 2 : //explore
            active_game = handle_explore(player);
            break;

        case 3 : //show items
            display_inventory(player->bag);
            break;

        case 4 : // use item
            use_item(player);
            break;
        
        case 5 : // drop item
            drop_item(player);
            break;
        case 6 : // show_stats
            break;
    }

    return active_game;
}

void * play_game(void *arg) 
{
    special_enabled = 0;
    item_index_counter = 0;

    seed_random();
    uint8_t active_game = 1;
    context_t* player = create_new_player();

    while (1 == active_game)
    {
        if (special_enabled == 1)
        {
            printf("=====\n\tYou win the game!\n=====\n");
            break;
        }
        printf("\n");
        display_stats(player);
        active_game = show_main_menu(player);
    }
    return arg;
}




#ifdef STANDALONE_MODE
int main(int argc, char** argv)
{
    play_game(NULL);
}
#endif

