/*
Tittle:restaurant_management
Date:2021/05/19 --
Author:���ջ� ����� ����
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
            printf("\t��¼ʧ��\n");
            break;
    }
    return 0;
}
