#ifndef RFID_H
#define RFID_H


/*��ѯƥ�俨�û������ҵ��򷵻��û�λ��*/
int search_user(unsigned int *cardid);


//��ʼ��RFID
int rfid_init(void);

//�ر�RFID����
void close_rfid();


int rfid_read_card(unsigned int *cardid);


#endif //RFID_H