/*----2016/11/30----
1  在void rqtTECswitch(char channel,char c)函数增加 thermoPara_LD.en_tec = c; 串口指令会觉得否启动温控算法
2  将gs_protocol.c char temp[62];	//32 -> 62 可以防止TEC标志位不再被sprintf修改为0x3a或可能其他值
3  在void writeDAC(unsigned char HW, unsigned short value)函数增加设置电流大于0.1会点亮LED，类似于出光指示灯
3  在main函数中增加run指示灯（ptb1）
4  去除module_ctrl_task任务，会影响到出光按钮
5  在main函数中取消capture_init的初始化 频率脉冲
if(!(GPIO_ReadBit(EN_TEC) ==0x00 && ((~c)&0x01)==0x01))
*/


/*-2016/12/30-version:1711
1	取消串口发送fflush()函数。防止设置电流等参数被修改
2 将1W 30W 60W项目代码合并成一个版本。方便维护
*/