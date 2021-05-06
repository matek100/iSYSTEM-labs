/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h750b_discovery_qspi.h"
#include "stm32h750b_discovery_sdram.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "retarget.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "types.h"
//#include "printf.h"
//#include "stm32h7xx_hal_uart.h"

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc3;

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

LTDC_HandleTypeDef hltdc;

QSPI_HandleTypeDef hqspi;

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

SDRAM_HandleTypeDef hsdram2;

/* Definitions for GUITask */
osThreadId_t GUITaskHandle;
const osThreadAttr_t GUITask_attributes = {
  .name = "GUITask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 8192 * 4
};
/* Definitions for GPS_RX */
osThreadId_t GPS_RXHandle;
const osThreadAttr_t GPS_RX_attributes = {
  .name = "GPS_RX",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 2048 * 4
};
/* Definitions for DefaultTask */
osThreadId_t DefaultTaskHandle;
const osThreadAttr_t DefaultTask_attributes = {
  .name = "DefaultTask",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 128 * 4
};
/* Definitions for XbeeTask */
osThreadId_t XbeeTaskHandle;
const osThreadAttr_t XbeeTask_attributes = {
  .name = "XbeeTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 2048 * 4
};
/* Definitions for Battery_task */
osThreadId_t Battery_taskHandle;
const osThreadAttr_t Battery_task_attributes = {
  .name = "Battery_task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for GPS_Sem */
osSemaphoreId_t GPS_SemHandle;
const osSemaphoreAttr_t GPS_Sem_attributes = {
  .name = "GPS_Sem"
};
/* Definitions for TX1_Done_Sem */
osSemaphoreId_t TX1_Done_SemHandle;
const osSemaphoreAttr_t TX1_Done_Sem_attributes = {
  .name = "TX1_Done_Sem"
};
/* Definitions for StartBatt */
osSemaphoreId_t StartBattHandle;
const osSemaphoreAttr_t StartBatt_attributes = {
  .name = "StartBatt"
};
/* Definitions for XBEE_Sem */
osSemaphoreId_t XBEE_SemHandle;
const osSemaphoreAttr_t XBEE_Sem_attributes = {
  .name = "XBEE_Sem"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CRC_Init(void);
static void MX_LTDC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_QUADSPI_Init(void);
static void MX_FMC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC3_Init(void);
static void MX_ADC1_Init(void);
void TouchGFX_Task(void *argument);
void StartGPS_RX(void *argument);
void StartDefaultTask(void *argument);
void StartXbeeTask(void *argument);
void Start_Battery_task(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//Moje globalke
static uint8_t RX_1_BUFFER[200];
static uint8_t RX_2_BUFFER[200];

nav_t GPS_1;


void usart_rx_check(void) { // $PUBX,00,180508.00,4601.91226,N,01431.82146,E,357.068,G3,7.0,5.0,3.425,205.39,0.125,,1.34,1.83,1.34,7,0,0*6C

	static int old_pos;
	static int pos;
	static int Ponovi = 0;
	int nm_data = 0;
	int Lat_mm = 0;
	int Long_mm = 0;

	static char OUTPUT[150];

	char TX_OUTPUT[160];

	char Latitude_ch[30] = {0};
	char Longitude_ch[30] = {0};
	char Altitude_ch[30] = {0};
	char Stat_ch[30] = {0};
	char Time_ch[30] = {0};
	char Speed_ch[30] = {0};
	char COG_ch[30] = {0};
	char Prefix_ch[30] = {0};

	    /* Calculate current position in buffer */
	    pos = sizeof(RX_1_BUFFER) - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

	    if (pos != old_pos) {                       /* Check change in received data */
	        if (pos > old_pos) {                    /* Current position is over previous one */
	            /* We are in "linear" mode */
	            /* Process data directly by subtracting "pointers" */
	        	for(int i = 0; i < (pos - old_pos); i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[old_pos + i]);
	        		OUTPUT[i] = RX_1_BUFFER[old_pos + i];
	        		nm_data++;
	        		//strncat(OUTPUT, RX_1_BUFFER[old_pos + i], 1);
	        	}
	        	OUTPUT[nm_data+1] = '\0';  //Kaj se zgodi ko pride cez 100 bytov
	        } else {
	            /* We are in "overflow" mode */
	            /* First process data to the end of buffer */
	        	for(int i = old_pos; i < sizeof(RX_1_BUFFER); i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[i]);
	        		OUTPUT[nm_data] = RX_1_BUFFER[i];
	        		nm_data++;
	        	}
	            /* Check and continue with beginning of buffer */
	        	for(int i = 0; i < pos; i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[i]);
	        		OUTPUT[nm_data] = RX_1_BUFFER[i];
	        		nm_data++;
	        	}
	        	OUTPUT[nm_data+1] = '\0';
	        }
	        printf("POSITION: %d \n", pos);

	        printf("DATA: %s \n", OUTPUT); //problem je ker blocka


	        //RAZREZE STRING NA VEJICE
	        if(strlen(OUTPUT) < 80){
	        	return;
	        }
	        strcpy(TX_OUTPUT, OUTPUT);

	        char OUT_array[40][40];
			char *ptr = strtok(OUTPUT, ",");
			int i = 0;

			while(ptr != NULL){
				strcpy(OUT_array[i], ptr);
				//printf("OUT_ARRAY: %d    %s \n", i, OUT_array[i]);
				ptr = strtok(NULL, ",");
				i++;
			}

			//CAS
			strcpy(Time_ch, OUT_array[2]);
			GPS_1.Time = (int)(atof(Time_ch)/100) + 200;

	        strcpy(Stat_ch, OUT_array[8]);
	        strcpy(Prefix_ch, OUT_array[0]);

	        if(Stat_ch[0] != 'N' && Stat_ch[1] != 'F'){ //Preveri ali imam gps LOCK

				GPS_1.Status = 1010; // Vse je OK, imam GPS lock

	        	//SPEED OVER GND
				strcpy(Speed_ch, OUT_array[11]);
				GPS_1.SOG = (int)(atof(Speed_ch));

	        	//DIRECTION
				strcpy(COG_ch, OUT_array[12]);
				GPS_1.COG = (int)(atof(COG_ch));

	        	//LAT; LONG; ALT
	        	strcpy(Latitude_ch, OUT_array[3]);
	        	strcpy(Longitude_ch, OUT_array[5]); ///TUKAJ SI IMEL NEKE HARDFAULTE PAZI!!!!
	        	strcpy(Altitude_ch, OUT_array[7]);

	        	GPS_1.Lat = ((int)(atof(Latitude_ch)*100000));
	        	Lat_mm = (GPS_1.Lat%10000000);
	        	GPS_1.Lat = (GPS_1.Lat - Lat_mm)+((Lat_mm/60)*100);

	        	GPS_1.Long = (int)(atof(Longitude_ch)*100000);
	        	Long_mm = (GPS_1.Long%10000000);
	        	GPS_1.Long = (GPS_1.Long - Long_mm)+((Long_mm/60)*100);

	        	GPS_1.Alt = (int)(atof(Altitude_ch)*1000);
	        	printf("Latitude: %d \n", GPS_1.Lat);
	        	printf("Longitude: %d \n", GPS_1.Long);
	        	printf("Altitude: %d \n", GPS_1.Alt);
	        	//printf("INTEGER %d", atoi(OUTPUT + 20));
	        	printf("CAS: %02d:%02d\n", (int)(GPS_1.Time/100), (int)(GPS_1.Time%100));
	        	printf("Speed: %d\n", GPS_1.SOG);
	        	printf("COG: %d\n", GPS_1.COG);

	        	//Poslji vsako n-to iteracijo _ ker drgace s dma buffer zafila in vse skup crkne


	        	Ponovi++;
	        	if(Ponovi > 3){
	        		printf("TRANSMIT: %s \n", TX_OUTPUT);
	        		//char TEST[150] = "$PUBX,00,180508.00,4601.91226,N,01431.82146,E,357.068,G3,7.0,5.0,3.425,205.39,0.125,,1.34,1.83,1.34,7,0,0*6C \n";
	        		HAL_UART_Transmit(&huart2, (uint8_t*)TX_OUTPUT, strlen(TX_OUTPUT), 1000); //Poslji podatke XBEE-ju
	        		Ponovi = 0;
	        	}


	        }
	        else{
	        	GPS_1.Alt = 0;
	        	GPS_1.COG = 45;
				GPS_1.Lat = 0;
				GPS_1.Long = 0;
				GPS_1.SOG = 0;
				//GPS_1.Time = 0;
				GPS_1.Status = 1111;
				char *MSG = "NO GPS";

				Ponovi++;
				if(Ponovi > 4){
					char TEST[150] = "$PUBX,00,000000.00,0000.00000,N,0100.00000,E,999.000,G3,7.0,5.0,0.000,300.00,0.125,,1.34,1.83,1.34,7,0,0*6C \n";
					HAL_UART_Transmit_DMA(&huart2, (uint8_t*)TEST, strlen(TEST)); //Poslji podatke XBEE-ju
					Ponovi = 0;
				}

				//HAL_UART_Transmit_DMA(&huart2, (uint8_t*)MSG , sizeof(*MSG));
	        }

	        memset(OUTPUT, 0, 150);
	        old_pos = pos;                          /* Save current position as old */
	    }
	}



