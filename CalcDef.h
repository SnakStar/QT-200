#ifndef CALCDEF_H
#define CALCDEF_H

#define CODE_DATA_SIZE    400      //定义条码采集数据点数
#define SCAN_DATA_SIZE    320      //定义荧光信号采集数据点数
#define SCAN_DATA_SIZE1   (320)     //定义荧光信号采集数据点数
#define SCAN_DATA_SIZE2   (440)     //定义荧光信号采集数据点数，二联卡的
#define SCAN_DATA_SIZE3   (440)     //定义荧光信号采集数据点数，三联卡的
#define SCAN_DATA_MAX     (440)
#define BARCODELENTH      14


//变量声明

#define MAXRECODENUM     500

#define MAX_CALNUM       (5)
#define CT_CALNUM        (0)       //默认计算方案, 质控峰在前，检测峰在后
#define TC_CALNUM        (1)       //检测峰在前，质控峰在后
#define CT2_CALNUM       (2)       //双联卡计算方案, 质控峰在前，检测峰在后
#define T2C_CALNUM       (3)       //双联卡计算方案, 检测峰在前，质控峰在后
#define CT3_CALNUM       (4)       //三联卡计算方案, 质控峰在前，检测峰在后
#define T3C_CALNUM       (5)       //三联卡计算方案, 检测峰在前，质控峰在后
#define NOTHRE_CALNUM    (10)      //直接在最大值位置两边积分

#define NO_TEST_NAME1    (0)
#define NO_TEST_NAME2    (1)
#define NO_TEST_NAME3    (2)

#define ID_TEST_NAME_LEN                (15)     //测试名称长度占用字节
#define ID_UNIT_LEN                     (15)     //测试单位长度占用字节
#define ID_TEST_VAL_LEN                 (1)      //测试结果小数点位数占用字节
#define ID_MIN_RESULT_LEN               (3)      //结果最小值占用字节
#define ID_MAX_RESULT_LEN               (3)      //结果最大值占用字节
#define ID_T_SIGNAL_LEN                 (1)      //检测峰积分长度占用字节
#define ID_ADJUST_POINT_LEN             (3*10)   //校准点占用字节
#define ID_ADJUST_A_LEN                 (4*9)    //校准斜率占用字节
#define ID_ADJUST_B_LEN                 (4*9)    //校准截距占用字节
#define ID_PARA_LEN                     (ID_TEST_NAME_LEN+ID_UNIT_LEN+ID_TEST_VAL_LEN+ID_MIN_RESULT_LEN+ID_MAX_RESULT_LEN+ID_T_SIGNAL_LEN+ID_ADJUST_POINT_LEN+ID_ADJUST_A_LEN+ID_ADJUST_B_LEN)//多联卡参数2和参数3占用的字节数

#define IDMESSAGELENTH                  148         //旧的ID卡存储的信息长度，此长度的ID卡信息没有包含计算方案和质控峰积分长度，但已经包含条码和日期信息
#define CT_IDMESSAGELENTH               (184)       //单联卡ID卡存储的信息长度,此长度的ID卡信息没有包含计算方案和质控峰积分长度以及新卡标志、新卡测试名称、测试单位、测试最大值
#define CT2_IDMESSAGELENTH              (CT_IDMESSAGELENTH+ID_PARA_LEN)    //双联卡ID卡存储的信息长度
#define CT3_IDMESSAGELENTH              (CT2_IDMESSAGELENTH+ID_PARA_LEN)   //三联卡ID卡存储的信息长度
#define MAX_IDMESSAGELENTH              (CT3_IDMESSAGELENTH+1)

#define ID_NO_CALNUM                    (148)                  //计算方案在ID信息的位置
#define ID_NO_CT_CIntegralLength2       (149)                  //质控峰积分长度在ID信息的位置
#define MAX_CSignallength               (250)                  //最大质控峰积分长度
#define ID_NO_NEW_CARD_FLAG             (150)                  //新卡标志在ID信息的位置
#define ID_NEW_CARD_TAG                 (0x10)                 //新卡标志

#define ID_NO_TEST1_NAME                (ID_NO_NEW_CARD_FLAG+1)                         //15位长度的测试名称1在ID信息的位置
#define ID_NO_TEST1_UNIT                (ID_NO_TEST1_NAME+ID_TEST_NAME_LEN)             //15位长度的测试单位1在ID信息的位置
#define ID_NO_TEST1_MAX_RESULT          (ID_NO_TEST1_UNIT+ID_UNIT_LEN)                  //3位长度的测试最大值1在ID信息的位置

