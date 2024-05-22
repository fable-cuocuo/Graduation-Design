#include <SoftwareSerial.h>
//右路电机PWMA引脚5,左路电机PWMB引脚6，必须是有PWM功能的端口
#define PWMA 5
#define PWMB 6
// 右边电机引脚7-8，左边电机引脚10-11
#define AIN1 7
#define AIN2 8
#define BIN1 10
#define BIN2 11
// 循迹引脚 9-2-3-4，从车头方向的最左边开始排序
#define track1 2	//Arduino的9号引脚连接在循迹模块的D0口，以下同理
#define track2 9
#define track3 3
#define track4 4
// 设置Arduino软件串口，0-RX,1-TX
const int BT_RX = 12;
const int BT_TX = 13;
// Pin0为RX，接HC05的TXD
// Pin1为TX，接HC05的RXD
SoftwareSerial BT(BT_RX, BT_TX );
char val;
int right_speed=40;
int left_speed=40;
int flag=0;


int Sensor[4] = {0, 0, 0, 0}; //初始化循迹传感器检测的值

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 设置串口波特率为9600
  Serial.println("BT is ready!");// 测试与PC之间串口是否正常
  BT.begin(9600);// HC-05的AT模式默认通信波特率为38400
  Track_Init(); //循迹模块初始化
  Motor_Init(); //电机引脚初始化
  pinMode(LED_BUILTIN,OUTPUT);
  Motor_Toward(1);
}

void loop() {
  // Motor_Speed(50, 50);
  // put your main code here, to run repeatedly:
  // four();//四路循迹数据传送
  if(flag==1){
    Sensor_Read();  //不断地读取循迹模块的高低电平
    xun_ji(); //循迹函数
  }
  else{
    Motor_Speed(0,0);
  }
  // Print();  //打印循迹的高低电平

  if (Serial.available()) {
		val = Serial.read();
		BT.print(val); //将PC发来的数据存在val内，并发送给HC-05模块
    delay(5);
	}
	if (BT.available()) {
		val = BT.read();
    printf(val);
		Serial.println(val); //将HC-05模块发来的数据存在val内，并发送给PC
    delay(5);
    if(val=='1'){// 发送1
      digitalWrite(LED_BUILTIN,HIGH);// 小灯闪烁
      // delay(1000);
      // digitalWrite(LED_BUILTIN,LOW);
      // delay(1000);
      flag=1;
    }
    else{
      digitalWrite(LED_BUILTIN,LOW);
      flag=0;
    }
	}
}
 
void Track_Init()
{
  //循迹模块D0引脚初始化，设置为输入模式
  pinMode(track1, INPUT);
  pinMode(track2, INPUT);
  pinMode(track3, INPUT);
  pinMode(track4, INPUT);
}
 
void Motor_Init()
{
  //电机引脚初始化，设置为输出模式
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
}
void move(){  // 移动
  pinMode(AIN1, LOW);
  pinMode(AIN2, HIGH);
  pinMode(BIN1, LOW);
  pinMode(BIN2, HIGH);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
}
void Sensor_Read()
{
  Sensor [0] = digitalRead(track1); //检测到黑线为高电平（1），白线为低电平（0)
  Sensor [1] = digitalRead(track2);
  Sensor [2] = digitalRead(track3);
  Sensor [3] = digitalRead(track4);
}
 
void Motor_Toward(int toward)//控制电机的前行模式
{
  if(toward==0){//右轮停止，左轮停止  小车停
    digitalWrite(AIN1,0);
    digitalWrite(AIN2,0);
    digitalWrite(BIN1,0);
    digitalWrite(BIN2,0);
  }else if(toward==1){//右轮正转，左轮正转  小车前进
    digitalWrite(AIN1,1);
    digitalWrite(AIN2,0);
    digitalWrite(BIN1,1);
    digitalWrite(BIN2,0);
  }else if(toward==2){//右轮反转，左轮反转  小车倒退
    digitalWrite(AIN1,0);
    digitalWrite(AIN2,1);
    digitalWrite(BIN1,0);
    digitalWrite(BIN2,1);
  }
}
 
void Motor_Speed(int Right_Speed, int Left_Speed)
{
  analogWrite(PWMB, Right_Speed); //控制电机的速度，不同的车模，代码执行效果不同！
  analogWrite(PWMA, Left_Speed);
}

void mv_go(int Right_Speed, int Left_Speed){// 直行
  Motor_Toward(1);//设置前进模式
  Motor_Speed(Right_Speed, Left_Speed);
}

void xunji_right_slightly(int Right_Speed, int Left_Speed){// 小右转
  Motor_Toward(1);//设置前进模式
  Motor_Speed(Right_Speed*0.5, Left_Speed); //右轮速度 变小 左轮速度 不变
}

void xunji_left_slightly(int Right_Speed, int Left_Speed){// 小左转
  Motor_Toward(1);//设置前进模式
  Motor_Speed(Right_Speed, Left_Speed*0.5); //右轮速度 不变 左轮速度 变小
}

