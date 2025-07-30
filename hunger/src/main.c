#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int main(void) 
{
    // Initialize hunger level
    int hunger = 100;

    // Simulate eating food
    while (hunger > 0)
    {
        printf("Hunger level: %d\n", hunger);
        hunger -= 10; // Decrease hunger by 10 each iteration
    }

    printf("You are no longer hungry!\n");
    return 0;
}
