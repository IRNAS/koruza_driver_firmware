/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "frame.h"
//#include "AccelStepper.h"
//#include "stepper.h"
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

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM3){
		/* Get angles form encoder X and encoder Y */
		koruza_encoders_get_angles(&koruza_encoders);
		/* Calculate absolute position of encoders */
		koruza_encoders_absolute_position(&koruza_encoders);
		/* check stepper motor error */
		koruza_encoders_absolute_position_steps(&koruza_encoders);

		koruza_encoder_stepper_error(&koruza_steppers, &koruza_encoders);


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

	MX_SPI2_Init();


	koruza_encoders_init(&koruza_encoders, CONNECTED, NOT_CONNECTED);

	MX_TIM3_Init();
	HAL_TIM_Base_MspInit(&TimHandle);
/*
	int a = 0;
	while(koruza_encoders.encoder_x.abs_angle == 0);{
		a++;
		if(a > 1000){
			break;
		}
	}*/

/*
while(1){
	HAL_Delay(5000);
	if(koruza_encoders.encoder_x.turn_cnt >= 0){
		printf("\ntrue angle X: %f\tabs angle X: %f", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.abs_angle);
	}
	else{
		printf("\ntrue angle X: %f\tabs angle X: -%f", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.abs_angle);
	}
	//printf("\nAngle Y: %ld", koruza_encoders.encoder_y.abs_angle);
}
*/
	driver_state_t state = IDLE;


	/* Stepper motors initialization */
	koruza_motors_init(&koruza_steppers, STEPPER_CONNECTED, STEPPER_CONNECTED, STEPPER_CONNECTED);
	set_motor_coordinate(&koruza_steppers.stepper_x.stepper, (long)koruza_encoders.encoder_x.steps);
	current_motor_position.x = (uint32_t)koruza_encoders.encoder_x.steps;

#ifdef DEBUG_MODE
	/* Generate message - test message */
	message_t msg;
	message_init(&msg);
	//message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
	message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
	tlv_motor_position_t position = {-1024, 0, 0};
	message_tlv_add_motor_position(&msg, &position);
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
#ifdef DEBUG_MODE
		/*
		if(koruza_encoders.encoder_x.turn_cnt >= 0){
			printf("\ntrue angle X: %f\tabs angle X: %f", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.abs_angle);
		}
		else{
			printf("\ntrue angle X: %f\tabs angle X: -%f", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.abs_angle);
		}*/
		//printf("$%d,%d;", (int)koruza_encoders.encoder_x.steps, (int)koruza_steppers.stepper_x.stepper._currentPos);
		//printf("\nAngle X: -%f\tStepper X: %ld", koruza_encoders.encoder_x.encoder.true_angle, koruza_steppers.stepper_x.stepper._currentPos);

		if(koruza_encoders.encoder_x.end != ENCODER_RUN){
			printf("\nencoder end X: %d", koruza_encoders.encoder_x.end);
		}
		if(koruza_encoders.encoder_x.turn_cnt < 0){
			printf("\nAngle X: -%f\tSteps X: %f\tStepper X: %ld", koruza_encoders.encoder_x.abs_angle, koruza_encoders.encoder_x.steps, koruza_steppers.stepper_x.stepper._currentPos);

		}else{
			printf("\nAngle X: %f\tSteps X: %f\tStepper X: %ld", koruza_encoders.encoder_x.abs_angle, koruza_encoders.encoder_x.steps, koruza_steppers.stepper_x.stepper._currentPos);
		}

#endif
		/* Move steppers. */
		run_motors(&koruza_steppers, &koruza_encoders);

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
							/* Koruza motors X and Y homing*/
							if(parsed_position.x == -1 && parsed_position.y == -1 && parsed_position.z == -1){
								//TODO: add if the encoders are not connected, just send the motors to specified location
								koruza_steppers.mode = STEPPERS_HOMING_MODE;
#ifdef DEBUG_MODE
								printf("\nStart homing routin");
#endif
								parsed_position.x = HOME_X_COORDINATE;
								parsed_position.y = HOME_Y_COORDINATE;
								parsed_position.z = 0;
								/* Calculate number of move steps to new position */
								move_steppers = Claculate_motors_move_steps(&parsed_position, &current_motor_position);
								/* Move motors to homing position */
								move(&koruza_steppers.stepper_x.stepper, (long)move_steppers.x);
								move(&koruza_steppers.stepper_y.stepper, (long)move_steppers.y);
								move(&koruza_steppers.stepper_z.stepper, (long)move_steppers.z);

							}
							/* Move Koruza motors to received coordinates*/
							else{
								koruza_steppers.mode = STEPPERS_IDLE_MODE;
								/* Calculate number of move steps to new position */
								move_steppers = Claculate_motors_move_steps(&parsed_position, &current_motor_position);
								/* Move motors to sent position */
								move(&koruza_steppers.stepper_x.stepper, (long)move_steppers.x);
								move(&koruza_steppers.stepper_y.stepper, (long)move_steppers.y);
								move(&koruza_steppers.stepper_z.stepper, (long)move_steppers.z);
							}
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
						JumpToBootLoader();
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
 * @brief  SPI error callbacks
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	Error_Handler();
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
