/*Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
	
/* Task flags */ 
signed char Task1_Flag = -1;  //task 1 flag
signed char Task2_Flag = -1;  //task 2 flag
signed char Task3_Flag = -1;  //task 3 flag

/*init the state of pins*/
pinState_t Task1_Past_State=PIN_IS_LOW;
pinState_t Task1_Current_State=PIN_IS_LOW;
pinState_t Task2_Past_State=PIN_IS_LOW;
pinState_t Task2_Current_State=PIN_IS_LOW;
/*end*/


/* Task 1 */
void Button_1_Monitor( void * pvParameters ){    
		int i=0;
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = 50;
	
     /* Set the xLastWakeTime variable with the current time. */
     xLastWakeTime = xTaskGetTickCount();
		vTaskSetApplicationTaskTag(NULL, (void *) 1 );
    for( ;; )
    {
		Task1_Current_State=GPIO_read(PORT_0, PIN0);
		if(Task1_Current_State!=Task1_Past_State){
			if ( Task1_Current_State == PIN_IS_LOW ){
				Task1_Flag=0;
			}
			else {
				Task1_Flag=1;
			}
		}
		else{
			Task1_Flag=-1;
		}
		Task1_Past_State=Task1_Current_State;
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

/* Task 2 */
void Button_2_Monitor( void * pvParameters ){

		int i=0;
			TickType_t xLastWakeTime;
		const TickType_t xFrequency = 50;
	
	     xLastWakeTime = xTaskGetTickCount();

		vTaskSetApplicationTaskTag(NULL, (void *) 2 );

    for( ;; )
    {
			
		Task2_Current_State=GPIO_read(PORT_0, PIN1);
		
		if(Task2_Current_State!=Task2_Past_State){
			if ( Task2_Current_State == PIN_IS_LOW ){
				Task2_Flag=0;
			}
			else {
				Task2_Flag=1;
			}
		}
		else{
			Task2_Flag=-1;
		}
		Task2_Past_State=Task2_Current_State;
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

    }
}

/* Task 3 */
void Periodic_Transmitter( void * pvParameters ){

			TickType_t xLastWakeTime;
		const TickType_t xFrequency = 100;
     xLastWakeTime = xTaskGetTickCount();

		vTaskSetApplicationTaskTag(NULL, (void *) 3 );

    for( ;; )
    {
		Task3_Flag=1;
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

    }
}

/* Task 4 */
void Uart_Receiver( void * pvParameters ){
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = 20;
     xLastWakeTime = xTaskGetTickCount();

		vTaskSetApplicationTaskTag(NULL, (void *) 4 );


    for( ;; )
    {
		if (Task1_Flag ==0 ){
			
			vSerialPutString(NULL,"Button 1 Falling Edge\n", 28);
			/* Reset the flag */
			Task1_Flag=-1;
			
		}
		else if (Task1_Flag ==1 ){
			
			vSerialPutString(NULL,"Button 1 Rising Edge\n", 27);
			/* Reset the flag */
			Task1_Flag=-1;				 
		}
		if (Task2_Flag ==0 ){
			
			vSerialPutString(NULL,"Button 2 Falling Edge\n", 28);	
			/* Reset the flag */
			Task1_Flag=-1;
		}
		else if (Task2_Flag ==1 ){
			vSerialPutString(NULL,"Button 2 Rising Edge\n", 27);
			/* Reset the flag */
			Task2_Flag=-1;
		}
		if (Task3_Flag ==1 ){	

			vSerialPutString(NULL,"Period\n", 18);
			/* Reset the flag */
			Task3_Flag=-1;
		}
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

    }
}

/* Task 5 */
void Load_1_Simulation( void * pvParameters ){		//load1 monitor
		int i=0;
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = 10;
    xLastWakeTime = xTaskGetTickCount();
		vTaskSetApplicationTaskTag(NULL, (void *) 5 );
	
    for( ;; )
		{
			for( i=0; i<37000; i++){
			i=i;	
			}
			vTaskDelayUntil( &xLastWakeTime, xFrequency );
			}
}

/* Task 6 */
void Load_2_Simulation( void * pvParameters ){		//load 2 monitor
		int i=0;
		TickType_t xLastWakeTime;
		const TickType_t xFrequency = 100;
    xLastWakeTime = xTaskGetTickCount();
		vTaskSetApplicationTaskTag(NULL,(void*)6);
    for( ;; )
		{
			for( i=0; i<87995; i++){
				i=i;
			}
			vTaskDelayUntil( &xLastWakeTime, xFrequency );
		}
}



/* Application Hooks */
void vApplicationIdleHook(){
}
void vApplicationTickHook(){	
}

/* Function to reset timer */
void timer1Reset(void){
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}
/* Function to initialize and start timer */
static void configTimer1(void){
	T1PR = 1000;
	T1TCR |= 0x1;
}
static void prvSetupHardware( void ){
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE,100);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}




TaskHandle_t task1Handler = NULL;
TaskHandle_t task2Handler = NULL; 
TaskHandle_t task3Handler = NULL;
TaskHandle_t task4Handler = NULL;
TaskHandle_t task5Handler = NULL;
TaskHandle_t task6Handler = NULL;

int main(){
	prvSetupHardware();
	xTaskPeriodicCreate(Button_1_Monitor,"Button_1_Monitor",200,0,1,&task1Handler,50);
	xTaskPeriodicCreate(Button_2_Monitor,"Button_2_Monitor",200,0,1,&task2Handler,50);
	xTaskPeriodicCreate(Periodic_Transmitter,"Periodic_Transmitter",200,0,1,&task3Handler,100);
	xTaskPeriodicCreate(Uart_Receiver,"Uart_Receiver",200,0,1,&task4Handler,20);
	xTaskPeriodicCreate(Load_1_Simulation,"Load_1_Simulation",200,0,1,&task5Handler,10);
	xTaskPeriodicCreate(Load_2_Simulation,"Load_2_Simulation",200,0,1,&task6Handler,100);
	vTaskStartScheduler();
	while(1);
}
/*-----------------------------------------------------------*/
