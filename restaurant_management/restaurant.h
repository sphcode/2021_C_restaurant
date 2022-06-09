#ifndef __RESTAURANT_H__
#define __RESTAURANT_H__

#define USERNAME_MAX 20
#define FOODNAME_MAX 20
#define PASSWORDLENTH 6
#define RESTAURANT_SIZE_MAX 80
#define HASHP 131
#define HASHMO 10007
#define QUEUESIZE 100

//菜品
typedef struct Food
{
    //菜品ID
    int idx;
    //菜品名
    char name[FOODNAME_MAX + 1];
    //菜品价格
    int price;
    //菜品销售次数
    int order_cnt;
}Food;

//菜品链表
typedef struct Fnode
{
    Food food;
    struct Fnode* next;
}Fnode;

//顾客
typedef struct Customer
{
    //用户号
    int idx;
    //用户名
    char name[USERNAME_MAX + 1];
    //密码
    char password[PASSWORDLENTH + 1];
    //是否VIP
    int is_vip;
    //余额
    double balance;
    //积分
    int points;
}Customer;

typedef struct Cnode
{
    Customer customer;
    struct Cnode* next;
}Cnode;

//服务员
typedef struct Waiter
{
    //服务员ID
    int idx;
    //姓名
    char name[USERNAME_MAX + 1];
    //密码
    char password[PASSWORDLENTH + 1];
    //完成订单数
    int num_order;
}Waiter;

typedef struct Wnode
{
    Waiter waiter;
    struct Wnode* next;
}Wnode;

//订单
typedef struct Order
{
    //订单号
    int idx;
    //是否外卖 1为外卖
    int is_takeaway;
    //当前订单的菜品数
    int food_num;
    //点餐
    Fnode bill;
    //花费
    int totalcost;
    //顾客
    Cnode* customer;
}Order;

//订单链表
typedef struct Onode
{
    Order order;
    struct Onode* next;
}Onode;

typedef struct Restaurant
{
    //菜品链表
    Fnode* food_list;
    //菜品总数
    int num_food;
    //服务员个数
    int num_waiter;
    //服务员链表
    Wnode* waiter_list;
    //顾客个数
    int num_customer;
    //顾客哈希表
    Cnode* customer_list[HASHMO];
    //系统管理员密码
    char admin_password[PASSWORDLENTH + 1];
    //订单链表
    Onode* order_list;
    //订单数
    int num_orders;
    //订单号队列
    int que[QUEUESIZE];
    //队头
    int qhh;
    //队尾
    int qtt;
    //收入
    int income;
    //流水数
    int total_turnover;
    //座位行数
    int seat_row;
    //座位列数
    int seat_col;
    //座位图
    char seats[RESTAURANT_SIZE_MAX + 1][RESTAURANT_SIZE_MAX + 1][USERNAME_MAX + 1];
    //剩余座位
    int rest_seats;
    //是否满座
    int is_full;
    //目前操作员
    Wnode* nowwaiter;
    //目前顾客
    Cnode* nowcustomer;
}Restaurant;

//初始化 返回0代表正常运行
int init();

//读取餐厅营业情况
void load_trade();
//写入餐厅营业情况
void write_trade();
//餐厅营业情况归零
void zero_trad();
//读取菜品信息
void load_food_info();
//写入菜品信息
void write_food_info();
//读取用户信息
void load_customer_info();
//写入用户信息
void write_customer_info();
//读取服务员信息
void load_waiter_info();
//写入服务员信息
void write_waiter_info();
//读取管理员密码
void load_admin_passwd();
//写入管理员密码
void write_admin_passwd();
//读取订单信息
void load_order_info();
//写入订单信息
void write_order_info();
//读取订单队列
void load_queue_info();
//写入订单队列
void write_queue_info();
//插入订单
void order_push(int idx);
//弹出订单
int order_pop();
//读取座位信息
void load_seat();
//写入座位信息
void write_seat();


//获取食物ID
int get_food_id();
//获取职员ID
int get_waiter_id();
//获取顾客ID
int get_customer_id();
//获取订单ID
int get_order_id();
//菜品搜索 不带头结点
Fnode* food_search(Fnode* head, int idx);
//增加新菜
Fnode* add_food(Fnode* head, Food food);
//移除菜品
Fnode* remove_food(Fnode* head, int food_idx);
//服务员查找
Wnode* waiter_search(int idx);
//服务员入职
Wnode* add_waiter(Wnode* head, Waiter waiter);
//服务员离职
Wnode* remove_waiter(Wnode* head, int idx);
//增加顾客
Cnode* add_customer(Cnode** head, Customer customer);
//哈希
unsigned long long hash(char* str);
//顾客搜索
Cnode* customer_search(char* name);
//根据姓名匹配订单
Onode* match_order(Cnode* customer);
//增加订单
Onode* add_order(Onode* head, Order order);
//订单查找
Onode* order_search(Onode* head, int idx);
//移除订单
Onode* remove_order(Onode* head, int idx);
//座位搜索 返回1找到 否则返回0
int seats_search(char* name, int* row, int* col);
//结账
void pay_bill();

//UI
//清除缓存区
void clear_screen_buffer();
//用户类型选择
int UI_user_type();
//打印食物链表
void print_linked_list_f(Fnode* head);
//打印服务员链表
void print_linked_list_w(Wnode* head);
//打印座位
void print_seat_map(int x, int y);
//打印订单链表
void print_linked_list_o(Onode* head);


//管理员界面
void UI_administrators();
//管理员密码验证 返回1验证通过
int UI_adminpasswd_verify();
//菜品管理
void UI_food_management();
//职员管理
void UI_waiter_management();
//座位管理
void UI_seat_management();
//餐厅总览
void UI_restaurant_overview();
//业绩分析的快排
void quick_sort_w(Wnode* a[], int l, int r);
//菜品分析的快排
void quick_sort_f(Fnode* a[], int l, int r);
//数据分析
void UI_data_analyse();
//消费最高n个顾客
void UI_cosumeption_list(int n);
//最受欢迎的n个菜品；
void UI_cnt_food_list(int n);
//业绩排名
void UI_num_order_list();
//管理员密码修改
void UI_adminpasswd_change();
//服务员密码修改
void UI_waiterpasswd_change();
//服务员自主密码修改
void UI_waiterpasswd_change_own();

//服务员界面
void UI_waiters();
//服务员密码验证 返回1验证通过
int UI_waiterpasswd_verify();
//登录注册退出界面
void UI_signin_logout(Cnode* customer);
//顾客登录 返回NULL登录失败
Cnode* UI_login();
//顾客注册
void UI_register();
//订单管理
void UI_order_management(Cnode* customer);
//展示订单更多信息
void UI_show_more_detail(int idx);
//顾客仪表盘 目前支持充值功能
void UI_customer_dashboard(Cnode* customer);
//点单
void UI_order(Cnode* customer);
//选座位
void UI_seats(Cnode* customer);
//支付账单
void UI_pay();
//菜单浏览
void UI_menu();

#endif
