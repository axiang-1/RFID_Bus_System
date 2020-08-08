#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
//    /usr/include/linux/input.h


int touch_fd;
//初始化触摸屏
int event_init()
{
	
	touch_fd = open("/dev/input/event0", O_RDWR);
	if(touch_fd ==-1)
	{
		perror("open event0 failed!\n");
		return -1;
	}
	return 0;
}
//触摸事件
void get_x_y(int *x,int *y)
{
	int  pressure;
	struct input_event events;//存储报告
	while(1)
	{
		read(touch_fd, &events, sizeof(events)); //-----------read具有阻塞等待
		if(events.type == EV_ABS)//代表是触摸屏事件
		{
			if(events.code == ABS_X)//x坐标
				*x = events.value;
			if(events.code == ABS_Y)//y坐标
				*y = events.value;
			if(events.code == ABS_PRESSURE)//压力
			{
				pressure = events.value;
				if(pressure == 0)
				{
					printf("x=%d, y=%d\n", *x, *y);
					break;
				}
			}
		}
		if(events.type == EV_KEY && events.code == BTN_TOUCH)
		{
			pressure = events.value;
			if(pressure == 0)
			{
				printf("x=%d, y=%d\n", *x, *y);
				break;
			}
		}
	}	
}
