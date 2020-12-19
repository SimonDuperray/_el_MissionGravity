/*===========================
 *          INCLUDE
 ==========================*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "macro_types.h"
#include "stm32f1_adc.h"
#include "stm32f1_gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32f1_timer.h"
#include "tft_ili9341/stm32f1_ili9341.h"
//#include "ESP8266_HAL.h"

/*===========================
 *  VARIABLES DECLARATION
 ==========================*/

#define GOAL_FIRST_LEVEL			5
#define MAXHEIGHT							220
#define MAXWIDTH							305
#define ADC_SENSOR_CHANNEL		ADC_1

char GAMELAUNCHED[]="Game launched";
char GAMEINWAIT[]="Game in wait";

int score;
char scoreArray[10];

int goal=GOAL_FIRST_LEVEL;
char goalArray[10];

int16_t photoVal;
char photoValArray[10];

int level;
char levelArray[10];

int remain;
char remainArray[10];

char nonePlayer[10]="N";
char playerArray[10]="Sim";

int isLaunch=0;

int frontUltrasonic;
char frontUltrasonicArray[10];

int leftUltrasonic;
char leftUltrasonicArray[10];

int rightUltrasonic;
char rightUltrasonicArray[10];

/*===========================
 *      P. METHODS
 ==========================*/

/*=== LEDS & BUZZER & BP ===*/

bool_e readButton(void){
	return HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

void writeGreenLed(bool_e b){
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

void blinkGreenLed(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 0);
		HAL_Delay(delay);
	}
}

void blinkGreenLedWithBuzzer(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 0);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 0);
		HAL_Delay(delay);
	}
}

void writeYellowLed(bool_e b){
	HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, b);
}

void blinkYellowLed(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 0);
		HAL_Delay(delay);
	}
}

void blinkYellowLedWithBuzzer(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 1);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 0);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 0);
		HAL_Delay(delay);
		}
}

void writeRedLed(bool_e b){
	HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, b);
}

void blinkRedLed(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 0);
		HAL_Delay(delay);
	}
}

void blinkRedLedWithBuzzer(int cpt, uint32_t delay){
	for(int i=0; i<cpt; i++){
		HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 1);
		HAL_Delay(delay);
		HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 0);
		HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, 0);
		HAL_Delay(delay);
		}
}

void writeBuzzer(bool_e b){
	HAL_GPIO_WritePin(BUZZER_GPIO, BUZZER_PIN, b);
}

void toneBuzzer(int cpt){
	for(int i=0; i<cpt; i++){
		writeBuzzer(1);
		HAL_Delay(200);
		writeBuzzer(0);
		HAL_Delay(200);
	}
}

void allLedOn(void){
	HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);
	HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 1);
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);
}

void allLedOff(void){
	HAL_GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 0);
	HAL_GPIO_WritePin(LED_YELLOW_GPIO, LED_YELLOW_PIN, 0);
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 0);
}

/*=== SCREEN ===*/

