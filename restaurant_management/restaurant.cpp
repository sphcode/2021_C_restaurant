#ifndef __CORE_H__
#define __CORE_H__

#include "restaurant.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <Windows.h>

Restaurant restaurant;

//初始化 返回0代表正常运行
int init()
{
    //设置控制台前/背景色
    //system("COLOR F0");
    //设置控制台大小
    //system("mode con cols = 50 lines = 30");

    //分配指针
    restaurant.food_list = (Fnode*)malloc(sizeof(Fnode));
    restaurant.food_list->next = NULL;
    restaurant.waiter_list = (Wnode*)malloc(sizeof(Wnode));
    restaurant.waiter_list->next = NULL;
    for (int i = 0; i < HASHMO; i++)
    {
        restaurant.customer_list[i] = (Cnode*)malloc(sizeof(Cnode));
        restaurant.customer_list[i]->next = NULL;
    }
    restaurant.order_list = (Onode*)malloc(sizeof(Onode));
    restaurant.order_list->next = NULL;
    restaurant.nowcustomer = NULL;
    restaurant.nowwaiter = NULL;

   //载入营业信息
    load_trade();

    //载入菜品信息
    load_food_info();

    //载入用户数据
    load_customer_info();

    //载入服务员
    load_waiter_info();

    //载入管理员密码
    load_admin_passwd();

    //载入订单信息
    load_order_info();

    //载入订单队列
    load_queue_info();

    //载入座位图信息
    load_seat();

    return 0;
}

//读取餐厅营业情况

void load_trade()
{
    FILE* fp = fopen("trade.data", "r");
    fscanf(fp, "%d %d", &restaurant.income, &restaurant.total_turnover);
    fclose(fp);
}

//写入餐厅营业情况
void write_trade()
{
    FILE* fp = fopen("trade.data", "w");
    fprintf(fp, "%d %d", restaurant.income, restaurant.total_turnover);
    fclose(fp);
}

//餐厅营业情况归零
void zero_trad()
{
    restaurant.income = restaurant.total_turnover = 0;
    write_trade();
}

