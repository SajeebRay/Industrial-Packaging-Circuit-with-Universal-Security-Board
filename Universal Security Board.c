/**************JoyGuru***************
 *                                  *
 *     Author: Sajeeb Kumar Ray     *
 * Contact: sajeebbro2002@gmail.com *
 *                                  *
 ************************************/
char arraCC[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
char arraCA[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
short int bt_zero = 0, bt_one = 0, bt_two = 0, bt_three  = 0, cnt = 0, res = 1, mod = 0;
short int pin1[] = {1,3,0}, pin2[] = {8,9,2}, m_pin[] = {4,7,4,5}, code_seq = 1;  //passwords
unsigned short i = 0, k = 0, add_hour = 120, add_min = 6, add_state = 0, temp, d1, d2, d3, p14 = 2, p24 = 0; // error! minute = 60,
                                                                              // and three errors in time_count() funcion
void button_click();
void password_check();
void EEPROM_Init();
void time_count();
void display(short int v1,short int v2,short int v3,short int v4);
void run_forever();
void display_on();
void delay_func();

void main()
{
   TRISB = 0x00;
   TRISC = 0x00;
   TRISD.f0 = TRISD.f1 = TRISD.f2 =  TRISD.f3 = 0xff;
   TRISD.f4 = TRISD.f5 = TRISD.f6 = TRISD.f7 = 0x00;
   portb = 0x00;
   portc = 0x00;
   portd.f4 = portd.f5 = portd.f6 = portd.f7= 0x00;
   ADCON1=0x06; //Disable Analog PIN
   CMCON=0x07; //Disable Comparator
   TRISA = 0x00;
   EEPROM_Init();
   if(add_state == 1 || add_state == 2)
   {
       delay_func();   //delay 10s
       time_count();
   }
   if(add_state == 3)
   {
       delay_func(); //delay 10s
       run_forever();
   }

   while(1)
   {
       button_click();
       password_check();

       display(bt_zero, bt_one, bt_two, bt_three);

       portc.f6 = 0xff; // password led on
       portc.f7 = 0x00;  // trial led off
       time_count(); // automatically start when password found
   }
}
int button_response(int bt)
{
     if(portd.f2 == 0xff)  // click initialize
     {
        delay_ms(100);
        if(portd.f2 == 0xff)  // click stability check
        {
            bt++;        // digit increment
            if(bt == 10) // after 9, next is 0
            {
               bt = 0;
            }
        }
     }
     return bt;
}
void button_click()
{
     if(portd.f3 == 0xff)
     {
        delay_ms(100);
        if(portd.f3 == 0xff)
        {
            cnt++;
            if(cnt == 4)
            {
               cnt = 0;
            }
        }
     }
     switch(cnt)
     {
         case 0:
            portd.f4 = 0xff;   // active power for digit left
            portb = 0x7F; // provide data for left digit
            delay_ms(10);
            portd.f4 = 0x00;   // Deactive power for digit left
            bt_zero = button_response(bt_zero);
         break;
         case 1:
            portd.f5 = 0xff;     // active power for second digit
            portb = 0x7F;    // provide data for second digit
            delay_ms(10);
            portd.f5 = 0x00;      // deactive power for second digit
            bt_one = button_response(bt_one);
         break;
         case 2:
            portd.f6 = 0xff;   // active power for third digit
            portb = 0x7F; // provide data for third digit
            delay_ms(10);
            portd.f6 = 0x00;   // Deactive power for third digit
            bt_two = button_response(bt_two);
         break;
         case 3:
            portd.f7 = 0xff;   // active power for digit right
            portb = 0x7F; // provide data for right digit
            delay_ms(10);
            portd.f7 = 0x00;   // Deactive power for digit right
            bt_three = button_response(bt_three);
         break;
     }
}
void password_check()
{
     if((bt_zero == pin1[0] && bt_one == pin1[1] && bt_two == pin1[2] && bt_three == p14)
                 || add_state == 1) // checking password
       {
           add_state = 1;
           EEPROM_write(0x02, add_state);
           delay_func();
       }
       // pass 8 9 2 0, this show 1 1 0 0
       else if((bt_zero == pin2[0] && bt_one == pin2[1] && bt_two == pin2[2] && bt_three == p24)
              || add_state == 2)
       {
           add_state = 2;
           EEPROM_write(0x02, add_state);
           delay_func();   // 10 second delay
       }
       else if(bt_zero == m_pin[0] && bt_one == m_pin[1] && bt_two == m_pin[2] && bt_three == m_pin[3])
       {
            add_state = 3;
            EEPROM_write(0x02, add_state);
            delay_func(); // delay 10s
            run_forever();
       }
       else   // if password don't match
       {
           porta.f0 = 0x00;
           porta.f1 = 0x00;
           porta.f2 = 0x00;
           porta.f3 = 0x00;
       }
}
void time_count()
{
    porta.f0 = 0x00;   // output for all time
    porta.f1 = 0xff;
    porta.f2 = 0xff;
    porta.f3 = 0x00;
    while(add_state == 1 || add_state == 2)
    {
        portc.f7 = 0xff;  // trial led on
        portc.f6 = 0x00; // password led off
        temp = add_hour; // for displaying remaining hours
        d1 = temp%10; temp/= 10;
        d2 = temp%10; temp/= 10;
        d3 = temp%10;
        display(0, d3, d2, d1); // calling display function
        if(add_hour == 0)
        {
            if(add_state == 1)
            {
                p14 = 99; // pin1 destroyed, this is not for unsigned
                EEPROM_write(0x11,p14);
            }
            if(add_state == 2)
            {
                p24 = 99; // pin2 destroyed
                EEPROM_write(0x22,p24);
            }
            add_state = 0;
            EEPROM_write(0x02, add_state);

            // Initalizing data
            add_hour = 120;
            add_min = 6;  // error! minute = 60
            EEPROM_Write(0x24,add_hour);
            EEPROM_Write(0x60,add_min);
            bt_zero = bt_one = bt_two = bt_three = 0; // restart buttons. as password invalid
            delay_ms(1000);

            password_check();
            break;
        }

        for(k = 1; k <= 3; k++) // error! for 1 minute k <= 50x60
        {
            display(0, d3, d2, d1); // calling display function
            delay_ms(10);
            display(0, d3, d2, d1);
            delay_ms(10);
        }
        if(add_min > 0)
        {
             add_min--;
        }
        EEPROM_write(0x60, add_min);
        if(add_min == 0)
        {
            if(add_hour > 0)
            {
                add_hour--;
            }
            add_min = 6; // error! minute = 60
            EEPROM_write(0x60, add_min);
            EEPROM_write(0x24, add_hour);
        }
    }
}
void EEPROM_Init()
{
    if(EEprom_Read(0x24) == 0xff) //initialization of hours in EEPROM
        EEPROM_Write(0x24,add_hour);
    else
        add_hour = EEPROM_read(0x24); // read previos values
    delay_ms(30);
    if(EEprom_Read(0x60 ) == 0xff) //initialization of minute in EEPROM
        EEPROM_Write(0x60,add_min);
    else
        add_min = EEPROM_read(0x60);
    delay_ms(30);
    if(EEprom_Read(0x02) == 0xff) //initialization of state in EEPROM
        EEPROM_Write(0x02,add_state);
    else
        add_state = EEPROM_read(0x02);
    delay_ms(30);

    if(EEprom_Read(0x11 ) == 0xff) //initialization of pin1 last digit check in EEPROM
        EEPROM_Write(0x11,p14);
    else
        p14 = EEPROM_read(0x11);
    delay_ms(30);
    if(EEprom_Read(0x22) == 0xff) //initialization of pin2 last digit check in EEPROM
        EEPROM_Write(0x22,p24);
    else
        p24 = EEPROM_read(0x22);
    delay_ms(30);
}
void display(short int v1,short int v2,short int v3,short int v4)
{
     portd.f4 = 0xff;   // active power for digit left
     portb = arraCA[v1]; // provide data for left digit
     delay_ms(5);
     portd.f4 = 0x00;   // Deactive power for digit left

     portd.f5 = 0xff;     // active power for second digit
     portb = arraCA[v2];    // provide data for second digit
     delay_ms(5);
     portd.f5 = 0x00;      // deactive power for second digit

     portd.f6 = 0xff;   // active power for third digit
     portb = arraCA[v3]; // provide data for third digit
     delay_ms(5);
     portd.f6 = 0x00;   // Deactive power for third digit

     portd.f7 = 0xff;   // active power for digit right
     portb = arraCA[v4]; // provide data for right digit
     delay_ms(5);
     portd.f7 = 0x00;   // Deactive power for digit right
}
void run_forever()
{
    portc.f7 = 0x00;
    portc.f6 = 0x00;
    portc.f5 = 0xff;  // pro led on
    while(1)
    {
           display_on();
           porta.f0 = 0x00;
           porta.f1 = 0xff;
           porta.f2 = 0xff;
           porta.f3 = 0x00;
     }
}
void display_on()
{
     portd.f6 = 0xff;   // active power for third digit
     portb = arraCA[0]; // provide data for third digit
     delay_ms(10);
     portd.f6 = 0x00;   // Deactive power for third digit

     portd.f7 = 0xff;   // active power for digit right
     portb = 0xC8; // provide data (n) for right digit
     delay_ms(10);
     portd.f7 = 0x00;   // Deactive power for digit right
}
void delay_func()
{
    for(k = 10; k > 0; k--)
    {
        mod = k%10;
        res = k/10;
        for(i = 0; i < 50; i++)
        {
            portd.f4 = 0xff;   // active power for digit left
            portb = arraCA[res]; // provide data for left digit
            delay_ms(10);
            portd.f4 = 0x00;   // Deactive power for digit left

            portd.f5 = 0xff;     // active power for second digit
            portb = arraCA[mod];    // provide data for second digit
            delay_ms(10);
            portd.f5 = 0x00;      // deactive power for second digit
        }
    }
    for(k = 1; k < 100; k++)   // sequence only pass for 2second
    {
        if(add_state == 1)
        {
            porta.f0 = 0xff;
            porta.f1 = 0x00;
            porta.f2 = 0xff;
            porta.f3 = 0xff;
        }
        if(add_state == 2)
        {
            porta.f0 = 0xff;
            porta.f1 = 0xff;
            porta.f2 = 0x00;
            porta.f3 = 0x00;
        }
        if(add_state == 3)
        {
            porta.f0 = 0x00;
            porta.f1 = 0xff;
            porta.f2 = 0x00;
            porta.f3 = 0xff;
        }
        display_on(); //machine on
    }
}