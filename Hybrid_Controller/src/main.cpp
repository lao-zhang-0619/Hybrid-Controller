#include <SPI.h>
#include <math.h>
#include <Arduino.h>
#include <esp_adc_cal.h>
#include "adc_continuous.h"
#include "main.hpp"


/*Task definition*/
void cs_map(uint8_t);
void dpWrite2(uint16_t p_D, uint8_t s);
void Task_Pot1(void *);
void Task_Pot2(void *);
void Task_Pot3(void *);
void Task_Pot4(void *);
void TaskRead(void *);
void Task_Pot5(void *);
void Task_Pot6(void *);
void Task_Pot7(void *);
void Task_Pot8(void *);
void dpWrite(int, int, int);
void Command(void *);
// void change1(int *read_found, float *pre, float *s2, const char *s1, String *tem3, String *tem2, bool *changed,Mes* mes);
void IcMode(void);
void OpMode(void);
void Reapeat(void *);
void Halt(void);

/*-----------------------------------------------------------*/

/**Digital Potentiometer (referred to as THAT) Change Parameter Structure Definition
 * potentiomere work as coefficient potentiometer which used to change the value of coefficients.
 * Settings of 0-10 correspond to coefficients of 0-1
 */
typedef struct
{
  /*Max output*/
  int max_range = 10;
  /*Min output*/
  int min_range = 0;
  /*Time to complete a cycle*/
  float time = 5000;
  /*constant output value in constant output mode*/
  float val_set;
  int cs;
  int read;
  void (*Func_Ptr)(void*);
} Mes;
void change1(int *read_found, float *pre, float *s2, const char *s1, String *tem3, String *tem2, bool *changed, Mes *mes);
/*All eight potentiometers has their own message structure*/
Mes block;
Mes block_sin_pot1;
Mes block_line_pot4;
Mes block_increase_pot3;
Mes block_decline__pot2;
Mes block_sin1_pot5;
Mes block_line1_pot8;
Mes block_increase1_pot7;
Mes block_decline1_pot6;
Mes Mes_POT1;
Mes Mes_POT2;
Mes Mes_POT3;
Mes Mes_POT4;
Mes Mes_POT5;
Mes Mes_POT6;
Mes Mes_POT7;
Mes Mes_POT8;
Mes Task_Line;
Mes REAP_TASK;

/*buffer to store the readed data from THAT*/
int DataRead[6000];

/*
 *@brief Hybrid Controller instruction definition array
 *
 */
enum read
{
  READX,
  READY,
  READZ,
  READU,
  POT1,
  POT2,
  POT3,
  POT4,
  POT5,
  POT6,
  POT7,
  POT8,
  REP,
  OP,
  IC,
  POT1_OFF,
  POT2_OFF,
  POT3_OFF,
  POT4_OFF,
  POT5_OFF,
  POT6_OFF,
  POT7_OFF,
  POT8_OFF
};

/*
 * @brief POT paramter setting
 */
enum vari
{
  TIME,
  MAX,
  MIN,
  SET,
  VAL,
  BREAK
};
const char *pVari[] = {"TIME", "MAX", "MIN", "SET", "VAL","BREAK"};
const char *pCommand[] = {"READX", "READY", "READZ", "READU", "POT1", "POT2", "POT3", "POT4", "POT5", "POT6",
                          "POT7", "POT8", "REP", "OP", "IC", "POT1_OFF", "POT2_OFF", "POT3_OFF", "POT4_OFF", "POT5_OFF", "POT6_OFF", "POT7_OFF", "POT8_OFF"};

