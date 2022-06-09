#ifndef __RESTAURANT_H__
#define __RESTAURANT_H__

#define USERNAME_MAX 20
#define FOODNAME_MAX 20
#define PASSWORDLENTH 6
#define RESTAURANT_SIZE_MAX 80
#define HASHP 131
#define HASHMO 10007
#define QUEUESIZE 100

//��Ʒ
typedef struct Food
{
    //��ƷID
    int idx;
    //��Ʒ��
    char name[FOODNAME_MAX + 1];
    //��Ʒ�۸�
    int price;
    //��Ʒ���۴���
    int order_cnt;
}Food;

//��Ʒ����
typedef struct Fnode
{
    Food food;
    struct Fnode* next;
}Fnode;

//�˿�
typedef struct Customer
{
    //�û���
    int idx;
    //�û���
    char name[USERNAME_MAX + 1];
    //����
    char password[PASSWORDLENTH + 1];
    //�Ƿ�VIP
    int is_vip;
    //���
    double balance;
    //����
    int points;
}Customer;

typedef struct Cnode
{
    Customer customer;
    struct Cnode* next;
}Cnode;

//����Ա
typedef struct Waiter
{
    //����ԱID
    int idx;
    //����
    char name[USERNAME_MAX + 1];
    //����
    char password[PASSWORDLENTH + 1];
    //��ɶ�����
    int num_order;
}Waiter;

typedef struct Wnode
{
    Waiter waiter;
    struct Wnode* next;
}Wnode;

//����
typedef struct Order
{
    //������
    int idx;
    //�Ƿ����� 1Ϊ����
    int is_takeaway;
    //��ǰ�����Ĳ�Ʒ��
    int food_num;
    //���
    Fnode bill;
    //����
    int totalcost;
    //�˿�
    Cnode* customer;
}Order;

//��������
typedef struct Onode
{
    Order order;
    struct Onode* next;
}Onode;

typedef struct Restaurant
{
    //��Ʒ����
    Fnode* food_list;
    //��Ʒ����
    int num_food;
    //����Ա����
    int num_waiter;
    //����Ա����
    Wnode* waiter_list;
    //�˿͸���
    int num_customer;
    //�˿͹�ϣ��
    Cnode* customer_list[HASHMO];
    //ϵͳ����Ա����
    char admin_password[PASSWORDLENTH + 1];
    //��������
    Onode* order_list;
    //������
    int num_orders;
    //�����Ŷ���
    int que[QUEUESIZE];
    //��ͷ
    int qhh;
    //��β
    int qtt;
    //����
    int income;
    //��ˮ��
    int total_turnover;
    //��λ����
    int seat_row;
    //��λ����
    int seat_col;
    //��λͼ
    char seats[RESTAURANT_SIZE_MAX + 1][RESTAURANT_SIZE_MAX + 1][USERNAME_MAX + 1];
    //ʣ����λ
    int rest_seats;
    //�Ƿ�����
    int is_full;
    //Ŀǰ����Ա
    Wnode* nowwaiter;
    //Ŀǰ�˿�
    Cnode* nowcustomer;
}Restaurant;

//��ʼ�� ����0������������
int init();

//��ȡ����Ӫҵ���
void load_trade();
//д�����Ӫҵ���
void write_trade();
//����Ӫҵ�������
void zero_trad();
//��ȡ��Ʒ��Ϣ
void load_food_info();
//д���Ʒ��Ϣ
void write_food_info();
//��ȡ�û���Ϣ
void load_customer_info();
//д���û���Ϣ
void write_customer_info();
//��ȡ����Ա��Ϣ
void load_waiter_info();
//д�����Ա��Ϣ
void write_waiter_info();
//��ȡ����Ա����
void load_admin_passwd();
//д�����Ա����
void write_admin_passwd();
//��ȡ������Ϣ
void load_order_info();
//д�붩����Ϣ
void write_order_info();
//��ȡ��������
void load_queue_info();
//д�붩������
void write_queue_info();
//���붩��
void order_push(int idx);
//��������
int order_pop();
//��ȡ��λ��Ϣ
void load_seat();
//д����λ��Ϣ
void write_seat();


//��ȡʳ��ID
int get_food_id();
//��ȡְԱID
int get_waiter_id();
//��ȡ�˿�ID
int get_customer_id();
//��ȡ����ID
int get_order_id();
//��Ʒ���� ����ͷ���
Fnode* food_search(Fnode* head, int idx);
//�����²�
Fnode* add_food(Fnode* head, Food food);
//�Ƴ���Ʒ
Fnode* remove_food(Fnode* head, int food_idx);
//����Ա����
Wnode* waiter_search(int idx);
//����Ա��ְ
Wnode* add_waiter(Wnode* head, Waiter waiter);
//����Ա��ְ
Wnode* remove_waiter(Wnode* head, int idx);
//���ӹ˿�
Cnode* add_customer(Cnode** head, Customer customer);
//��ϣ
unsigned long long hash(char* str);
//�˿�����
Cnode* customer_search(char* name);
//��������ƥ�䶩��
Onode* match_order(Cnode* customer);
//���Ӷ���
Onode* add_order(Onode* head, Order order);
//��������
Onode* order_search(Onode* head, int idx);
//�Ƴ�����
Onode* remove_order(Onode* head, int idx);
//��λ���� ����1�ҵ� ���򷵻�0
int seats_search(char* name, int* row, int* col);
//����
void pay_bill();

//UI
//���������
void clear_screen_buffer();
//�û�����ѡ��
int UI_user_type();
//��ӡʳ������
void print_linked_list_f(Fnode* head);
//��ӡ����Ա����
void print_linked_list_w(Wnode* head);
//��ӡ��λ
void print_seat_map(int x, int y);
//��ӡ��������
void print_linked_list_o(Onode* head);


//����Ա����
void UI_administrators();
//����Ա������֤ ����1��֤ͨ��
int UI_adminpasswd_verify();
//��Ʒ����
void UI_food_management();
//ְԱ����
void UI_waiter_management();
//��λ����
void UI_seat_management();
//��������
void UI_restaurant_overview();
//ҵ�������Ŀ���
void quick_sort_w(Wnode* a[], int l, int r);
//��Ʒ�����Ŀ���
void quick_sort_f(Fnode* a[], int l, int r);
//���ݷ���
void UI_data_analyse();
//�������n���˿�
void UI_cosumeption_list(int n);
//���ܻ�ӭ��n����Ʒ��
void UI_cnt_food_list(int n);
//ҵ������
void UI_num_order_list();
//����Ա�����޸�
void UI_adminpasswd_change();
//����Ա�����޸�
void UI_waiterpasswd_change();
//����Ա���������޸�
void UI_waiterpasswd_change_own();

//����Ա����
void UI_waiters();
//����Ա������֤ ����1��֤ͨ��
int UI_waiterpasswd_verify();
//��¼ע���˳�����
void UI_signin_logout(Cnode* customer);
//�˿͵�¼ ����NULL��¼ʧ��
Cnode* UI_login();
//�˿�ע��
void UI_register();
//��������
void UI_order_management(Cnode* customer);
//չʾ����������Ϣ
void UI_show_more_detail(int idx);
//�˿��Ǳ��� Ŀǰ֧�ֳ�ֵ����
void UI_customer_dashboard(Cnode* customer);
//�㵥
void UI_order(Cnode* customer);
//ѡ��λ
void UI_seats(Cnode* customer);
//֧���˵�
void UI_pay();
//�˵����
void UI_menu();

#endif
