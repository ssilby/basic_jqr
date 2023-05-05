#include "game_test.h"

// TODO: convert this to game.h and put it in include


/*
    Reference the CUnit Users Guide:
    http://cunit.sourceforge.net/doc/writing_tests.html#tests

    Step 1. Create the test cases
    Step 2. _add the test suite to the registry
    Step 3. _add the test to the suite.
*/


#define PIPE_READ 0
#define PIPE_WRITE 1

static int    saved_stdout;
static int saved_stdin;
int pipe_fd_stdin[2];
int pipe_fd_stdout[2];

uint8_t check_game_win_executed = 0;



/**
 * @brief checks for needle in haystack in null-terminated strings
 * 
 * @param needle - null-terminated string of needle, or the string being searched for
 * @param haystack - null-terminated string of the haystack, of the string being searched
 * @return uint8_t 0 if full needle is not found, 1 if found
 */

uint32_t _read_all_from_stdout_pipe(char ** buffer)
{
    if (*buffer != NULL)
    {
        free(*buffer);
        *buffer = NULL;
    }
    uint32_t buf_size = 4096; 
    char * new_buff = NULL;
    uint8_t slept = 0;
    int64_t bytes_read = 0; 
    uint32_t total_bytes_read  = 0;
    uint32_t read_offset = 0;
    *buffer = calloc(1, buf_size); 

    if (*buffer != NULL)
    {
        while ( 1 )
        {
            bytes_read = read(pipe_fd_stdout[PIPE_READ], (*buffer) + read_offset, buf_size - read_offset);

            if ( 0 >=  bytes_read )
            {
                if ( 
                        -1 == bytes_read && 
                        ( 
                            errno == EINTR || 
                            errno == EAGAIN 
                        ) 
                    )
                {
                    if ( errno == EAGAIN )
                    {
                        if(total_bytes_read == 0 && slept < 3)
                        {
                            usleep(10000);
                            slept += 1;
                        } else 
                        {
                            break;
                        }
                    }
                    continue;
                }
                break;
            } 

            total_bytes_read += (uint32_t) bytes_read;

            
            buf_size *= 2;
            new_buff = realloc(*buffer, buf_size);

            if ( NULL != new_buff )
            {
                *buffer = new_buff; 
            } /* else 
            {
                break;
            }*/

            read_offset += bytes_read; 
        }

    }
    if (total_bytes_read == 0)
    {
        free(*buffer);
        free(new_buff);
        *buffer = NULL;
    }

    return total_bytes_read;
}



static void _debug_msg(const char * format, ...) 
{
#ifndef DISABLE_DEBUG_MESSAGES
  va_list args; // thanks to https://www.ozzu.com/cpp-tutorials/tutorial-writing-custom-printf-wrapper-function-t89166.html
  va_start( args, format);
  vdprintf( saved_stdout, format, args );
  va_end( args );
#endif
}
void _say(char * msg, uint32_t bytes)
{
    int32_t write_val = write(pipe_fd_stdin[PIPE_WRITE],msg,bytes);
    if ( 0> write_val )
    {
        perror("error on write");
    } 
    else if ( (uint32_t) write_val != bytes)
    {
        _debug_msg("expected write bytes: %d, actual:%d\n",bytes, write_val);
    }
    usleep(10000);
}

uint8_t at_location(char * msg, char * location)
{
    if (msg == NULL || location == NULL)
    {
        return 0;
    }
    return (strstr(msg, location) != NULL)?1:0;
}

uint8_t not_at_location(char * msg, char * location)
{
    return ! at_location(msg, location) ;
}

void error_and_die(char * msg)
{
    perror(msg);
    exit(1);
}

pthread_t * _setup_game() // TODO: add error checking
{
    pthread_t * cur_game_thread = calloc(1,sizeof(pthread_t));
    if ( NULL != cur_game_thread) 
    {

        saved_stdout = dup(STDOUT_FILENO);
        saved_stdin = dup(STDIN_FILENO);

        if (saved_stdin == -1 || saved_stdout == -1)
        {
            error_and_die("unable to sdup stdin or stdout");
        }

        if ( 
            -1 == pipe(pipe_fd_stdin)  ||
            -1 == pipe(pipe_fd_stdout)
        )
        {
            error_and_die("creating pipes failed");
        }

        if ( -1 == fcntl(pipe_fd_stdout[PIPE_READ], F_SETFL, O_NONBLOCK) ) // allow us to read from pipe without getting blocked
        {
            error_and_die("put pipe in non-block");
        } 
    
        if ( 
            -1 == dup2(pipe_fd_stdin[PIPE_READ], STDIN_FILENO) ||
            -1 == dup2(pipe_fd_stdout[PIPE_WRITE], STDOUT_FILENO)
        )
        {
            error_and_die ("dup2ing onto fd array");
        }

        if ( 0 != pthread_create(cur_game_thread, NULL, &play_game, NULL) )
        {
            error_and_die ("creating thread");
        }
    }
    return cur_game_thread;
}

