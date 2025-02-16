/*
 * system.c
 *
 *  Created on: Jan 8, 2024
 *      Author: brian
 */

#include "system.h"
#include "main.h"


//SYSTEM VARIABLES
extern TIM_HandleTypeDef htim5;
typedef enum
{
	// LED Arrays are 1 indexed. 0 is unsued.

	// Orange LEDs on back of gun that represent shots remaining.
	SHOTCOUNTER_LED_B1 = 1,
	SHOTCOUNTER_LED_B2,
	SHOTCOUNTER_LED_B3,
	SHOTCOUNTER_LED_B4,
	SHOTCOUNTER_LED_B5,
	SHOTCOUNTER_LED_B6,
	SHOTCOUNTER_LED_B7,
	SHOTCOUNTER_LED_B8,
	SHOTCOUNTER_LED_B9,
	SHOTCOUNTER_LED_B10,

	// Status RGB
	SHOTCOUNTER_LED_BLUE,
	SHOTCOUNTER_LED_RED,
	SHOTCOUNTER_LED_GREEN,

	SHOTCOUNTER_NUMBER_OF_LEDS,
} SHOT_COUNTER_LEDS;

long int LED_STATE[SHOTCOUNTER_NUMBER_OF_LEDS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};//LED0, LED1....LED10, BLUE,RED,GREEN

uint8_t randNums[10];
void _Shuffle(uint8_t *pArr) {
    for (int i = 0; i < 9; i++) {
        int j = i + rand() / (RAND_MAX / (10 - i) + 1);
        int t = pArr[j];
        pArr[j] = pArr[i];
        pArr[i] = t;
    }
}

uint8_t random_numbers_led_dimming[244] = {
225	,
207	,
103	,
19	,
205	,
33	,
197	,
110	,
118	,
201	,
146	,
178	,
102	,
190	,
46	,
48	,
208	,
116	,
221	,
74	,
61	,
57	,
214	,
133	,
107	,
76	,
117	,
227	,
249	,
77	,
27	,
142	,
12	,
112	,
167	,
62	,
66	,
179	,
168	,
42	,
175	,
22	,
204	,
55	,
127	,
246	,
202	,
99	,
35	,
84	,
100	,
36	,
169	,
128	,
124	,
219	,
230	,
140	,
50	,
173	,
218	,
138	,
115	,
141	,
164	,
54	,
113	,
223	,
101	,
174	,
153	,
143	,
148	,
111	,
75	,
105	,
156	,
199	,
119	,
234	,
32	,
28	,
126	,
242	,
231	,
215	,
220	,
211	,
236	,
70	,
144	,
67	,
209	,
88	,
233	,
217	,
254	,
213	,
232	,
52	,
122	,
134	,
163	,
44	,
161	,
125	,
82	,
247	,
253	,
80	,
136	,
195	,
47	,
188	,
243	,
13	,
172	,
45	,
30	,
20	,
59	,
224	,
147	,
198	,
86	,
98	,
176	,
109	,
93	,
18	,
14	,
177	,
78	,
154	,
157	,
26	,
53	,
182	,
87	,
149	,
16	,
252	,
106	,
255	,
129	,
145	,
23	,
63	,
240	,
71	,
56	,
40	,
97	,
222	,
245	,
104	,
181	,
192	,
85	,
96	,
72	,
185	,
200	,
64	,
49	,
31	,
24	,
228	,
139	,
212	,
83	,
187	,
73	,
15	,
196	,
235	,
34	,
210	,
43	,
171	,
38	,
166	,
194	,
51	,
130	,
184	,
17	,
151	,
248	,
131	,
251	,
170	,
90	,
79	,
39	,
160	,
81	,
180	,
186	,
238	,
69	,
206	,
241	,
114	,
29	,
58	,
152	,
226	,
244	,
41	,
92	,
162	,
216	,
189	,
229	,
135	,
237	,
65	,
25	,
150	,
123	,
158	,
203	,
21	,
165	,
159	,
239	,
183	,
132	,
68	,
60	,
191	,
94	,
37	,
137	,
155	,
108	,
121	,
120	,
95	,
250	,
91	,
193	,
89};

