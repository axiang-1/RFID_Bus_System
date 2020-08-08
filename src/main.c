#include <stdio.h>
#include <pthread.h>
#include "touch.h"
#include "lcd_draw_bmp.h"
#include "rfid.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


//多文件编程 -- 多个文件一起编程  编译时  需要把所有用到的.c 加入编译
//编译命令  arm-linux-gcc face_chance.c lcd_draw_bmp.c touch.c -o face_chance -lpthread
//方法2 arm-linux-gcc *.c -o face_chance -lpthread

 
int chance_val; // 记录修改金额,充值面额
int now_user;   //总用户数
int seting_user; //表示当前设置的用户

struct user_info{
	unsigned int card_id;
	int card_money;		//记录总金额
};

struct user_info user[100] = {0};


void *set_thread_run(void *argv)
{
	int i = 0; //临时记录用户位置 
	unsigned int cardid = 0;
	while(1)
	{
		
		if(rfid_read_card(&cardid) == -1) //一直读取卡片
		{
			sleep(1);
			continue; //如果 没有找到卡，从新读卡
		}
		
		i = search_user(&cardid); //在user中去查找有没有当前卡id
		
		if(i != -1) //用户存在
		{
			draw_color(312,25 ,655,90,0xFFFFFF); //清空刷卡id位置
			draw_color(300,120,658,195,0xFFFFFF); //擦除金额框
			show_card_id(320, 40, user[i].card_id);		  //显示卡id
			show_money(340, 140, user[i].card_money); //显示剩余金额
			seting_user = i;
			break;
		}
		else //用户不存在
		{
			user[now_user].card_id = cardid;
			user[now_user].card_money = 0;
			draw_color(312,25 ,655,90,0xFFFFFF); //清空刷卡id位置
			draw_color(300,120,658,195,0xFFFFFF); //擦除金额框
			show_card_id(320, 40, user[now_user].card_id);		  //显示卡id

			show_money(340, 140, user[now_user].card_money); //显示剩余金额
			
			now_user++;
			seting_user = now_user-1;
			break;
		}	
	}
	
}


//充值金额
void setting(void)
{
	int x, y;
	int i = -1;
	//进入设置、充值界面
	lcd_draw_bmp(0, 0, "add_money.bmp");
	draw_color(300,120,658,195,0xFFFFFF); //擦除金额框
	draw_color(312,25 ,655,90,0xFFFFFF); //清空刷卡id位置

	//启动线程检测卡片
	pthread_t id;
	pthread_create(&id, NULL, set_thread_run, NULL);
	pthread_detach(id);
	

	while(1)
	{
		get_x_y(&x,&y); //等待点击屏幕
		if(x > 0 && x < 100 && y > 0 && y < 100)/*xy坐标在退出区域内*/
		{
			lcd_draw_bmp(0, 0, "main.bmp"); //先画主界面图
			pthread_cancel(id);
			seting_user = -1;
			break;
		}
		
		if(x > 366 && y > 296 && x < 478 && y < 370)  //100 金额按钮
		{
			chance_val = 100;
			printf("当前充值面额为100元\n");
		}
		if(x > 520 && y > 296 && x < 631 && y < 370)  //10 金额按钮
		{
			chance_val = 10;
			printf("当前充值面额为10元\n");					
		}
		if(x > 666 && y > 291 && x < 774 && y < 370)  //1 金额按钮
		{
			chance_val = 1;	
			printf("当前充值面额为1元\n");
		}
		if(x > 200 && y > 353 && x < 307 && y < 428)/*加金额*/
		{
			//金额加操作
			user[seting_user].card_money += chance_val;
			if(user[seting_user].card_money > 9999) 
					user[seting_user].card_money = 9999; //如果金额超过9999，就不允许在充值
				printf("[--%d--]\n", user[seting_user].card_money);
			draw_color(300,120,658,195,0xFFFFFF);
			show_money(340, 140, user[seting_user].card_money); //显示剩余金额;
		}
		if(x > 200 && y > 251 && x < 307 && y < 325)/*减金减*/
		{
			//金额减操作
			user[seting_user].card_money -= chance_val;
			if(user[seting_user].card_money < 0) 
					user[seting_user].card_money = 0; //如果金额减到0就不能再减
				printf("[--%d--]\n", user[seting_user].card_money);
			draw_color(300,120,658,195,0xFFFFFF);
			show_money(340, 140, user[seting_user].card_money); //显示剩余金额;
		}
	}
}


