/**************JoyGuru***************
 *                                  *
 *     Author: Sajeeb Kumar Ray     *
 * Contact: sajeebbro2002@gmail.com *
 *                                  *
 ************************************/
//char arraCC[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
char arraCA[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
short int i = 0, k = 0, cnt = 1, rs = 1, set = 1, cur_position = 0;
unsigned short cur_packet = 0, cb = 1, cc = 1, cd = 1, add_state = 1, grp = 0, group = 0, packet = 0, temp, one, two, three;
                                                                              // and three errors in time_count() funcion
void button_set_reset();
void button_select();
short int password_check();
void EEPROM_Init();
void call_display(short int n, unsigned short int grp);
void display(short int v2,short int v3,short int v4);
void display_letter(short int n);
void system();
void counting();
void breaking();
void cutting();
void dismount();


void main()
{
   ADCON1=0x06; //Disable Analog PIN
   CMCON=0x07; //Disable Comparator
   TRISA.f0 = TRISA.f1 = 0xff;
   TRISA.f2 = TRISA.f3 = TRISA.f4 = TRISA.f5 = 0x00;
   TRISB.f0 = TRISB.f1 = TRISB.f2 = TRISB.f3 = 0xff;
   TRISB.f4 = TRISB.f5 = TRISB.f6 = TRISB.f7 = 0x00;
   TRISC.f0 = TRISC.f1 = TRISC.f2 = TRISC.f3 = 0xff;
   TRISC.f4 = TRISC.f5 = TRISC.f6 = TRISC.f7 = 0x00;
   
   porta.f2 = porta.f3 = porta.f4 = porta.f5 = 0x00;
   portb.f4 = portb.f5 = portb.f6 = portb.f7 = 0x00;
   portc.f4 = portc.f5 = portc.f6 = portc.f7 = 0x00;
   TRISD = 0x00;
   TRISE = 0x00;
   portd = 0x00;
   porte = 0x00;
   
   while(1)
   {
       if(password_check() != 0)
            break;
   }
   EEPROM_Init();
   system();

}
void system()
{
   while(1)
   {
       while(add_state == 1)
       {
           button_select();
           button_set_reset();
       }
       grp = group;
       while(add_state == 3 && grp > 0)
       {

            if(cur_packet < packet) //for new start cur_packet always 0
                counting();
            if(cb == 1)      // checkinig interrupt in break because cur_packet = packet
                  breaking();
            if(cc == 1)    // checking interrupt in cutting
                  cutting();
            if(cd == 1)     // cheking interrupt in dismount
                  dismount();


            for(i = 0; i < 25; i++)    // 1 second delay
            {
                  call_display(1, grp);
                  button_set_reset(); // anytime program can be reset
            }


            grp--;
            EEPROM_Write(0x11,grp);
            cur_packet = 0;  // for new cur_packet start
            EEPROM_Write(0x33, cur_packet);
            cb = 1;    // set current break as 1
            EEPROM_Write(0x44,cb);
            cc = 1;    //  set current cutting as 1
            EEPROM_Write(0x55,cc);
            cd = 1;    // set current dismount as 1
            EEPROM_Write(0x66,cd);

       }
       //if(grp == 0)   // after completing all group successfully
       //{   // for ignoring EEPROM infinite loop, if condition reduced
           add_state = 1;
           EEPROM_Write(0x02,add_state);
           group = 0;
           EEPROM_Write(0x11,group);
           packet = 0;
           EEPROM_Write(0x22, packet);  // means operation complete
           cnt = 1; // for new running, group selection
      // }
   }
}
void counting()
{
     short i=0;
     portb.f4 = 0xff;
     porte.f2 = 0xff;
     cur_position = 0;
     
     while(cur_packet < packet)
     {
         if(porta.f0 == 0x00 && i==0)
         {
              if(cur_position == 0)  // cur_position = 0 means, current position not selected
              {
                    cur_packet++;
                    cur_position = 1;
                    i=1;
              }
         }
         else if(porta.f0 == 0xff && i==1)
         {
              if(cur_position == 1)
                   cur_position = 0; // set for next detection
                   i=0;
         }
         call_display(2, cur_packet);
         EEPROM_Write(0x33, cur_packet);
     }
     portb.f4 = 0x00;
     porte.f2 = 0x00;
     breaking(); // next operation
}
void breaking()
{
     portb.f5 = 0xff;
     for(i = 0; i < 20; i++) // error, here i < 13 for 0.5 second delay
         call_display(1, grp);
     portb.f5 = 0x00;
     cb = 2;
     EEPROM_Write(0x44,cb);
     cutting();  // next operation
}
void cutting()
{
     portb.f6 = 0xff;
     porte.f1 = 0xff;
     for(i = 0; i < 37; i++)  // 1.5 second delay
     {
         call_display(1, grp);
     }
     portb.f6 = 0x00;
     porte.f1 = 0x00;
     cc = 2;
     EEPROM_Write(0x55,cc);
     dismount();  // next operation
}
void dismount()
{
     portb.f7 = 0xff;
     porte.f0 = 0xff;
     for(i = 0; i < 50; i++)    // 2second delay
     {
         call_display(1, grp);
     }
     porte.f0 = 0x00;
     portb.f7 = 0x00;
     cd = 2;
     EEPROM_Write(0x66,cd);
}
void button_set_reset()
{
     if(portb.f3 == 0xff && add_state == 1)  // set all
     {
        delay_ms(1000);
        if(portb.f3 == 0xff)
        {
            add_state = 3;
            EEPROM_Write(0x02,add_state);
            EEPROM_Write(0x11,group);  // set the memory
            EEPROM_Write(0x22,packet);
            for(i = 4; i >= 0; i--)    // delay for preventing reset
            {
                 for(k = 0; k < 50; k++)
                     call_display(0, i);
            }
        }
     }

     if(portb.f3 == 0xff && add_state == 3)  // set all
     {
        delay_ms(1000);
        if(portb.f3 == 0xff)
        {
            add_state = 1;
            EEPROM_Write(0x02,add_state);
            // preparetion for reset all
            cur_packet = 0;  // for new cur_packet start
            EEPROM_Write(0x33, cur_packet);
            cb = 1;    // set current break as 1
            EEPROM_Write(0x44,cb);
            cc = 1;    //  set current cutting as 1
            EEPROM_Write(0x55,cc);
            cd = 1;    // set current dismount as 1
            EEPROM_Write(0x66,cd);
            group = grp = 0;
            EEPROM_Write(0x11,group);
            packet = 0;
            EEPROM_Write(0x22, packet);  // means operation complete
            cnt = 1;
            for(i = 4; i >= 0; i--)    // delay for preventing set
            {
                 for(k = 0; k < 50; k++)
                     call_display(0, i);
            }
        }
     }
}
int button_response(int number)
{
     if(portb.f0 == 0xff)  // click initialize
     {
        delay_ms(80);
        if(portb.f0 == 0xff)  // click stability check
        {
            if(number < 255)
                number++;        // digit increment
        }
     }
     if(portb.f1 == 0xff)  // click initialize
     {
        delay_ms(80);
        if(portb.f1 == 0xff)  // click stability check
        {
            if(number > 0)
                number--;        // digit decrement
        }
     }
     return number;
}
void button_select()
{
     if(portb.f2 == 0xff) // group and packet selecting button
     {
        delay_ms(100);
        if(portb.f2 == 0xff)
        {
            cnt++;
            if(cnt == 3)
            {
               cnt = 1;
            }
        }
     }
     switch(cnt)
     {
         case 1:
            group = button_response(group);
            call_display(1, group);
         break;
         case 2:
            packet = button_response(packet);
            call_display(2, packet);
         break;
     }
}
void display(short int v2,short int v3,short int v4)
{
     portc.f5 = 0xff;     // active power for second digit
     portd = arraCA[v2];    // provide data for second digit
     delay_ms(10);
     portc.f5 = 0x00;      // deactive power for second digit

     portc.f6 = 0xff;   // active power for third digit
     portd = arraCA[v3]; // provide data for third digit
     delay_ms(10);
     portc.f6 = 0x00;   // Deactive power for third digit

     portc.f7 = 0xff;   // active power for digit right
     portd = arraCA[v4]; // provide data for right digit
     delay_ms(10);
     portc.f7 = 0x00;   // Deactive power for digit right
}
void display_letter(short int n)
{
     if(n == 1)
     {
         portc.f4 = 0xff;   // active power for digit left
         portd = 0x42; // provide 'G.' for left digit
         delay_ms(10);
         portc.f4 = 0x00;   // Deactive power for digit left
     }
     else if(n == 2)
     {
         portc.f4 = 0xff;   // active power for digit left
         portd = 0x0C; // provide 'P.' for left digit
         delay_ms(10);
         portc.f4 = 0x00;   // Deactive power for digit left
     }
}
void call_display(short int n, unsigned short int grp)
{
     display_letter(n);
     temp = grp;
     three = temp%10;
     temp /= 10;
     two = temp%10;
     temp /= 10;
     one = temp;
     display(one, two, three);
}
void EEPROM_Init()
{
    /*if(EEprom_Read(0x24) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x24,add_hour);
    else
        add_hour = EEPROM_read(0x24); // read previos values
    delay_ms(30);
    if(EEprom_Read(0x60 ) == 0xff) //initialization of minute in EEPROM
        EEPROM_Write(0x60,add_min);
    else
        add_min = EEPROM_read(0x60);
    delay_ms(30);  */
    if(EEprom_Read(0x02) == 0xff) //initialization of state in EEPROM
        EEPROM_Write(0x02,add_state);
    else
        add_state = EEPROM_read(0x02);
    delay_ms(30);

    if(EEprom_Read(0x11 ) == 0xff) //initialization of pin1 last digit check in EEPROM
        EEPROM_Write(0x11,group);
    else
        group = EEPROM_read(0x11);
    delay_ms(30);
    if(EEprom_Read(0x22) == 0xff) //initialization of pin2 last digit check in EEPROM
        EEPROM_Write(0x22,packet);
    else
        packet = EEPROM_read(0x22);
    delay_ms(30);
    if(EEprom_Read(0x33) == 0xff)
        EEPROM_Write(0x33,cur_packet);
    else
        cur_packet = EEPROM_Read(0x33);
    delay_ms(30);
    if(EEprom_Read(0x44) == 0xff) //initialization of pin1 last digit check in EEPROM
        EEPROM_Write(0x44,cb);
    else
        cb = EEPROM_read(0x44);
    delay_ms(30);
    if(EEprom_Read(0x55) == 0xff) //initialization of pin2 last digit check in EEPROM
        EEPROM_Write(0x55,cc);
    else
        cc = EEPROM_read(0x55);
    delay_ms(30);
    if(EEprom_Read(0x66) == 0xff)
        EEPROM_Write(0x66,cd);
    else
        cd = EEPROM_Read(0x66);
    delay_ms(30);
}
short int password_check()
{
      if(portc.f3 == 0x00 && portc.f2 == 0xff && portc.f1 == 0x00 && portc.f0 == 0xff) // master pin
      {
           return 3;
      }
      if(portc.f3 == 0xff && portc.f2 == 0x00 && portc.f1 == 0xff && portc.f0 == 0xff)  // pin 1
      {
           return 1;
      }
      if(portc.f3 == 0xff && portc.f2 == 0xff && portc.f1 == 0x00 && portc.f0 == 0x00)   // pin 2
      {
           return 2;
      }
      return 0;
}