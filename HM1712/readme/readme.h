/*----2016/11/30----
1  ��void rqtTECswitch(char channel,char c)�������� thermoPara_LD.en_tec = c; ����ָ�����÷������¿��㷨
2  ��gs_protocol.c char temp[62];	//32 -> 62 ���Է�ֹTEC��־λ���ٱ�sprintf�޸�Ϊ0x3a���������ֵ
3  ��void writeDAC(unsigned char HW, unsigned short value)�����������õ�������0.1�����LED�������ڳ���ָʾ��
3  ��main����������runָʾ�ƣ�ptb1��
4  ȥ��module_ctrl_task���񣬻�Ӱ�쵽���ⰴť
5  ��main������ȡ��capture_init�ĳ�ʼ�� Ƶ������
if(!(GPIO_ReadBit(EN_TEC) ==0x00 && ((~c)&0x01)==0x01))
*/


/*-2016/12/30-version:1711
1	ȡ�����ڷ���fflush()��������ֹ���õ����Ȳ������޸�
2 ��1W 30W 60W��Ŀ����ϲ���һ���汾������ά��
*/