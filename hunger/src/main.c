#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "hunger.h"

#define MAX_HUNGER  2000
#define FOOD_AMOUNT 42

int main(void) 
{
    time_t current_time = time(NULL);
    time_t prev_time = current_time;

    srand(current_time % 3600 );

    printf("Hunger Management System\n");
    // Initialize hunger structure
    Hunger hunger;

    hunger_init(&hunger, MAX_HUNGER);
    hunger_display(&hunger);
    // Simulate hunger increase over time
    for (int iter = 0; iter < 250; iter++)
    {
        if (hunger_is_full(&hunger))
        {
            printf("Hunger is full!\n");
            break; // Exit loop if hunger is full
        }

        // Simulate time passing and increase hunger
        if (0u == (iter % (FOOD_AMOUNT >> 1)))
        {
            // Eat food to decrease hunger level
            hunger_eat(&hunger, (rand() % FOOD_AMOUNT) + 1);
        }
        else
        {
            // Simulate hunger increase over time
            hunger_simulate(&hunger, 1);
        }

        printf("%04d : ", iter);
        hunger_display(&hunger);

        while (prev_time == current_time)
        {
            current_time = time(NULL);
        }
        prev_time = current_time;
    }
    return 0;
}