void _teardown_game(pthread_t * game_thread)
{
    if (NULL != game_thread) 
    {
        if ( 0 != pthread_join(*game_thread, NULL) )
        {
            error_and_die("joining thread");
        }
     
        if (
            -1 == dup2(saved_stdout, STDOUT_FILENO) || 
            -1 == dup2(saved_stdin, STDIN_FILENO)
        )
        {
            error_and_die("dup2 on cleanup error");
        }

        if (
            -1 == close(saved_stdout) ||
            -1 == close(saved_stdin)
        )
        {
            error_and_die("cleaning up saved fds");
        }

        clearerr(stdout);
        clearerr(stdin);
 
        /*if (
            fsetpos(stdout, &saved_stdout_pos) == -1 || 
            fsetpos(stdin, &saved_stdin_pos) == -1
        )
        {
            error_and_die("setting positions back");
        } */

        if(
            -1 == close(pipe_fd_stdin[PIPE_READ]) ||
            -1 == close(pipe_fd_stdin[PIPE_WRITE]) ||
            -1 == close(pipe_fd_stdout[PIPE_WRITE]) ||
            -1 == close(pipe_fd_stdout[PIPE_READ])
        )
        {
            error_and_die("closing fds on pipes");
        }
    }
    free(game_thread);
}



/**
 * @brief drops into an interactive console with the game
 * type quit to break out of console
 * 
 */
void go_interactive()
{
    uint32_t max_buff = 5000;
    uint32_t read_count = 0;
    char * game_msg = NULL;
    char user_input[max_buff];
    user_input[0] = '\0';
    while (strstr(user_input,"quit") == NULL) 
    {
        read_count = _read_all_from_stdout_pipe(&game_msg);
        write(saved_stdout,game_msg,read_count);
        read_count = read(saved_stdin,&user_input, max_buff);
        _say( (char *) &user_input,read_count);
    }
    free(game_msg);
}

char * get_item_from_list(char * list,char * item)
{
    uint32_t offset = 0;
    char * return_val = NULL;
    if(list != NULL && item != NULL)
    {
        char * position  = strstr(list,item);
        if (position != NULL)
        {
            offset = position-list - 1;

            return_val = list + offset;
        } 
    } 
    return return_val;
}

char * get_sword_from_list(char * list)
{
    return get_item_from_list(list,"] - a sword");
}

char * get_special_from_list(char * list)
{
    return get_item_from_list(list,"] - a special item");
}


char * get_doge_from_list(char * list)
{
    return get_item_from_list(list,"] - a small doge");
}


void sell_sword()
{
    char * game_msg = NULL; 
    char * resp = NULL;

    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);

    if (at_location(game_msg,"Location: local village"))
    {
        _say("2\n",2);
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "While exploring, you found something!") != NULL)
        {
            _say("0\n",2);            
        }
        
        _read_all_from_stdout_pipe(&game_msg);

        _say("a\n",2);

        _read_all_from_stdout_pipe(&game_msg);
        if( strstr(game_msg, "You have no items.") == NULL)
        {
            resp = get_sword_from_list(game_msg);
            if (resp != NULL)
            {
                _say( resp,1);
                _say( "\n",1);
            } else
            {
                _say( "0\n",2);
            }
        }

        _say("b\n",2);
        free(game_msg);
    }

}


