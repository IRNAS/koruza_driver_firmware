/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "frame.h"
#include "AccelStepper.h"
#include "stepper.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "dma.h"
#include <stdio.h>
#include "main.h"
#include <inttypes.h>


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/* Struct FILE is implemented in stdio.h */
FILE __stdout;

/* Private variables ---------------------------------------------------------*/
int test, i = 0;
int len, message_len = 0;

volatile uint8_t Rx_Buffer[100];
uint8_t Rx_data[2];
uint8_t tx_responce_buffer[1024];
int Rx_indx;
int Transfer_cplt;
char Rx_last[2] = {0, 0};
char test1 = 0;
/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;


/* UART RX Interrupt callback routine */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	uint8_t i;
	/* Use selected UART for receive */
	if (huart->Instance == USART1){

		//TODO: check if this is necessary!!!
		/* Clear Rx_Buffer before receiving new data */
		if (Rx_indx==0){
			for (i=0;i<100;i++) Rx_Buffer[i]=0;
		}
		/* Start byte received */
		if(Rx_data[0] == FRAME_MARKER_START){
			/* Start byte received in the frame */
			if(Rx_last[0] == FRAME_MARKER_ESCAPE && Rx_Buffer[0] == FRAME_MARKER_START){
				Rx_Buffer[Rx_indx++]=Rx_data[0];
			}
			/* Real start byte received */
			else if(Rx_last[0] != FRAME_MARKER_ESCAPE){
				Rx_indx = 0;
				Rx_Buffer[Rx_indx++]=Rx_data[0];

			}
		}
		/* End byte received */
		else if(Rx_data[0] == FRAME_MARKER_END){
			/* End byte received in the frame */
			if(Rx_last[0] == FRAME_MARKER_ESCAPE && Rx_Buffer[0] == FRAME_MARKER_START){
				Rx_Buffer[Rx_indx++]=Rx_data[0];
			}
			/* Real end byte received */
			else if(Rx_last[0] != FRAME_MARKER_ESCAPE && Rx_Buffer[0] == FRAME_MARKER_START){
				Rx_Buffer[Rx_indx++]=Rx_data[0];
				message_len = Rx_indx;
				Rx_indx=0;
				/* Transfer complete, data is ready to read */
				Transfer_cplt=1;
				/* Disable USART1 interrupt */
				HAL_NVIC_DisableIRQ(USART1_IRQn);
			}
		}
		else{
			if(Rx_Buffer[0] == FRAME_MARKER_START){
				Rx_Buffer[Rx_indx++]=Rx_data[0];
			}
		}

		/* Store last received byte for ESC check */
		Rx_last[0] = Rx_data[0];
		/* Activate UART receive interrupt every time */
		if(HAL_UART_Receive_IT(&huart1, (uint8_t *)&Rx_data, 1) != HAL_OK){
			/* Start UART error*/
			Error_Handler();
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle) {
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &uhADCxConvertedValue, 1) != HAL_OK) {
		/* Start Conversation Error */
		Error_Handler();
	}else{
		if(uhADCxConvertedValue >= OVERCURRENT_LIMIT){
			//TODO: add current protection system
			NVIC_SystemReset();
		}
	}
}