//FUNCTION DEFINITIONS
volatile uint32_t led_dimmer_count=0;
volatile uint32_t random_number_index = 0;
void LED_Dimmer_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(led_dimmer_count>=10) {
		led_dimmer_count=0;
		HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, GPIO_PIN_SET);

		uint16_t current_reload = __HAL_TIM_GET_AUTORELOAD(htim);
		if(current_reload > 100){
			current_reload = 12;
		} else {
			current_reload+=10;
		}
		__HAL_TIM_SET_AUTORELOAD(htim, random_numbers_led_dimming[random_number_index]);
		random_number_index++;
		if(random_number_index >= 243){
			random_number_index = 0;
		}
	} else {
		led_dimmer_count++;
		HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, GPIO_PIN_RESET);
	}

//	__HAL_TIM_SET_AUTORELOAD(htim, 8+randNums[led_dimmer_count]);
}

short unsigned getSwitch(void)
{
	int SWA, SWB, SWC, SWD, SWSTATE;
	SWA = !HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin);
	SWB = !HAL_GPIO_ReadPin(SWB_GPIO_Port, SWB_Pin);
	SWC = !HAL_GPIO_ReadPin(SWC_GPIO_Port, SWC_Pin);
	SWD = !HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin);

	SWSTATE = (SWD*8)+(SWC*4)+(SWB*2)+(SWA*1);
	return SWSTATE;
}

void initGPIOStates(void)
{
	HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
	HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET);
	HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET);
}

short unsigned getMode(void)
{
	int SW5, SW4, MODE_STATE;
	SW5 = HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
	SW4 = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);

	MODE_STATE = (SW5*2)+(SW4*1);
	return MODE_STATE;

}

void updateMode(long int mode) 
{ 
	if(mode == 0) 
	{//SW4 & SW5 NO FLAG --- NORMAL MODE 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		if (!startStealth || state[SFTD_STATE].next_state == CHARGE) {
			HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET);
			HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET);
		}
		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7000); 
	} 
	if(mode == 1) 
	{//SW4 FLAG SW5 NO FLAG -- STEALTH 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, RESET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET); 
		HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, SET);

		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7000); 
	} 
	if(mode == 2) 
	{//SW5 FLAG, SW4 NO FLAG WARN_REENG, RED LED 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET); 
	} 
	if(mode == 3) 
	{//IN ARM STATE NO SWITCHES PRESSED 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET); 
		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,1000); 
	} 
 
} 

void keepOnTest(void)
{
	if(HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin))
	{
		HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, SET);
	}
	else HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, RESET);
}

short unsigned turnAllOff(void)
{
	HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
	HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, RESET);
	HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET);
	//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7100);
	HAL_Delay(1);

	//TURN OFF ALL DISPLAY LEDS...
	/*HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, SET);*/
return 0;

}

void updateDisp(short int led_num, short int on_off)
{//use bitwise AND function to use mode, bit by bit to determine DISP LED states.
	/*B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,*/

#if CONFIG_SHOT_COUNTER_LED_RANDOM
	LED_STATE[randNums[led_num-1]] = on_off;
#else //default is defined(CONFIG_SHOT_COUNTER_LED_SEQUENTIAL)
	LED_STATE[(SHOTCOUNTER_LED_B10+1) - led_num] = on_off; 
#endif

	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
}

void turnOffDisp(void)
{
	//TURN OFF ALL DISPLAY LEDS...
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, SET);

}

void initDisp(void)
{    
	turnOffDisp();
    // Initialize the array with numbers from 1 to 10
    for (int i = 0; i < 10; i++) {
        randNums[i] = i + 1;
    }
    // Shuffle the array
    _Shuffle(&randNums[0]);

	for (int i = 0; i<=12; i ++) {
		LED_STATE[i] = 0;
	}
	LED_STATE[SHOTCOUNTER_LED_GREEN] = 0;
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
	HAL_Delay(CONFIG_SHOT_COUNTER_CHASE_SPEED_MS);
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, 1);//blu
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, 1);//red
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, 0);//grn
}

void recallDisp(void)
{
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, LED_STATE[SHOTCOUNTER_LED_BLUE]);//blu
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, LED_STATE[SHOTCOUNTER_LED_RED]);//red
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, LED_STATE[SHOTCOUNTER_LED_GREEN]);//grn

}

void keepOnState(short int keepon)
{

	HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, keepon);
}

short unsigned battTest(void)
{
	return 0;
}
/*
void testSW1(void)
{
	short int mode = getSwitch();

	if(mode == 0)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	}
	if(mode == 1)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	}
	if(mode == 2)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
	}
	if(mode == 3)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
	}
}*/