//刷卡线程
void *thread_run(void *argv)
{
	int i = 0; //临时记录用户位置 
	unsigned int cardid = 0;
	while(1)
	{
		if(rfid_read_card(&cardid) == -1)
		{
			sleep(1);
			continue;
		}
		i = search_user(&cardid);
		if(i != -1)
		{
			if(user[i].card_money < 2)
			{
				printf("余额不足\n");	
			}
			else{
				user[i].card_money -= 2;
				draw_color(312,43 ,655,106,0xFFFFFF);  //清空刷卡id位置
				draw_color(312,136,655,209,0xFFFFFF);  //清空刷卡金额位置
				draw_color(312,255,655,327,0xFFFFFF);  //清空剩余金额位置
				show_card_id(324, 64, user[i].card_id);	//显示卡id
				show_num16x32(344 ,164, 2, 0xFF0000);  //显示刷卡金额
				show_money(344, 278, user[i].card_money); //显示剩余金额
			}
			sleep(3);
		}
		else{
			printf("无效卡\n");
		}
		
	}
}


//刷卡
void swipe_card()
{
	int x, y;
	//进入刷卡界面
	lcd_draw_bmp(0, 0, "read_card.bmp");
	draw_color(312,43 ,655,106,0xFFFFFF);  //清空刷卡id位置
	draw_color(312,136,655,209,0xFFFFFF);  //清空刷卡金额位置
	draw_color(312,255,655,327,0xFFFFFF);  //清空剩余金额位置
	
	//启动线程检测卡片
	pthread_t id;
	pthread_create(&id, NULL, thread_run, NULL);
	pthread_detach(id);
	
	while(1)
	{
		get_x_y(&x,&y); //等待点击屏幕
		if(x > 700 && x < 800 && y > 380 && y < 480)/*xy坐标在退出区域内*/
		{
			pthread_cancel(id);
			usleep(30000);
			lcd_draw_bmp(0, 0, "main.bmp"); //先画主界面图
			break;
		}
	}
}

//数据读取文件中用户数据
void read_user_info()
{
	int i = 0, ret ;
	int fd = open("user_data/data.txt", O_RDWR|O_CREAT, 0777);
	if(fd == -1){
		perror("open user data error");
		return ;
	}
	do{
		ret = read(fd, &user[i++], sizeof(struct user_info));
		printf("已有用户%u\n", user[i-1].card_id);
		printf("金额%d\n", user[i-1].card_money);
	}
	while(ret != 0 && ret != -1);
	now_user = i-1;
}

//存储用户数据
void write_user_info()
{
	int i = 0;
	int fd = open("user_data/data.txt", O_RDWR);
	if(fd == -1)
		perror("open user data error");
	while(i < now_user){
		write(fd, &user[i++], sizeof(struct user_info));}
}


int main()
{
	int x, y;	
	//1  lcd初始化  触摸屏，初始化用户列表
	event_init();
	open_lcd();
	rfid_init();
	read_user_info();
	chance_val = 0; // 记录修改金额,充值面额
	seting_user = -1; //表示当前设置的用户
	
	lcd_draw_bmp(0, 0, "main.bmp"); //先画主界面图
	
	while(1)
	{
		get_x_y(&x,&y); //等待点击屏幕
		if(x > 100 && x < 262 && y > 133 && y < 299)
		{
			setting(); //进入金额充值
		}
		if(x > 421 && x < 593 && y > 127 && y < 304)
		{
			//进入刷卡
			swipe_card();
		}
		if(x > 700 && x < 800 && y > 400 && y < 480)
		{
			write_user_info();
			printf("system quit\n");
			break;
		}
	}	
}