int main(void){
	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();

	enum states state = IDLE;

	/* Stepper motors struts */
	Stepper_t stepper_motor_x;
	Stepper_t stepper_motor_y;
	Stepper_t stepper_motor_z;

	/* Stepper motors initialization */
	Init_koruza_motors(&stepper_motor_x, &stepper_motor_y, &stepper_motor_z);

#ifdef DEBUG_MODE
	/* Generate message - test message */
	message_t msg;
	message_init(&msg);
	//message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
	message_tlv_add_command(&msg, COMMAND_REBOOT);
	tlv_motor_position_t position = {1000, 1000, 1000};
	//message_tlv_add_motor_position(&msg, &position);
	message_tlv_add_checksum(&msg);

	uint8_t test_frame[1024];
	ssize_t test_frame_size;

	printf("Generated protocol message: ");
	message_print(&msg);
	printf("\n");

	uint8_t buffer[1024];
	size_t length = message_serialize(buffer, 1024, &msg);
	printf("Serialized protocol message:\n");
	for (size_t i = 0; i < length; i++) {
		printf("%02X ", buffer[i]);
	}
	printf("\n");

	test_frame_size = frame_message(test_frame, sizeof(test_frame), &msg);
	printf("Serialized protocol message with frame:\n");
	for (size_t i = 0; i < test_frame_size; i++) {
		printf("%02X ", test_frame[i]);
	}
	printf("\n");
	message_free(&msg);
#endif
	/* Parsed message */
	message_t msg_parsed;
	tlv_command_t parsed_command;
	tlv_motor_position_t parsed_position;

	/* Response message */
	message_t msg_responce;

	tlv_motor_position_t next_motor_position;
	next_motor_position.x = 0;
	next_motor_position.y = 0;
	next_motor_position.z = 0;

	/* Move steppers for, from absolute position */
	tlv_motor_position_t move_steppers;
	move_steppers.x = 0;
	move_steppers.y = 0;
	move_steppers.z = 0;

	uint8_t frame[1024];
	ssize_t frame_size;

#ifdef DEBUG_MODE
	printf("Hello\r\nKoruza driver terminal \r\n");
	printf("\n\n");
#endif

	/* Activate UART RX interrupt every time receiving 1 byte. */
	if(HAL_UART_Receive_IT(&huart1, (uint8_t *)&Rx_data, 1) != HAL_OK){
		/* Start UART error*/
		Error_Handler();
	}

	/* Start the conversion process and enable interrupt */
	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &uhADCxConvertedValue, 1) != HAL_OK) {
		/* Start Conversation Error */
		Error_Handler();
	}

	/* Infinite loop */
	while(True){
		test = 0;

		/* Move steppers. */
		run_motors(&stepper_motor_x, &stepper_motor_y, &stepper_motor_z);

		switch(state){
			case IDLE:
				if(Transfer_cplt != 0){
#ifdef DEBUG_MODE
					printf("\nReceived serialized protocol message:\n");
					//HAL_UART_Transmit(&huart1, (uint8_t *)&Rx_Buffer, message_len, 1000);
					for (size_t i = 0; i < message_len; i++) {
						printf("%02X ", Rx_Buffer[i]);
					}
					printf("\n");
#endif
					state = MESSAGE_PARSE;
				}else{
					state = IDLE;
				}
				break;

			case MESSAGE_PARSE:
				//parse received message
				frame_parser((uint8_t *)&Rx_Buffer, message_len, &msg_parsed);
#ifdef DEBUG_MODE
				printf("\nParsed protocol message: ");
				message_print(&msg_parsed);
				printf("\n");
#endif
				//parse received message command
				if (message_tlv_get_command(&msg_parsed, &parsed_command) != MESSAGE_SUCCESS) {
#ifdef DEBUG_MODE
					printf("Failed to get command TLV.\n");
#endif
					message_free(&msg_parsed);
					state = ERROR_STATE;
				}else{
					state = ACTICVE_STATE;
				}
				break;

			case ACTICVE_STATE:
				switch(parsed_command){
					case COMMAND_GET_STATUS:
						//Response message
						message_init(&msg_responce);
						message_tlv_add_reply(&msg_responce, REPLY_STATUS_REPORT);
						message_tlv_add_motor_position(&msg_responce, &current_motor_position);
						message_tlv_add_checksum(&msg_responce);
#ifdef DEBUG_MODE
						printf("\n");
						printf("Current motor position (%ld, %ld, %ld)\n",
								(long)current_motor_position.x, (long)current_motor_position.y, (long)current_motor_position.z
						  );
						printf("\n");
						printf("Parsed protocol message response: ");
						message_print(&msg_responce);
						printf("\n");

#endif
						frame_size = frame_message(frame, sizeof(frame), &msg_responce);
#ifdef DEBUG_MODE
						printf("\nResponce serialized protocol message:\n");
						for (size_t i = 0; i < frame_size; i++){
							printf("%02X ", frame[i]);
						}
						printf("\n");
#endif
						//send status message
						HAL_UART_Transmit(&huart1, (uint8_t *)&frame, frame_size, 1000);

						message_free(&msg_responce);

						state = END_STATE;
						break;

					case COMMAND_MOVE_MOTOR:
						/* Get command to move motors */
						if (message_tlv_get_motor_position(&msg_parsed, &parsed_position) != MESSAGE_SUCCESS) {
#ifdef DEBUG_MODE
							printf("Failed to get motor position TLV.\n");
#endif
							message_free(&msg_parsed);
							state = ERROR_STATE;
						}else{
#ifdef DEBUG_MODE
							printf("Parsed command %u and motor position (%"PRIu32", %"PRIu32", %"PRIu32")\n",
								parsed_command,
								parsed_position.x, parsed_position.y, parsed_position.z
							  );
#endif
							/* Calculate number of move steps to new position */
							move_steppers = Claculate_motors_move_steps(&parsed_position, &current_motor_position);
							/* Save new motor position */
							next_motor_position.x = parsed_position.x;
							next_motor_position.y = parsed_position.y;
							next_motor_position.z = parsed_position.z;
							/* Move motors to sent position */
							move(&stepper_motor_x, (long)move_steppers.x);
							move(&stepper_motor_y, (long)move_steppers.y);
							move(&stepper_motor_z, (long)move_steppers.z);



							state = END_STATE;
						}

						break;
					case COMMAND_SEND_IR:

						break;
					case COMMAND_REBOOT:
#ifdef DEBUG_MODE
						printf("Restarting MCU software....\n");
#endif
						/* Software restart of the system */
						NVIC_SystemReset();
						state = END_STATE;
						break;
					case COMMAND_FIRMWARE_UPGRADE:
#ifdef DEBUG_MODE

						printf("Entering bootloder mode.\n");
#endif
						/* Go to Bootloader mode, and whait for new firmware */
						//JumpToBootLoader();

						Set_motors_coordinates(&stepper_motor_x,  0, &stepper_motor_y, 1000, &stepper_motor_z, 1000);

						state = END_STATE;
						break;
				}
				break;

			case ERROR_STATE:

				state = END_STATE;
				break;

			case END_STATE:
				//free messages memory
				message_free(&msg_parsed);
				/* Enable USART1 interrupt */
				HAL_NVIC_EnableIRQ(USART1_IRQn);
				/*Activate UART RX interrupt every time receiving 1 byte.*/
				HAL_UART_Receive_IT(&huart1, (uint8_t *)&Rx_data, 1);
				/* Reset transfer_complete flag */
				Transfer_cplt=0;

				//HAL_Delay(500);

				state = IDLE;
				break;

		}//end switch(state)
	}//end while(true)
}//end main()

/**Prototype for the printf() function**/
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/** System Clock Configuration*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* It is devided by 100000 because of AccelStepper*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/100000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  /* SysTick_IRQn needs to be higher priority then other IRQs */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif
