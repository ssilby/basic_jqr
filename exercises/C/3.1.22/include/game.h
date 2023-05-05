#ifndef JQR_3122_GAME_H
#define JQR_3122_GAME_H

/*

   JQR Items Covered:

       3.1.22:
      title: 'DDemonstrate skill in controlling memory:'
      condition: C
      standard: B
      sub_items:
        3.1.22.a: Identify memory leaks
        3.1.22.b: Remove identified memory leaks
        3.1.22.c: Make effective use of valgrind with --leak-check=full to
                  identify and correct memory leaks and context errors

 */


/**
 * @brief play_game triggers the game for this exercise. Fix its source file so there are no leaks when it is called.
 * 
 */

void * play_game(void *arg);

extern volatile uint32_t inverse_walking_risk;

#endif