/*Creating evetn group handles. xeventgroup will point to a task event group. This task event group will indicate whether the task is running or not.*/
EventGroupHandle_t xEventGroup;
#define CONFIG_IDF_TARGET_ESP32S3
void setup()
{

  // PinMode(33,INPUT);
  // analogSetPinAttenuation(32, ADC_2_5db);
  xEventGroup = xEventGroupCreate();
  disableCore0WDT();
  pinMode(DIR, OUTPUT);
  pinMode(X_PIN, INPUT);
  pinMode(OP_PIN, OUTPUT);
  pinMode(IC_PIN, OUTPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(U_PIN, INPUT);
  pinMode(Z_PIN, INPUT);
  pinMode(SBIT0, OUTPUT);
  pinMode(HC138, OUTPUT);
  pinMode(SBIT1, OUTPUT);
  pinMode(SBIT2, OUTPUT);
  // PinMode(32,INPUT);
  digitalWrite(DIR, LOW);
  digitalWrite(HC138, LOW);
  Serial.begin(250000);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE1));// mode 0 

  /*RTOS systm task create */
  xTaskCreatePinnedToCore(Command, "Command", 4096, &block, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskRead, "TaskRead", 9000, &block, 1, NULL, 0);
  xTaskCreatePinnedToCore(Reapeat, "Reapeat", 1000, &block, 1, NULL, 0);

  xTaskCreatePinnedToCore(Task_Pot1, "Task_Pot1", 4096, &Mes_POT1, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot2, "Task_Pot1", 4096, &Mes_POT2, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot3, "Task_Pot3", 4096, &Mes_POT3, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot4, "Task_Pot4", 4096, &Mes_POT4, 1, NULL, 0);

  xTaskCreatePinnedToCore(Task_Pot5, "Task_Pot5", 4096, &Mes_POT5, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot6, "Task_Pot6", 4096, &Mes_POT6, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot7, "Task_Pot7", 4096, &Mes_POT7, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task_Pot8, "Task_Pot8", 4096, &Mes_POT8, 1, NULL, 0);
}

void dpWrite2(uint16_t P_D, uint8_t s)
{
  digitalWrite(HC138, HIGH);
  cs_map(s);
  //delay(10);
  SPI.transfer16(0x1802);
  digitalWrite(HC138, LOW);
  digitalWrite(HC138, HIGH);
  cs_map(s);
  //delay(10);
  SPI.transfer16(P_D);
  digitalWrite(HC138, LOW);
}
void cs_map(uint8_t num)
{
  switch (num)
  {
  case 1:
    digitalWrite(SBIT0, LOW);
    digitalWrite(SBIT1, LOW);
    digitalWrite(SBIT2, LOW);
    break;
  case 2:
    digitalWrite(SBIT0, HIGH);
    digitalWrite(SBIT1, LOW);
    digitalWrite(SBIT2, LOW);
    break;
  case 3:
    digitalWrite(SBIT0, LOW);
    digitalWrite(SBIT1, HIGH);
    digitalWrite(SBIT2, LOW);
    break;
  case 4:
    digitalWrite(SBIT0, HIGH);
    digitalWrite(SBIT1, HIGH);
    digitalWrite(SBIT2, LOW);
    break;
  case 5:
    digitalWrite(SBIT0, LOW);
    digitalWrite(SBIT1, LOW);
    digitalWrite(SBIT2, HIGH);
    break;
  case 6:
    digitalWrite(SBIT0, HIGH);
    digitalWrite(SBIT1, LOW);
    digitalWrite(SBIT2, HIGH);
    break;
  case 7:
    digitalWrite(SBIT0, LOW);
    digitalWrite(SBIT1, HIGH);
    digitalWrite(SBIT2, HIGH);
    break;
  case 8:
    digitalWrite(SBIT0, HIGH);
    digitalWrite(SBIT1, HIGH);
    digitalWrite(SBIT2, HIGH);
    break;
  default:
    digitalWrite(SBIT0, LOW);
    digitalWrite(SBIT1, LOW);
    digitalWrite(SBIT2, LOW);
    break;
  }
}

/**
 * @brief OP mode setting PIN
 * @retval None
 */
void OpMode(void)
{
  digitalWrite(IC_PIN, HIGH);
  digitalWrite(OP_PIN, LOW);
}

/**
 * @brief HALT mode setting PIN
 * @retval None
 */
void Halt(void)
{
  digitalWrite(OP_PIN, HIGH);
  digitalWrite(IC_PIN, HIGH);
}

/**
 * @brief IC mode setting PIN
 * @retval None
 */

void IcMode(void)
{
  digitalWrite(OP_PIN, HIGH);
  digitalWrite(IC_PIN, LOW);
}

/**
 * @brief Reapeat mode setting PIN
 * @param param: not used yet. can be used as data interface to set the reprtition fequency.
 * @retval None
 */
void Reapeat(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    /*Task Reapeat will be obstacle untill the REP been setted to 1 */
    xEventGroupWaitBits(xEventGroup, REP_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    OpMode();
    vTaskDelay(REAP_TASK.time);
    IcMode();
    vTaskDelay(REAP_TASK.time);
  }
}

/**
 * @brief Read data from THAT and send the data to computer
 * @param param:pointer to a Mes structure that contains the configuration information for the task.
 * @retval None
 */
