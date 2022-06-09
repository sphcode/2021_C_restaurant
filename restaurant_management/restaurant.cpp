#ifndef __CORE_H__
#define __CORE_H__

#include "restaurant.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <Windows.h>

Restaurant restaurant;

//��ʼ�� ����0������������
int init()
{
    //���ÿ���̨ǰ/����ɫ
    //system("COLOR F0");
    //���ÿ���̨��С
    //system("mode con cols = 50 lines = 30");

    //����ָ��
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

   //����Ӫҵ��Ϣ
    load_trade();

    //�����Ʒ��Ϣ
    load_food_info();

    //�����û�����
    load_customer_info();

    //�������Ա
    load_waiter_info();

    //�������Ա����
    load_admin_passwd();

    //���붩����Ϣ
    load_order_info();

    //���붩������
    load_queue_info();

    //������λͼ��Ϣ
    load_seat();

    return 0;
}

//��ȡ����Ӫҵ���

void load_trade()
{
    FILE* fp = fopen("trade.data", "r");
    fscanf(fp, "%d %d", &restaurant.income, &restaurant.total_turnover);
    fclose(fp);
}

//д�����Ӫҵ���
void write_trade()
{
    FILE* fp = fopen("trade.data", "w");
    fprintf(fp, "%d %d", restaurant.income, restaurant.total_turnover);
    fclose(fp);
}

//����Ӫҵ�������
void zero_trad()
{
    restaurant.income = restaurant.total_turnover = 0;
    write_trade();
}