void xunji_right_severely(int Right_Speed, int Left_Speed){// 大右转
  Motor_Toward(1);//设置前进模式
  Motor_Speed(Right_Speed*0.1, Left_Speed); //右轮速度 变小 左轮速度 不变
}
void xunji_left_severely(int Right_Speed, int Left_Speed){// 大左转
  Motor_Toward(1);//设置前进模式
  Motor_Speed(Right_Speed, Left_Speed*0.1); //右轮速度 不变 左轮速度 变小
}

void xun_ji() //小车的速度可根据实际地图进行调节
{
  /*if (Sensor [0] == 1  && Sensor [1] == 1 && Sensor [2] == 0 && Sensor [3] == 1){  //1-1-0-1 小右转  灯不亮是1 检测到黑线灯会亮即为0
    Motor_Toward(1);//设置前进模式
    Motor_Speed(50, 120); //右轮速度 50 左轮速度 120
  }else if (Sensor [0] == 1 && Sensor [1] == 0 && Sensor [2] == 1 && Sensor [3] == 1){  //1-0-1-1 小左转
    Motor_Toward(1);//设置前进模式
    Motor_Speed(120, 50); //右轮速度 120 左轮速度 50
  }else if(Sensor [0] == 1 && Sensor [1] == 0 && Sensor [2] == 0 && Sensor [3] == 1){  //直走
    Motor_Toward(1);//设置前进模式
    Motor_Speed(50, 50);}
  if (Sensor [0] == 1  && Sensor [1] == 1 && Sensor [2] == 0 && Sensor [3] == 0){  //1-1-0-0 大右转  灯不亮是1 检测到黑线灯会亮即为0
    Motor_Toward(1);//设置前进模式
    Motor_Speed(50, 150); //右轮速度 50 左轮速度 120
  }else if (Sensor [0] == 1 && Sensor [1] == 0 && Sensor [2] == 1 && Sensor [3] == 1){  //0-0-1-1 大左转
    Motor_Toward(1);//设置前进模式
    Motor_Speed(150, 50); //右轮速度 120 左轮速度 50
  }*/

  if (Sensor [0]==1 && Sensor [1]==0 && Sensor [2]==0 && Sensor [3]==1)//1001 直行
    {
        mv_go(right_speed,left_speed);
    }
    else if(Sensor [0]==1 && Sensor [1]==1 && Sensor [2]==0 && Sensor [3]==0)//1100
    {
        xunji_right_slightly(right_speed,left_speed);
    }
    else if(Sensor [0]==1 && Sensor [1]==1 && Sensor [2]==0 && Sensor [3]==1)//1101
    {
        xunji_right_slightly(right_speed,left_speed);
    }
    else if(Sensor [0]==1 && Sensor [1]==1 && Sensor [2]==1 && Sensor [3]==0)//1110
    {
        xunji_right_severely(right_speed,left_speed);
    }
    else if(Sensor [0]==0 && Sensor [1]==0 && Sensor [2]==1 && Sensor [3]==1)//0011
    {
        xunji_left_slightly(right_speed,left_speed);
    }
    else if(Sensor [0]==1 && Sensor [1]==0 && Sensor [2]==1 && Sensor [3]==1)//1011
    {
        xunji_left_slightly(right_speed,left_speed);
    }
    else if(Sensor [0]==0 && Sensor [1]==1 && Sensor [2]==1 && Sensor [3]==1)//0111
    {
        xunji_left_severely(right_speed,left_speed);
    }
    else if(Sensor [0]==0 && Sensor [1]==0 && Sensor [2]==0 && Sensor [3]==1)//0001
    {
        xunji_left_severely(right_speed,left_speed);
        delay(100);
    }
    else if(Sensor [0]==1 && Sensor [1]==0 && Sensor [2]==0 && Sensor [3]==0)//1000
    {
        xunji_right_severely(right_speed,left_speed);
        delay(100);
    }
    else if(Sensor [0]==1 && Sensor [1]==1 && Sensor [2]==1 && Sensor [3]==1)//1111
    {
        mv_go(right_speed,left_speed);
        delay(100);
    }
}
 
void Print()	//在串口打印循迹的高低电平
{
  Serial.print(Sensor [0]);
  Serial.print("---");
  Serial.print(Sensor [1]);
  Serial.print("---");
  Serial.print(Sensor [2]);
  Serial.print("---");
  Serial.println(Sensor [3]);
}

void four(){
  int data[4];                //定义一个数组data[]包含四个数据，读取来自9,2,3,4引脚的信号存储到这个数组中，而9,2,3,4引脚连接的就是四个传感器，因此这个数组的功能就是记录四个传感器的信号，由下面的程序可以知道，通过对这几个传感器信号的判断，并根据这几个不同的情况作出不同的反应
  data[0] = digitalRead(9);
  data[1] = digitalRead(2);
  data[2] = digitalRead(3);
  data[3] = digitalRead(4);
  Serial.print("引脚1=");
  Serial.println(data[0]);
  Serial.print("");
  Serial.print("引脚2=");
  Serial.println(data[1]);
  Serial.print("");
  Serial.print("引脚3=");
  Serial.println(data[2]);
  Serial.print("");
  Serial.print("引脚4=");
  Serial.println(data[3]);
  Serial.print("");
  delay(1000);
}