void TaskRead(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, READ, pdTRUE, pdFALSE, portMAX_DELAY);
    IcMode();
    vTaskDelay(10);
    OpMode();
    for (int i = 0; i < 1000; i++)
    {
      DataRead[i] = analogRead(ptr->read);
      //vTaskDelay(1);
    }
    for (int i = 0; i < 1000; i++)
    {
      Serial.println(DataRead[i]);
      //Serial.println("read1");
    }
    xEventGroupClearBits(xEventGroup, READ);
    //Serial.println("read1");
  }
}


/**
 * @brief The value of Pot 3 on Chip 1 does not change unless the set value change.
 * @param param:pointer to a Mes structure that contains the configuration information for the task.
 * @retval None
 */
void Task_Pot1(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot1_bit, pdFALSE, pdFALSE, portMAX_DELAY);
    dpWrite2(uint16_t(1024+102.4*ptr->val_set), 1);
    Mes_POT1.Func_Ptr(&Mes_POT1);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Task_Pot2(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot2_bit, pdFALSE, pdFALSE, portMAX_DELAY);
   
    Mes_POT2.Func_Ptr(&Mes_POT2);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Task_Pot3(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot3_bit, pdFALSE, pdFALSE, portMAX_DELAY);

    Mes_POT3.Func_Ptr(&Mes_POT3);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Task_Pot4(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot4_bit, pdFALSE, pdFALSE, portMAX_DELAY);
    
    Mes_POT4.Func_Ptr(&Mes_POT4);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Task_Pot5(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot5_bit, pdFALSE, pdFALSE, portMAX_DELAY);
   
    Mes_POT5.Func_Ptr(&Mes_POT5);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Task_Pot6(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot6_bit, pdFALSE, pdFALSE, portMAX_DELAY);
    
    Mes_POT6.Func_Ptr(&Mes_POT6);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Task_Pot7(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot7_bit, pdFALSE, pdFALSE, portMAX_DELAY);
    // dpWrite(3, (ptr->val_set * 255) / 10, PIN1);
    //dpWrite2(uint16_t(1024+102.4*ptr->val_set), 1);
    //int(1024+102.4*ptr->val_set/10)
    //Serial.println("line");
    Mes_POT7.Func_Ptr(&Mes_POT7);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Task_Pot8(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    xEventGroupWaitBits(xEventGroup, Task_Pot8_bit, pdFALSE, pdFALSE, portMAX_DELAY);
    // dpWrite(3, (ptr->val_set * 255) / 10, PIN1);
    //dpWrite2(uint16_t(1024+102.4*ptr->val_set), 1);
    //int(1024+102.4*ptr->val_set/10)
    //Serial.println("line");
    Mes_POT8.Func_Ptr(&Mes_POT8);
    //vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


void Line(void *param)
{
  Mes *ptr = (Mes *)param;

    // dpWrite(3, (ptr->val_set * 255) / 10, PIN1);
  dpWrite2(uint16_t(1024+102.3*ptr->val_set), ptr->cs);
  //dpWrite2(uint16_t(2047), ptr->cs);
    //int(1024+102.4*ptr->val_set/10)
    //Serial.println("line");
  vTaskDelay(100 / portTICK_PERIOD_MS);
}


/**
 * @brief The value of Pot 2 on Chip 1 increase linearly
 * @param param:pointer to a Mes structure that contains the configuration information for the task.
 * @retval None
 */
void TaskPrintincrease(void *param)
{
  Mes *ptr = (Mes *)param;
  while (1)
  {
    float max = ptr->max_range * 255 / 10;
    float min = ptr->min_range * 255 / 10;
    for (int x = (int)min; x < max; x++)
    {

      vTaskDelay(ptr->time / (max - min));
    }
  }
}

/**
 * @brief The value of Pot 1 on Chip 1 decrease linearly
 * @param param:pointer to a Mes structure that contains the configuration information for the task.
 * @retval None
 */
void TaskPrintdecline(void *param)
{
  Mes *ptr = (Mes *)param;

  while (1)
  {
    float max = ptr->max_range * 255 / 10;
    float min = ptr->min_range * 255 / 10;
    for (int x = max; x > min; x--)
    {
      vTaskDelay(ptr->time / (max - min)); // ptr->time/(max-min)
    }
  }
}


/**
 * @brief The value of Pot 1 on Chip 1 change as a sin function
 * @param param:pointer to a Mes structure that contains the configuration information for the task.
 * @retval None
 */
void TaskPrintsin(void *param)
{
  Mes *ptr = (Mes *)param;
  double y;
  int x;
  while (1)
  {
    for (y = 0; y < 2 * PAI; y += 2 * PAI / 300)
    {
      //xEventGroupWaitBits(xEventGroup, SIN, pdFALSE, pdFALSE, portMAX_DELAY);
      x = ptr->max_range * (sin(y) + 1) * 256 / 20;
      // dpWrite(0, x, PIN1);
      vTaskDelay(1 / (300));
    }
  }
}


/**
 * @brief Received the commands from computer,accroding the commands to update the status and
 *        parameters of the task.
 * @param param: pointer to a Mes structure that contains the configuration information for the READ funtion.
 * @retval None
 */
void Command(void *param)
{
  Mes *ptr = (Mes *)param;
  /* Three branches are set up in the command task to ensure that
   * the string data read is translated into the correct command.
   */
  int read_found = 0;
  //dpWrite2(0x05ff, 1);
  const char *s;
  const char *s1;
  float s2;
  String tem = "";
  String tem1 = "";
  String tem2 = "";
  bool changed = false;
  float pre;
  int state;
  while (1)
  {
    vTaskDelay(20 / portTICK_RATE_MS);
    if (read_found == 0)
    {
      tem = "";
    }
    else if (read_found == 1)
    {
      tem1 = "";
    }
    else
    {
      tem2 = "";
    }
    char c;
    /*Receiveing data from the serial port*/
    while (Serial.available() > 0)
    {
      if ((c = Serial.read()) != '\n')
        if (read_found == 0)
        {
          tem += c;
        }
        else if (read_found == 1)
        {
          tem1 += c;
        }
        else
        {
          tem2 += c;
        }
    }
    s = tem.c_str();
    s1 = tem1.c_str();
    s2 = atof(tem2.c_str());
    #ifdef TEST
    /*debugging text use*/
      Serial.println(".....");
      Serial.println(read_found);
      Serial.println(s);
      Serial.println(s1);
      Serial.println(s2);
      Serial.println("---");
    #endif
    for (state = READX; state <= POT8_OFF; state++)
    {
      /*First determine if it is a branch one instruction*/
      if (strcmp(s, pCommand[state]) == 0)
      {
        switch (state)
        {
        case OP:
          //xEventGroupSetBits(xEventGroup, REP);
          OpMode();
          break;
        case HALT:
          //xEventGroupSetBits(xEventGroup, REP);
          Halt();
          break;
        case IC:
          xEventGroupClearBits(xEventGroup, REP_BIT);
          IcMode();
          break;
        case REP:
          //Serial.println("REP123");
          xEventGroupSetBits(xEventGroup, REP_BIT);
          break;
        case READX:
          //Serial.println("READ1");
          ptr->read = X_PIN;
          xEventGroupSetBits(xEventGroup, READ);
          break;
        case READY:
          ptr->read = Y_PIN;
          xEventGroupSetBits(xEventGroup, READ);
          //Serial.println("READ2");
          break;
        case READU:
          ptr->read = U_PIN;
          xEventGroupSetBits(xEventGroup, READ);
          //Serial.println("READ3");
          break;
        case READZ:
          ptr->read = Z_PIN;
          xEventGroupSetBits(xEventGroup, READ);
          //Serial.println("READ4");
          break;
        /* If the instruction is POT setting instruction,go into branch two and need to receive the EXIT command to exit branch two*/
        case POT1:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT1.cs=1;
          Mes_POT1.Func_Ptr=Line;
          /*change1 fucntion is a function */
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT1);
          //Mes_POT1.Func_Ptr(&Mes_POT1);
          
          xEventGroupSetBits(xEventGroup, Task_Pot1_bit);
          break;
        case POT1_OFF:
          xEventGroupClearBits(xEventGroup, Task_Pot1_bit);
          //Serial.println("POT1_OFF");
          break;
        case POT2:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT2.cs=2;
          Mes_POT2.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT2);
          //Mes_POT2.Func_Ptr(&Mes_POT2);
          xEventGroupSetBits(xEventGroup, Task_Pot2_bit);
          break;
        case POT2_OFF:
          //Serial.println("POT2_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot2_bit);
          break;
        case POT3:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT3.cs=3;
          Mes_POT3.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT3);
          //Serial.println("POT3");
          xEventGroupSetBits(xEventGroup, Task_Pot3_bit);
          break;
        case POT3_OFF:
          //Serial.println("POT3_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot3_bit);
          break;
        case POT4:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT4.cs=4;
          Mes_POT4.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT4);
          //Serial.println("POT4");
          xEventGroupSetBits(xEventGroup, Task_Pot4_bit);
          break;
        case POT4_OFF:
          //Serial.println("POT4_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot4_bit);
          break;
        case POT5:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT5.cs=5;
          Mes_POT5.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT5);
          //Serial.println("POT5");
          xEventGroupSetBits(xEventGroup, Task_Pot5_bit);
          break;
        case POT5_OFF:
          //Serial.println("POT5_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot5_bit);
          break;
        case POT6:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT6.cs=6;
          Mes_POT6.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT6);
          //Serial.println("POT6");
          xEventGroupSetBits(xEventGroup, Task_Pot6_bit);
          break;
        case POT6_OFF:
          //Serial.println("POT6_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot6_bit);
          break;
        case POT7:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT7.cs=7;
          Mes_POT7.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT7);
          //Serial.println("POT7");
          xEventGroupSetBits(xEventGroup, Task_Pot7_bit);
          break;
        case POT7_OFF:
          //Serial.println("POT7_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot7_bit);
          break;

        case POT8:
          if (read_found == 0)
          {
            read_found = 1;
            break;
          }
          Mes_POT8.cs=8;
          Mes_POT8.Func_Ptr=Line;
          change1(&read_found, &pre, &s2, s1, &tem2, &tem1, &changed, &Mes_POT8);
          //Serial.println("POT8");
          xEventGroupSetBits(xEventGroup, Task_Pot8_bit);
          break;

        case POT8_OFF:
          //Serial.println("POT8_OFF");
          xEventGroupClearBits(xEventGroup, Task_Pot8_bit);
          break;
        default:
          //Serial.println("default");
          break;
        }
      }
    }
  }
}

