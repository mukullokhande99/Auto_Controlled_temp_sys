#include<reg51.h> 							//header file
	#define MYDATA P3 							//Port 3 as "MYDATA", input from ADC
	#define ldata P1  							//Port 1 as data pins for lcd, output to LCD
	#define FL3i 35	  							//temperature limits
	#define FL2ii 35  
	#define FL2i 30	 
	#define FL1ii 30
	#define FL1i 25
	#define NLii 25
	#define NLi 20
	#define HL1i 20
	#define HL1ii 10
	#define HL2i 10
	#define HL2ii 00



		sbit rd=P2^5;	  									//P2.5 with identifier rd, read command to ADC
		sbit wr=P2^4;	  									//P2.4 with identifier wr, write command to ADC
		sbit INTR=P2^3;	  								//P2.3 with identifier INTR, detect start and end of conversion by ADC
		sbit rs=P2^0;	  									//P2.0 value to RS_register of LCD
		sbit rw=P2^1;	  									//P2.1 value to RW_register of LCD
		sbit en=P2^2;	  									//P2.2 value to EN_register of LCD
		sbit MTR=P2^6;	  								//P2.6 PWM signal to Motor
		sbit HTR1=P0^0;	  								//P0.0 led1
		sbit HTR2=P0^1;	  								//P0.0 led2
		sbit busy=P1^7;	 									//P1.7, identifier busy

	void msDelay(unsigned int value)			//provides delay value*1ms
	{ 
	unsigned int x,y;
	for(x=0;x<value;x++)
    for(y=0;y<1000;y++); 
	}


	void lcdReady()												//if lcd is ready or busy?
		{  
		busy=1;
		rs=0;
		rw=1;
		while(busy==1)
		{
			en=0;
			en=1;
		}
		return;
		}
		void lcdCmd(unsigned char value)     //command to lcd if ready
			{ 
		lcdReady();										
		ldata=value;
		rs=0;							
		rw=0;
		en=1;
		en=0;
		return;
		}
			
		void lcdInit() 											//Initializes lcd
			{ 
		lcdCmd(0x38);
		lcdCmd(0x0c);
		lcdCmd(0x01);
		lcdCmd(0x80);												//calling lcd command
		return;	
			}
		void lcdData(char value)							//input to lcd
			{ 							
		ldata=value;
		rs=1;
		rw=0;
		en=1;
		en=0;
		return;
			}

		void display(char d1,char d2)					//display on lcd
			{
    lcdData(d2);
		msDelay(30);
		msDelay(30);
		lcdData(d1);
		msDelay(30);
		msDelay(' ');
		msDelay(30);
		lcdData('C');
			}



			void convert(char value)					//binary to ASCII to display
			{
			char y,d1,d2,d3;
			y=value/10;
			d1=value%10;
			d2=y%10;
			d3=y/10;
			d1=d1|0x30;
			d2=d2|0x30;
			d3=d3|0x30;
			display(d1,d2);
			}

			void update(char value)								//Updates the data in lcd if data is changed
		{
			char y,d1,d2,d3;     
			y=value/10;      
			d1=value%10;
			d2=y%10;
			d3=y/10;
			d1=d1|0x30;
			d2=d2|0x30;
			d3=d3|0x30;
			lcdData(d2);
			msDelay(30);
			msDelay(30);
			lcdData(d1);
			msDelay(30);
			lcdCmd(0xc0);
		}



char adcRead()															//Reads ADC output in binary format
				{							
			char value;
			wr=0;																	//pulse to ADC to Start the conv
			wr=1;
			while(INTR==1);												//Waits till conv complete
			rd=0;																	//pulse to ADC to read the data conv by ADC
			value=MYDATA;													//Receiving the conv data into the P3 
			rd=1;
			return value;
				}