//Tole je grdo ampak bom predelal funkcije za vsako prejemanje posebaj
nav_t GPS_2;

void usart_rx_XBEE(void) { // $PUBX,00,180508.00,4601.91226,N,01431.82146,E,357.068,G3,7.0,5.0,3.425,205.39,0.125,,1.34,1.83,1.34,7,0,0*6C

	static int old_pos_X;
	static int pos_X;

	int nm_data = 0;
	int Lat_mm = 0;
	int Long_mm = 0;

	static char OUTPUT_X[200];

	char Latitude_ch[30] = {0};
	char Longitude_ch[30] = {0};
	char Altitude_ch[30] = {0};
	char Stat_ch[30] = {0};
	char Time_ch[30] = {0};
	char Speed_ch[30] = {0};
	char COG_ch[30] = {0};
	char Prefix_ch[30] = {0};

	    /* Calculate current position in buffer */
	    pos_X = sizeof(RX_2_BUFFER) - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
	    if (pos_X != old_pos_X) {                       /* Check change in received data */
	        if (pos_X > old_pos_X) {                    /* Current position is over previous one */
	            /* We are in "linear" mode */
	            /* Process data directly by subtracting "pointers" */
	        	for(int i = 0; i < (pos_X - old_pos_X); i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[old_pos + i]);
	        		OUTPUT_X[i] = RX_2_BUFFER[old_pos_X + i];
	        		nm_data++;
	        		//strncat(OUTPUT, RX_1_BUFFER[old_pos + i], 1);
	        	}
	        	OUTPUT_X[nm_data+1] = '\0';  //Kaj se zgodi ko pride cez 100 bytov
	        } else {
	            /* We are in "overflow" mode */
	            /* First process data to the end of buffer */
	        	for(int i = old_pos_X; i < sizeof(RX_2_BUFFER); i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[i]);
	        		OUTPUT_X[nm_data] = RX_2_BUFFER[i];
	        		nm_data++;
	        	}
	            /* Check and continue with beginning of buffer */
	        	for(int i = 0; i < pos_X; i++){
	        		//printf("DATA: %c \n", RX_1_BUFFER[i]);
	        		OUTPUT_X[nm_data] = RX_2_BUFFER[i];
	        		nm_data++;
	        	}
	        	OUTPUT_X[nm_data+1] = '\0';
	        }
	        printf("POSITION: %d \n", pos_X);

	        printf("DATA: %s \n", OUTPUT_X); //problem je ker blocka

	        if(strlen(OUTPUT_X) < 80){
				return;
			}

	        //RAZREZE STRING NA VEJICE
	        char OUT_array[40][40];
			char *ptr = strtok(OUTPUT_X, ",");
			int i = 0;

			while(ptr != NULL){
				strcpy(OUT_array[i], ptr);
				//printf("OUT_ARRAY: %d    %s \n", i, OUT_array[i]);
				ptr = strtok(NULL, ",");
				i++;
			}

	        strcpy(Stat_ch, OUT_array[8]);
	        strcpy(Prefix_ch, OUT_array[0]);

			//STATUS
			/*if(strcmp(OUT_array[0], "$PUBX") == 0){ //Preveri ce si dobil od zacetka
				return; //Ce nisi ustavi funkcijo
			}*/
			if(Stat_ch[0] != 'N' && Stat_ch[1] != 'F'){ //Preveri ali imam gps LOCK

				GPS_2.Status = 1010; // Vse je OK, imam GPS lock

	        	//SPEED OVER GND
				strcpy(Speed_ch, OUT_array[11]);
				GPS_2.SOG = (int)(atof(Speed_ch));

	        	//DIRECTION
				strcpy(COG_ch, OUT_array[12]);
				GPS_2.COG = (int)(atof(COG_ch));

	        	//CAS
	        	strcpy(Time_ch, OUT_array[2]);
	        	GPS_2.Time = (int)(atof(Time_ch)/100) + 200;

	        	//LAT; LONG; ALT
	        	strcpy(Latitude_ch, OUT_array[3]);
	        	strcpy(Longitude_ch, OUT_array[5]); ///TUKAJ SI IMEL NEKE HARDFAULTE PAZI!!!!
	        	strcpy(Altitude_ch, OUT_array[7]);

	        	GPS_2.Lat = ((int)(atof(Latitude_ch)*100000));
	        	Lat_mm = (GPS_2.Lat%10000000);
	        	GPS_2.Lat = (GPS_2.Lat - Lat_mm)+((Lat_mm/60)*100);

	        	GPS_2.Long = (int)(atof(Longitude_ch)*100000);
	        	Long_mm = (GPS_2.Long%10000000);
	        	GPS_2.Long = (GPS_2.Long - Long_mm)+((Long_mm/60)*100);

	        	GPS_2.Alt = (int)(atof(Altitude_ch)*1000);
	        	printf("Latitude: %d \n", GPS_2.Lat);
	        	printf("Longitude: %d \n", GPS_2.Long);
	        	printf("Altitude: %d \n", GPS_2.Alt);
	        	//printf("INTEGER %d", atoi(OUTPUT + 20));
	        	printf("CAS: %02d:%02d\n", (int)(GPS_2.Time/100), (int)(GPS_2.Time%100));
	        	printf("Speed: %d\n", GPS_2.SOG);
	        	printf("COG: %d\n", GPS_2.COG);


	        	//HAL_UART_Transmit_DMA(&huart2, (uint8_t*)OUTPUT, sizeof(OUTPUT));
	        }
	        else{
	        	GPS_2.Alt = 0;
	        	GPS_2.COG = 0;
	        	GPS_2.Lat = 0;
	        	GPS_2.Long = 0;
	        	GPS_2.SOG = 0;
				GPS_2.Time = 0;
				GPS_2.Status = 1111;

				//HAL_UART_Transmit_DMA(&huart2, (uint8_t*)"NO GPS" , sizeof("NO GPS"));
	        }
	        memset(OUTPUT_X, 0, 160);
	        old_pos_X = pos_X;                          /* Save current position as old */
	    }
	}