void loop()
{
}
/**
 *@brief The change1 function will determine which values should be changed and store these values in the accordingly Mes sturcture,
         and it change some variant to ensure that the program doesn't go into other branches.
 @param read_found branch informtion.
 @param pre: a pointer to last moment value of s2.
 @param s2: a pointer to current moment value of s2.
 @param s1: a pointer to current moment of s1, the string to be validated as a command.
 @param tem2:  a pointer to tem1,rewrite it with blank string to forbid program enter other branchs.
 @param tem1: a pointer to tem1,rewrite it with blank string to forbid program enter other branchs.
 @param change: a pointer to bool, The sign of whether the value of s2 changes.
 @param mes: a pointer to a Mes structure,which will offer the specific value for other task.
*/
void change1(int *read_found, float *pre, float *s2, const char *s1, String *tem2, String *tem1, bool *changed, Mes *mes)
{
  int vari1;

  for (vari1 = TIME; vari1 <= BREAK; vari1++)
  {
    if (strcmp(s1, pVari[vari1]) == 0)
    {
      *read_found = 2;
      if (*changed == false)
      {
        *pre = *s2;
        *changed = true;
      }
      switch (vari1)
      {
      case TIME:
        if (*pre != *s2)
        {
          *changed = false;
          mes->time = *s2;
          *tem2 = "";
          *s2 = 0;
          *read_found = 1;
        }
        break;
      case MAX:
        if (*pre != *s2)
        {
          *changed = false;
          mes->max_range = *s2;
          *s2 = 0;
          *tem2 = "";
          *read_found = 1;
        }
        break;
      case MIN:
        if (*pre != *s2)
        {
          *changed = false;
          mes->min_range = *s2;
          *s2 = 0;
          *tem2 = "";
          *read_found = 1;
        }
        break;
      case VAL:
        if (*pre != *s2)
        {
          *changed = false;
          mes->val_set = 10*(*s2);
          Serial.println(mes->val_set);
          *s2 = 0;
          *tem2 = "";
          *read_found = 1;
        }
        break;
      case SET:
        if (*pre != *s2)
        {
          *changed = false;
          REAP_TASK.time = int((*s2));
          *s2 = 0;
          *tem2 = "";
          *read_found = 1;
        }
        break;
      case BREAK:
        *tem1 = "";
        *read_found = 0;
        break;
      default:

        break;
      }
    }
    else
      ;
  }
}

/**
 * @brief SPI write function
 * @param N_D: first eight bits of address
 * @param V_D: eight data bits
 * @param S: CS
 */
void dpWrite(int N_D, int V_D, int s)
{
  digitalWrite(s, LOW);
  SPI.transfer(N_D);
  SPI.transfer(V_D);
  digitalWrite(s, HIGH);
}
