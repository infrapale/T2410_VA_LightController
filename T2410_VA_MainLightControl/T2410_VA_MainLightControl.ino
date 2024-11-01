/*********************************************************************************************************
  Villa Astrid Main Light Controller
  HW:  TinyPico PCB with a Pi Pico (not W)
  Main Functions:
  - Home-alone simulation
  - Fast come home and leave functions
  - Real Time Clock
**********************************************************************************************************
Github repositories:
    https://github.com/infrapale/T2410_VA_LightController
    https://github.com/infrapale/T2401_KeyToRfm   (old sketch)
    https://github.com/infrapale/T2312_Keypad_2x4
    https://github.com/infrapale/T2311_RFM69_Modem
https://arduino-pico.readthedocs.io/en/latest/serial.html

**********************************************************************************************************

---------------------       ------------------           ---------------------     ------------------  
| T2312_Keypad_2x4  |       | T2401_KeyToRfm |           | T2311_RFM69_Modem |     | RFM69 Receiver |
---------------------       ------------------           ---------------------     ------------------
   |                            |                              |                          | 
   |   <KP1:4=0> \n             |                              |                          |
   |--------------------------->|                              |                          |
   |                            |                              |                          |
   |                            |    <#X1N:RMH1;RKOK1;T;->\n   |                          |
   |                            |----------------------------->|                          |
   |                            |                              |                          | 
   |                            |                              |  {"Z":"MH1","S":"RKOK1", |
   |                            |                              |  "V":"T","R":"-"}        |
   |                            |                              |- - - - - - - - - - - - ->|
   |                            |                              |                          | 
   |     UART 1                 |       UART 2                 |    . . . 433MHz . . .    | 


  <KPx:y=z>\n
      x = keypad module index 
      y = keypad keyindex
      z = key value  '1', '0'
  <KP2:1=1>


*********************************************************************************************************/

#define LCD_I2C_ADDR    (0x27)
#define EDOG_I2C_ADDR   (0x24)

#include "Arduino.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Adafruit_NeoPixel.h>
#include "main.h"
#include "io.h"
#include "kbd_uart.h"
#include "atask.h"
#include "relay.h"
#include "va_signal.h"
#include "menu4x2.h"
#include "autom.h"
#include "supervisor.h"
#include "rtc.h"

  

main_ctrl_st main_ctrl = {0x00};


void debug_print_task(void);

atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};

int show = -1;
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display


void initialize_tasks(void)
{
  atask_initialize();
  atask_add_new(&debug_print_handle);
  Serial.printf("Tasks initialized (%d)\n",TASK_NBR_OF);
 
  uint8_t mode = rtc_get_ram_byte();


}


void setup() {
  // put your setup code here, to run once:
  int error;
  Serial1.setTX(PIN_SERIAL_1_TX);   // UART0
  Serial1.setRX(PIN_SERIAL_1_RX);
  Serial2.setTX(PIN_SERIAL_2_TX);   // UART1
  Serial2.setRX(PIN_SERIAL_2_RX);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);

  delay(3000);
  // while (!Serial);
  Serial.printf("%s: %s %s\n",APP_NAME,__DATE__,__TIME__);
  io_initialize_tiny_pico(); 
  Wire.setSCL(PIN_I2C_SCL);
  Wire.setSDA(PIN_I2C_SDA);
  Wire1.setClock(100000);
  Wire1.setSCL(PIN_I2C1_SCL);
  Wire1.setSDA(PIN_I2C1_SDA);
  
  Wire.begin();
  Wire1.begin();
  initialize_tasks();
  rtc_initialize();
  va_signal_initialize();

  delay(1000);
  
  kbd_uart_initialize();
  
  Wire1.beginTransmission(LCD_I2C_ADDR);
  error = Wire1.endTransmission();
  Serial.printf("Error: %d\n",error);

  if (error == 0) {
    Serial.println(": LCD found.");
    show = 0;
    lcd.begin(20, 4, Wire1);  // initialize the lcd
  } 
  else 
  {
    Serial.println(": LCD not found.");
  } 

  menu4x2_initialize();
  supervisor_initialize();

  autom_initialize(6, 37);
  autom_randomize();
  Serial.printf("Setup() completed\n");
    
}

void loop() {
  //edog_clear_watchdog();
  atask_run();    
}


void debug_print_task(void)
{
  atask_print_status(true);
  //sema_status_print();
  supervisor_debug_print();

  // uint16_t a_ldr = analogRead(PIN_LDR);
  // Serial.println(a_ldr);
  // Serial.print("PIR="); Serial.println(digitalRead(PIN_PIR));
  
  // uint16_t clock_state = clock24_get_state());
  // if (clock_state & CLOCK_STATE_AT_HOME) Serial.print("At home -");
  // else  Serial.print("Away -");
  // if (clock_state & CLOCK_STATE_SENDING) Serial.print("Sending -");
  // if (clock_state & CLOCK_STATE_OPTION) Serial.print("Option -");
  // if (clock_state & CLOCK_STATE_CNTDWN) Serial.print("Countdown -");
  // Serial.println("");

}
 