void displayLeftPart(){
	ILI9341_Puts(20, 50, "Level:", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 90, "Player:", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 130, "Score:", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 170, "Remain:", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(20, 210, "Goal:", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void displayRightPart(){
	ILI9341_DrawLine(152, 40, 152, 220, ILI9341_COLOR_BLACK);
	ILI9341_Puts(162, 60, "See results on", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(172, 100, "192.168.1.42", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_DrawLine(160, 130, 300, 130, ILI9341_COLOR_BLACK);
	ILI9341_Puts(180, 140, "Robot name", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(185, 170, "PSE 20/21", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	ILI9341_Puts(162, 200, "Denis/Duperray", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void displayData(void){
	if(level!=0 && remain != 0 && score != 0 && goal != 0){
		itoa(level, levelArray, 10);
		itoa(remain, remainArray, 10);
		itoa(score, scoreArray, 10);
		itoa(goal, goalArray, 10);
		ILI9341_Puts(105, 50, levelArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(105, 90, nonePlayer, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(105, 130, scoreArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		ILI9341_Puts(105, 170, remainArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	}
}

void displayPlayer(void){
	ILI9341_Puts(105, 90, playerArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void displayLevel(int level){
	itoa(level, levelArray, 10);
	ILI9341_Puts(105, 50, levelArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void displayGoal(int goal){
	itoa(goal, goalArray, 10);
	ILI9341_Puts(105, 210, goalArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void basicData(void){
	level=0;
	remain=0;
	goal=0;
}

void updateData(void){
	level=2;
	remain=27;
}

void displayInterface(){
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	setGameStatus(isLaunch);
	displayLeftPart();
	displayRightPart();
}

void setGameStatus(bool_e b){
	// clear "d"
	ILI9341_Puts(234, 10, " ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	if(b)
		ILI9341_Puts(105, 10, GAMELAUNCHED, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
	else
		ILI9341_Puts(105, 10, GAMEINWAIT, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

void updatePhotoValCont(void){
	itoa(ADC_getValue(ADC_SENSOR_CHANNEL), photoValArray, 10);
	ILI9341_Puts(105, 50, photoValArray, &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
}

/*=== GAME ===*/

bool_e gameTimed(void){
	// int second;
	uint16_t currentS=0;
	updateData();
	displayData();
	score=0;
	writeYellowLed(1);
	displayPlayer();
	displayLevel(2);
	displayGoal(5);
	TIMER_run_us(TIMER1_ID, 10000000, 0);
	while(score<GOAL_FIRST_LEVEL || currentS<(uint16_t)5000000){
		if(ADC_getValue(ADC_SENSOR_CHANNEL)>3000){
			score++;
			writeGreenLed(1);
			HAL_Delay(500);
		}
		writeGreenLed(0);
		ILI9341_Puts(105, 130, itoa(score, scoreArray, 10), &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		currentS = TIMER_read(TIMER1_ID);
		remain=(5000000/1000000)-((10*currentS)/38600);
		ILI9341_Puts(105, 170, itoa(remain, remainArray, 10), &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		if(score>=GOAL_FIRST_LEVEL || currentS>(uint16_t)5000000){
			// win/lost scren ?
			setGameStatus(0);
			if(score>GOAL_FIRST_LEVEL)
				win();
				return 1;
			else
				lost();
				return 0;
			// clean score & remain data on screen
			ILI9341_Puts(105, 130, "  ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			ILI9341_Puts(105, 170, "  ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			ILI9341_Puts(105, 210, "  ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			ILI9341_Puts(105, 50, "  ", &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
			setToZero();
			allLedOff();
			break;
		}
	}
}

void win(void){
	blinkGreenLedWithBuzzer(4, 200);
}

void lost(void){
	blinkRedLedWithBuzzer(4, 200);
}

void setToZero(void){
	level=0;
	remain=0;
	score=0;
	goal=0;
}

void deleteMatchData(void){
	score=0;
	remain=0;
}

bool_e testTimer(void){
	TIMER_run_us(TIMER1_ID, 10000000, 0);
	char timerArray[20];
	while(1){
		ILI9341_Puts(105, 130, itoa(TIMER_read(TIMER1_ID), timerArray, 10), &Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_WHITE);
		writeGreenLed(1);
		if(TIMER_read(TIMER1_ID)>(uint16_t)5000000){
			stop_timer(TIMER1_ID);
			writeGreenLed(0);
			break;
		}
	}
	writeRedLed(1);
	writeGreenLed(0);
	return 1;
}

/*=== AUTONOMOUS ===*/

int getLengthFromFrontUS(void){
	// determine length with time between em and rec
	HAL_GPIO_ReadPin(FRONT_ULTRASONIC_GPIO, FRONT_ULTRASONIC_PIN);
}

int getLengthFromLeftUs(void){
	leftUltrasonic = HAL_GPIO_ReadPin(LEFT_ULTRASONIC_GPIO, LEFT_ULTRASONIC_PIN);
}

int getLengthFromRightUs(void){
	rightUltrasonic = HAL_GPIO_ReadPin(RIGHT_ULTRASONIC_GPIO, RIGHT_ULTRASONIC_PIN);
}

void autonomousMode(void){
	while(readButton()){
		while(getLengthFromFrontUS()<50){
			// move();
		}
	}
}

/*===========================
 *         MAIN
 ==========================*/

int main(void) {
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer) (1ere ligne !!)
	HAL_Init();

	//Initialisation de l'UART2 � la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
	//Attention, les pins PA2 et PA3 ne sont pas reli�es jusqu'au connecteur de la Nucleo.
	//Ces broches sont redirig�es vers la sonde de d�bogage, la liaison UART �tant ensuite encapsul�e sur l'USB vers le PC de d�veloppement.
	UART_init(UART2_ID,115200);

	//"Indique que les printf sortent vers le p�riph�rique UART2."
	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	// Digital Pin Initialisation
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(LED_YELLOW_GPIO, LED_YELLOW_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(LED_RED_GPIO, LED_RED_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(BUZZER_GPIO, BUZZER_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	writeBuzzer(0);

	displayInterface();

	ADC_init();

	while(1)
	{
		// machine à états finis pour le choix du mode de jeu (écran tactile / web interface)
		if(ADC_getValue(ADC_SENSOR_CHANNEL)>3000){
			setGameStatus(1);
			gameTimed();
		}
		// setGameStatus(0);
		displayData();
	}
}