/////////////////////////////////////////////////////////////////////////////
////NASTIMAJ UART NA 19200 bps////
void UART_19200(){

	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 19200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
	  Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
	  Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
	  Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
	  Error_Handler();
	}
}

void UART_1_idle (UART_HandleTypeDef *huart){
	osSemaphoreRelease(GPS_SemHandle);
	printf("Callback_GPS\n");
}

void UART_2_idle (UART_HandleTypeDef *huart){
	osSemaphoreRelease(XBEE_SemHandle);
	printf("Callback_XBEE\n");
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		//Ko se konca transmit setup_a
		//CHANGE BAUDRATE
		HAL_UART_DeInit(&huart1);
		UART_19200();

		if (USART1->ISR & USART_ISR_ORE) // Overrun Error
		{
			USART3->ICR = USART_ICR_ORECF;
		}
		if (USART1->ISR & USART_ISR_NE) // Noise Error
		{
			USART1->ICR = USART_ICR_NECF;
		}
		if (USART1->ISR & USART_ISR_FE) // Framing Error
		{
			USART1->ICR = USART_ICR_FECF;
		}
		printf("UART IS SET TO:  ");
		printf("%08X\n", (unsigned int*)(USART1 -> BRR));



		osSemaphoreRelease(TX1_Done_SemHandle);
	}
}

