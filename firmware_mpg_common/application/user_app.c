/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */
volatile u16 UserApp_u86timescounter=0;                      /*6times counter*/
volatile u16 UserApp_u16namedetected=0;                         /*how many times name been detected*/
volatile u32 u32Alllettercounter=0;                 /*the num of all letters*/
volatile u16 u16tempcounter;/*gether returned value of DebugScanf*/
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                     /* From debug.c  */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */



/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  u8 u8MessageBuffer[]=" My name is donglu";
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,u8MessageBuffer );
  LedPWM(LCD_RED, LED_PWM_50);
  LedPWM(LCD_BLUE,LED_PWM_50);
  LedPWM(LCD_GREEN, LED_PWM_20);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
 
  static u8 u8NameBuffer[6]="donglu";
  static u8 u8emptymessage[]="empty";
  static u8 u8clearmessage[]="Character Count Cleared";
  static u8 u8totlemessage[]="Totlal characters received";
  static u8 u8AllScanfBuffer[1000]={0};                  /*all letters stored in this buffer*/
  static u8 u8screanBuffer[20];                       /*scream uffer*/
  static u32 u32Alllettercounter=0;                 /*the num of all letters*/
  static u8 u8screanletternum=0;                    /*how many letters show in the scream*/
  static u16 u16Templettercounter=0;                /**/
  static u8 u8partbuffer[6]={0,0,0,0,0};              /*part of name buffer*/
  
  static u8 u8temp=50;      /*delay 50ms*/
  u16tempcounter=0;
  u8temp--;
  if(u8temp==0)
  {
    /*所有字符保存开始指针*/
  u16tempcounter=(DebugScanf(u8AllScanfBuffer+u32Alllettercounter));
  
  if(u16tempcounter)//state 3**********************************************************
  {
    /*compare witch name */
    for(u8 i=0;i<u16tempcounter;i++)
  {
    /*compare every letter with my name*/
    if(u8NameBuffer[UserApp_u86timescounter]==(*(u8AllScanfBuffer+u32Alllettercounter+i)))
    {
      u8partbuffer[UserApp_u86timescounter]=u8NameBuffer[UserApp_u86timescounter];
      
      UserApp_u86timescounter++;
      if(UserApp_u86timescounter==6)
      {
        UserApp_u86timescounter=0;
        UserApp_u16namedetected++;
      }
    }
  }
    
    
    
   u16Templettercounter+=u16tempcounter; 
   u32Alllettercounter+=u16tempcounter;
   u8AllScanfBuffer[u32Alllettercounter] = '\0';
   if(u16Templettercounter==0)
   {
     LCDClearChars(LINE2_START_ADDR,20);
   }
   else
   {

     /*how many letters printed in screan*/
     u8screanletternum=(u16Templettercounter-1)%20+1;
     
     /* if the screan is full   then  print from line2_start*/
     if(u8screanletternum==1||u8screanletternum==0)
     {
       LCDClearChars(LINE2_START_ADDR,20);
     }
    
     if(u8screanletternum!=0)
     {
     //u8AllScanfBuffer+u16Alllettercounter-u8screanletternum是显示字符开始指针
     strcpy(u8screanBuffer,u8AllScanfBuffer+u32Alllettercounter-u8screanletternum);  
     LCDMessage(LINE2_START_ADDR,u8screanBuffer);
     }
   }
  }
  if(WasButtonPressed(BUTTON0))//button 0
  {
    
    ButtonAcknowledge(BUTTON0);
    u16Templettercounter=0;
    u8screanletternum=0;
    LCDClearChars(LINE2_START_ADDR,20); 
  }
  else if(WasButtonPressed(BUTTON1))//button 1
  {
    
    ButtonAcknowledge(BUTTON1);
    LCDClearChars(LINE2_START_ADDR,20); 
    DebugLineFeed();
    DebugPrintf(u8totlemessage);
    DebugLineFeed();
    DebugPrintNumber(u32Alllettercounter);
  }
  else if(WasButtonPressed(BUTTON2))//button 2
  {
    
    ButtonAcknowledge(BUTTON2);
    u32Alllettercounter=0;
    u16Templettercounter=0;
    u8screanletternum=0;
    DebugPrintf(u8clearmessage);
    DebugLineFeed();
  }
  else if(WasButtonPressed(BUTTON3))//button 3
  {
    ButtonAcknowledge(BUTTON3);
    if(UserApp_u86timescounter==0)
    {
      LCDClearChars(LINE2_START_ADDR,20); 
    }
    else
    {
    DebugLineFeed();
    DebugPrintf(u8partbuffer);
    DebugLineFeed(); 
    }
  }
  
  u8temp==50;
  }

} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
