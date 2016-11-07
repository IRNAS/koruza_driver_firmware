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
#include "stdlib.h"
#include "eeprom.h"

//#include "IRremote.h"
//#include "IRremoteInt.h"

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
volatile int Transfer_cplt;
char Rx_last[2] = {0, 0};
char test1 = 0;
/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;

message_t msg_rep;
//tlv_error_report_t koruza_error_report;
//uint32_t koruza_error_report_check;

uint8_t frame_rep[1024];
ssize_t frame_size_rep;

/* UART RX Interrupt callback routine */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	uint8_t i;
	/* Use selected UART for receive */
	if (huart->Instance == USART1){

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
		//TODO: test over current limit error message
		if(uhADCxConvertedValue >= OVERCURRENT_LIMIT){
			koruza_error_report_ch.code |= 1 << 0;
			//send the error report to witi
			koruza_error_report.code = koruza_error_report_ch.code;
			message_init(&msg_rep);
			message_tlv_add_reply(&msg_rep, REPLY_ERROR_REPORT);
			message_tlv_add_error_report(&msg_rep, &koruza_error_report);
			message_tlv_add_checksum(&msg_rep);
			frame_size_rep = frame_message(frame_rep, sizeof(frame_rep), &msg_rep);
			HAL_UART_Transmit(&huart1, (uint8_t *)&frame_rep, frame_size_rep, 1000);
			message_free(&msg_rep);
			//restart the MCU
			NVIC_SystemReset();
		}else{
			koruza_error_report_check &= ~(1 << 0);
		}
	}
}


