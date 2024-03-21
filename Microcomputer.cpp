#include <stdio.h>
#include <stdlib.h>
#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSPI.h"
#include "LCD_Driver2.h"
#include "Image_Process.h"
#include "Driver\DrvRTC.h"
#include "math.h"
#include "ScanKey.h"
#include "Seven_Segment2.h"
#include "LCD_Driver.h"
#include "DrvGPIO.h"
#include "DrvTIMER.h"
#include "DrvSYS.h"

#define BAUDRATE 9600
#define PI 3.1415926535

uint8_t keynumber=0;
uint8_t keypress=0;
uint8_t keystatus=0xFF;
uint8_t keybuffer[10];
uint8_t keybufferptr=0;

extern  char Clock[];
char date_array[9]={'0','0','/','0','0','/','0','0',0};
char time_array[9]={'0','0',':','0','0',':','0','0',0};
char t_flag=1;
unsigned int close_buzzer=0;
extern unsigned char DisplayBuf[8*128];
unsigned char picture[128*8];

void draw_LCD(unsigned char *buffer);
void DrawTime(int CenterX, int CenterY);

void clearclock(int nr, int nc, unsigned char * buf);
void RTC_IRQHandler(void); //RTC INT Function
void Init_LED();
void Disable_Buzzer(int set);
void Square(int x1, int y1,int color);
void seg_display(int16_t value);
void Delay(int count);
void InitADC(void);
void InitPWM(void);
void Scankeypad(void);
void TMR_Callback(void) ;
void InitTimer0(uint32_t freq);
void PWM_Freq(uint32_t PWM_frequency, uint8_t PWM_duty);
uint32_t Tone_Freq(uint8_t note);
void InitHCLK12M(void);

int answer,K1=0;
int b[9]={0},d[9]={0};   //////  判斷 方形  米字
int C=0,choose;
int R=0;   ////////   判斷改圖案後的變數
int a[9]={0};
int K,I=0;   //////  I鎖中斷    K跑回圈
int  y2=0,i1,s1=0,y3=0;  
 int z[59]={80,77,77,0,78,75,75,0,73,75,77,78,80,80,80,0,80,77,77,0,78,75,75,0,73,77,80,80,73,0,
                   75,75,75,75,75,77,78,0,77,77,77,77,77,78,80,0,80,77,77,0,78,75,75,0,73,77,80,80,73};
 int z1[41]={82,84,85,89,87,0,84,87,80,87,85,0,82,84,85,89,87,85,89,0,
                 82,84,85,89,87,0,92,87,90,89,87,89,87,0,82,84,85,89,87,85,85};
 int z2[68]={80,87,87,87,89,0,87,85,80,82,0,85,85,85,87,89,0,
                     87,87,87,89,0,87,85,80,82,0,85,85,85,89,89,87,0,
                     80,87,87,87,89,0,87,85,80,82,0,85,85,85,85,89,0,
                     87,87,87,85,0,87,87,87,85,0,82,85,85,0,82,85,85};
 int w=0;

void EINT1Callback(void)
{
  // I=1 OOXX  I=2 接球  I=3 輸掉後禁止按中斷
  if(I==1){
  int key=0,temp_key=0;
  while(1){
  Disable_Buzzer(0);
  clr_all_pannal();
  while(1){
  print_lcd(0,"Want to ");
  print_lcd(1,"change image ?");
  print_lcd(2,"Button 1 YES");
  print_lcd(3,"Button 2 NO");
  key=Scankey();
  if(key!=temp_key && key!=0){
    if(key==1){answer=0;break;}
  else if(key==2){answer=1;break;}
  }
  }
  if(answer==0){
  clr_all_pannal();
  print_lcd(0,"Player1 is Sq");
  print_lcd(1,"Player2 is *");
  DrvSYS_Delay(335000);
  DrvSYS_Delay(335000);
  DrvSYS_Delay(335000);
  DrvSYS_Delay(335000);}
  if(answer==0){R=1;break;}
  if(answer==1)break;
  }
    
  if(answer==1){ //不改圖
    clr_all_pannal();
    print_lcd(0,"EXIT!!");
    DrvSYS_Delay(335000);
    clr_all_pannal();
    LineBresenham(0,21,128,21,1);  //第一條橫線
    LineBresenham(0,42,128,42,1);  //第二條橫線
    LineBresenham(42,0,42,64,1);   //第一條直線
    LineBresenham(84,0,84,64,1);   //第二條直線
    draw_LCD(DisplayBuf);
   
    if(C%2==0){
    if( b[0]==1&&C==0){CircleBresenham(21,10,7,1);}
    if( b[1]==1&&C==1){CircleBresenham(63,10,7,1);}
    if( b[2]==1&&C==2){CircleBresenham(106,10,7,1);}
    if( b[3]==1&&C==3){CircleBresenham(21,31,7,1);}
    if( b[4]==1&&C==4){CircleBresenham(63,31,7,1);}
    if( b[5]==1&&C==5){CircleBresenham(106,31,7,1);}
    if( b[6]==1&&C==6){CircleBresenham(21,53,7,1);}
    if( b[7]==1&&C==7){CircleBresenham(63,53,7,1);}
    if( b[8]==1&&C==8){CircleBresenham(106,53,7,1);}
    }
    
    else if(C%2!=0){ 
    if(d[0]==2&&C==1){
         LineBresenham(11,2,31,19,1);
         LineBresenham(31,2,11,19,1); }
    if(d[1]==2&&C==2){ LineBresenham(53,2,73,19,1);
         LineBresenham(73,2,53,19,1);}
    if(d[2]==2&&C==3){ LineBresenham(96,2,116,19,1);
         LineBresenham(116,2,96,19,1);}
    if(d[3]==2&&C==4){ LineBresenham(11,23,31,40,1);
         LineBresenham(31,23,11,40,1);}
    if(d[4]==2&&C==5){ LineBresenham(53,23,73,40,1);
         LineBresenham(73,23,53,40,1);}
    if(d[5]==2&&C==6){  LineBresenham(96,23,116,40,1);
         LineBresenham(116,23,96,40,1);}
    if(d[6]==2&&C==7){ LineBresenham(11,45,31,62,1);
         LineBresenham(31,45,11,62,1);}
    if(d[7]==2&&C==8){LineBresenham(53,45,73,62,1);
         LineBresenham(73,45,53,62,1);}
    if(d[8]==2&&C==9){LineBresenham(96,45,116,62,1);
         LineBresenham(116,45,96,62,1);}
    }     
    draw_LCD(DisplayBuf);
  }
  if(answer==0){ //改圖
    clr_all_pannal(); 
    clr_part_pannal(0,128,0,8);
    draw_part_LCD(DisplayBuf,0,128,0,8);
    LineBresenham(0,21,128,21,1);  //第一條橫線
    LineBresenham(0,42,128,42,1);  //第二條橫線
    LineBresenham(42,0,42,64,1);   //第一條直線
    LineBresenham(84,0,84,64,1);   //第二條直線}
    draw_LCD(DisplayBuf);
    for(K=1;K<=9;K++){
        if(a[K-1]==1){
            if(K==1){
         Square(15,6,1);    //方1
        }
        else if(K==2){
         Square(58,6,1);   //方2
        }
        else if(K==3){
          Square(101,6,1); //方3
        }
        else if(K==4){
          Square(15,27,1);   //方4
        }
        else if(K==5){
          Square(58,27,1);  //方5
        }
        else if(K==6){
          Square(101,27,1);  //方6
        }
        else if(K==7){
          Square(15,48,1);  //方7
        }
        else if(K==8){
         Square(58,48,1);    //方8
        }
        else if(K==9){
         Square(101,48,1);   //方9
        }
         draw_LCD(DisplayBuf);
        }
        else if(a[K-1]==2){
            if(K==1){
         LineBresenham(11,2,31,19,1);
         LineBresenham(11,11,31,11,1);
         LineBresenham(31,2,11,19,1);   //*1
        }
        else if(K==2){
         LineBresenham(53,2,73,19,1);
         LineBresenham(53,11,73,11,1);
         LineBresenham(73,2,53,19,1);   //*2
        }
        else if(K==3){
         LineBresenham(96,2,116,19,1);
         LineBresenham(96,11,116,11,1);
         LineBresenham(116,2,96,19,1);  //*3
        }
        else if(K==4){
         LineBresenham(11,23,31,40,1);
         LineBresenham(11,32,31,32,1);
         LineBresenham(31,23,11,40,1);  //*4
        }
        else if(K==5){
         LineBresenham(53,23,73,40,1);
         LineBresenham(53,32,73,32,1);
         LineBresenham(73,23,53,40,1);  //*5
        }
        else if(K==6){
         LineBresenham(96,23,116,40,1);
         LineBresenham(96,32,116,32,1);
         LineBresenham(116,23,96,40,1); //*6
        }
        else if(K==7){
         LineBresenham(11,45,31,62,1);
         LineBresenham(11,54,31,54,1);
         LineBresenham(31,45,11,62,1);  //*7
        }
        else if(K==8){
         LineBresenham(53,45,73,62,1);
         LineBresenham(53,54,73,54,1);
         LineBresenham(73,45,53,62,1);  //*8
        }
        else if(K==9){
         LineBresenham(96,45,116,62,1);
         LineBresenham(96,54,116,54,1);
         LineBresenham(116,45,96,62,1); //*9
        }
         draw_LCD(DisplayBuf);   
        }      
      }   
  }
  }
  if(I==2){
      int key=0,temp_key=0;
      clr_all_pannal();
        while(1){
        print_lcd(0,"change speed !");
        print_lcd(1,"Level 1");
        print_lcd(2,"Level 2");
        key=Scankey();
        if(key!=temp_key && key!=0){
        if(key==1){K1=1;break;}
        else if(key==2){K1=3;break;}
            }
        }
  }
}