/////////////////////////////////////////////////////////////////////////////
void GPS_Set_up(){
	static char Buffer[] = {
		#include "Out_GPS_Conf.txt"
	};
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)Buffer, sizeof(Buffer));
}

nav_t Get_GPS_1_data(void) {
	return GPS_1;
}

nav_t Get_XBEE_data(void) {
	return GPS_2;
}
/////////////////////////////////////////////////////////////////////////////

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Explicit enabling interrupt to support debugging in CubeIDE when using external flash loader */
  __enable_irq();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CRC_Init();
  MX_LTDC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_USART2_UART_Init();
  MX_ADC3_Init();
  MX_ADC1_Init();
  MX_TouchGFX_Init();
  /* USER CODE BEGIN 2 */
  /*HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3); //BUZZER
  HAL_Delay(300);
  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3); //BUZZER*/
  RetargetInit(&huart3);

  printf("\nSTART\n");

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of GPS_Sem */
  GPS_SemHandle = osSemaphoreNew(1, 1, &GPS_Sem_attributes);

  /* creation of TX1_Done_Sem */
  TX1_Done_SemHandle = osSemaphoreNew(1, 1, &TX1_Done_Sem_attributes);

  /* creation of StartBatt */
  StartBattHandle = osSemaphoreNew(1, 1, &StartBatt_attributes);

  /* creation of XBEE_Sem */
  XBEE_SemHandle = osSemaphoreNew(1, 1, &XBEE_Sem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  //Pojej tokene ker ne mores v cube nastimat da zac
  osSemaphoreAcquire(GPS_SemHandle, osWaitForever);
  osSemaphoreAcquire(TX1_Done_SemHandle, osWaitForever);
  osSemaphoreAcquire(StartBattHandle, osWaitForever);
  osSemaphoreAcquire(XBEE_SemHandle, osWaitForever);
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of GUITask */
  GUITaskHandle = osThreadNew(TouchGFX_Task, NULL, &GUITask_attributes);

  /* creation of GPS_RX */
  GPS_RXHandle = osThreadNew(StartGPS_RX, NULL, &GPS_RX_attributes);

  /* creation of DefaultTask */
  DefaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &DefaultTask_attributes);

  /* creation of XbeeTask */
  XbeeTaskHandle = osThreadNew(StartXbeeTask, NULL, &XbeeTask_attributes);

  /* creation of Battery_task */
  Battery_taskHandle = osThreadNew(Start_Battery_task, NULL, &Battery_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LTDC
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_QSPI
                              |RCC_PERIPHCLK_FMC;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 120;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 5;
  PeriphClkInitStruct.PLL3.PLL3N = 160;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 70;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable USB Voltage detector
  */
  HAL_PWREx_EnableUSBVoltageDetector();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10;
  hadc3.Init.Resolution = ADC_RESOLUTION_16B;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.Init.BytesSwap = DMA2D_BYTES_REGULAR;
  hdma2d.Init.LineOffsetMode = DMA2D_LOM_PIXELS;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 39;
  hltdc.Init.VerticalSync = 8;
  hltdc.Init.AccumulatedHBP = 42;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 522;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 528;
  hltdc.Init.TotalHeigh = 285;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */
  BSP_QSPI_Init_t qspi_initParams ;
  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 3;
  hqspi.Init.FifoThreshold = 1;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 26;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_ENABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */
  qspi_initParams.InterfaceMode = MT25TL01G_QPI_MODE;
  qspi_initParams.TransferRate  = MT25TL01G_DTR_TRANSFER ;
  qspi_initParams.DualFlashMode = MT25TL01G_DUALFLASH_ENABLE;
  BSP_QSPI_DeInit(0);
  if (BSP_QSPI_Init(0, &qspi_initParams) != BSP_ERROR_NONE)
  {
    Error_Handler( );
  }
  if(BSP_QSPI_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
  {
    Error_Handler( );
  }
  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_IDLE);
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM2 memory initialization sequence
  */
  hsdram2.Instance = FMC_SDRAM_DEVICE;
  /* hsdram2.Init */
  hsdram2.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram2.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram2.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram2.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram2.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram2.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram2.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram2.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram2.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram2.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 5;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram2, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  BSP_SDRAM_DeInit(0);
  if(BSP_SDRAM_Init(0) != BSP_ERROR_NONE)
  {
    Error_Handler( );
  }
  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DE_GPIO_Port, LCD_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RENDER_TIME_GPIO_Port, RENDER_TIME_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_Port, LCD_BL_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VSYNC_FREQ_GPIO_Port, VSYNC_FREQ_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LCD_DE_Pin */
  GPIO_InitStruct.Pin = LCD_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RENDER_TIME_Pin */
  GPIO_InitStruct.Pin = RENDER_TIME_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RENDER_TIME_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PG3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_CTRL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PK1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : VSYNC_FREQ_Pin */
  GPIO_InitStruct.Pin = VSYNC_FREQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(VSYNC_FREQ_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_TouchGFX_Task */
/**
  * @brief  Function implementing the GUITask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_TouchGFX_Task */
__weak void TouchGFX_Task(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartGPS_RX */
/**
* @brief Function implementing the GPS_RX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGPS_RX */
void StartGPS_RX(void *argument)
{
  /* USER CODE BEGIN StartGPS_RX */
	osSemaphoreAcquire(GPS_SemHandle, osWaitForever); //Caka da se konca setup
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	osSemaphoreAcquire(GPS_SemHandle, osWaitForever); //Caka da se konca prvi idle
	printf("RX ENABLE\n");
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //Pogruntaj da bos sele zacel sprejemat po prvem idle klicu
	HAL_UART_Receive_DMA(&huart1, RX_1_BUFFER, sizeof(RX_1_BUFFER)); //RESUJ TAKO DA PREVERIS PRVIH PAR BYTOV///POTENCIALNI PROBLEMI CE NE BO OBDELAL PODATKOV PRED PONOVNIM SPREJETJEM
	/* Infinite loop */
  for(;;)
  {
	osSemaphoreAcquire(GPS_SemHandle, osWaitForever);
	printf("GOT DATA\n");
	usart_rx_check();
	printf("PROCESED DATA\n");
  }
  /* USER CODE END StartGPS_RX */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the DefaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	GPS_Set_up();
	osSemaphoreAcquire(TX1_Done_SemHandle, osWaitForever);

	osSemaphoreRelease(GPS_SemHandle);
	osSemaphoreRelease(XBEE_SemHandle);
	osSemaphoreRelease(StartBattHandle);
	printf("SET UP FINISH\n");

	//Po koncanem set up, dovoli idle interrupt
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);

	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);

	osThreadTerminate(DefaultTaskHandle);
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartXbeeTask */
/**
* @brief Function implementing the XbeeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartXbeeTask */
void StartXbeeTask(void *argument)
{
  /* USER CODE BEGIN StartXbeeTask */
	osSemaphoreAcquire(XBEE_SemHandle, osWaitForever);
	__HAL_UART_CLEAR_IDLEFLAG(&huart2);
	osSemaphoreAcquire(XBEE_SemHandle, osWaitForever); //Caka da se konca prvi idle
	printf("XBEE ENABLE\n");

	HAL_UART_Receive_DMA(&huart2, RX_2_BUFFER, sizeof(RX_2_BUFFER));

  /* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(XBEE_SemHandle, osWaitForever);
		printf("GOT XBEE DATA\n");
		usart_rx_XBEE();
		printf("PROCESED XBEE DATA\n");
	}
  //osThreadTerminate(XbeeTaskHandle);
  /* USER CODE END StartXbeeTask */
}

/* USER CODE BEGIN Header_Start_Battery_task */
/**
* @brief Function implementing the Battery_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Battery_task */
void Start_Battery_task(void *argument)
{
  /* USER CODE BEGIN Start_Battery_task */
	osSemaphoreAcquire(StartBattHandle,osWaitForever);
	long unsigned int ADC_U1 = 500000; //Neka random velika cifra ki je ne more dosezt
	long unsigned int ADC_U3 = 500000;

	/* Infinite loop */
	for(;;)
	{
		HAL_ADC_Start(&hadc1); // Ta zgornji ADC R2 = 300 R1 = 500

		if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK){
			ADC_U1 = HAL_ADC_GetValue(&hadc1);
		}
		else{
			ADC_U1 = 500000;
		}

		HAL_ADC_Start(&hadc3); // Ta spodnji ADC ADC R2 = 300 R1 = 300

		if(HAL_ADC_PollForConversion(&hadc3, 10) == HAL_OK){
			ADC_U3 = HAL_ADC_GetValue(&hadc3);
		}
		else{
			ADC_U3 = 500000;
		}

		if(ADC_U1 != 500000){
			GPS_1.V1 = (((double)ADC_U1*3.3)/(65535.0))/(298.0/(465.5 + 298.0));
			GPS_1.V2 = (((double)ADC_U3*3.3)/(65535.0))/(298.0/(298.0 + 298.0)); //ta je od 4.2 do 3

			GPS_1.V1 = GPS_1.V1 - GPS_1.V2;

			if(HAL_GPIO_ReadPin(GPIOK, GPIO_PIN_1) != 1){

			if(GPS_1.V1 > 4.2){
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);
			}
			else if(GPS_1.V1 < 4.1){
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);
			}

			if(GPS_1.V2 > 4.2){
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
			}
			else if(GPS_1.V2 < 4.1){
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
			}

			}
			//printf("Napetost_1 [mV]: %d    ADC_VAL: %d\n", (int)(Voltage_1*1000), (int)(ADC_U1));//STM bug = float, rtos in printf ne gresta skupaj
			//printf("Napetost_2 [mV]: %d    ADC_VAL: %d\n", (int)(Voltage_2*1000), (int)(ADC_U3));
		}
		else{
			printf("Zero ADC");
			GPS_1.V1 = 0;
			GPS_1.V2 = 0;
		}

		osDelay(300);
	}
  /* USER CODE END Start_Battery_task */
}

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
