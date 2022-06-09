/*
Tittle:restaurant_management
Date:2021/05/19 --
Author:ËïÆÕ»ª ¹ùÌìå· ÖÜæÃ
*/

#include "restaurant.h"
#include <stdio.h>
#include <assert.h>
#include <windows.h>


int main()
{
    extern Restaurant restaurant;
    assert(init() == 0);
    int op = UI_user_type();
    switch (op)
    {
        case 1:
            UI_waiters();
            break;
        case 2:
            UI_administrators();
            break;
        default:
            printf("\tµÇÂ¼Ê§°Ü\n");
            break;
    }
    return 0;
}
