#ifndef RFID_H
#define RFID_H


/*查询匹配卡用户，查找到则返回用户位置*/
int search_user(unsigned int *cardid);


//初始化RFID
int rfid_init(void);

//关闭RFID串口
void close_rfid();


int rfid_read_card(unsigned int *cardid);


#endif //RFID_H