//��ȡ��Ʒ��Ϣ
/*
��Ʒ��ʽ
���        ����        �۸�        ��Ʒ���۴���
eg.
   1        ������˿       20        1
*/
void load_food_info()
{
    FILE* fp = fopen("menu.data", "r");
    if (!fp)
    {
        printf("\"menu.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//д���Ʒ��Ϣ
void write_food_info()
{
    FILE* fp = fopen("menu.data", "w");
    if (!fp)
    {
        printf("\"menu.data\"�ļ���ȡʧ�� ��������˳�\n");
        getchar();
        exit(1);
    }
    for (Fnode* i = restaurant.food_list->next; i; i = i->next)
        fprintf(fp, "%d %s %d %d\n", i->food.idx, i->food.name, i->food.price, i->food.order_cnt);
    fclose(fp);
}

//��Ʒ���� ����ͷ���
Fnode* food_search(Fnode* head, int idx)
{
    for (Fnode* i = head; i; i = i->next)
        if (idx == i->food.idx)
            return i;
    return NULL;
}

//�����²�
Fnode* add_food(Fnode* head, Food food)
{
    Fnode* node = (Fnode*)malloc(sizeof(Fnode));
    Fnode* last = head;

    //��ֵ
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

//�Ƴ���Ʒ
Fnode* remove_food(Fnode* head, int food_idx)
{
    Fnode* node, * last;
    for (node = head->next, last = head; node; last = node, node = node->next)
    {
        if (node->food.idx == food_idx)
        {
            //printf("��Ʒɾ���ɹ�\n");
            last->next = node->next;
            restaurant.num_food--;
            break;
        }
    }
    return head;
}

//��ȡ�˿���Ϣ
/*
�˿͸�ʽ
���        ����        ����        ���       �Ƿ���VIP   ����
eg.
   1       �����       150        100         0           111111
*/
void load_customer_info()
{
    FILE* fp = fopen("customer.data", "r");
    if (!fp)
    {
        printf("\"customer.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//д���û���Ϣ
void write_customer_info()
{
    FILE* fp = fopen("customer.data", "w");
    if (!fp)
    {
        printf("\"customer.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//���ӹ˿�
Cnode* add_customer(Cnode** head, Customer customer)
{
    int h = hash(customer.name);
    Cnode* node = (Cnode*)malloc(sizeof(Cnode));
    Cnode* last = head[h];

    //��ֵ
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

//��ϣ
unsigned long long hash(char* str)
{
    unsigned long long res = 0;
    for (char* i = str; *i; i++)
        res = (res * HASHP + *i) % HASHMO;
    return res;
}

//��ȡ����Ա��Ϣ
/*
����Ա��ʽ
���        ����        ����    ����    
eg.
   1       �����       150    123456
*/
void load_waiter_info()
{
    FILE* fp = fopen("waiter.data", "r");
    if (!fp)
    {
        printf("\"wiator.data\"�ļ���ȡʧ��\n");
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

//д�����Ա��Ϣ
void write_waiter_info()
{
    FILE* fp = fopen("waiter.data", "w");
    if (!fp)
    {
        printf("\"wiator.data\"�ļ���ȡʧ��\n");
        getchar();
        exit(1);
    }
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next)
        fprintf(fp, "%d %s %s %d\n", i->waiter.idx, i->waiter.name, i->waiter.password, i->waiter.num_order);
    fclose(fp);
}

//ְԱ��ְ
Wnode* add_waiter(Wnode* head, Waiter waiter)
{
    Wnode* node = (Wnode*)malloc(sizeof(Wnode));
    Wnode* last = head;
    //��ֵ
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

//ְԱ��ְ
Wnode* remove_waiter(Wnode* head, int waiter_idx)
{
    Wnode* node, * last;
    for (node = head->next, last = head; node; last = node, node = node->next)
    {
        if (node->waiter.idx == waiter_idx)
        {
            //printf("ְԱɾ���ɹ�\n");
            last->next = node->next;
            break;
        }
    }
    if (!node)
        printf("û�в�ѯ����ID��ְԱ\n");
    return head;
}

//��ȡ����Ա����
void load_admin_passwd()
{
    FILE* fp;
    fp = fopen("admin.data", "r");
    if (!fp)
    {
        printf("\"admin.data\"�ļ���ȡʧ�� ��������˳�\n");
        getchar();
        exit(1);
    }
    fscanf(fp, "%s", restaurant.admin_password);
    fclose(fp);
}

//д�����Ա����
void write_admin_passwd()
{
    FILE* fp;
    fp = fopen("admin.data", "w");
    if (!fp)
    {
        printf("\"admin.data\"�ļ���ȡʧ�� ��������˳�\n");
        getchar();
        exit(1);
    }
    fprintf(fp, "%s", restaurant.admin_password);
    fclose(fp);
}

//��ȡ������Ϣ
/*
������ʽ
�˿����� ������ �Ƿ����� ��Ʒ���� �ܻ���
��Ʒ1ID ��Ʒ2ID...
eg.
�����     1       0       2       30
1 3
*/
void load_order_info()
{
    FILE* fp = fopen("order.data", "r");
    if (!fp)
    {
        printf("\"order.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//д�붩����Ϣ
void write_order_info()
{
    FILE* fp = fopen("order.data", "w");
    if (!fp)
    {
        printf("\"order.data\"�ļ���ȡʧ�� ��������˳�\n");
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


//��ȡ��������
/*
�������и�ʽ
(һ������������)
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

//���Ӷ���
Onode* add_order(Onode* head, Order order)
{
    Onode* node = (Onode*)malloc(sizeof(Onode));
    Onode* last = head;

    //��ֵ
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

//�Ƴ�����
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

//д�붩������
void write_queue_info()
{
    FILE* fp;
    fp = fopen("queue.data", "w");
    if (!fp)
    {
        printf("\"queue.data\"�ļ���ȡʧ�� ��������˳�\n");
        getchar();
        exit(1);
    }
    for (int i = restaurant.qhh; i != restaurant.qtt; i = (i + 1) % QUEUESIZE)
        fprintf(fp, "%d ", restaurant.que[i]);
    fclose(fp);
}

//���붩��
void order_push(int idx)
{
    if (restaurant.qtt != -1 && restaurant.qhh == (restaurant.qtt + 1) % QUEUESIZE)
    {
        printf("���пռ䲻��!");
        exit(1);
    }
    restaurant.qtt = (restaurant.qtt + 1) % QUEUESIZE;
    restaurant.que[restaurant.qtt] = idx;
}

//��������
int order_pop()
{
    int res = restaurant.que[restaurant.qhh++];
    (restaurant.qhh == QUEUESIZE) && (restaurant.qhh = 0);

    return res;
}

//��ȡ��λ��Ϣ
/*
��λͼ���� ��һ�д�����������ʣ����λ�� ?����û�� 
2 3 3
"�����" "?" "?"
"?" "����" "������"
*/
void load_seat()
{
    FILE* fp = fopen("seatmap.data", "r");
    if (!fp)
    {
        printf("\"seatmap.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//д����λ��Ϣ
void write_seat()
{
    FILE* fp = fopen("seatmap.data", "w");
    if (!fp)
    {
        printf("\"seatmap.data\"�ļ���ȡʧ�� ��������˳�\n");
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

//��ȡʳ��ID
int get_food_id()
{
    int id = 0;
    if (restaurant.num_food == 0)
    {
        id = 1;
    }
    else
    {
        if (restaurant.food_list->next->food.idx > 1)//�����һ��Ԫ��id����1
        {
            id = 1;//����һ��1
        }
        else
        {
            Fnode* p;
            for (p = restaurant.food_list->next; p; p = p->next)
            {
                if (p->next == NULL)//����������һ��Ԫ��
                {
                    id = 1 + p->food.idx;//���ɱ����һ��Ԫ��id��1��id
                    break;
                }
                else
                {
                    if ((p->next->food.idx - p->food.idx) > 1)//���ĳ����Ԫ�ص�id����������
                    {
                        id = 1 + p->food.idx;//���ɱ�ǰһ��Ԫ��id��1��id
                        break;
                    }
                }
            }
        }
    }
    if (!id)
    {
        printf("��ȡְԱIDʱ�������󣬰�������˳�\n");
        getchar();
        exit(1);
    }

    return id;
}

//��ȡְԱID
int get_waiter_id()
{
    int id = 0;
    if (restaurant.num_waiter == 0)
    {
        id = 1;
    }
    else
    {
        if (restaurant.waiter_list->next->waiter.idx > 1)//�����һ��Ԫ��id����1
        {
            id = 1;//����һ��1
        }
        else
        {
            Wnode* p;
            for (p = restaurant.waiter_list->next; p; p = p->next)
            {
                if (p->next == NULL)//����������һ��Ԫ��
                {
                    id = 1 + p->waiter.idx;//���ɱ����һ��Ԫ��id��1��id
                    break;
                }
                else
                {
                    if ((p->next->waiter.idx - p->waiter.idx) > 1)//���ĳ����Ԫ�ص�id����������
                    {
                        id = 1 + p->waiter.idx;//���ɱ�ǰһ��Ԫ��id��1��id
                        break;
                    }
                }
            }
        }
    }
    if (!id)
    {
        printf("��ȡְԱIDʱ�������󣬰�������˳�\n");
        getchar();
        exit(1);
    }

    return id;
}

//��ȡ�˿�ID
int get_customer_id()
{
    return restaurant.num_customer + 1;
}

//��ȡ����ID
int get_order_id()
{
    return order_pop();
}

//��ӡʳ������
void print_linked_list_f(Fnode* head)
{
    for (Fnode* p = head->next; p; p = p->next)
        printf("%d\t %-15s\t%-4d Ԫ\n", p->food.idx, p->food.name, p->food.price);
}

//��ӡ����Ա����
void print_linked_list_w(Wnode* head)
{
    for (Wnode* p = head->next; p; p = p->next)
        printf("%d\t %-15s\t%-4d\n", p->waiter.idx, p->waiter.name, p->waiter.num_order);
}

//���������
void clear_screen_buffer(void)
{
    char c;
    while ((c = getchar()) != EOF && c != '\n');
}

//�û�����ѡ�� ����1Ϊ����Ա ����2Ϊ����Ա
int UI_user_type()
{
    int op;
    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("\t��ѡ���������\n\n");
        printf("\t1:����Ա\n\n");
        printf("\t2:����Ա\n\n");
        printf("�����������ݶ�Ӧ�����, ��0�˳�: [ ]\b\b");
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

//����Ա������֤ ����1��֤ͨ��
int UI_adminpasswd_verify()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("���������Ա�����Ա���֤������\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH + 1];//����
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //���벻ƥ��
        if (strcmp(password_input, restaurant.admin_password))
        {
            printf("���벻��ȷ !\n");
            printf("��������������, ��������������룬������#�˳�\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //����ƥ��
        else
            return 1;
    }
    return 0;
}

//����Ա����
void UI_administrators()
{
    while (1)
    {
        system("cls");
        int op = 0;
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("\n");
        printf("\t1) ��Ʒ����\n\n");
        printf("\t2) ����Ա����\n\n");
        printf("\t3) ��λ����\n\n");
        printf("\t4) ��������\n\n");
        printf("\t5) ���ݷ���\n\n");
        printf("\t6) ����Ա�����޸�\n\n");
        printf("\t7) ����Ա�����޸�\n\n");
        printf("\t8) �˳�ϵͳ\n\n");
        while (!(op >= 1 && op <= 8))
        {
            printf("�����������Ӧ�����: [ ]\b\b");
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


//��Ʒ����
void UI_food_management()
{
    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("���\t ��Ʒ��\t                ����\n");
        printf("=======================================\n\n");
        print_linked_list_f(restaurant.food_list);
        printf("=======================================\n\n");
        printf("���� \"add [��Ʒ��] [����]\" ��Ӳ�Ʒ\n");
        printf("���� \"del [���]\" ɾ����Ʒ\n");
        printf("���� \"quit\" ������һ���˵�\n\n");
        printf("@admin > ");
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "add"))
        {
            //����ʳ������
            char name[FOODNAME_MAX + 1];
            int price, idx;
            scanf("%s", name);
            scanf("%d", &price);
            clear_screen_buffer();
            idx = get_food_id();

            //����ʳ��ṹ��
            Food newfood;
            newfood.idx = idx;
            strcpy(newfood.name, name);
            newfood.price = price;
            newfood.order_cnt = 0;

            //����ʳ�����ݵ�ϵͳ���Ҵ���
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
            printf("�޷�ʶ�������");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//����Ա����
void UI_waiter_management()
{
    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("���\t ����\t                ��ɶ�����\n");
        printf("=======================================\n\n");
        print_linked_list_w(restaurant.waiter_list);
        printf("=======================================\n\n");
        printf("���� \"add [����]\" ����˺�\n");
        printf("���� \"del [���]\" ɾ���˺�\n");
        printf("���� \"quit\" ������һ���˵�\n\n");
        printf("@admin > ");
        char cmd[30];
        scanf("%s", cmd);
        if (!strcmp(cmd, "add"))
        {
            //���ܷ���Ա����
            char name[30];
            int price, idx;
            if (scanf("%s", name) == 1)
            {
                idx = get_waiter_id();

                //�������Ա�ṹ��
                Waiter newwaiter;
                newwaiter.idx = idx;
                strcpy(newwaiter.name, name);
                strcpy(newwaiter.password, "123456"); //Ĭ������
                newwaiter.num_order = 0;

                //���·���Ա���ݵ�ϵͳ���Ҵ���
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
            printf("�޷�ʶ�������\n");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//��λ����
void UI_seat_management()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    print_seat_map(0, 0);
    if (restaurant.rest_seats != restaurant.seat_col * restaurant.seat_row)
    {
        printf("��ǰ�й˿����òͣ����ܵ�����λ\n");
        printf("�����������");
        getchar();
    }
    else
    {
        printf("��ǰ������λ����=%3d, ��λ����=%3d\n", restaurant.seat_row, restaurant.seat_col);
        printf("����\"[����] [����]\"������λ ����Ϊ0�����˳���һ���˵�\n");
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
                printf("���³ɹ�\n");
                Sleep(1500);
                break;
            }
            else
            {
                printf("����\"[����] [����]\"������λ\n");
            }
        }
        clear_screen_buffer();
    }
    write_seat();
}

//����Ա����
Wnode* waiter_search(int idx)
{
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next)
        if (idx == i->waiter.idx)
            return i;
    return NULL;
}

//��������
void UI_restaurant_overview()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("��ǰ������\n");
    printf("\t%d����Ʒ\n", restaurant.num_food);
    printf("\t%d������Ա\n", restaurant.num_waiter);
    printf("\t%d��ע���û�\n", restaurant.num_customer);
    printf("\t%d���û����ò�\n", restaurant.seat_col * restaurant.seat_row - restaurant.rest_seats);
    puts("");
    printf("���չ����%d������ ������%dԪ\n", restaurant.total_turnover, restaurant.income);
    puts("");
    printf("�����������\n");
    getchar();
}

//�������
void UI_cosumeption_list(int n)
{
    Cnode** stack = (Cnode**)calloc(n, sizeof(Cnode*));
    //ջ��
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
                //����
                for (int k = top - 1; k > 0; k--)
                    if (stack[k - 1]->customer.points < stack[k]->customer.points)
                    {
                        Cnode* temp = stack[k];
                        stack[k] = stack[k - 1];
                        stack[k - 1] = temp;
                    }
            }
    printf("\t���Ѱ�\n");
    printf("  ��������ǰ%d�˿�\n", n);
    printf("����\t\t\t���Ѷ�\n");
    //����Ҫ��i < top ������i < n
    for (int i = 0; i < top; i++)
        printf("%s\t\t\t%d\n", stack[i]->customer.name, stack[i]->customer.points);
    free(stack);
}

//ҵ������
void UI_num_order_list()
{  
    Wnode** array = (Wnode**)calloc(restaurant.num_waiter, sizeof(Wnode*));
    int count = 0;
    for (Wnode* i = restaurant.waiter_list->next; i; i = i->next, ++count)
        array[count] = i;
    quick_sort_w(array, 0, restaurant.num_waiter - 1);
    printf("\tҵ����\n");
    printf("����\t\t\t��ɶ�����\n");
    for (int i = 0; i < restaurant.num_customer; i++)
        printf("%s\t\t\t%d\n", array[i]->waiter.name, array[i]->waiter.num_order);
    free(array);
}

//ҵ�������Ŀ���
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

//���ܻ�ӭ��n����Ʒ
void UI_cnt_food_list(int n)
{
    Fnode** array = (Fnode**)calloc(restaurant.num_food, sizeof(Fnode*));
    int count = 0;
    for (Fnode* i = restaurant.food_list->next; i; i = i->next, ++count)
        array[count] = i;
    quick_sort_f(array, 0, restaurant.num_food - 1);
    printf("  ���ܻ�ӭ��ǰ%d��Ʒ\n", n);
    printf("��Ʒ��\t\t\t������\n");
    for (int i = 0; i < n; i++)
        printf("%s\t\t %d\n", array[i]->food.name, array[i]->food.order_cnt);
}

//��Ʒ�����Ŀ���
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

//���ݷ���
void UI_data_analyse()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    UI_cosumeption_list(3); //������ߵ�n���˿�
    puts("");
    UI_cnt_food_list(5);
    puts("");
    UI_num_order_list();
    puts("");
    printf("�����������\n");
    getchar();
}

//����Ա�����޸�
void UI_adminpasswd_change()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("�������µ�����.\n");
    for (int i = 0; i < PASSWORDLENTH; i++)
    {
        restaurant.admin_password[i] = getchar();
        if (!isalnum(restaurant.admin_password[i]))
            --i;
    }
    restaurant.admin_password[PASSWORDLENTH] = 0;
    write_admin_passwd();
    printf("�޸ĳɹ�");
    Sleep(1500);
    system("cls");
}

//����Ա������֤
int UI_waiterpasswd_verify()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("�����빤��\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    int idx;
    Wnode* ptw;
    fscanf(stdin, "%d", &idx);
    clear_screen_buffer();
    while (1)
    {
        if (!(ptw = waiter_search(idx)))
        {
            printf("δ��ѯ���ù���\n");
            printf("���������빤��\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%d", &idx);
            clear_screen_buffer();
        }
        else
            break;
    }
    printf("����������\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH + 1];//����
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //���벻ƥ��
        if (!(strcmp(password_input, ptw->waiter.password) == 0))
        {
            printf("���벻��ȷ !\n");
            printf("��������������, ��������������룬������#�˳�\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //����ƥ��
        else
        {
            restaurant.nowwaiter = ptw;
            return 1;
        }
    }
    return 0;
}

//����Ա�����޸�
void UI_waiterpasswd_change()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("�����빤��\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    int idx;
    Wnode* ptw;
    fscanf(stdin, "%d", &idx);
    clear_screen_buffer();
    while (1)
    {
        if (!(ptw = waiter_search(idx)))
        {
            printf("δ��ѯ���ù���\n");
            printf("���������빤��\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%d", &idx);
            clear_screen_buffer();
        }
        else
            break;
    }
    printf("�������µ�����.\n");
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
    printf("�޸ĳɹ���");
    Sleep(1500);
    system("cls");
}

void UI_waiterpasswd_change_own()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("�������µ�����.\n");
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
    printf("�޸ĳɹ�");
    Sleep(1500);
    system("cls");
}
//����Ա����
void UI_waiters()
{
    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n");
        if (restaurant.nowcustomer)
        {
            printf("%s %s\n", restaurant.nowcustomer->customer.is_vip == 0 ? "�𾴵Ļ�Ա" : "�𾴵�VIP��Ա"
                , restaurant.nowcustomer->customer.name);
            printf("����%d\t����%s �߳�Ϊ�����\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        }
        else
            printf("����%d\t����%s\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("\n");
        printf("\t1) �˿͵�¼��ע��\n\n");
        printf("\t2) �˵����\n\n");
        printf("\t3) �˿ͳ�ֵ\n\n");
        printf("\t4) ��������\n\n");
        printf("\t5) ������Ϣ���\n\n");
        printf("\t6) ����Ա�����޸�\n\n");
        printf("\t7) �˳�ϵͳ\n\n");
        printf("=======================================\n");
        int op = 0;
        while (!(op >= 1 && op <= 7))
        {
            printf("�����������Ӧ�����: [ ]\b\b");
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

//��¼ע���˳�����
void UI_signin_logout(Cnode* customer)
{
    int op;
    Cnode* ptc = NULL;
    int flag = 1;
    while (flag == 1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("\t1:������һ������\n\n");
        printf("\t2:�û���¼\n\n");
        printf("\t3:�û�ע��\n\n");
        if (customer)
            printf("\t4:�˳���ǰ�û�\n\n");
        printf("�������Ӧ�����: [ ]\b\b");
        fscanf(stdin, "%d", &op);
        clear_screen_buffer();
        switch (op)
        {
            case 1:
                return;
            case 2:
                if (restaurant.nowcustomer)
                {
                    printf("�����˳���ǰ�˺�\n");
                    Sleep(1500);
                    break;
                }
                ptc = UI_login();
                if (ptc)
                {
                    flag = 0;
                    restaurant.nowcustomer = ptc;
                    printf("%s %s ��¼�ɹ�\n", restaurant.nowcustomer->customer.is_vip == 0 ? "�𾴵Ļ�Ա" : "�𾴵�VIP��Ա"
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
                printf("�Ƿ�����\n");
                printf("�������Ӧ�����: [ ]\b\b");
                break;
        }
    }
}

//�˿�����
Cnode* customer_search(char* name)
{
    for (Cnode* i = restaurant.customer_list[hash(name)]->next; i; i = i->next)
        if (!strcmp(name, i->customer.name))
            return i;
    return NULL;
}

//�˿͵�¼
Cnode* UI_login()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    Cnode* ptc = NULL;
    while (1)
    {
        char name[USERNAME_MAX + 1];
        printf("�������Ա��\n\n>__________\b\b\b\b\b\b\b\b\b\b");
        assert(fscanf(stdin, "%s", name) == 1);
        clear_screen_buffer();
        ptc = customer_search(name);
        if (!ptc)
        {
            printf("��ѯ�����û�Ա��");
        }
        else
        {
            break;
        }
    }
    printf("����������\n\n");
    printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    char password_input[PASSWORDLENTH * 3];//���� ����Ϊ�˰�ȫ�����˿ռ�
    fscanf(stdin, "%s", password_input);
    clear_screen_buffer();
    for (int i = 0; i < 3; i++)
    {
        //���벻ƥ��
        if (!(strcmp(password_input, ptc->customer.password) == 0))
        {
            printf("���벻��ȷ !\n");
            printf("��������������, ��������������룬������#�˳�,��������������������\n\n");
            printf(">___________________________\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            fscanf(stdin, "%s", password_input);
            clear_screen_buffer();
            if (!strcmp(password_input, "#"))
                break;
        }
        //����ƥ��
        else
        {
            return ptc;
        }
    }
    return NULL;
}

//�˿�ע��
void UI_register()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    Customer newcustomer;
    printf("�������û�����%d�ַ����ڣ�\n\n>__________\b\b\b\b\b\b\b\b\b\b", USERNAME_MAX);
    assert(fscanf(stdin, "%s", newcustomer.name) == 1);
    clear_screen_buffer();
    printf("��Ա����ֵ��Ԥ��ֵ��500���ɳ�ΪVIP���ܰ��۷���\n\n>________\b\b\b\b\b\b\b\b");
    assert(fscanf(stdin, "%lf", &newcustomer.balance) == 1);
    clear_screen_buffer();
    printf("������λ����\n\n>________\b\b\b\b\b\b\b\b");
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
    printf("%s��Աע��ɹ��� ��ԱID��%d���μ�\n\n", newcustomer.name, newcustomer.idx);
    printf("��������˳�\n");
    getchar();
}

//ƥ�䶩��
Onode* match_order(Cnode* customer)
{
    //����ö��
    for (Onode* i = restaurant.order_list->next; i; i = i->next)
        if (i->order.customer == customer)
            return i;
    return NULL;
}

//��������
Onode* order_search(Onode* head, int idx)
{
    for (Onode* i = head->next; i && i->order.idx <= idx; i = i->next)
        if (i->order.idx == idx)
            return i;
    return NULL;
}

//��������
void UI_order_management(Cnode* customer)
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    if (restaurant.nowcustomer)
    {
        printf("%s %s\n", restaurant.nowcustomer->customer.is_vip == 0 ? "�𾴵Ļ�Ա" : "�𾴵�VIP��Ա"
            , restaurant.nowcustomer->customer.name);
        printf("����%d\t����%s �߳�Ϊ�����\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("\t��ǰ����%d������\n\n", restaurant.num_orders);
        print_linked_list_o(restaurant.order_list);
        printf("\n���� \"more\" [������] �鿴������Ϣ\n");
        printf("���� \"new\" �����¶������޸ĵ�ǰ����\n");
        printf("���� \"pay\" ����\n");
        printf("���� \"quit\" ������һ���˵�\n\n");
    }
    else
    {
        printf("����%d\t����%s\n\n", restaurant.nowwaiter->waiter.idx, restaurant.nowwaiter->waiter.name);
        printf("��ǰ����%d������\n\n", restaurant.num_orders);
        print_linked_list_o(restaurant.order_list);
        printf("\n���� \"more\" [������] �鿴������Ϣ\n\n");
        printf("���� \"quit\" ������һ���˵�\n\n");
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
            printf("�޷�ʶ�������");
            printf("\"%s\"\n", cmd);
            Sleep(1500);
        }
    }
}

//��ӡ��������
void print_linked_list_o(Onode* head)
{
    printf("������\t\t�˿�\t\t���Ѷ�\n");
    for (Onode* i = restaurant.order_list->next; i; i = i->next)
    {
        printf("%d\t\t%s\t\t%d\n", i->order.idx, i->order.customer->customer.name, i->order.totalcost);
    }
}

//չʾ����������Ϣ
void UI_show_more_detail(int idx)
{
    Onode* pto = order_search(restaurant.order_list, idx);
    printf("������%d\n�˿�:%s\n���Ѷ�:%d\n", pto->order.idx, pto->order.customer->customer.name, pto->order.totalcost);
    printf("��%d����Ʒ\n", pto->order.food_num);
    print_linked_list_f(&pto->order.bill);

}

//���
void UI_order(Cnode* customer)
{
    int op;
    Onode* order = match_order(customer), *pto = NULL;
    if (order == NULL) //�¶���
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
            printf("############# ɽ����ѧ ############\n\n");
            printf("1:�������\n");
            printf("2:����Ͳ�\n");
            printf("�������Ӧ�����: [ ]\b\b");
            fscanf(stdin, "%d", &op);
            clear_screen_buffer();
            if (op == 1 || op == 2)
            {
                break;
            }
            else
            {
                printf("�Ƿ����룡\n");
            }
        }
        order->order.is_takeaway = 2 - op; //��Ϊ1��Ӧ1 2��Ӧ0
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
        printf("############# ɽ����ѧ ############\n\n");
        printf("���\t ����\t                ����\n");
        printf("=======================================\n\n");
        print_linked_list_f(restaurant.food_list);
        printf("=======================================\n\n");
        if (pto->order.food_num)
        {
            printf("�������Ѿ���Ĳ�Ʒ\n");
            printf("���\t ����\t                ����\n");
        }
        printf("=======================================\n\n");
        print_linked_list_f(&(pto->order.bill));
        printf("\n===========================================\n");
        printf("�ܲ�Ʒ��: %2d     Ӧ֧�����: %5.2f\n",
            pto->order.food_num, pto->order.totalcost * customer->customer.is_vip ? "0.9" : "1.0");
        printf("=======================================\n\n");
        printf("�����������Ĳ˶�Ӧ�����,�˲�������\"d\" + ��� \n���� 0 �˳�������һ���˵�\n\n");
        printf("��Ʒ���: [ ]\b\b");
        int idx;
        op = getchar();
        if (op == 'd')//�˲�
        {
            scanf("%d", &idx);
            clear_screen_buffer();
            Fnode* ptf = food_search(pto->order.bill.next, idx);
            if (!ptf)
            {
                printf("�����Ų����ڣ���������ȷ�Ĳ�Ʒ���.\n");
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
                    printf("�����Ų����ڣ���������ȷ�Ĳ�Ʒ���.\n");
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

//�˿��Ǳ��� Ŀǰ֧�ֳ�ֵ����
void UI_customer_dashboard(Cnode* customer)
{
    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        if (!customer)
        {
            printf("û���û���¼");
            printf("\n\n��������˳�\n");
            getchar();
            return;
        }
        printf(" ��ӭ�� %s  %s\n\n", restaurant.nowcustomer->customer.is_vip == 0 ? "�𾴵�VIP��Ա" : "�𾴵�VIP��Ա"
            , restaurant.nowcustomer->customer.name);
        printf("\t������%.2lf\n\n", customer->customer.balance);
        printf("\t���Ļ��֣�%d\n\n", customer->customer.points);
        printf("����\"cz\" [���] ���˻���ֵ\n");
        printf("���� \"quit\" ������һ���˵�\n\n");
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
            printf("�޷�ʶ�������");
            printf("\"%s\"", cmd);
            Sleep(1500);
        }
    }
}

//��λ���� ����1�ҵ� ���򷵻�0
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

//ѡ��λ
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
    printf("############# ɽ����ѧ ############\n\n");
    printf("���ǲ�����ʵʱ��λ���ͼ.\n");
    print_seat_map(col, row);
    if (customer && match_order(customer) && !match_order(customer)->order.is_takeaway)
    {
        printf("\n�����԰��� [�����] [�����] \n�������ʽ��ѡ��һ����λ.����Ϊ0�����˳���һ���˵�\n\n");
        flag = seats_search(customer->customer.name, &row, &col);
        while (1)
        {
            printf("λ��: ");
            if (scanf("%d %d", &newrow, &newcol) != 2)
                continue;
            clear_screen_buffer();
            if (newrow == 0 || newcol == 0)
                return;
            if (newrow > restaurant.seat_row || newcol > restaurant.seat_col || newrow < 1 || newcol < 1)
            {
                printf("�Ƿ�����!\n");
                Sleep(1000);
            }
            if (newrow == row && newcol == col)
                continue;
            if (strcmp(restaurant.seats[newrow][newcol], "?")) //λ������
            {
                printf("���λ���Ѿ�����\n");
                Sleep(1000);
            }
            else
            {
                strcpy(restaurant.seats[newrow][newcol], customer->customer.name);
                if (!flag) --restaurant.rest_seats; //˵��ԭ����û��λ�õ�
                break;
            }
        }
    }
    write_seat();
    printf("\n\n��������˳�\n");
    getchar();
}

//��ӡ��λ��
void print_seat_map(int x, int y)
{
    if (!restaurant.is_full)
        printf("��ǰ������%d������λ��\n", restaurant.rest_seats);
    else
        printf("��ǰ�����Ѿ�����\n");
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

//�˵����
void UI_menu()
{
    system("cls");
    printf("############# ɽ����ѧ ############\n\n");
    printf("���\t ����\t                ����\n");
    printf("=======================================\n\n");
    print_linked_list_f(restaurant.food_list);
    printf("=======================================\n\n");
    printf("������������û���һ���˵�...");
    getchar();
    system("cls");
}

//֧���˵�
void UI_pay()
{

    while (1)
    {
        system("cls");
        printf("############# ɽ����ѧ ############\n\n");
        printf("=======================================\n\n");
        Onode* pto = match_order(restaurant.nowcustomer);
        if (!pto)
            return;
        if (pto->order.food_num)
        {
            printf("�������Ѿ���Ĳ�Ʒ\n");
            printf("���\t ����\t                ����\n");
        }
        printf("=======================================\n\n");
        print_linked_list_f(&(pto->order.bill));
        printf("\n===========================================\n");
        double should_pay = pto->order.totalcost;
        if (restaurant.nowcustomer->customer.is_vip)
            should_pay *= 0.9;
        printf("�ܲ�Ʒ��: %2d     Ӧ֧�����: %5.2f\n",
            pto->order.food_num, should_pay);
        printf("=======================================\n\n");
        printf("\t1)���֧��\n\n");
        printf("\t2)�ֽ�\n\n");
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
                        printf("���� �����ֽ�֧��\n");
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


//����
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
    printf("����ɹ� лл�ݹ�\n");
    Sleep(1500);
}


#endif