uint8_t do_cowardly_combat()
{
    char * game_msg = NULL; 
    uint8_t alive = 1;
    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);
    _debug_msg("traveling to do cowardly combat\n");
    if (not_at_location(game_msg,"Location: local village"))
    {
        _say("2\n",2);
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "you found something!") != NULL)
        {
            _debug_msg("found item\n");
            _say("0\n",2);
            _read_all_from_stdout_pipe(&game_msg);
        }
        if (strstr(game_msg, "You have encountered") != NULL)
        {
            _debug_msg("found enemy, entering combat\n");

            _say(get_item_from_list(game_msg,"] - run away"),1);
            _say("\n",1);
            _read_all_from_stdout_pipe(&game_msg);
            if (strstr(game_msg, "You successfully ran away") == NULL )
            {
                _debug_msg("%s",game_msg);
                alive = 0;
            }

        } else
        {
            _debug_msg("did not encounter enemy\n");
            free(game_msg) ; game_msg = NULL;
            alive = do_cowardly_combat();
        }
        
    }
    free(game_msg);
    return alive;
}


// requires sword
uint8_t do_combat()
{
    char * game_msg = NULL; 
    uint8_t alive = 1;
    uint8_t in_combat = 1;
    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);
    _debug_msg("traveling to do combat\n");
    if (not_at_location(game_msg,"Location: local village"))
    {
        _say("2\n",2);
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "you found something!") != NULL)
        {
            _debug_msg("found item\n");
            _say("0\n",2);
            _read_all_from_stdout_pipe(&game_msg);
        }
        if (strstr(game_msg, "You have encountered") != NULL)
        {
            _debug_msg("found enemy, entering combat\n");
            while (in_combat == 1)
            {
                _say(get_item_from_list(game_msg,"] - use item"),1);
                _say("\n",1);
                _read_all_from_stdout_pipe(&game_msg);
                if (strstr(game_msg, "You do not have any items.") == NULL && strstr(game_msg, "sword") != NULL)
                {
                    _say(get_item_from_list(game_msg,"] - a sword"),1);
                    _say("\n",1);     
                    _read_all_from_stdout_pipe(&game_msg);
                }
                
                if(game_msg != NULL)
                {
                    if(strstr(game_msg, "has killed you") != NULL)
                    {
                        _debug_msg("Died in combat\n");
                        in_combat = 0;
                        alive = 0;
                    } else if (strstr(game_msg, "You have slain") != NULL)
                    {
                        _debug_msg("Won in combat:\n%s\n",game_msg);
                        in_combat = 0;
                    } else
                    {
                        _debug_msg("round results:\n%s\n",game_msg);
                    }
                } else
                {
                    _debug_msg("No game message;\n");
                    in_combat = 0;
                }
                
                
            }
        } else
        {
            _debug_msg("did not encounter enemy\n");
            free(game_msg) ; game_msg = NULL;
            alive = do_combat();
        }
        
    }
    free(game_msg);
    return alive;
}


uint8_t check_if_alive(char * msg)
{
    uint8_t ret_val = 1;
    if (msg != NULL && strstr(msg,"You have died"))
    {
        _debug_msg("Died while traveling:\n%s",msg);
        ret_val = 0;
    }
    return ret_val;
}

/**
 * @brief travel to location indicated, or to any location except the one indicated if inverse_match is set
 * 
 * @param location  null terminated string of location
 * @param inverse_match when set to 1, inverse match location (any location except one indicated)
 * @return uint8_t returns 1 on success, or 0 on death while traveling
 */
uint8_t travel_to(char * location, uint8_t inverse_match)
{
    
    char * game_msg = NULL; 
    uint8_t return_val = 1;
    char * substr = NULL;
    char * first_response = NULL;
    char * second_response = NULL;
    _read_all_from_stdout_pipe(&game_msg);
    first_response = game_msg;
    return_val = check_if_alive(game_msg) & return_val;
    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);
    second_response = game_msg;
    return_val = check_if_alive(game_msg) & return_val;
    if (game_msg != NULL)
    {
        substr = strstr(game_msg, location);
    }
    
    if ( 
         (substr == NULL && inverse_match == 0 ) ||
         (substr != NULL && inverse_match == 1 )
       )
    {
        _debug_msg("need to travel\n");
        _say("1\n",2);
        _read_all_from_stdout_pipe(&game_msg);
        return_val = check_if_alive(game_msg) & return_val;
        if(first_response == NULL && second_response == NULL && game_msg == NULL) // remote end of pipe has stopped responding
        {
            return_val = 0;
        } else if (game_msg != NULL && return_val != 0)
        {
            return_val = travel_to(location,inverse_match);
        }
    } else 
    {
         _debug_msg("already at destination\n");
    }
    free(game_msg), game_msg = NULL;
    return return_val; 
}