/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = {0x5555, 0x6666, 0x7777};
uint16_t VarDataTab[NB_OF_VAR] = {0, 0, 0};
uint16_t VarValue = 0x1234,VarDataTmp = 0;
int main(void){

	int send_ir_test_count = 0;
	unsigned long send_ir_test[] = {0x001, 0x002, 0x003, 0x004, 0x005};
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

	ir_decode_results results;
	koruza_irlink_init(&koruza_irlink);

	/* LED variable init */
	WS2812B_color_t special_LED_color;
	special_LED_color.red = 0xFF;
	special_LED_color.green = 0x00;
	special_LED_color.blue = 0xFF;

	/* LEDs init, all off*/
	WS2812B_level_indicator_wLED(special_LED_color, 0, 23);

#ifdef DEBUG_MODE
	printf("\n*********Hello*********\r\nKoruza driver terminal \r\n");
	printf("***********************");
	printf("\n\n");
#endif
	MX_SPI2_Init();

	koruza_encoders.encoder_x.calibration.amplitude = 125;
	koruza_encoders.encoder_x.calibration.start = -270;
	koruza_encoders.encoder_x.calibration.offset = -68;

	koruza_encoders.encoder_y.calibration.amplitude = 12.5;
	koruza_encoders.encoder_y.calibration.start = 10;
	koruza_encoders.encoder_y.calibration.offset = -16;
	/* Wait for encoders to boot*/
	HAL_Delay(200000);

	/* Stepper motors initialization */
	koruza_motors_init(&koruza_steppers, STEPPER_CONNECTED, STEPPER_CONNECTED, STEPPER_NOT_CONNECTED);
	/* Encoder initialization with connection and magnetic field check*/
	koruza_encoders_init(&koruza_encoders, CONNECTED, CONNECTED);

	driver_state_t state = IDLE;

	/* Encoders max power-on time is 10 ms */
	HAL_Delay(200000);

	/* Get encoder angles for start position calculation */
	koruza_encoders_get_angles(&koruza_encoders);

	if(koruza_encoders.encoder_x.encoder_connected == CONNECTED){
		set_motor_coordinate(&koruza_steppers.stepper_x.stepper, (long)(koruza_encoders.encoder_x.encoder.true_angle * ONE_ANGLE_STEPPS));
		current_motor_position.x = (int32_t)(koruza_encoders.encoder_x.encoder.true_angle * ONE_ANGLE_STEPPS);
	}
	if(koruza_encoders.encoder_y.encoder_connected == CONNECTED){
		set_motor_coordinate(&koruza_steppers.stepper_y.stepper, (long)(koruza_encoders.encoder_y.encoder.true_angle * ONE_ANGLE_STEPPS));
		current_motor_position.y = (int32_t)(koruza_encoders.encoder_y.encoder.true_angle * ONE_ANGLE_STEPPS);
	}

#ifdef DEBUG_MODE_MSG_GENERATOR
	/* Generate message - test message */
	message_t msg;
	message_init(&msg);
	message_tlv_add_command(&msg, COMMAND_GET_STATUS);
	//message_tlv_add_command(&msg, COMMAND_MOVE_MOTOR);
	//message_tlv_add_reply(&msg, REPLY_ERROR_REPORT);
	//tlv_motor_position_t position = {-100000, -100000, -100000};
	//message_tlv_add_motor_position(&msg, &position);
	message_tlv_add_power_reading(&msg, 0x0444);
	//tlv_error_report_t error_report;
	//error_report.code = 35;
	//message_tlv_add_error_report(&msg, &error_report);
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

	/* Error report */
	koruza_error_report.code = 0xFFFFFFFF;
	koruza_error_report_check = 0x00000000;

	/* Move steppers for, from absolute position */
	tlv_motor_position_t move_steppers;
	move_steppers.x = 0;
	move_steppers.y = 0;
	move_steppers.z = 0;

	uint8_t frame[1024];
	ssize_t frame_size;



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


#ifdef DEBUG_MODE
	printf("\nKoruza driver ready\n");
#endif
#ifdef DEBUG_ENCODER_POSITION_MODE
	tlv_motor_position_t test_position;
	test_position.x = 30000;
	test_position.y = 10000;
	test_position.z = 0;
	koruza_set_stored_values(&koruza_encoders, &koruza_steppers, test_position);
#endif


	int restore_receive = 0;

	/* LEDs init, all off*/
	int test = 0;
	while(1){
		WS2812B_level_indicator_wLED(special_LED_color, test, 23);
		test++;
		if(test > 23){
			test = 0;
		}
		HAL_Delay(50000);
	}
		WS2812B_level_indicator_wLED(special_LED_color, 1, 23);

	/* Infinite loop */
	while(True){
		test = 0;
		/* Send command RESTORE_MOTOR until witi is ready*/
		if(restore_receive == 0){
			message_init(&msg_rep);
			message_tlv_add_command(&msg_rep, COMMAND_RESTORE_MOTOR);
			message_tlv_add_motor_position(&msg_rep, &move_steppers);
			message_tlv_add_checksum(&msg_rep);
			frame_size = frame_message(frame, sizeof(frame), &msg_rep);
			//printf("\nResponce serialized protocol message:\n");
			//for (size_t i = 0; i < frame_size; i++){
			//	printf("%02X ", frame[i]);
			//}
			//printf("\n");
			HAL_UART_Transmit(&huart1, (uint8_t *)&frame, frame_size, 1000);
			message_free(&msg_rep);
			HAL_Delay(50000);
		}
		else{
			/* Check the status of the encoders*/
			koruza_encoders_magnetic_filed_check(&koruza_error_report_ch.code, &koruza_encoders, 0);

			/* If status has changed send message to witi*/
			if(koruza_error_report_ch.code != koruza_error_report.code){
				koruza_error_report.code = koruza_error_report_ch.code;
				message_init(&msg_responce);
				message_tlv_add_reply(&msg_responce, REPLY_ERROR_REPORT);
				message_tlv_add_error_report(&msg_responce, &koruza_error_report);
				message_tlv_add_checksum(&msg_responce);
				frame_size_rep = frame_message(frame_rep, sizeof(frame_rep), &msg_responce);
				HAL_UART_Transmit(&huart1, (uint8_t *)&frame_rep, frame_size_rep, 1000);
				message_free(&msg_responce);
			}
		}

		/* IR link receive check */
		//koruza_irlink_receive(&koruza_irlink);
		//HAL_Delay(400);
//		if (IRrecv_decode(&results))
//		{
//			//TODO: when received than what?
//			//xyz = results.value;
//			printf("got signal %#08x\n", (unsigned int)results.value);
//		    IRrecv_resume(); // Receive the next value
//		}
#ifdef DEBUG_ENCODER_POSITION_MODE
		//printf("%f, %f, %ld\n", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.steps, koruza_steppers.stepper_x.stepper._currentPos);//, koruza_encoders.encoder_y.steps, koruza_steppers.stepper_y.stepper._currentPos);
		//printf("$%d %d %d;", (int)koruza_encoders.encoder_x.steps, (int)koruza_steppers.stepper_x.stepper._currentPos, (int)(degreesToRadians((double)koruza_encoders.encoder_x.encoder.true_angle - koruza_encoders.encoder_x.calibration.start)));
		//true_angle_calibrate_x += (float)(koruza_encoders.encoder_x.calibration.amplitude * sin(degreesToRadians((double)koruza_encoders.encoder_x.encoder.true_angle - koruza_encoders.encoder_x.calibration.start)) + koruza_encoders.encoder_x.calibration.offset)/ONE_ANGLE_STEPPS;
		//true_angle_calibrate_y += (float)(koruza_encoders.encoder_y.calibration.amplitude * sin(degreesToRadians((double)koruza_encoders.encoder_y.encoder.true_angle - koruza_encoders.encoder_y.calibration.start)) + koruza_encoders.encoder_y.calibration.offset)/ONE_ANGLE_STEPPS;

		//printf("%ld, %f, %f, %ld, %f, %f\n", koruza_steppers.stepper_x.stepper._currentPos, koruza_encoders.encoder_x.encoder.true_angle, true_angle_calibrate_x, koruza_steppers.stepper_y.stepper._currentPos, koruza_encoders.encoder_y.encoder.true_angle, true_angle_calibrate_y);
		//printf("true angle X: %f\tAngle X: %f\tSteps X: %f\tStepper X: %ld\t",koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_x.abs_angle, koruza_encoders.encoder_x.steps, koruza_steppers.stepper_x.stepper._currentPos);
		//printf("true angle y: %f\tAngle y: %f\tSteps y: %f\tStepper y: %ld\n",koruza_encoders.encoder_y.encoder.true_angle, koruza_encoders.encoder_y.abs_angle, koruza_encoders.encoder_y.steps, koruza_steppers.stepper_y.stepper._currentPos);

		//printf("Error X: %f\tPeak Hold X: %f\tError Y: %f\tPeak Hold Y: %f\n", step_error_x, peak_error_x, step_error_y, peak_error_y);
		//printf("Angle X: %f\t Angle Y: %f\t", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_y.encoder.true_angle);
		printf("step X: %ld\tangle x: %f\tstep y: %ld\tangle y: %f\n", koruza_steppers.stepper_x.stepper._currentPos, koruza_encoders.encoder_x.encoder.true_angle, koruza_steppers.stepper_y.stepper._currentPos, koruza_encoders.encoder_y.encoder.true_angle);
#endif
		/* Read encoder and check if end is reached */
		//printf("step X: %ld\tangle x: %f\tstep y: %ld\tangle y: %f", koruza_steppers.stepper_x.stepper._currentPos, koruza_encoders.encoder_x.encoder.true_angle, koruza_steppers.stepper_y.stepper._currentPos, koruza_encoders.encoder_y.encoder.true_angle);
		//printf("angle x: %f\tangle y: %f\t", koruza_encoders.encoder_x.encoder.true_angle, koruza_encoders.encoder_y.encoder.true_angle);
		//printf("%ld, %f, %ld, %f, %f, %f\n", koruza_steppers.stepper_x.stepper._currentPos, koruza_encoders.encoder_x.encoder.true_angle, koruza_steppers.stepper_y.stepper._currentPos, koruza_encoders.encoder_y.encoder.true_angle, koruza_encoders.encoder_x.diff, koruza_encoders.encoder_y.diff);

		/* Get new angles from encoders */
		koruza_encoders_get_angles(&koruza_encoders);
		/* Get error between stepper and encoder*/
		//koruza_steppers.stepper_x.mode = STEPPER_MAXIMUM_REACHED;
		koruza_steppers_encoder_error_calculation(&koruza_encoders, &koruza_steppers);

		/* Move steppers. */
		run_motors(&koruza_steppers, &koruza_encoders);

		switch(state){
			case IDLE:
				if(Transfer_cplt != 0){
#ifdef DEBUG_RECEIVE_MSG_MODE
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
#ifdef DEBUG_RECEIVE_MSG_MODE
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
#ifdef DEBUG_STATUS_MODE
						printf("message receive: GET_STATUS\n");
						printf("Current motor position (%ld, %ld, %ld)\n",
								(long)current_motor_position.x, (long)current_motor_position.y, (long)current_motor_position.z
						  );
						//printf("\nParsed protocol message response: ");
						//message_print(&msg_responce);
#endif
						frame_size = frame_message(frame, sizeof(frame), &msg_responce);
#ifdef DEBUG_STATUS_SERIAL_MODE
						printf("\nResponce serialized protocol message:\n");
						for (size_t i = 0; i < frame_size; i++){
							printf("%02X ", frame[i]);
						}
						printf("\n");
#endif
						//send status message
						if(restore_receive == 1){
							HAL_UART_Transmit(&huart1, (uint8_t *)&frame, frame_size, 1000);
						}
						message_free(&msg_responce);
#ifdef DEBUG_IRLINK
						koruza_irlink_send(&koruza_irlink, send_ir_test[send_ir_test_count], 12);
						if(send_ir_test_count == 4){
							send_ir_test_count = 0;
						}else{
							send_ir_test_count++;
						}
#endif
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
#ifdef DEBUG_MOTOR_MOVE_MODE
							printf("message receive: MOTOR_MOVE\n");
							printf("Parsed command %u and motor position (%ld, %ld, %ld)\n",
								parsed_command,
								(long)parsed_position.x, (long)parsed_position.y, (long)parsed_position.z
							  );
#endif

							koruza_steppers.mode = STEPPERS_IDLE_MODE;
							/* Calculate number of move steps to new position */
							//parsed_position.x *= -1;
							//parsed_position.y *= -1;
							move_steppers = Claculate_motors_move_steps(&parsed_position, &current_motor_position);
							/* Move motors to sent position */
							move(&koruza_steppers.stepper_x.stepper, (long)move_steppers.x);
							move(&koruza_steppers.stepper_y.stepper, (long)move_steppers.y);
							move(&koruza_steppers.stepper_z.stepper, (long)move_steppers.z);

							state = END_STATE;
						}

						break;
					case COMMAND_HOMING:
						koruza_steppers.mode = STEPPERS_HOMING_MODE;
#ifdef DEBUG_MODE
						printf("homing routine: START\n");
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
						state = END_STATE;

						break;

					case COMMAND_RESTORE_MOTOR:
						if (message_tlv_get_motor_position(&msg_parsed, &parsed_position) != MESSAGE_SUCCESS) {
#ifdef DEBUG_MODE
							printf("Failed to get motor position TLV.\n");
#endif
							message_free(&msg_parsed);
							state = ERROR_STATE;
						}else{
							koruza_set_stored_values(&koruza_encoders, &koruza_steppers, parsed_position);
							restore_receive = 1;
						}
						state = END_STATE;
						break;

					case COMMAND_SEND_IR:
						//test code
						//koruza_irlink_send(&koruza_irlink, 0xF00, 12);

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
  RCC_OscInitStruct.PLL.PLLM = 8;//16;
  RCC_OscInitStruct.PLL.PLLN = 100;//336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;//RCC_PLLP_DIV4;
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

void IRrecv_DataReadyCallback(unsigned long data)
{
	//IRsend_sendSony(0xF00, 12);
	//HAL_Delay(10000); //1000ms delay
	printf("got signal %#08x\n", (unsigned int)data);
	IRrecv_resume(); // Receive the next value
}

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