int main(void)
{
    UNLOCKREG();
    DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);
    SysTimerDelay(5000);
    /* HCLK clock source. 0: external 12MHz; 4:internal 22MHz RC oscillator */
    DrvSYS_SelectHCLKSource(0);  
    LOCKREG();    
     /*Interrupt Set*/
    DrvGPIO_Open(E_GPB, 15, E_IO_INPUT);
    DrvGPIO_EnableDebounce(E_GPB, 15);
    DrvGPIO_SetDebounceTime(4,E_DBCLKSRC_10K);

    /* Configure EINT1 interrupt */
    DrvGPIO_EnableEINT1(E_IO_FALLING, E_MODE_EDGE, EINT1Callback);

    /*LED Set*/
    DrvGPIO_Open(E_GPA, 13, E_IO_OUTPUT);
    DrvGPIO_Open(E_GPA, 14, E_IO_OUTPUT);
    DrvGPIO_SetBit(E_GPA, 13);
    DrvGPIO_SetBit(E_GPA, 14);       

    /*LCD Set*/
    Initial_pannel();
    clr_all_pannal();
    
    InitPWM();
    InitADC();

   
    draw_LCD(DisplayBuf);
    
    
    DrvGPIO_ClrBit(E_GPD,14);  //LCD亮
  
    int ten=0,one=0;
    int c=0;
    int x=0,k=1000;
    int count=0,temp_key=0,key=0;
   
    print_lcd(0,"Set countdown");
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
    while(1)
    {
      print_lcd(0,"Countdown ten :");
      ten = Scankey();     
      if(ten!=0){ Show_Word(1,0,ten+48);break;}
    }
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
    while(1)
    {
      print_lcd(0,"Countdown one :");
      one = Scankey();     
      if(one!=0){Show_Word(1,0,one+48);break;}
    }
      DrvSYS_Delay(335000);
      DrvSYS_Delay(335000);
      clr_all_pannal();
      
      DrvRTC_SetTickMode (DRVRTC_TICK_1_2_SEC) ;
      
    print_lcd(0,"Set password");
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
   
    while(1)
    {
      key = Scankey();     
      if(key!=temp_key && key!=0)
      {
        Show_Word(0,count,key+48);
        
        count++;
        x=x+k*key;
        
        k/=10;
      }
      temp_key=key;      
      if(count>3){break;}
    }
    
 
    DrvSYS_Delay(335000);
    clr_all_pannal();    
    
    print_lcd(0,"Enter password");
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
    
     Init_RTC(ten,one);
     while(1){
     int k=1000,sum=0;
     count=0,temp_key=0,key=0;
     int value=100;
     
     while(1)
      {
      get_time(time_array);
      print_lcd(1, time_array);
      key = Scankey();       
      if(key!=temp_key && key!=0)
      {
        sum=sum+key*k;
        Show_Word(0,count,key+48);        
        k=k/10;
        count++;
      }      
      temp_key=key;     
      if(count>3){break;}     
      }
     while(value){
       seg_display(sum);
       value--;
     }   
     close_seven_segment();
     
      DrvSYS_Delay(335000);
      clr_all_pannal();      
      if(sum==x){print_lcd(0,"TRUE");
      Disable_Buzzer(1);DrvSYS_Delay(200000);Disable_Buzzer(0);DrvSYS_Delay(200000);
      Disable_Buzzer(1);DrvSYS_Delay(200000);Disable_Buzzer(0);
      break;}        
      else{print_lcd(0,"FAIL");c++;DrvSYS_Delay(335000);clr_all_pannal();}
      if(c==3){
        DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"LOCK !");
        Disable_Buzzer(1);
        while(1){}
          }
        }
        
        
    Init_RTC(6,1);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
    
    draw_LCD(picture);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);
    clr_all_pannal();
    
    temp_key=0;
    k=0;
    key=0;
    int choose1;
    while(1)
    {
      print_lcd(0,"choose game ?");
      print_lcd(1,"game 1 is 1 !");
      print_lcd(2,"game 2 is 2 !");
      print_lcd(3,"music is 3  !");
      key = Scankey();     
      if(key!=temp_key && key!=0)
      {
        if(key==1){choose1=0;break;}
        else if(key==2){choose1=1;break;}
         else if(key==3){choose1=2;break;}
              }    
          }
     temp_key=0;
    k=0;
    key=0;
    clr_all_pannal();
    if(choose1==0){
     while(1)
    {
      print_lcd(0,"choose O or X ?");
      print_lcd(1,"Button 1 is O !");
      print_lcd(2,"Button 2 is X !");
      key = Scankey();     
      if(key!=temp_key && key!=0)
      {
        if(key==1){choose=0;break;}
        else if(key==2){choose=1;break;}
              }    
          }
    clr_all_pannal();
  
    DrvSYS_Delay(335000);
    DrvSYS_Delay(335000);    
    
    LineBresenham(0,21,128,21,1);  //第一條橫線
    LineBresenham(0,42,128,42,1);  //第二條橫線
    LineBresenham(42,0,42,64,1);   //第一條直線
    LineBresenham(84,0,84,64,1);   //第二條直線

  
  draw_LCD(DisplayBuf);
  
  int i=0,j=0,s=0;
  temp_key=0;
  k=0;
  C=choose;
  
  I=1;    ////鎖中斷
  while(1){  //////////////////////////////////////////////////      GAME
    k = Scankey();
    if(a[k-1]!=0)continue;
    a[k-1] = C%2 + 1;
    if(k!=temp_key && k!=0){
      if(C%2==0){
        if(R==1){
          if(k==1){
         Square(15,6,1);    //方1
        }
        else if(k==2){
         Square(58,6,1);   //方2
        }
        else if(k==3){
          Square(101,6,1); //方3
        }
        else if(k==4){
          Square(15,27,1);   //方4
        }
        else if(k==5){
          Square(58,27,1);  //方5
        }
        else if(k==6){
          Square(101,27,1);  //方6
        }
        else if(k==7){
          Square(15,48,1);  //方7
        }
        else if(k==8){
         Square(58,48,1);    //方8
        }
        else if(k==9){
         Square(101,48,1);   //方9
        }
         draw_LCD(DisplayBuf);        
        }
        else if(R!=1){
          if(k==1){
         CircleBresenham(21,10,7,1); 
         b[0]=1;//圓1
        }
        else if(k==2){
         CircleBresenham(63,10,7,1); 
         b[1]=1;//圓2
        }
        else if(k==3){
          CircleBresenham(106,10,7,1); 
          b[2]=1;//圓3
        }
        else if(k==4){
          CircleBresenham(21,31,7,1); 
          b[3]=1;//圓4
        }
        else if(k==5){
          CircleBresenham(63,31,7,1);
          b[4]=1;//圓5
        }
        else if(k==6){
          CircleBresenham(106,31,7,1);
          b[5]=1;//圓6
        }
        else if(k==7){
          CircleBresenham(21,53,7,1); 
          b[6]=1;//圓7
        }
        else if(k==8){
         CircleBresenham(63,53,7,1); 
         b[7]=1;//圓8
        }
        else if(k==9){
         CircleBresenham(106,53,7,1);
         b[8]=1;//圓9
        }
        }
         draw_LCD(DisplayBuf);         
      }
      else if(C%2!=0){
        if(R==1){
          if(k==1){
         LineBresenham(11,2,31,19,1);
         LineBresenham(11,11,31,11,1);
         LineBresenham(31,2,11,19,1);   //*1
        }
        else if(k==2){
         LineBresenham(53,2,73,19,1);
         LineBresenham(53,11,73,11,1);
         LineBresenham(73,2,53,19,1);   //*2
        }
        else if(k==3){
         LineBresenham(96,2,116,19,1);
         LineBresenham(96,11,116,11,1);
         LineBresenham(116,2,96,19,1);  //*3
        }
        else if(k==4){
         LineBresenham(11,23,31,40,1);
         LineBresenham(11,32,31,32,1);
         LineBresenham(31,23,11,40,1);  //*4
        }
        else if(k==5){
         LineBresenham(53,23,73,40,1);
         LineBresenham(53,32,73,32,1);
         LineBresenham(73,23,53,40,1);  //*5
        }
        else if(k==6){
         LineBresenham(96,23,116,40,1);
         LineBresenham(96,32,116,32,1);
         LineBresenham(116,23,96,40,1); //*6
        }
        else if(k==7){
         LineBresenham(11,45,31,62,1);
         LineBresenham(11,54,31,54,1);
         LineBresenham(31,45,11,62,1);  //*7
        }
        else if(k==8){
         LineBresenham(53,45,73,62,1);
         LineBresenham(53,54,73,54,1);
         LineBresenham(73,45,53,62,1);  //*8
        }
        else if(k==9){
         LineBresenham(96,45,116,62,1);
         LineBresenham(96,54,116,54,1);
         LineBresenham(116,45,96,62,1); //*9
        }
         draw_LCD(DisplayBuf);
        
        }
        else if(R!=1){ 
        if(k==1){
         LineBresenham(11,2,31,19,1);
         LineBresenham(31,2,11,19,1); 
         d[0]=2;//X1
        }
        else if(k==2){
         LineBresenham(53,2,73,19,1);
         LineBresenham(73,2,53,19,1);
         d[1]=2;//X2
        }
        else if(k==3){
         LineBresenham(96,2,116,19,1);
         LineBresenham(116,2,96,19,1); 
         d[2]=2;//X3
        }
        else if(k==4){
         LineBresenham(11,23,31,40,1);
         LineBresenham(31,23,11,40,1);
         d[3]=2;//X4
        }
        else if(k==5){
         LineBresenham(53,23,73,40,1);
         LineBresenham(73,23,53,40,1);
         d[4]=2;//X5
        }
        else if(k==6){
         LineBresenham(96,23,116,40,1);
         LineBresenham(116,23,96,40,1);
         d[5]=2;//X6
        }
        else if(k==7){
         LineBresenham(11,45,31,62,1);
         LineBresenham(31,45,11,62,1);
         d[6]=2;//X7
        }
        else if(k==8){
         LineBresenham(53,45,73,62,1);
         LineBresenham(73,45,53,62,1);
         d[7]=2;//X8
        }
        else if(k==9){
         LineBresenham(96,45,116,62,1);
         LineBresenham(116,45,96,62,1);
         d[8]=2;//X9
        }
        }
         draw_LCD(DisplayBuf);         
      }
      temp_key=k;
      if(C>=4){                             //判斷勝利      
      for(i=0;i<7;i+=3){
          s=0;
         if(a[i]==1 && a[i]==a[i+1] && a[i+1]==a[i+2]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player1 wins !");s++;break;}
         else if(a[i]==2 && a[i]==a[i+1] && a[i+1]==a[i+2]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player2 wins !");s++;break;}  //橫的        
      }
      if(s!=0){break;}
      s=0;
      for(j=0;j<3;j++){
        s=0;
        if(a[j]==1 && a[j]==a[j+3] && a[j+3] == a[j+6]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player1 wins !");s++;break;}
        else if(a[j]==2 && a[j] == a[j+3] && a[j+3] == a[j+6]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player2 wins !");s++;break;}  //直的       
      }
      if(s!=0){break;}
      if(C%2==0 && a[0]==a[4]&&a[4]==a[8]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player1 wins !");break;}                          // O 斜的
      else if(C%2==0 && a[2] == a[4] && a[4] == a[6]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player1 wins !");break;}            // O 斜的
      else if(C%2!=0 && a[0] == a[4] && a[4] == a[8]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player2 wins !");break;}            // X 斜的
      else if(C%2!=0 && a[2] == a[4] && a[4] == a[6]){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"Player2 wins !");break;}            // X 斜的
      else if(C>=8){DrvSYS_Delay(335000);clr_all_pannal();print_lcd(0,"No winner !");break;}                  //  超過8次
    }    
    C++;  
    } 
  }                                          ////////////////////////      GAME
  int clock=3;
  while(clock){
  Disable_Buzzer(1);DrvSYS_Delay(100000);Disable_Buzzer(0);DrvSYS_Delay(100000);
Disable_Buzzer(1);DrvSYS_Delay(100000);Disable_Buzzer(0);DrvSYS_Delay(100000);
Disable_Buzzer(1);DrvSYS_Delay(100000);Disable_Buzzer(0);DrvSYS_Delay(100000);
Disable_Buzzer(1);DrvSYS_Delay(100000);Disable_Buzzer(0);DrvSYS_Delay(100000);
DrvSYS_Delay(300000);
clock--;
  }
  int p=3;
  while(p){
        
        DrvGPIO_ClrBit(E_GPA,12); 
    	DrvGPIO_SetBit(E_GPA,13); 
    	DrvGPIO_SetBit(E_GPA,14); 	
        
        DrvGPIO_ClrBit(E_GPC,12);
        DrvGPIO_ClrBit(E_GPC,13);
        DrvGPIO_SetBit(E_GPC,14);
        DrvGPIO_SetBit(E_GPC,15);
        DrvSYS_Delay(335000);			  
	
	 
    	DrvGPIO_SetBit(E_GPA,12);	
    	DrvGPIO_ClrBit(E_GPA,13); 
    	DrvGPIO_SetBit(E_GPA,14); 	
        
        DrvGPIO_SetBit(E_GPC,12);
        DrvGPIO_ClrBit(E_GPC,13);
        DrvGPIO_ClrBit(E_GPC,14);
        DrvGPIO_SetBit(E_GPC,15);
           DrvSYS_Delay(335000);
                
        DrvGPIO_SetBit(E_GPA,12);	
    	DrvGPIO_SetBit(E_GPA,13); 
    	DrvGPIO_ClrBit(E_GPA,14); 	
        
        DrvGPIO_SetBit(E_GPC,12);
        DrvGPIO_SetBit(E_GPC,13);
        DrvGPIO_ClrBit(E_GPC,14);
        DrvGPIO_ClrBit(E_GPC,15);
            DrvSYS_Delay(335000);
        DrvGPIO_ClrBit(E_GPA,12);	
    	DrvGPIO_ClrBit(E_GPA,13); 
    	DrvGPIO_ClrBit(E_GPA,14); 	
        
        DrvGPIO_ClrBit(E_GPC,12);
        DrvGPIO_ClrBit(E_GPC,13);
        DrvGPIO_ClrBit(E_GPC,14);
        DrvGPIO_ClrBit(E_GPC,15);
             DrvSYS_Delay(335000);
                
        DrvGPIO_SetBit(E_GPA,12);	
    	DrvGPIO_SetBit(E_GPA,13); 
    	DrvGPIO_SetBit(E_GPA,14); 	
        
        DrvGPIO_SetBit(E_GPC,12);
        DrvGPIO_SetBit(E_GPC,13);
        DrvGPIO_SetBit(E_GPC,14);
        DrvGPIO_SetBit(E_GPC,15);
            DrvSYS_Delay(335000);
                  
        DrvGPIO_ClrBit(E_GPA,12);	
    	DrvGPIO_ClrBit(E_GPA,13); 
    	DrvGPIO_ClrBit(E_GPA,14); 	
        
        DrvGPIO_ClrBit(E_GPC,12);
        DrvGPIO_ClrBit(E_GPC,13);
        DrvGPIO_ClrBit(E_GPC,14);
        DrvGPIO_ClrBit(E_GPC,15);
		DrvSYS_Delay(335000);
         
        DrvGPIO_SetBit(E_GPA,12);	
    	DrvGPIO_SetBit(E_GPA,13); 
    	DrvGPIO_SetBit(E_GPA,14); 	
        
        DrvGPIO_SetBit(E_GPC,12);
        DrvGPIO_SetBit(E_GPC,13);
        DrvGPIO_SetBit(E_GPC,14);
        DrvGPIO_SetBit(E_GPC,15);
               DrvSYS_Delay(335000);
                p--;
  }
    }
    if(choose1==1){     
      I=2;
      while(1){
     // y2=0,i1,s1=0,y3=0;  
      while(1)
	{ 
             for(y2=0;y2<=60;){  	
		PWMA->CMR0=(4095-ADC->ADDR[7].RSLT)*2.42857+3314;	
		ADC->ADCR.ADST=1;
                CircleBresenham(60,10+y2,7,1);
                draw_LCD(DisplayBuf); 
                clr_part_pannal(0,128,0,8);
                draw_part_LCD(DisplayBuf,0,128,0,8);
                y2+=1+K1;
                if((10+y2)>56){
                  I=3;
                  clr_all_pannal();
                  while(1){print_lcd(0,"LOSE !");}
                }
                if(PWMA->CMR0>5000){s1=1;break;}
                    }
             if(s1==1)break;
        }///////////下降+轉盤
    
       if(s1==1)       
       for(y3=0;y3<=60;){
         PWMA->CMR0=(4095-ADC->ADDR[7].RSLT)*2.42857+3314;	
	ADC->ADCR.ADST=1;
          CircleBresenham(60,10+y2-y3,7,1);
          draw_LCD(DisplayBuf); 
          clr_part_pannal(0,128,0,8);
          draw_part_LCD(DisplayBuf,0,128,0,8);
          y3+=1+K1;
          if((10+y2-y3)<10){
          clr_part_pannal(0,128,0,8);
          draw_part_LCD(DisplayBuf,0,128,0,8);
          break;}
                   }
          continue;           
                }
         }
    if(choose1==2){
      int choose2;
        uint8_t i, duty_cycle;
        uint8_t number=0;
        uint16_t tone_freq;
        duty_cycle = 90; // Duty = 50%
        InitHCLK12M();
      while(1){
     print_lcd(0,"choose misic ?");
     print_lcd(1,"music1 is bee  ");
     print_lcd(2,"music2 is miss ");
     print_lcd(3,"music3 is kilo");
      key = Scankey();     
      if(key!=temp_key && key!=0)
      {
        if(key==1){choose2=0;break;}
        else if(key==2){choose2=1;break;}
        else if(key==3){choose2=2;break;}
              }    
    }
    if(choose2==0){
    for(w=0;w<59;w++){
                PWM_Freq(Tone_Freq(z[w]),duty_cycle);
                DrvSYS_Delay(200000);
                PWM_Freq(Tone_Freq(0),duty_cycle);
                DrvSYS_Delay(200000);
                }  
    }
    if(choose2==1){
    for(w=0;w<41;w++){
                PWM_Freq(Tone_Freq(z1[w]),duty_cycle);
                DrvSYS_Delay(335000);
                PWM_Freq(Tone_Freq(0),duty_cycle);
                DrvSYS_Delay(335000);
                }  
    }
     if(choose2==2){
    for(w=0;w<68;w++){
                PWM_Freq(Tone_Freq(z2[w]),duty_cycle);
                DrvSYS_Delay(225000);
                PWM_Freq(Tone_Freq(0),duty_cycle);
                DrvSYS_Delay(225000);
                }  
    }
    
    }
}





unsigned char picture[128*8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80,
0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xE0, 0xF0, 0xF0, 0xF8, 0xF8,
0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8,
0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x38, 0x7C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x3F, 0x1F, 0x1F, 0x1F, 0x1F, 0x9F, 0x8F, 0x8F, 0x8F, 0x07, 0x07,
0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7C, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x73, 0xF7, 0x1F, 0x3F, 0xBF, 0xFF, 0xFF, 0xFF, 0x8F,
0x07, 0x03, 0x00, 0x01, 0x02, 0x02, 0x03, 0x11, 0x11, 0x11, 0x19, 0x19, 0x09, 0x01, 0x19, 0x19,
0x19, 0x11, 0x1B, 0x03, 0x13, 0x12, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x02, 0x12, 0x33, 0x33, 0x33,
0x31, 0x11, 0x01, 0x11, 0x30, 0x31, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC7,
0xFF, 0x7F, 0x1F, 0x1F, 0x2F, 0x67, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x03, 0x03, 0x07, 0x1E,
0x1E, 0x78, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03,
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x38, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xE0,
0xE0, 0xC0, 0xC0, 0xC1, 0x81, 0x81, 0x81, 0x81, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x81, 0x81, 0x81, 0x81, 0x81, 0xC0, 0xC0, 0x40, 0x60,
0x60, 0x60, 0x30, 0x30, 0x10, 0x10, 0x38, 0xC0, 0xC4, 0xC2, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0,
0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8,
0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xC3,
0xC7, 0x87, 0x8F, 0x8F, 0x8F, 0x9F, 0x3F, 0x3F, 0x33, 0x73, 0x63, 0x63, 0x43, 0x43, 0x03, 0x03,
0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE,
0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x04, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x1C, 0x1C,
0x1E, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void clearclock(int nr, int nc, unsigned char * buf)
{
     int i,j;
     for (i=0; i<nr; i++) 
     {     //清理資料
        for (j=0; j<nc; j++) 
        {
             DisplayBuf[j+i*128] = buf[j+i*128]; 
        } 
     }
}



void RTC_IRQHandler(void)
{
        
        if (RTC->RIIR.TIF == 0x1)      		   //tick interrupt occurred 
	{
          outpw(&RTC->RIIR, 0x2); 
                       
          if(t_flag==1)
          {
            DrvGPIO_ClrBit(E_GPC,15);
            t_flag=0;

          }  
          else
          {  
            DrvGPIO_SetBit(E_GPC,15); 
            t_flag=1;
          }
        
          close_buzzer++;
          if(close_buzzer==44)
              Disable_Buzzer(0);
          
        }

        if (RTC->RIIR.AIF == 0x1)                  // alarm interrupt occurred 
        {
            outpw(&RTC->RIIR, 0x1); 
            DrvGPIO_ClrBit(E_GPC,12);
            //print_lcd(3,"Alarm!!!");          
            Disable_Buzzer(1);
            clr_all_pannal();
            print_lcd(0,"LOCK !");
            while(1){}
        }
        
}

void Init_LED()
{     		
		DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);		//Alarm  LED
                DrvGPIO_Open(E_GPC, 15, E_IO_OUTPUT);           //Tick   LED
                DrvGPIO_SetBit(E_GPC,12); 
                DrvGPIO_SetBit(E_GPC,15); 
}

void Disable_Buzzer(int set)
{
 	DrvGPIO_Open(E_GPB,11, E_IO_OUTPUT);	
	if(set==0)
            DrvGPIO_SetBit(E_GPB,11);        
        if(set==1)
            DrvGPIO_ClrBit(E_GPB,11);	
}



void Square(int x1, int y1,int color)
{
  for(int xxx=0;xxx<10;xxx++){
    for(int yyy=0;yyy<10;yyy++){
        draw_pixel(x1+xxx,y1+yyy,1);
    }
  }
}

void delay(int count)
{
	while(count--)
	{
//		__NOP;
	 }
}
// display 4-digit value (0~9999) on 7-segment LEDs
void seg_display(int16_t value)
{
  int8_t digit;
		digit = value / 1000;
		close_seven_segment();
		show_seven_segment(3,digit);
		delay(5000);
			
		value = value - digit * 1000;
		digit = value / 100;
		close_seven_segment();
		show_seven_segment(2,digit);
		delay(5000);

		value = value - digit * 100;
		digit = value / 10;
		close_seven_segment();
		show_seven_segment(1,digit);
		delay(5000);

		value = value - digit * 10;
		digit = value;
		close_seven_segment();
		show_seven_segment(0,digit);
		delay(5000);
}

void InitADC(void)
{
	/* Step 1. GPIO initial */ 
	GPIOA->OFFD|=0x00800000; 	//Disable digital input path
	SYS->GPAMFP.ADC7_SS21_AD6=1; 		//Set ADC function 
				
	/* Step 2. Enable and Select ADC clock source, and then enable ADC module */          
	SYSCLK->CLKSEL1.ADC_S = 2;	//Select 22Mhz for ADC
	SYSCLK->CLKDIV.ADC_N = 1;	//ADC clock source = 22Mhz/2 =11Mhz;
	SYSCLK->APBCLK.ADC_EN = 1;	//Enable clock source
	ADC->ADCR.ADEN = 1;			//Enable ADC module

	/* Step 3. Select Operation mode */
	ADC->ADCR.DIFFEN = 0;     	//single end input
	ADC->ADCR.ADMD   = 0;     	//single mode
		
	/* Step 4. Select ADC channel */
	ADC->ADCHER.CHEN = 0x80;
	
	/* Step 5. Enable ADC interrupt */
	ADC->ADSR.ADF =1;     		//clear the A/D interrupt flags for safe 
	ADC->ADCR.ADIE = 1;
//	NVIC_EnableIRQ(ADC_IRQn);
	
	/* Step 6. Enable WDT module */
	ADC->ADCR.ADST=1;
}
//--------------------------------------------
void InitPWM(void)
{
 	/* Step 1. GPIO initial */ 
	SYS->GPAMFP.PWM0_AD13=1;
				
	/* Step 2. Enable and Select PWM clock source*/		
	SYSCLK->APBCLK.PWM01_EN = 1;//Enable PWM clock
	SYSCLK->CLKSEL1.PWM01_S = 3;//Select 22.1184Mhz for PWM clock source

	PWMA->PPR.CP01=1;			//Prescaler 0~255, Setting 0 to stop output clock
	PWMA->CSR.CSR0=0;			// PWM clock = clock source/(Prescaler + 1)/divider
				         
	/* Step 3. Select PWM Operation mode */
	//PWM0
	PWMA->PCR.CH0MOD=1;			//0:One-shot mode, 1:Auto-load mode
								//CNR and CMR will be auto-cleared after setting CH0MOD form 0 to 1.
	PWMA->CNR0=110500;
	PWMA->CMR0=55245;

	PWMA->PCR.CH0INV=0;			//Inverter->0:off, 1:on
	PWMA->PCR.CH0EN=1;			//PWM function->0:Disable, 1:Enable
 	PWMA->POE.PWM0=1;			//Output to pin->0:Diasble, 1:Enable
}

void Delay(int count)
{
	while(count--)
	{
//		__NOP;
	 }
}

void Scankeypad(void)
{

	int8_t irow,keyno;
//--------------------------------------------------------------	
//check keypad which key press
//--------------------------------------------------------------	
	keyno = 0;		
	for (irow=1; irow <4; irow++)
	{
		GPIOA->DOUT |= (0x7);           //xxxx-x111
		GPIOA->DOUT &= ~(1<<(3-irow));  //011, 101, 110
		//scan row1 GPIOA[3] for 1,2,3
		if((GPIOA->PIN & (0x1<<3)) == 0)keyno = irow; 
		//scan row2 GPIOA[4] for 4,5,6
		if((GPIOA->PIN & (0x1<<4)) == 0)keyno = irow+3; 
		//scan row3 GPIOA[5] for 7,8,9
		if((GPIOA->PIN & (0x1<<5)) == 0)keyno = irow+6; 
	}

//--------------------------------------------------------------	
//record key status
//--------------------------------------------------------------	
	if (keyno) 
	{	// key pressed
		if (keyno == keynumber ) 				// same key
			keystatus = (keystatus<<1 );	// record key status=0   
		else //different key
		{
			keystatus = 0xFE;// initial key status
			keynumber=keyno;// record key number
		}
	}	
	else
	{ // key not pressed
		keystatus = (keystatus<<1 | 1 );// record key status=1
	}
	
	
//--------------------------------------------------------------	
// check key pressed, add a new key, shift digit left
// if get 4 continuous low level, the key is pressed.
//--------------------------------------------------------------	
	if( !keypress && !(keystatus & 0x0F) )
	{
		keypress=1;
		keybuffer[keybufferptr++]=keynumber;
	}

//--------------------------------------------------------------	
// check key released
//--------------------------------------------------------------	
	if(keypress && ((keystatus & 0x3)==3) )	
	{
		keystatus = 0xFF; //reset key status
		keynumber=0;			//reset key number
		keypress=0;				//key released
	}
	
}


//============================================================================
// Timer 0 interrupt routine
//============================================================================
void TMR_Callback(void) 
{
	
	//286clock,12.9us(22M),23.8us(12M)
	Scankeypad();

}			


//============================================================================
// Initial Timer0
//============================================================================
void InitTimer0(uint32_t freq)
{

//----------------------------------------------------------------------------
// Step 1. Enable and Select Timer clock source */          
//----------------------------------------------------------------------------

	// Select TIMER0 clock source 
	// 0=external, 1=32k, 2=hclk, 7=22M
	DrvSYS_SelectIPClockSource (E_SYS_TMR0_CLKSRC, 0);
	//SYSCLK->CLKSEL1.TMR0_S =0;

	// Enable TIMER0 engine clock
	DrvSYS_SetIPClock (E_SYS_TMR0_CLK, 1); 
	//SYSCLK->APBCLK.TMR0_EN =1;

//----------------------------------------------------------------------------
// Step 2. Select Operation mode 	
//----------------------------------------------------------------------------

	// User must to call this function before any timer operations after system boot up.
	DrvTIMER_Init ();
		
	// Open the specified timer channel with specified operation mode.
	// E_TIMER_CHANNEL= E_TMR0 / E_TMR1 / E_TMR2 / E_TMR3
	// E_TIMER_OPMODE=  E_ONESHOT_MODE / E_PERIODIC_MODE / E_TOGGLE_MODE / E_CONTINUOUS_MODE
	DrvTIMER_Open (E_TMR0, freq, E_PERIODIC_MODE);
		
	//SYSCLK->APBCLK.TMR0_EN = 1;
    
	//disable timer
	//outpw((uint32_t)&TIMER0->TCSR, 0);                  
    
	//write 1 to clear interrupt flag for safty
	//TIMER0->TISR.TIF = 1;                               
		
	//TCMPR[23:0]=Timer Compared Value, Set TCMPR [0~16,777,215]
	// (1/12,000,000)*(0+1)*6,000,000 = 0.5 sec or 2Hz
	//TIMER0->TCMPR = 0x5B8D80; //6,000,000
		
	//TCSR[7:0]=0, Set Prescale [0~255]
	//TIMER0->TCSR.PRESCALE=0;
		
	//TCSR[28:27]=01, Select period mode for operation mode
    //TIMER0->TCSR.MODE=01 
		

//----------------------------------------------------------------------------
// Step 4. Enable interrupt */
//----------------------------------------------------------------------------

	// Install the interrupt callback function of the specified timer channel.
	//uTimerEventNo = DrvTIMER_SetTimerEvent (E_TMR0, 2, (TIMER_CALLBACK)TMR_Callback, 0);
		
	// enable the specified timer interrupt.
	DrvTIMER_EnableInt (E_TMR0);

	//TCSR[29]=1, enable Timer0 interrupt
	//TIMER0->TCSR.IE = 1;
	
	//NVIC_IPR2[PRI_8]=2
	
	//(core_cm0.h) Enable IRQ8(Timer0) Interrupt	
	//NVIC_EnableIRQ(TMR0_IRQn);	
	//NVIC_ISER[TMR0_IRQn]=1; 	

	// Install the interrupt callback function of the specified timer channel.
	// ch [in]= E_TMR0 / E_TMR1 / E_TMR2 / E_TMR3
	// uInterruptTicks [in] = Number of timer interrupt occurred
	// pTimerCallback [in] = The function pointer of the interrupt callback function
	DrvTIMER_SetTimerEvent (E_TMR0, 1,(TIMER_CALLBACK)TMR_Callback, 0);


//----------------------------------------------------------------------------
// Step 5. Enable Timer module */
//----------------------------------------------------------------------------

	// Start to count the specified timer channel.
	DrvTIMER_Start (E_TMR0);
		
	//TIMER0.TCSR[30]=1, Enable Timer0
	//TIMER0->TCSR.CEN = 1;			

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
	//TIMER0.TCSR[16]=1, Enable TDR function
	//TIMER0->TCSR.TDR_EN=1;		
	//TIMER0.TCSR[26]=1, Reset up-timer, pre-scale counter, TCSR.CEN=0
	//TIMER0->TCSR.CRST = 1;
}		



//=====================================================================================
void PWM_Freq(uint32_t PWM_frequency, uint8_t PWM_duty)
{
	uint8_t  PWM_PreScaler;
	uint16_t PWM_ClockDivider;
	uint16_t CNR0, CMR0;
	uint32_t PWM_Clock;

 	if (PWM_frequency == 0) 
		PWMA->POE.PWM0=0;
	else 			      
		PWMA->POE.PWM0=1;

	if (PWM_frequency != 0) 
	{
		// PWM setting
		if(	SYSCLK->CLKSEL1.PWM01_S == 0)
			PWM_Clock = 12000000; // Clock source = 12 MHz
		if(	SYSCLK->CLKSEL1.PWM01_S == 3)
			PWM_Clock = 22118400; // Clock source = 22.1184MHz
		
		PWM_PreScaler = 5;    // clock is divided by (PreScaler + 1)

		PWM_ClockDivider = 2;  // 0: 1/2, 1: 1/4, 2: 1/8, 3: 1/16, 4: 1

		//PWM_Freq = PWM_Clock / (PWM_PreScaler + 1) / PWM_ClockDivider / (PWM_CNR0 + 1); 
		CNR0 = PWM_Clock / PWM_frequency / (PWM_PreScaler + 1) / PWM_ClockDivider - 1;

		// Duty Cycle = (CMR0+1) / (CNR0+1)
		CMR0 = (CNR0 +1) * PWM_duty /100  - 1;

		//PWM setting	  
		PWMA->CSR.CSR0 = 0;                // 0: 1/2, 1: 1/4, 2: 1/8, 3: 1/16, 4: 1
		PWMA->PPR.CP01 = PWM_PreScaler;    // set PreScaler
		PWMA->CNR0 = CNR0;	 			   // set CNR0
		PWMA->CMR0 = CMR0;				   // set CMR0
	}
}

//=====================================================================================
uint32_t Tone_Freq(uint8_t note)
{
	uint32_t note_frequency;
	switch (note) 
	{/////////////i
		case  0: note_frequency =   0; break; // null    =       0Hz	
		case  1: note_frequency =  16; break; // C0      =   16.35Hz
		case  2: note_frequency =  17; break; // C0#/D0b =   17.32Hz
		case  3: note_frequency =  18; break; // D0      =   18.35Hz
		case  4: note_frequency =  19; break; // D0#/E0b =   19.45Hz
		case  5: note_frequency =  21; break; // E0      =   20.60Hz
		case  6: note_frequency =  22; break; // F0      =   21.83Hz
		case  7: note_frequency =  23; break; // F0#/G0b =   23.12Hz
		case  8: note_frequency =  24; break; // G0      =   24.50Hz
		case  9: note_frequency =  26; break; // G0#/A0b =   25.96Hz
		case 10: note_frequency =  27; break; // A0      =   27.50Hz
		case 11: note_frequency =  29; break; // A0#/B0b =   29.14Hz
		case 12: note_frequency =  31; break; // B0      =   30.87Hz
		case 13: note_frequency =  33; break; // C1      =   32.70Hz
		case 14: note_frequency =  35; break; // C1#/D1b =   34.65Hz
		case 15: note_frequency =  37; break; // D1      =   36.71Hz
		case 16: note_frequency =  39; break; // D1#/E1b =   38.89Hz
		case 17: note_frequency =  41; break; // E1      =   41.20Hz
		case 18: note_frequency =  44; break; // F1      =   43.65Hz
		case 19: note_frequency =  46; break; // F1#/G1b =   46.25Hz
		case 20: note_frequency =  49; break; // G1      =   49.00Hz
		case 21: note_frequency =  52; break; // G1#/A1b =   51.91Hz
		case 22: note_frequency =  55; break; // A1      =   55.00Hz
		case 23: note_frequency =  58; break; // A1#/B1b =   58.27Hz
		case 24: note_frequency =  62; break; // B1      =   61.74Hz
		case 25: note_frequency =  65; break; // C2      =   65.41Hz
		case 26: note_frequency =  69; break; // C2#/D2b =   69.30Hz
		case 27: note_frequency =  73; break; // D2      =   73.42Hz
		case 28: note_frequency =  78; break; // D2#/E2b =   77.78Hz
		case 29: note_frequency =  82; break; // E2      =   82.41Hz
		case 30: note_frequency =  87; break; // F2      =   87.31Hz
		case 31: note_frequency =  92; break; // F2#/G2b =   92.50Hz
		case 32: note_frequency =  98; break; // G2      =   98.00Hz
		case 33: note_frequency = 104; break; // G2#/A2b =  103.83Hz
		case 34: note_frequency = 110; break; // A2      =  110.00Hz
		case 35: note_frequency = 117; break; // A2#/B2b =  116.54Hz
		case 36: note_frequency = 123; break; // B2      =  123.47Hz
		case 37: note_frequency = 131; break; // C3      =  130.81Hz
		case 38: note_frequency = 139; break; // C3#/D3b =  138.59Hz
		case 39: note_frequency = 147; break; // D3      =  146.83Hz
		case 40: note_frequency = 156; break; // D3#/E3b =  155.56Hz
		case 41: note_frequency = 165; break; // E3      =  164.81Hz
		case 42: note_frequency = 175; break; // F3      =  174.61Hz
		case 43: note_frequency = 185; break; // F3#/G3b =  185.00Hz
		case 44: note_frequency = 196; break; // G3      =  196.00Hz
		case 45: note_frequency = 208; break; // G3#/A3b =  207.65Hz
		case 46: note_frequency = 220; break; // A3      =  220.00Hz
		case 47: note_frequency = 233; break; // A3#/B3b =  233.08Hz
		case 48: note_frequency = 247; break; // B3      =  246.94Hz
		case 49: note_frequency = 262; break; // C4      =  261.63Hz
		case 50: note_frequency = 277; break; // C4#/D4b =  277.18Hz
		case 51: note_frequency = 294; break; // D4      =  293.66Hz
		case 52: note_frequency = 311; break; // D4#/E4b =  311.13Hz
		case 53: note_frequency = 330; break; // E4      =  329.63Hz
		case 54: note_frequency = 349; break; // F4      =  349.23Hz
		case 55: note_frequency = 370; break; // F4#/G4b =  369.99Hz
		case 56: note_frequency = 392; break; // G4      =  392.00Hz
		case 57: note_frequency = 415; break; // G4#/A4b =  415.30Hz
		case 58: note_frequency = 440; break; // A4      =  440.00Hz
		case 59: note_frequency = 466; break; // A4#/B4b =  466.16Hz
		case 60: note_frequency = 494; break; // B4      =  493.88Hz
		case 61: note_frequency = 523; break; // C5      =  523.25Hz
		case 62: note_frequency = 554; break; // C5#/D5b =  554.37Hz
		case 63: note_frequency = 587; break; // D5      =  587.33Hz
		case 64: note_frequency = 622; break; // D5#/E5b =  622.25Hz
		case 65: note_frequency = 659; break; // E5      =  659.26Hz
		case 66: note_frequency = 698; break; // F5      =  698.46Hz
		case 67: note_frequency = 740; break; // F5#/G5b =  739.99Hz
		case 68: note_frequency = 784; break; // G5      =  783.99Hz
		case 69: note_frequency = 831; break; // G5#/A5b =  830.61Hz
		case 70: note_frequency = 880; break; // A5      =  880.00Hz
		case 71: note_frequency = 932; break; // A5#/B5b =  932.33Hz
		case 72: note_frequency = 988; break; // B5      =  987.77Hz
		case 73: note_frequency =1047; break; // C6      = 1046.50Hz
		case 74: note_frequency =1109; break; // C6#/D6b = 1108.73Hz
		case 75: note_frequency =1175; break; // D6      = 1174.66Hz
		case 76: note_frequency =1245; break; // D6#/E6b = 1244.51Hz
		case 77: note_frequency =1319; break; // E6      = 1318.51Hz
		case 78: note_frequency =1397; break; // F6      = 1396.91Hz
		case 79: note_frequency =1480; break; // F6#/G6b = 1479.98Hz
		case 80: note_frequency =1568; break; // G6      = 1567.98Hz
		case 81: note_frequency =1661; break; // G6#/A6b = 1661.22Hz
		case 82: note_frequency =1760; break; // A6      = 1760.00Hz
		case 83: note_frequency =1865; break; // A6#/B6b = 1864.66Hz
		case 84: note_frequency =1976; break; // B6      = 1975.53Hz
		case 85: note_frequency =2093; break; // C7      = 2093.00Hz
		case 86: note_frequency =2217; break; // C7#/D7b = 2217.46Hz
		case 87: note_frequency =2349; break; // D7      = 2349.32Hz
		case 88: note_frequency =2489; break; // D7#/E7b = 2489.02Hz
		case 89: note_frequency =2637; break; // E7      = 2637.02Hz
		case 90: note_frequency =2794; break; // F7      = 2793.83Hz
		case 91: note_frequency =2960; break; // F7#/G7b = 2959.96Hz
		case 92: note_frequency =3136; break; // G7      = 3135.96Hz
		case 93: note_frequency =3322; break; // G7#/A7b = 3322.44Hz
		case 94: note_frequency =3520; break; // A7      = 3520.00Hz
		case 95: note_frequency =3729; break; // A7#/B7b = 3729.31Hz
		case 96: note_frequency =3951; break; // B7      = 3951.07Hz
		case 97: note_frequency =4186; break; // C8      = 4186.01Hz
		case 98: note_frequency =4435; break; // C8#/D8b = 4434.92Hz
		case 99: note_frequency =4699; break; // D8      = 4698.64Hz
		case 100:note_frequency =4978; break; // D8#/E8b = 4978.03Hz
		default: note_frequency =   0; break; // null    =       0Hz
	}
	return note_frequency;
}


//=====================================================================================
//	Initial 12M and set HCLK
//=====================================================================================
void InitHCLK12M(void)
{
	UNLOCKREG();

	//External 4~24 MHz High Speed Crystal Enable (write-protection bit)
	SYSCLK->PWRCON.XTL12M_EN = 1;//1 = Enable external 12 MHz

	//HCLK clock source select (write-protection bits)
	SYSCLK->CLKSEL0.HCLK_S = 0;//000 = Clock source from external 12 MHz

	LOCKREG();
	
}