#define ID_NO_TEST2_NAME                (CT_IDMESSAGELENTH)                             //测试名称2在ID信息的位置
#define ID_NO_TEST2_UNIT                (ID_NO_TEST2_NAME+ID_TEST_NAME_LEN)             //测试单位2在ID信息的位置
#define ID_NO_TEST2_VAL_LEN             (ID_NO_TEST2_UNIT+ID_UNIT_LEN)                  //测试结果小数点位数2在ID信息的位置
#define ID_NO_TEST2_MIN_RESULT          (ID_NO_TEST2_VAL_LEN+ID_TEST_VAL_LEN)           //测试最小值2在ID信息的位置
#define ID_NO_TEST2_MAX_RESULT          (ID_NO_TEST2_MIN_RESULT+ID_MIN_RESULT_LEN)      //测试最大值2在ID信息的位置
#define ID_NO_TEST2_T_SIGNAL            (ID_NO_TEST2_MAX_RESULT+ID_MAX_RESULT_LEN)      //测试检测峰积分长度2在ID信息的位置
#define ID_NO_TEST2_ADJUST_POINT        (ID_NO_TEST2_T_SIGNAL+ID_T_SIGNAL_LEN)          //测试校准点2在ID信息的位置
#define ID_NO_TEST2_ADJUST_A            (ID_NO_TEST2_ADJUST_POINT+ID_ADJUST_POINT_LEN)  //测试校准斜率2在ID信息的位置
#define ID_NO_TEST2_ADJUST_B            (ID_NO_TEST2_ADJUST_A+ID_ADJUST_A_LEN)          //测试校准截距2在ID信息的位置

#define ID_NO_TEST3_NAME                (ID_NO_TEST2_ADJUST_B+ID_ADJUST_B_LEN)          //测试名称3在ID信息的位置
#define ID_NO_TEST3_UNIT                (ID_NO_TEST3_NAME+ID_TEST_NAME_LEN)             //测试单位3在ID信息的位置
#define ID_NO_TEST3_VAL_LEN             (ID_NO_TEST3_UNIT+ID_UNIT_LEN)                  //测试结果小数点位数3在ID信息的位置
#define ID_NO_TEST3_MIN_RESULT          (ID_NO_TEST3_VAL_LEN+ID_TEST_VAL_LEN)           //测试最小值3在ID信息的位置
#define ID_NO_TEST3_MAX_RESULT          (ID_NO_TEST3_MIN_RESULT+ID_MIN_RESULT_LEN)      //测试最大值3在ID信息的位置
#define ID_NO_TEST3_T_SIGNAL            (ID_NO_TEST3_MAX_RESULT+ID_MAX_RESULT_LEN)      //测试检测峰积分长度3在ID信息的位置
#define ID_NO_TEST3_ADJUST_POINT        (ID_NO_TEST3_T_SIGNAL+ID_T_SIGNAL_LEN)          //测试校准点3在ID信息的位置
#define ID_NO_TEST3_ADJUST_A            (ID_NO_TEST3_ADJUST_POINT+ID_ADJUST_POINT_LEN)  //测试校准斜率3在ID信息的位置
#define ID_NO_TEST3_ADJUST_B            (ID_NO_TEST3_ADJUST_A+ID_ADJUST_A_LEN)          //测试校准截距3在ID信息的位置

#define MENU_INDEX_SHOW_RESULT          (10)

#define RECORDLENTH                     (2+7+5+15+15)                           //一条结果记录的长度长度
#define RECORD_NO_TEST_UNIT             (14)                                    //测试单位在结果记录的位置
#define RECORD_NO_TEST_NAME             (RECORD_NO_TEST_UNIT+ID_UNIT_LEN)  //测试名称在结果记录的位置

#define SHOW_RESULT_POS                 (20)                                //液晶屏显示测试结果的起始位置
#define SHOW_MAX_RESULT_LEN             (9)                                //液晶屏可以显示最大的测试结果长度
#define SHOW_UNIT_POS                   (30)                                //液晶屏显示测试单位的起始位置
#define SHOW_MAX_UNIT_LEN               (10)                                //液晶屏可以显示最大的测试单位长度


#define BARCODELENTH                    14              //一条条码记录的长度长度
#define COMMANDLENGTH                   8               //命令长度







#endif // CALCDEF_H