void motorcontrol(){
   unsigned char i;
	 unsigned char value;
	 unsigned char x[6]="TEMPR:";
	 unsigned char x1[5]="FANON";												//TEMPR:Temperature
	 unsigned char x2[5]="HTRON";												//HTRON:Heater on
	 unsigned char y[12]="SPEED:LEVEL";
	 unsigned char z[11]="HEAT:LEVEL";
	 unsigned char u1[5]="NORM.";												//NOR: Normal
	 unsigned char u2[11]="FAN,HTR OFF";								//FAN,HTR OFF:both off
	 
	while(1)																				  	//Infinite loop : no OS
		 {  																			
		value=adcRead(); 																	//read data from adc
    if(value<20){																			//Heater On logic	  
    lcdCmd(0x01);   																	//clear display
for(i=0;i<6;i++) 																			//display string TEMPR
     {
     msDelay(50);
     lcdData(x[i]);
     }
		convert(value); 																	//convert data and display
		lcdCmd(0x8b);					 
for(i=0;i<5;i++) 																			//display HTRON
		 {
		msDelay(30);
		lcdData(x2[i]);
	   }
	 
     while(value>HL1ii && value<HL1i)					  			//level 1 heat for heater
			 { 						
       MTR=1;  
	     HTR2=1;
	     HTR1=0;
	     msDelay(1);
	     z[10]='1';
	     lcdCmd(0xc0);
for(i=0;i<11;i++)
				 {
	     msDelay(30);
	     lcdData(z[i]);
	       }
	     bkl5:
			 value=adcRead();
		if(value>HL1ii && value<HL1i)
	      {
	     lcdCmd(0x86);
	     update(value);
	     goto bkl5;
	      }
		else
		break;
	    } 
	 
    while(value>=HL2ii && value<=HL2i)								//level 2 heat for heater
			{ 		
      MTR=1;	
			HTR1=0;
			msDelay(30);
			HTR2=0;
			msDelay(1);
			lcdCmd(0xc0);
			z[10]='2';
	  for(i=0;i<11;i++)
				{
			msDelay(30);
	    lcdData(z[i]);
				}
			bkl6:
			value=adcRead();
	 if(value>=HL2ii && value<=HL2i)
			{
	    lcdCmd(0x86);
	    update(value);
	    goto bkl6;
			}
	 else
	 break;
			}
   } 
   
   while(value>=NLi&&value<NLii)							 	//Normal state logic
		 {
     HTR1=1;
		 HTR2=1;
	   MTR=1;
	   lcdCmd(0x01); 															//clear display
	 for(i=0;i<6;i++)															//display TEMPR
			 { 
     msDelay(50);
     lcdData(x[i]);
			 }
	 convert(value);                            	//convert data and display
	 for(i=0;i<5;i++)
			 {
	   msDelay(30);
	   lcdData(u1[i]);
	     }
     lcdCmd(0xc0);
	 for(i=0;i<11;i++)
			 {
	   msDelay(30);
	   lcdData(u2[i]);
	     }
	   bklbkl:
	   value=adcRead();
	 if(value>=NLi&&value<NLii)
		  {
	   lcdCmd(0x86);
	   update(value);
	   goto bklbkl;
	    }
   else
	 break;
   }
    
		if(value>=25) 																   //fan on logic
				{
	     HTR1=1;
	     HTR2=1;
	     lcdCmd(0x01); 																 //clear display
	  for(i=0;i<6;i++) 														     //display TEMPR
				{
       msDelay(50);
       lcdData(x[i]);
        }
	     convert(value); 															//convert data and display
	     lcdCmd(0x8b);
	  for(i=0;i<5;i++)
	      {
	     msDelay(30);
	     lcdData(x1[i]);
	      }

     while(value>=FL1i && value<FL1ii)							//level 1 speed for fan
			 { 
	     lcdCmd(0xc0);
	     y[11]='1';
	   for(i=0;i<12;i++)
				 {
	     msDelay(30);
	     lcdData(y[i]);
	       }
	     bkl1:
	     MTR=0;
       msDelay(50);
       MTR=1;
       msDelay(50);
	     value=adcRead();
	  if(value>=FL1i && value<FL1ii)
			{   
	     lcdCmd(0x86);
	     update(value);
	  goto bkl1;
	    }
	  else
	  break;
	  }
	 
     while(value>=FL2i && value<FL2ii)								//level 2 speed for fan
			 {
	      y[11]='2';
	      lcdCmd(0xc0);
	   for(i=0;i<12;i++)
				 {
	      msDelay(30);
	      lcdData(y[i]);
	       }
	      bkl2:
	      MTR=0;
        msDelay(75);
        MTR=1;
        msDelay(25);
        value=adcRead();
	   if(value>=FL2i && value<FL2ii)
			   {
	      lcdCmd(0x86);
	      update(value);
	   goto bkl2;
	       }
	   else
	   break;
	     } 

	 while(value>=FL3i)													//level 3 speed for fan
		 { 
				y[11]='3';
				lcdCmd(0xc0);
	 for(i=0;i<12;i++)
			 {
				msDelay(30);
				lcdData(y[i]);
			 }
	      bkl3:
	      MTR=0;
	      value=adcRead();
	 if(value>=FL3i)
		  {
	      lcdCmd(0x86);
	      update(value);
	 goto bkl3;
	    }
	 else
	 break;
	  }  		 
}
}  
}  

  void main()
		{ 
   P0=0xff;	 												//all P0=1 as output port in negative logic
   MYDATA=0xff; 										//all P3 as input port in positive logic
   INTR=1; 													//Active low config, init as high
   rd=1; 														//Active low config, init as high
   wr=1; 														//Active low config, init as high
   MTR=1; 													//Active low config, init as high
   HTR1=0; 										
   HTR2=0; 													//leds for heater in active low config
   msDelay(50);
   HTR1=1; 													//50ms delay Leds are turned off
   HTR2=1;
   lcdInit(); 											//Initializes LCD
   motorcontrol(); 									//Calls motorcontrol subroutine
   } 


				 					  