void get_sword_from_village()
{

    char * game_msg = NULL; 
    uint8_t found_item = 0;
    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);

    if (at_location(game_msg,"Location: local village"))
    {
        _say("2\n",2);
        free(game_msg); game_msg = NULL;
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "While exploring, you found something!") != NULL)
        {
            
            if ( strstr(game_msg, "sword") == NULL )
            {
                _say("0\n",2);
                free(game_msg);
                game_msg = NULL;
                
            } else 
            {   
                _say("1\n",2);
                found_item = 1;
            }
            
        }
        _say("b\n",2);
        free(game_msg);
        game_msg = NULL;
        if (found_item != 1)
        {
            get_sword_from_village();
        }
    }
    free(game_msg);
}

void buy_special_item_from_village()
{

    char * game_msg = NULL; 
    uint8_t found_item = 0;
    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);

    if (at_location(game_msg,"Location: local village"))
    {
        _say("2\n",2);
        free(game_msg); game_msg = NULL;
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "While exploring, you found something!") != NULL)
        {
                _say("0\n",2);          
        }
        _read_all_from_stdout_pipe(&game_msg);
        _say("9\n",2);
        _say("b\n",2);
        free(game_msg);
        if (found_item != 1)
        {
            get_sword_from_village();
        }
    }
      
}


void get_doge_from_village()
{
    char * game_msg = NULL; 
    uint8_t found_doge = 0;

    _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);

    if (strstr(game_msg, "Location: local village") != NULL)
    {
        _say("2\n",2);
        free(game_msg); game_msg = NULL;
        _read_all_from_stdout_pipe(&game_msg);
        if (strstr(game_msg, "While exploring, you found something!") != NULL)
        {
            if ( strstr(game_msg, "a small doge") == NULL )
            {
                
                _say("0\n",2);
                
            } else 
            {   
                _say("1\n",2);
                _read_all_from_stdout_pipe(&game_msg);
                found_doge = 1;
                _say("my doge <3\n",11);
            }
            
        }
        _say("b\n",2);
        free(game_msg);
        if (found_doge != 1)
        {
            get_doge_from_village();
        }
    }
      
}

uint32_t get_coins()
{
    char * game_msg = NULL;
    char * coins_str = NULL;
    uint32_t coins = 0xFFFF; 
     _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);
    if (game_msg != NULL)
    {
        coins_str = strstr(game_msg,"Coins: ");
    }
    if (NULL != coins_str)
    {
        sscanf(coins_str,"Coins: %d", &coins);
    }
    _debug_msg("coins: %d\n",coins);
    free(game_msg);
    return coins;
}


uint32_t get_health()
{
    char * game_msg = NULL;
    char * health_str = NULL;
    uint32_t health = 0xFFFFFF; 
     _say("6\n",2);
    _read_all_from_stdout_pipe(&game_msg);
    if(game_msg != NULL)
    {
        health_str = strstr(game_msg,"Health: ");
    }
    
    if (NULL != health_str)
    {
        sscanf(health_str,"Health: %d", &health);
    }
    if (health % 1000 == 0)
    {
        _debug_msg("Health: %d\n",health);
    }
    
    free(game_msg);
    return health;
}

void use_special_item()
{
    char * game_msg = NULL; 
    char * item = NULL;

    _say("4\n",2);
    _read_all_from_stdout_pipe(&game_msg);

    if (game_msg != NULL && strstr(game_msg,"Please make a selection") != NULL)
    {
        item = get_special_from_list(game_msg);

        if (item != NULL)
        {
            _say( item,1);
            _say( "\n",1);
        }
        else
        {
            _say( "0\n",2);
        }
    }   
    free(game_msg);
}

void pet_the_doge()
{
    char * game_msg = NULL; 
    char * item = NULL;

    _say("4\n",2);
   

    _read_all_from_stdout_pipe(&game_msg);
    item = get_doge_from_list(game_msg);

    if(item != NULL)
    {
        _say( item,1);
        _say( "\n",1);
    } else {
        _debug_msg("no response to item selection. may get stuck in loop");
    }


    free(game_msg);
}

uint8_t check_win_game()
{
    check_game_win_executed = 1;
    char * game_msg = NULL; 
    uint8_t return_val = 1;
    char * win_msg = NULL;
    _read_all_from_stdout_pipe(&game_msg);
    if (game_msg != NULL)
    {
        win_msg = strstr(game_msg,"You win the game!");
    }
    CU_ASSERT_PTR_NOT_NULL(win_msg);
    if (win_msg)
    {
        _debug_msg("Successfully won game.\n");
        return_val = 0;
    }
    free(game_msg);
    return return_val;
}