//读取菜品信息
/*
菜品格式
编号        菜名        价格        菜品销售次数
eg.
   1        鱼香肉丝       20        1
*/
void load_food_info()
{
    FILE* fp = fopen("menu.data", "r");
    if (!fp)
    {
        printf("\"menu.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    Food* ptf = (Food*)malloc(sizeof(Food));
    while (~fscanf(fp, "%d %s %d %d", &ptf->idx, ptf->name, &ptf->price, &ptf->order_cnt))
    {
        add_food(restaurant.food_list, *ptf);
        ++restaurant.num_food;
    }
    free(ptf);
    fclose(fp);
}

//写入菜品信息
void write_food_info()
{
    FILE* fp = fopen("menu.data", "w");
    if (!fp)
    {
        printf("\"menu.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    for (Fnode* i = restaurant.food_list->next; i; i = i->next)
        fprintf(fp, "%d %s %d %d\n", i->food.idx, i->food.name, i->food.price, i->food.order_cnt);
    fclose(fp);
}

//菜品搜索 不带头结点
Fnode* food_search(Fnode* head, int idx)
{
    for (Fnode* i = head; i; i = i->next)
        if (idx == i->food.idx)
            return i;
    return NULL;
}

//增加新菜
Fnode* add_food(Fnode* head, Food food)
{
    Fnode* node = (Fnode*)malloc(sizeof(Fnode));
    Fnode* last = head;

    //赋值
    node->food.idx = food.idx;
    node->food.order_cnt = food.order_cnt;
    node->food.price = food.price;
    strcpy(node->food.name, food.name);

    for (Fnode* i = head->next; i && i->food.idx < food.idx; i = i->next)
        last = i;
    node->next = last->next;
    last->next = node;
    return node;
}

//移除菜品
Fnode* remove_food(Fnode* head, int food_idx)
{
    Fnode* node, * last;
    for (node = head->next, last = head; node; last = node, node = node->next)
    {
        if (node->food.idx == food_idx)
        {
            //printf("菜品删除成功\n");
            last->next = node->next;
            restaurant.num_food--;
            break;
        }
    }
    return head;
}

//读取顾客信息
/*
顾客格式
编号        姓名        积分        余额       是否是VIP   密码
eg.
   1       田果果       150        100         0           111111
*/
void load_customer_info()
{
    FILE* fp = fopen("customer.data", "r");
    if (!fp)
    {
        printf("\"customer.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    Customer* ptc = (Customer *)malloc(sizeof(Customer));
    while (~fscanf(fp, "%d %s %d %lf %d %s", &ptc->idx, ptc->name, &ptc->points, &ptc->balance, &ptc->is_vip, ptc->password))
    {
        add_customer(restaurant.customer_list, *ptc);
        ++restaurant.num_customer;
    }
    free(ptc);
    fclose(fp);
}

//写入用户信息
void write_customer_info()
{
    FILE* fp = fopen("customer.data", "w");
    if (!fp)
    {
        printf("\"customer.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    for (int i = 0; i < HASHMO; i++)
        for (Cnode* j = restaurant.customer_list[i]->next; j; j = j->next)
            fprintf(fp, "%d %s %d %.2lf %d %s\n",
            j->customer.idx, j->customer.name, j->customer.points,
            j->customer.balance, j->customer.is_vip, j->customer.password);
    fclose(fp);
}

//增加顾客
Cnode* add_customer(Cnode** head, Customer customer)
{
    int h = hash(customer.name);
    Cnode* node = (Cnode*)malloc(sizeof(Cnode));
    Cnode* last = head[h];

    //赋值
    node->customer.idx = customer.idx;
    node->customer.is_vip = customer.is_vip;
    node->customer.balance = customer.balance;
    node->customer.points = customer.points;
    strcpy(node->customer.name, customer.name);
    strcpy(node->customer.password, customer.password);

    for (Cnode* i = head[h]->next; i && i->customer.idx < customer.idx; i = i->next)
        last = i;
    node->next = last->next;
    last->next = node;
    return node;
}

//哈希
unsigned long long hash(char* str)
{
    unsigned long long res = 0;
    for (char* i = str; *i; i++)
        res = (res * HASHP + *i) % HASHMO;
    return res;
}

//读取服务员信息
/*
服务员格式
编号        姓名        积分    密码    
eg.
   1       田果果       150    123456
*/
void load_waiter_info()
{
    FILE* fp = fopen("waiter.data", "r");
    if (!fp)
    {
        printf("\"wiator.data\"文件读取失败\n");
        getchar();
        exit(1);
    }
    Waiter* ptw = (Waiter*)malloc(sizeof(Waiter));
    while (~fscanf(fp, "%d %s %s %d", &ptw->idx, ptw->name, ptw->password, &ptw->num_order))
    {
        add_waiter(restaurant.waiter_list, *ptw);
        ++restaurant.num_waiter;
    }
    free(ptw);
    fclose(fp);
}

//写入服务员信息
void write_waiter_info()
{
    FILE* fp = fopen("waiter.data", "w");
    if (!fp)
    {
        printf("\"wiator.data\"文件读取失败\n");
        getchar();
        exit(1);
    }
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next)
        fprintf(fp, "%d %s %s %d\n", i->waiter.idx, i->waiter.name, i->waiter.password, i->waiter.num_order);
    fclose(fp);
}

//职员入职
Wnode* add_waiter(Wnode* head, Waiter waiter)
{
    Wnode* node = (Wnode*)malloc(sizeof(Wnode));
    Wnode* last = head;
    //赋值
    node->waiter.idx = waiter.idx;
    node->waiter.num_order = waiter.num_order;
    strcpy(node->waiter.name, waiter.name);
    strcpy(node->waiter.password, waiter.password);

    for (Wnode* i = head->next; i && i->waiter.idx < waiter.idx; i = i->next)
        last = i;
    node->next = last->next;
    last->next = node;
    return node;
}

//职员离职
Wnode* remove_waiter(Wnode* head, int waiter_idx)
{
    Wnode* node, * last;
    for (node = head->next, last = head; node; last = node, node = node->next)
    {
        if (node->waiter.idx == waiter_idx)
        {
            //printf("职员删除成功\n");
            last->next = node->next;
            break;
        }
    }
    if (!node)
        printf("没有查询到该ID的职员\n");
    return head;
}

//读取管理员密码
void load_admin_passwd()
{
    FILE* fp;
    fp = fopen("admin.data", "r");
    if (!fp)
    {
        printf("\"admin.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    fscanf(fp, "%s", restaurant.admin_password);
    fclose(fp);
}

//写入管理员密码
void write_admin_passwd()
{
    FILE* fp;
    fp = fopen("admin.data", "w");
    if (!fp)
    {
        printf("\"admin.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    fprintf(fp, "%s", restaurant.admin_password);
    fclose(fp);
}

//读取订单信息
/*
订单格式
顾客姓名 订单号 是否外卖 菜品个数 总花费
菜品1ID 菜品2ID...
eg.
田果果     1       0       2       30
1 3
*/
void load_order_info()
{
    FILE* fp = fopen("order.data", "r");
    if (!fp)
    {
        printf("\"order.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    Onode* pto = (Onode*)malloc(sizeof(Onode));
    char name[USERNAME_MAX + 1];
    while (~fscanf(fp, "%s %d %d %d %d", name, &pto->order.idx,
        &pto->order.is_takeaway, &pto->order.food_num, &pto->order.totalcost))
    {
        pto->order.bill.next = NULL;
        Cnode* ptc = customer_search(name);
        pto->order.customer = ptc;
        Fnode* ptf = restaurant.food_list->next;
        for (int i = 0; i < pto->order.food_num; i++)
        {
            int idx;
            fscanf(fp, "%d", &idx);
            ptf = food_search(ptf, idx);
            add_food(&pto->order.bill, ptf->food);
        }
        add_order(restaurant.order_list, pto->order);
        ++restaurant.num_orders;
    }
    free(pto);
    fclose(fp);
}

//写入订单信息
void write_order_info()
{
    FILE* fp = fopen("order.data", "w");
    if (!fp)
    {
        printf("\"order.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    for (Onode * i = restaurant.order_list->next; i; i = i->next)
    { 
        fprintf(fp, "%s %d %d %d %d\n", i->order.customer->customer.name, i->order.idx, i->order.is_takeaway, i->order.food_num, i->order.totalcost);
        for (Fnode * j = i->order.bill.next; j; j = j->next)
            fprintf(fp, "%d ", j->food.idx);
        fprintf(fp, "\n");
    }
    fclose(fp);
}


//读取订单队列
/*
订单队列格式
(一个正整数数列)
eg.
100 101 123 111 233...
*/
void load_queue_info()
{
    restaurant.qtt = -1;
    restaurant.qhh = 0;
    FILE* fp;
    fp = fopen("queue.data", "r");
    int idx;
    while (~fscanf(fp, "%d", &idx))
        order_push(idx);
    fclose(fp);
}

//增加订单
Onode* add_order(Onode* head, Order order)
{
    Onode* node = (Onode*)malloc(sizeof(Onode));
    Onode* last = head;

    //赋值
    node->order.idx = order.idx;
    node->order.bill = order.bill;
    node->order.customer = order.customer;
    node->order.is_takeaway = order.is_takeaway;
    node->order.food_num = order.food_num;
    node->order.totalcost = order.totalcost;

    for (Onode* i = head->next; i && i->order.idx < order.idx; i = i->next)
        last = i;
    node->next = last->next;
    last->next = node;
    return node;
}

//移除订单
Onode* remove_order(Onode* head, int idx)
{
    Onode* node, * last;
    for (node = head->next, last = head; node; last = node, node = node->next)
    {
        if (node->order.idx == idx)
        {
            last->next = node->next;
            break;
        }
    }

    return head;
}

//写入订单队列
void write_queue_info()
{
    FILE* fp;
    fp = fopen("queue.data", "w");
    if (!fp)
    {
        printf("\"queue.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    for (int i = restaurant.qhh; i != restaurant.qtt; i = (i + 1) % QUEUESIZE)
        fprintf(fp, "%d ", restaurant.que[i]);
    fclose(fp);
}

//插入订单
void order_push(int idx)
{
    if (restaurant.qtt != -1 && restaurant.qhh == (restaurant.qtt + 1) % QUEUESIZE)
    {
        printf("队列空间不足!");
        exit(1);
    }
    restaurant.qtt = (restaurant.qtt + 1) % QUEUESIZE;
    restaurant.que[restaurant.qtt] = idx;
}

//弹出订单
int order_pop()
{
    int res = restaurant.que[restaurant.qhh++];
    (restaurant.qhh == QUEUESIZE) && (restaurant.qhh = 0);

    return res;
}

//读取座位信息
/*
座位图样例 第一列代表行列数和剩余座位数 ?代表没人 
2 3 3
"田果果" "?" "?"
"?" "金亨亨" "闵其其"
*/
void load_seat()
{
    FILE* fp = fopen("seatmap.data", "r");
    if (!fp)
    {
        printf("\"seatmap.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    fscanf(fp, "%d %d %d", &restaurant.seat_row, &restaurant.seat_col, &restaurant.rest_seats);
    restaurant.is_full = restaurant.rest_seats ? 0 : 1;
    for (int i = 1; i <= restaurant.seat_row; i++)
        for (int j = 1; j <= restaurant.seat_col; j++)
        {
            char temp[USERNAME_MAX + 1];
            fscanf(fp, "%s", temp);
            strcpy(restaurant.seats[i][j], temp);
        }
    fclose(fp);
}

//写入座位信息
void write_seat()
{
    FILE* fp = fopen("seatmap.data", "w");
    if (!fp)
    {
        printf("\"seatmap.data\"文件读取失败 按任意键退出\n");
        getchar();
        exit(1);
    }
    fprintf(fp, "%d %d %d\n", restaurant.seat_row, restaurant.seat_col, restaurant.rest_seats);
    restaurant.is_full = restaurant.rest_seats ? 0 : 1;
    for (int i = 1; i <= restaurant.seat_row; i++)
    {
        for (int j = 1; j <= restaurant.seat_col; j++)
            fprintf(fp, "%s ", restaurant.seats[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

//获取食物ID
int get_food_id()
{
    int id = 0;
    if (restaurant.num_food == 0)
    {
        id = 1;
    }
    else
    {
        if (restaurant.food_list->next->food.idx > 1)//如果第一个元素id不是1
        {
            id = 1;//生成一个1
        }
        else
        {
            Fnode* p;
            for (p = restaurant.food_list->next; p; p = p->next)
            {
                if (p->next == NULL)//如果到了最后一个元素
                {
                    id = 1 + p->food.idx;//生成比最后一个元素id大1的id
                    break;
                }
                else
                {
                    if ((p->next->food.idx - p->food.idx) > 1)//如果某两个元素的id不是连续的
                    {
                        id = 1 + p->food.idx;//生成比前一个元素id大1的id
                        break;
                    }
                }
            }
        }
    }
    if (!id)
    {
        printf("获取职员ID时发生错误，按任意键退出\n");
        getchar();
        exit(1);
    }

    return id;
}

//获取职员ID
int get_waiter_id()
{
    int id = 0;
    if (restaurant.num_waiter == 0)
    {
        id = 1;
    }
    else
    {
        if (restaurant.waiter_list->next->waiter.idx > 1)//如果第一个元素id不是1
        {
            id = 1;//生成一个1
        }
        else
        {
            Wnode* p;
            for (p = restaurant.waiter_list->next; p; p = p->next)
            {
                if (p->next == NULL)//如果到了最后一个元素
                {
                    id = 1 + p->waiter.idx;//生成比最后一个元素id大1的id
                    break;
                }
                else
                {
                    if ((p->next->waiter.idx - p->waiter.idx) > 1)//如果某两个元素的id不是连续的
                    {
                        id = 1 + p->waiter.idx;//生成比前一个元素id大1的id
                        break;
                    }
                }
            }
        }
    }
    if (!id)
    {
        printf("获取职员ID时发生错误，按任意键退出\n");
        getchar();
        exit(1);
    }

    return id;
}

//获取顾客ID
int get_customer_id()
{
    return restaurant.num_customer + 1;
}

//获取订单ID
int get_order_id()
{
    return order_pop();
}

//打印食物链表
void print_linked_list_f(Fnode* head)
{
    for (Fnode* p = head->next; p; p = p->next)
        printf("%d\t %-15s\t%-4d 元\n", p->food.idx, p->food.name, p->food.price);
}

//打印服务员链表
void print_linked_list_w(Wnode* head)
{
    for (Wnode* p = head->next; p; p = p->next)
        printf("%d\t %-15s\t%-4d\n", p->waiter.idx, p->waiter.name, p->waiter.num_order);
}

//清除缓存区
void clear_screen_buffer(void)
{
    char c;
    while ((c = getchar()) != EOF && c != '\n');
}

//用户类型选择 返回1为服务员 返回2为管理员
int UI_user_type()
{
    int op;
    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("\t请选择您的身份\n\n");
        printf("\t1:服务员\n\n");
        printf("\t2:管理员\n\n");
        printf("请输入你的身份对应的序号, 按0退出: [ ]\b\b");
        scanf("%d", &op);
        clear_screen_buffer();
        if (op == 1 || op == 2)
        {
            break;
        }
        else if (op == 0)
        {
            exit(0);
        }
    }
    if (op == 1)
    {
        if (UI_waiterpasswd_verify())
            return op;
    }
    else
    {
        if (UI_adminpasswd_verify())
            return op;
    }

    return 0;
}

//管理员密码验证 返回1验证通过
int UI_adminpasswd_verify()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("请输入管理员密码以便验证你的身份\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH + 1];//密码
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //密码不匹配
        if (strcmp(password_input, restaurant.admin_password))
        {
            printf("密码不正确 !\n");
            printf("请重新输入密码, 如果你忘记了密码，请输入#退出\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //密码匹配
        else
            return 1;
    }
    return 0;
}

//管理员界面
void UI_administrators()
{
    while (1)
    {
        system("cls");
        int op = 0;
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("\n");
        printf("\t1) 菜品管理\n\n");
        printf("\t2) 服务员管理\n\n");
        printf("\t3) 座位管理\n\n");
        printf("\t4) 餐厅总览\n\n");
        printf("\t5) 数据分析\n\n");
        printf("\t6) 管理员密码修改\n\n");
        printf("\t7) 服务员密码修改\n\n");
        printf("\t8) 退出系统\n\n");
        while (!(op >= 1 && op <= 8))
        {
            printf("请输入操作对应的序号: [ ]\b\b");
            scanf("%d", &op);
            clear_screen_buffer();
        }
        if (op == 8)
        {
            system("cls");
            break;
        }
        switch (op)
        {
            case 1:
                UI_food_management();
                break;
            case 2:
                UI_waiter_management();
                break;
            case 3:
                UI_seat_management();
                break;
            case 4:
                UI_restaurant_overview();
                break;
            case 5:
                UI_data_analyse();
                break;
            case 6:
                UI_adminpasswd_change();
                break;
            case 7:
                UI_waiterpasswd_change();
                break;
        }
    }
}


//菜品管理
void UI_food_management()
{
    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("序号\t 菜品名\t                单价\n");
        printf("=======================================\n\n");
        print_linked_list_f(restaurant.food_list);
        printf("=======================================\n\n");
        printf("输入 \"add [菜品名] [单价]\" 添加菜品\n");
        printf("输入 \"del [序号]\" 删除菜品\n");
        printf("输入 \"quit\" 返回上一级菜单\n\n");
        printf("@admin > ");
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "add"))
        {
            //接受食物数据
            char name[FOODNAME_MAX + 1];
            int price, idx;
            scanf("%s", name);
            scanf("%d", &price);
            clear_screen_buffer();
            idx = get_food_id();

            //构造食物结构体
            Food newfood;
            newfood.idx = idx;
            strcpy(newfood.name, name);
            newfood.price = price;
            newfood.order_cnt = 0;

            //更新食物数据到系统并且存盘
            add_food(restaurant.food_list, newfood);
            ++restaurant.num_food;
            write_food_info();
        }
        else if (!strcmp(cmd, "del"))
        {
            int idx;
            scanf(" %d", &idx);
            clear_screen_buffer();
            remove_food(restaurant.food_list, idx);
            --restaurant.num_food;
            write_food_info();
        }
        else if (strcmp(cmd, "quit") == 0)
        {
            system("cls");
            break;
        }
        else
        {
            clear_screen_buffer();
            printf("无法识别的命令");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//服务员管理
void UI_waiter_management()
{
    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("序号\t 姓名\t                完成订单数\n");
        printf("=======================================\n\n");
        print_linked_list_w(restaurant.waiter_list);
        printf("=======================================\n\n");
        printf("输入 \"add [姓名]\" 添加账号\n");
        printf("输入 \"del [序号]\" 删除账号\n");
        printf("输入 \"quit\" 返回上一级菜单\n\n");
        printf("@admin > ");
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "add"))
        {
            //接受服务员数据
            char name[30];
            int price, idx;
            if (scanf("%s", name) == 1)
            {
                idx = get_waiter_id();

                //构造服务员结构体
                Waiter newwaiter;
                newwaiter.idx = idx;
                strcpy(newwaiter.name, name);
                strcpy(newwaiter.password, "123456"); //默认密码
                newwaiter.num_order = 0;

                //更新服务员数据到系统并且存盘
                add_waiter(restaurant.waiter_list, newwaiter);
                ++restaurant.num_waiter;
                write_waiter_info();
            }
            clear_screen_buffer();
        }
        else if (!strcmp(cmd, "del"))
        {
            int idx;
            if (scanf("%d", &idx) == 1)
            {
                remove_waiter(restaurant.waiter_list, idx);
                --restaurant.num_waiter;
                write_waiter_info();
                clear_screen_buffer();
            }
        }
        else if (strcmp(cmd, "quit") == 0)
        {
            clear_screen_buffer();
            system("cls");
            break;
        }
        else
        {
            clear_screen_buffer();
            printf("无法识别的命令\n");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//座位管理
void UI_seat_management()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    print_seat_map(0, 0);
    if (restaurant.rest_seats != restaurant.seat_col * restaurant.seat_row)
    {
        printf("当前有顾客在用餐，不能调整座位\n");
        printf("按任意键返回");
        getchar();
    }
    else
    {
        printf("当前餐厅座位行数=%3d, 座位列数=%3d\n", restaurant.seat_row, restaurant.seat_col);
        printf("输入\"[行数] [列数]\"调整座位 任意为0代表退出上一级菜单\n");
        while (1)
        {
            int n, m;
            if (scanf("%d %d", &n, &m) == 2)
            {
                if (n == 0 || m == 0)
                    return;
                restaurant.seat_row = n;
                restaurant.seat_col = m;
                for (int i = 1; i <= restaurant.seat_row; i++)
                    for (int j = 1; j <= restaurant.seat_col; j++)
                        strcpy(restaurant.seats[i][j], "?");
                restaurant.rest_seats = n * m;
                write_seat();
                printf("更新成功\n");
                Sleep(1500);
                break;
            }
            else
            {
                printf("输入\"[行数] [列数]\"调整座位\n");
            }
        }
        clear_screen_buffer();
    }
    write_seat();
}

//服务员查找
Wnode* waiter_search(int idx)
{
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next)
        if (idx == i->waiter.idx)
            return i;
    return NULL;
}

//餐厅总览
void UI_restaurant_overview()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("当前餐厅有\n");
    printf("\t%d个菜品\n", restaurant.num_food);
    printf("\t%d个服务员\n", restaurant.num_waiter);
    printf("\t%d个注册用户\n", restaurant.num_customer);
    printf("\t%d个用户在用餐\n", restaurant.seat_col * restaurant.seat_row - restaurant.rest_seats);
    puts("");
    printf("今日共完成%d个订单 总收入%d元\n", restaurant.total_turnover, restaurant.income);
    puts("");
    printf("按任意键返回\n");
    getchar();
}

//消费最高
void UI_cosumeption_list(int n)
{
    Cnode** stack = (Cnode**)calloc(n, sizeof(Cnode*));
    //栈顶
    int top = 0;
    for (int i = 0; i < n; i++)
        stack[i] = NULL;
    for (int i = 0; i < HASHMO; i++)
        for (Cnode* j = restaurant.customer_list[i]->next; j; j = j->next)
            if (top != n || stack[n - 1]->customer.points < j->customer.points)
            {
                if (top != n)
                    stack[top++] = j;
                else
                    stack[n - 1] = j;
                //插排
                for (int k = top - 1; k > 0; k--)
                    if (stack[k - 1]->customer.points < stack[k]->customer.points)
                    {
                        Cnode* temp = stack[k];
                        stack[k] = stack[k - 1];
                        stack[k - 1] = temp;
                    }
            }
    printf("\t消费榜\n");
    printf("  消费最多的前%d顾客\n", n);
    printf("姓名\t\t\t消费额\n");
    //这里要用i < top 不能用i < n
    for (int i = 0; i < top; i++)
        printf("%s\t\t\t%d\n", stack[i]->customer.name, stack[i]->customer.points);
    free(stack);
}

//业绩排名
void UI_num_order_list()
{  
    Wnode** array = (Wnode**)calloc(restaurant.num_waiter, sizeof(Wnode*));
    int count = 0;
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next, ++count)
        array[count] = i;
    quick_sort_w(array, 0, restaurant.num_waiter - 1);
    printf("\t业绩榜\n");
    printf("姓名\t\t\t完成订单数\n");
    for (int i = 0; i < restaurant.num_customer; i++)
        printf("%s\t\t\t%d\n", array[i]->waiter.name, array[i]->waiter.num_order);
    free(array);
}

//业绩分析的快排
void quick_sort_w(Wnode* a[], int l, int r)
{
    if (l >= r) return;

    int i = l - 1, j = r + 1, x = a[l + r >> 1]->waiter.num_order;
    while (i < j)
    {
        do i++; while (a[i]->waiter.num_order > x);
        do j--; while (a[j]->waiter.num_order < x);
        if (i < j)
        {
            Wnode* temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    quick_sort_w(a, l, j), quick_sort_w(a, j + 1, r);
}

//最受欢迎的n个菜品
void UI_cnt_food_list(int n)
{
    Fnode** array = (Fnode**)calloc(restaurant.num_food, sizeof(Fnode*));
    int count = 0;
    for (Fnode* i = restaurant.food_list->next; i; i = i->next, ++count)
        array[count] = i;
    quick_sort_f(array, 0, restaurant.num_food - 1);
    printf("  最受欢迎的前%d菜品\n", n);
    printf("菜品名\t\t\t销售量\n");
    for (int i = 0; i < n; i++)
        printf("%s\t\t %d\n", array[i]->food.name, array[i]->food.order_cnt);
}

//菜品分析的快排
void quick_sort_f(Fnode* a[], int l, int r)
{
    if (l >= r) return;

    int i = l - 1, j = r + 1, x = a[l + r >> 1]->food.order_cnt;
    while (i < j)
    {
        do i++; while (a[i]->food.order_cnt > x);
        do j--; while (a[j]->food.order_cnt < x);
        if (i < j)
        {
            Fnode* temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }
    quick_sort_f(a, l, j), quick_sort_f(a, j + 1, r);
}

//数据分析
void UI_data_analyse()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    UI_cosumeption_list(3); //消费最高的n个顾客
    puts("");
    UI_cnt_food_list(5);
    puts("");
    UI_num_order_list();
    puts("");
    printf("按任意键返回\n");
    getchar();
}

//管理员密码修改
void UI_adminpasswd_change()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("请输入新的密码.\n");
    for (int i = 0; i < PASSWORDLENTH; i++)
    {
        restaurant.admin_password[i] = getchar();
        if (!isalnum(restaurant.admin_password[i]))
            --i;
    }
    restaurant.admin_password[PASSWORDLENTH] = 0;
    write_admin_passwd();
    printf("修改成功");
    Sleep(1500);
    system("cls");
}

//服务员密码验证
int UI_waiterpasswd_verify()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("请输入工号\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    int idx;
    Wnode* ptw;
    fscanf(stdin, "%d", &idx);
    clear_screen_buffer();
    while (1)
    {
        if (!(ptw = waiter_search(idx)))
        {
            printf("未查询到该工号\n");
            printf("请重新输入工号\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%d", &idx);
            clear_screen_buffer();
        }
        else
            break;
    }
    printf("请输入密码\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH + 1];//密码
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //密码不匹配
        if (!(strcmp(password_input, ptw->waiter.password) == 0))
        {
            printf("密码不正确 !\n");
            printf("请重新输入密码, 如果你忘记了密码，请输入#退出\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //密码匹配
        else
        {
            restaurant.nowwaiter = ptw;
            return 1;
        }
    }
    return 0;
}

//服务员密码修改
void UI_waiterpasswd_change()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("请输入工号\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    int idx;
    Wnode* ptw;
    fscanf(stdin, "%d", &idx);
    clear_screen_buffer();
    while (1)
    {
        if (!(ptw = waiter_search(idx)))
        {
            printf("未查询到该工号\n");
            printf("请重新输入工号\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%d", &idx);
            clear_screen_buffer();
        }
        else
            break;
    }
    printf("请输入新的密码.\n");
    char newpasswd[PASSWORDLENTH + 1];
    for (int i = 0; i < PASSWORDLENTH; i++)
    {
        newpasswd[i] = getchar();
        if (!isalnum(newpasswd[i]))
            --i;
    }
    clear_screen_buffer();
    newpasswd[PASSWORDLENTH] = 0;
    strcpy(ptw->waiter.password, newpasswd);
    write_waiter_info();
    printf("修改成功！");
    Sleep(1500);
    system("cls");
}

void UI_waiterpasswd_change_own()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("请输入新的密码.\n");
    char newpasswd[PASSWORDLENTH + 1];
    for (int i = 0; i < PASSWORDLENTH; i++)
    {
        newpasswd[i] = getchar();
        if (!isalnum(newpasswd[i]))
            --i;
    }
    newpasswd[PASSWORDLENTH] = 0;
    strcpy(restaurant.nowwaiter->waiter.password, newpasswd);
    write_waiter_info();
    printf("修改成功");
    Sleep(1500);
    system("cls");
}
//服务员界面
void UI_waiters()
{
    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n");
        if (restaurant.nowcustomer)
        {
            printf("%s %s\n", restaurant.nowcustomer->customer.is_vip == 0 ? "尊敬的会员" : "尊敬的VIP会员"
                , restaurant.nowcustomer->customer.name);
            printf("工号%d\t姓名%s 竭诚为你服务\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        }
        else
            printf("工号%d\t姓名%s\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("\n");
        printf("\t1) 顾客登录与注销\n\n");
        printf("\t2) 菜单浏览\n\n");
        printf("\t3) 顾客充值\n\n");
        printf("\t4) 订单管理\n\n");
        printf("\t5) 餐桌信息浏览\n\n");
        printf("\t6) 服务员密码修改\n\n");
        printf("\t7) 退出系统\n\n");
        printf("=======================================\n");
        int op = 0;
        while (!(op >= 1 && op <= 7))
        {
            printf("请输入操作对应的序号: [ ]\b\b");
            fscanf(stdin, "%d", &op);
            clear_screen_buffer();
        }
        if (op == 7)
        {
            system("cls");
            break;
        }
        switch (op)
        {
            case 1:
                UI_signin_logout(restaurant.nowcustomer);
                break;
            case 2:
                UI_menu();
                break;
            case 3:
                UI_customer_dashboard(restaurant.nowcustomer);
                break;
            case 4:
                UI_order_management(restaurant.nowcustomer);
                break;
            case 5:
                UI_seats(restaurant.nowcustomer);
                break;
            case 6:
                UI_waiterpasswd_change_own();
                break;
        }
    }
}

//登录注册退出界面
void UI_signin_logout(Cnode* customer)
{
    int op;
    Cnode* ptc = NULL;
    int flag = 1;
    while (flag == 1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("\t1:返回上一级界面\n\n");
        printf("\t2:用户登录\n\n");
        printf("\t3:用户注册\n\n");
        if (customer)
            printf("\t4:退出当前用户\n\n");
        printf("请输入对应的序号: [ ]\b\b");
        fscanf(stdin, "%d", &op);
        clear_screen_buffer();
        switch (op)
        {
            case 1:
                return;
            case 2:
                if (restaurant.nowcustomer)
                {
                    printf("请先退出当前账号\n");
                    Sleep(1500);
                    break;
                }
                ptc = UI_login();
                if (ptc)
                {
                    flag = 0;
                    restaurant.nowcustomer = ptc;
                    printf("%s %s 登录成功\n", restaurant.nowcustomer->customer.is_vip == 0 ? "尊敬的会员" : "尊敬的VIP会员"
                        , restaurant.nowcustomer->customer.name);
                    Sleep(1500);
                }
                break;
            case 3:
                UI_register();
                restaurant.num_customer++;
                write_customer_info();
                flag = 0;
                break;
            case 4:
                if (restaurant.nowcustomer)
                {
                    flag = 0;
                    restaurant.nowcustomer = ptc = NULL;
                    break;
                }
            default:
                printf("非法输入\n");
                printf("请输入对应的序号: [ ]\b\b");
                break;
        }
    }
}

//顾客搜索
Cnode* customer_search(char* name)
{
    for (Cnode* i = restaurant.customer_list[hash(name)]->next; i; i = i->next)
        if (!strcmp(name, i->customer.name))
            return i;
    return NULL;
}

//顾客登录
Cnode* UI_login()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    Cnode* ptc = NULL;
    while (1)
    {
        char name[USERNAME_MAX + 1];
        printf("请输入会员名\n\n>__________\b\b\b\b\b\b\b\b\b\b");
        assert(fscanf(stdin, "%s", name) == 1);
        clear_screen_buffer();
        ptc = customer_search(name);
        if (!ptc)
        {
            printf("查询不到该会员名");
        }
        else
        {
            break;
        }
    }
    printf("请输入密码\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH * 3];//密码 这里为了安全开大了空间
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //密码不匹配
        if (!(strcmp(password_input, ptc->customer.password) == 0))
        {
            printf("密码不正确 !\n");
            printf("请重新输入密码, 如果你忘记了密码，请输入#退出,按其他键重新输入密码\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //密码匹配
        else
        {
            return ptc;
        }
    }
    return NULL;
}

//顾客注册
void UI_register()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    Customer newcustomer;
    printf("请输入用户名（%d字符以内）\n\n>__________\b\b\b\b\b\b\b\b\b\b", USERNAME_MAX);
    assert(fscanf(stdin, "%s", newcustomer.name) == 1);
    clear_screen_buffer();
    printf("会员卡充值（预充值满500即可成为VIP享受八折服务）\n\n>________\b\b\b\b\b\b\b\b");
    assert(fscanf(stdin, "%lf", &newcustomer.balance) == 1);
    clear_screen_buffer();
    printf("设置六位密码\n\n>________\b\b\b\b\b\b\b\b");
    for (int i = 0; i < PASSWORDLENTH; i++)
    {
        newcustomer.password[i] = getchar();
        if (!isalnum(restaurant.admin_password[i]))
            --i;
    }
    clear_screen_buffer();
    newcustomer.password[PASSWORDLENTH] = 0;
    newcustomer.is_vip = newcustomer.balance >= 500 ? 1 : 0;
    newcustomer.points = 0;
    newcustomer.idx = get_customer_id();
    add_customer(restaurant.customer_list, newcustomer);
    restaurant.num_customer++;
    printf("%s会员注册成功！ 会员ID是%d请牢记\n\n", newcustomer.name, newcustomer.idx);
    printf("按任意键退出\n");
    getchar();
}

//匹配订单
Onode* match_order(Cnode* customer)
{
    //暴力枚举
    for (Onode* i = restaurant.order_list->next; i; i = i->next)
        if (i->order.customer == customer)
            return i;
    return NULL;
}

//订单查找
Onode* order_search(Onode* head, int idx)
{
    for (Onode* i = head->next; i && i->order.idx <= idx; i = i->next)
        if (i->order.idx == idx)
            return i;
    return NULL;
}

//订单管理
void UI_order_management(Cnode* customer)
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    if (restaurant.nowcustomer)
    {
        printf("%s %s\n", restaurant.nowcustomer->customer.is_vip == 0 ? "尊敬的会员" : "尊敬的VIP会员"
            , restaurant.nowcustomer->customer.name);
        printf("工号%d\t姓名%s 竭诚为你服务\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("\t当前共有%d个订单\n\n", restaurant.num_orders);
        print_linked_list_o(restaurant.order_list);
        printf("\n输入 \"more\" [订单号] 查看更多信息\n");
        printf("输入 \"new\" 创建新订单或修改当前订单\n");
        printf("输入 \"pay\" 结账\n");
        printf("输入 \"quit\" 返回上一级菜单\n\n");
    }
    else
    {
        printf("工号%d\t姓名%s\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("当前共有%d个订单\n\n", restaurant.num_orders);
        print_linked_list_o(restaurant.order_list);
        printf("\n输入 \"more\" [订单号] 查看更多信息\n\n");
        printf("输入 \"quit\" 返回上一级菜单\n\n");
    }
    while (1)
    {
        printf("@%s > ", restaurant.nowwaiter->waiter.name);
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "more"))
        {
            int idx;
            if (scanf("%d", &idx) != 1)
            {
                clear_screen_buffer();
                continue;
            }
            clear_screen_buffer();
            UI_show_more_detail(idx);
        }
        else if (!strcmp(cmd, "new") && restaurant.nowcustomer)
        {
            clear_screen_buffer();
            UI_order(restaurant.nowcustomer);
            return;
        }
        else if (!strcmp(cmd, "quit"))
        {
            system("cls");
            clear_screen_buffer();
            return;
        }
        else if (!strcmp(cmd, "pay"))
        {
            clear_screen_buffer();
            UI_pay();
            return;
        }
        else
        {
            clear_screen_buffer();
            printf("无法识别的命令");
            printf("\"%s\"\n", cmd);
            Sleep(1500);
        }
    }
}

//打印订单链表
void print_linked_list_o(Onode* head)
{
    printf("订单号\t\t顾客\t\t消费额\n");
    for (Onode* i = restaurant.order_list->next; i; i = i->next)
    {
        printf("%d\t\t%s\t\t%d\n", i->order.idx, i->order.customer->customer.name, i->order.totalcost);
    }
}

//展示订单更多信息
void UI_show_more_detail(int idx)
{
    Onode* pto = order_search(restaurant.order_list, idx);
    printf("订单号%d\n顾客:%s\n消费额:%d\n", pto->order.idx, pto->order.customer->customer.name, pto->order.totalcost);
    printf("共%d个菜品\n", pto->order.food_num);
    print_linked_list_f(&pto->order.bill);

}

//点菜
void UI_order(Cnode* customer)
{
    int op;
    Onode* order = match_order(customer), *pto = NULL;
    if (order == NULL) //新订单
    {
        order = (Onode*)malloc(sizeof(Onode));
        order->order.customer = customer;
        order->order.bill.next = NULL;
        order->order.food_num = 0;
        order->order.totalcost = 0;
        order->order.idx = get_order_id();
        while (1)
        {
            system("cls");
            printf("############# 山东大学 ############\n\n");
            printf("1:打包带走\n");
            printf("2:到店就餐\n");
            printf("请输入对应的序号: [ ]\b\b");
            fscanf(stdin, "%d", &op);
            clear_screen_buffer();
            if (op == 1 || op == 2)
            {
                break;
            }
            else
            {
                printf("非法输入！\n");
            }
        }
        order->order.is_takeaway = 2 - op; //因为1对应1 2对应0
        pto = add_order(restaurant.order_list, order->order);
        restaurant.num_orders += 1;
    }
    else
    {
        pto = order;
    }

    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("序号\t 菜名\t                单价\n");
        printf("=======================================\n\n");
        print_linked_list_f(restaurant.food_list);
        printf("=======================================\n\n");
        if (pto->order.food_num)
        {
            printf("这是您已经点的菜品\n");
            printf("序号\t 名称\t                单价\n");
        }
        printf("=======================================\n\n");
        print_linked_list_f(&(pto->order.bill));
        printf("\n===========================================\n");
        printf("总菜品数: %2d     应支付金额: %5.2f\n",
            pto->order.food_num, pto->order.totalcost * customer->customer.is_vip ? "0.9" : "1.0");
        printf("=======================================\n\n");
        printf("请输入您想点的菜对应的序号,退菜请输入\"d\" + 序号 \n输入 0 退出返回上一级菜单\n\n");
        printf("菜品序号: [ ]\b\b");
        int idx;
        op = getchar();
        if (op == 'd')//退菜
        {
            scanf("%d", &idx);
            clear_screen_buffer();
            Fnode* ptf = food_search(pto->order.bill.next, idx);
            if (!ptf)
            {
                printf("这个序号不存在，请输入正确的菜品序号.\n");
                Sleep(1500);
                continue;
            }
            remove_food(&(pto->order.bill), idx);
            pto->order.food_num -= 1;
            pto->order.totalcost -= ptf->food.price;
            ptf->food.order_cnt -= 1;
        }
        else
        {
            ungetc(op, stdin);
            if (!scanf("%d", &idx) == 1)
            {
                clear_screen_buffer();
                continue;
            }
            clear_screen_buffer();
            system("cls");
            if (idx == 0)
                break;
            else
            {
                Fnode* ptf = food_search(restaurant.food_list->next, idx);
                if (!ptf)
                {
                    printf("这个序号不存在，请输入正确的菜品序号.\n");
                    Sleep(1500);
                    continue;
                }
                add_food(&(pto->order.bill), ptf->food);
                pto->order.food_num += 1;
                pto->order.totalcost += ptf->food.price;
                ptf->food.order_cnt += 1;
            }
        }
    }
    write_order_info();
    write_food_info();
    write_queue_info();
}

//顾客仪表盘 目前支持充值功能
void UI_customer_dashboard(Cnode* customer)
{
    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        if (!customer)
        {
            printf("没有用户登录");
            printf("\n\n按任意键退出\n");
            getchar();
            return;
        }
        printf(" 欢迎您 %s  %s\n\n", restaurant.nowcustomer->customer.is_vip == 0 ? "尊敬的VIP会员" : "尊敬的VIP会员"
            , restaurant.nowcustomer->customer.name);
        printf("\t您的余额：%.2lf\n\n", customer->customer.balance);
        printf("\t您的积分：%d\n\n", customer->customer.points);
        printf("输入\"cz\" [金额] 给账户充值\n");
        printf("输入 \"quit\" 返回上一级菜单\n\n");
        printf("@%s > ", restaurant.nowwaiter->waiter.name);
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "cz"))
        {
            int temp;
            if (scanf("%d", &temp) != 1 || temp <= 0) continue;
            clear_screen_buffer();
            customer->customer.balance += temp;
            write_customer_info();
            break;
        }
        else if (strcmp(cmd, "quit") == 0)
        {
            system("cls");
            break;
        }
        else
        {
            clear_screen_buffer();
            printf("无法识别的命令");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//座位搜索 返回1找到 否则返回0
int seats_search(char* name, int* row, int* col)
{
    for (int i = 1; i <= restaurant.seat_row; i++)
        for (int j = 1; j <= restaurant.seat_col; j++)
            if (!strcmp(restaurant.seats[i][j], name))
            {
                *row = i;
                *col = j;
                return 1;
            }
    return *row = *col = 0;
}

//选座位
void UI_seats(Cnode* customer)
{
    if (customer)
        Onode* pto = match_order(customer);
    int col = 0, row = 0;
    int newrow = 0, newcol = 0;
    int flag = 0;
    if (customer)
        flag = seats_search(customer->customer.name, &row, &col);
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("这是餐厅的实时座位情况图.\n");
    print_seat_map(col, row);
    if (customer && match_order(customer) && !match_order(customer)->order.is_takeaway)
    {
        printf("\n您可以按照 [排序号] [列序号] \n的输入格式来选择一个座位.任意为0代表退出上一级菜单\n\n");
        flag = seats_search(customer->customer.name, &row, &col);
        while (1)
        {
            printf("位置: ");
            if (scanf("%d %d", &newrow, &newcol) != 2)
                continue;
            clear_screen_buffer();
            if (newrow == 0 || newcol == 0)
                return;
            if (newrow > restaurant.seat_row || newcol > restaurant.seat_col || newrow < 1 || newcol < 1)
            {
                printf("非法输入!\n");
                Sleep(1000);
            }
            if (newrow == row && newcol == col)
                continue;
            if (strcmp(restaurant.seats[newrow][newcol], "?")) //位置有人
            {
                printf("这个位置已经有人\n");
                Sleep(1000);
            }
            else
            {
                strcpy(restaurant.seats[newrow][newcol], customer->customer.name);
                if (!flag) --restaurant.rest_seats; //说明原先是没有位置的
                break;
            }
        }
    }
    write_seat();
    printf("\n\n按任意键退出\n");
    getchar();
}

//打印座位表
void print_seat_map(int x, int y)
{
    if (!restaurant.is_full)
        printf("当前餐厅有%d个空余位置\n", restaurant.rest_seats);
    else
        printf("当前餐厅已经满座\n");
    for (int i = 1; i <= restaurant.seat_row; i++)
        for (int j = 1; j <= restaurant.seat_col; j++)
            if (x == i && y == j)
                printf("@%c", j == restaurant.seat_col ? '\n' : ' ');
            else
            {
                if (strcmp(restaurant.seats[i][j], "?"))
                    printf("1%c", j == restaurant.seat_col ? '\n' : ' ');
                else
                    printf("0%c", j == restaurant.seat_col ? '\n' : ' ');
            }
}

//菜单浏览
void UI_menu()
{
    system("cls");
    printf("############# 山东大学 ############\n\n");
    printf("序号\t 菜名\t                单价\n");
    printf("=======================================\n\n");
    print_linked_list_f(restaurant.food_list);
    printf("=======================================\n\n");
    printf("按任意键返回用户上一级菜单...");
    getchar();
    system("cls");
}

//支付账单
void UI_pay()
{

    while (1)
    {
        system("cls");
        printf("############# 山东大学 ############\n\n");
        printf("=======================================\n\n");
        Onode* pto = match_order(restaurant.nowcustomer);
        if (!pto)
            return;
        if (pto->order.food_num)
        {
            printf("这是您已经点的菜品\n");
            printf("序号\t 名称\t                单价\n");
        }
        printf("=======================================\n\n");
        print_linked_list_f(&(pto->order.bill));
        printf("\n===========================================\n");
        double should_pay = pto->order.totalcost;
        if (restaurant.nowcustomer->customer.is_vip)
            should_pay *= 0.9;
        printf("总菜品数: %2d     应支付金额: %5.2f\n",
            pto->order.food_num, should_pay);
        printf("=======================================\n\n");
        printf("\t1)余额支付\n\n");
        printf("\t2)现金\n\n");
        int op;
        if (scanf("%d", &op) == 1)
        {
            clear_screen_buffer();
            switch (op)
            {
                case 1:
                    if (restaurant.nowcustomer->customer.balance >= should_pay)
                    {
                        pay_bill();
                        return;
                    }
                    else
                    {
                        printf("余额不足 改用现金支付\n");
                    }
                case 2:
                    puts("  @@@@@@@@@@@@@@      @@    @@@@@@@@@@  @  @@@@@@@@@@@@@@");
                    puts("  @@          @@    @@      @@      @@@@   @@          @@ ");
                    puts("  @@  @@@@@@  @@  @@    @@  @@      @@     @@  @@@@@@  @@ ");
                    puts("  @@  @@@@@@  @@    @@        @@@@      @  @@  @@@@@@  @@ ");
                    puts("  @@  @@@@@@  @@    @@    @@  @@@@@@@@@@@  @@  @@@@@@  @@ ");
                    puts("  @@          @@      @@@@@@@@@@  @@@@     @@          @@ ");
                    puts("  @@@@@@@@@@@@@@  @@  @@  @@  @@  @@  @@   @@@@@@@@@@@@@@ ");
                    puts("                  @@    @@    @@@@@@  @@                  ");
                    puts("  @@@@@@  @@@@@@@@@@      @@@@@@  @@@@  @@@@@      @@     ");
                    puts("    @@@@    @@  @@  @@  @@  @@    @@     @@@@  @@    @@@@ ");
                    puts("      @@@@@@@@@@      @@@@@@    @@@@  @@@@@@@@@@@@@@@@@   ");
                    puts("    @@@@            @@@@@@@@    @@      @  @@@@  @@@@     ");
                    puts("  @@    @@@@@@  @@    @@  @@  @@@@@@  @@@@@@@  @@@@@@    ");
                    puts("  @@@@  @@@@  @@@@    @@@@@@@@  @@    @@       @@@@@@  @@ ");
                    puts("      @@          @@@@@@  @@@@             @@@@@@  @@@@   ");
                    puts("  @@  @@      @@        @@      @@@@@@@@@  @@@@@@  @@     ");
                    puts("    @@  @@  @@    @@@@@@      @@@@@@@@@@ @@        @@@@@@ ");
                    puts("  @@    @@  @@@@          @@@@@@  @@@@@@ @@@@@@@@    @@@@ ");
                    puts("                    @@@@@@    @@  @@@@  @      @@    @@@@ ");
                    puts("  @@@@@@@@@@@@@@  @@@@@@@@      @@    @@   @@  @@  @@@@   ");
                    puts("  @@          @@  @@@@    @@    @@@@    @@     @@@@@@     ");
                    puts("  @@  @@@@@@  @@  @@@@  @@    @@@@  @@  @@@@@@@@@@@    @@ ");
                    puts("  @@  @@@@@@  @@    @@@@    @@  @@  @@  @  @@@@@@  @@  @@ ");
                    puts("  @@  @@@@@@  @@  @@  @@@@    @@@@  @@       @@  @@@@  @@ ");
                    puts("  @@          @@  @@@@  @@@@    @@@@  @@     @@  @@@@@@   ");
                    puts("  @@@@@@@@@@@@@@    @@@@  @@@@    @@@@@@   @@  @@    @@@@ ");
                    pay_bill();
                    return;
                default:
                    break;
            }
        }
    }
}


//结账
void pay_bill()
{
    Onode* pto = match_order(restaurant.nowcustomer);
    double should_pay = pto->order.totalcost;
    if (restaurant.nowcustomer->customer.is_vip)
        should_pay *= 0.9;
    restaurant.nowcustomer->customer.balance -= should_pay;
    restaurant.nowcustomer->customer.points += pto->order.totalcost;
    int row, col;
    if (seats_search(restaurant.nowcustomer->customer.name, &row, &col))
        restaurant.rest_seats++;
    if (row && col)
        strcpy(restaurant.seats[row][col], "?");
    order_push(pto->order.idx);
    restaurant.nowwaiter->waiter.num_order += 1;
    restaurant.total_turnover += 1;
    restaurant.income += pto->order.totalcost;
    remove_order(restaurant.order_list, pto->order.idx);
    restaurant.nowcustomer = NULL;
    write_seat();
    write_trade();
    write_customer_info();
    write_waiter_info();
    write_order_info();
    write_queue_info();
    printf("付款成功 谢谢惠顾\n");
    Sleep(1500);
}


#endif