void jqr_test_play_quit_game(void)
{
    print_test_name();
    pthread_t * game_thread = _setup_game();
    
    char * game_msg = NULL; 

    _read_all_from_stdout_pipe(&game_msg);
    _say("0\n",2);
    _teardown_game(game_thread);
    free(game_msg);
    CU_PASS("Successfully played and quit.");
}

void jqr_test_play_full_game(void)
{   
    print_test_name();
    pthread_t * game_thread = _setup_game();
    char * game_msg = NULL; 

    

    uint8_t game_active = 1;
    get_doge_from_village();
    get_sword_from_village();
    sell_sword();
    _debug_msg("Getting coins\n");
    while (get_coins() < 2500)
    {
        get_sword_from_village();
        sell_sword();
    }
    _debug_msg("Accumulated coins: %d\n",get_coins());
    _debug_msg("Petting the doge for good health.\n");
    while (get_health() < 10000)
    {
        pet_the_doge();
    }
    _debug_msg("Accumulated health: %d\n",get_health());
    buy_special_item_from_village();

    get_sword_from_village();

    _debug_msg("leaving village\n");
    if (1 == travel_to("local village", 1))
    {
        _debug_msg("doing battle\n");
        if (1 == do_combat())
        {
            use_special_item();
            game_active = check_win_game();
        } else{
            game_active = 0;
        }
    } else {
        game_active = 0;
    }
    CU_ASSERT_EQUAL(check_game_win_executed,1);

    if(game_active){
        _debug_msg("sending quit chars\n");
        _say("0\n",2);
    }
    _read_all_from_stdout_pipe(&game_msg);
    _teardown_game(game_thread);
    free(game_msg);game_msg =NULL;
}


void jqr_test_travel_until_death(void)
{
    print_test_name();
    char * game_msg = NULL; 

    //these helper functions are not very memory safe.
    pthread_t * game_thread = _setup_game();
    usleep(1000);
    _read_all_from_stdout_pipe(&game_msg);
    _debug_msg(game_msg);
    uint32_t original_inverse_walking_risk = inverse_walking_risk;
    inverse_walking_risk = 2;
    _say("6\n",2);
    uint8_t alive = 1;
    while (alive == 1)
    {
        alive = travel_to("local village", 1);
        if (alive)
        {
            alive = travel_to("local village", 0);
        }
    }
    CU_ASSERT_EQUAL(alive, 0);
    inverse_walking_risk = original_inverse_walking_risk;
    _read_all_from_stdout_pipe(&game_msg);
    _teardown_game(game_thread);
    free(game_msg);game_msg =NULL;
}
void jqr_test_flee_until_death(void)
{
    print_test_name();
    char * game_msg = NULL;
    pthread_t * game_thread = _setup_game();
    usleep(1000);
    uint8_t alive = travel_to("local village", 1);
    while (alive == 1)
    {
        alive = do_cowardly_combat();
    }
    CU_ASSERT_EQUAL(alive, 0);
    _read_all_from_stdout_pipe(&game_msg);
    _teardown_game(game_thread);
    free(game_msg);game_msg =NULL;
    return;
}
void jqr_test_fight_until_death(void)
{
    print_test_name();
    char * game_msg = NULL;
    pthread_t * game_thread = _setup_game();
    usleep(1000);
    uint8_t alive = travel_to("local village", 1);
    while (alive == 1)
    {
        alive = do_combat();
    }
    CU_ASSERT_EQUAL(alive, 0);
    _read_all_from_stdout_pipe(&game_msg);
    _teardown_game(game_thread);
    free(game_msg);game_msg =NULL;
}
void jqr_test_use_after_free(void)
{
    print_test_name();
    char * game_msg = NULL;
    pthread_t * game_thread = _setup_game();
    usleep(1000);
    
    get_sword_from_village();

    _say("5\n0\n",4); // drop sword

    get_doge_from_village();
    _say("3\n",2); // show inventory, reading from freed memory

    

    _read_all_from_stdout_pipe(&game_msg);
    _say("0\n",2);
    _debug_msg(game_msg);
    _teardown_game(game_thread);
    CU_PASS("Successfully attempted use after free trigger");
    printf("\n");
    free(game_msg);game_msg =NULL;
}