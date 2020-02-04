#include <MKL05Z4.h>


/* 
		We are not fans of splitting project into different files
		In our opinion 200 lines is acceptable in single file 
		It may not seem professional in terms of code reusability but it is comfortable	 
*/

#define SCAN_OFFSET 544  // Offset for scan range 
#define SCAN_DATA TSI0->DATA & 0xFFFF /* Accessing TSI data */

#define BLUE 10  /* RGB LED pins */
#define GREEN 9
#define RED 8

#define RED_PWM   3   /* TPM channels for RGB LED*/
#define BLUE_PWM  1
#define GREEN_PWM 2

#define ex_RED 0      /* Indicator diodes pins */
#define ex_GREEN 11
#define ex_BLUE 13


#define SW_1 2

#define MOD_ 11000    /* MODULUS value for TPM */


void delayMs( int n) { 
int i; int j;
	for( i = 0 ; i < n; i++)
	for(j = 0; j < 7000; j++) {}  
}

void Touch_Init()
{

	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;   	/* Enable clock for TSI */

	TSI0->GENCS = TSI_GENCS_OUTRGF_MASK |/* Out of range flag, set to 1 to clear 							 */
								//TSI_GENCS_ESOR_MASK   |/* interrupt when out of range.  								  */
								TSI_GENCS_MODE(0u)    |/* Set at 0 for capacitive sensing.								 */
								TSI_GENCS_REFCHRG(0u) |/* Reference charge																*/
								TSI_GENCS_DVOLT(3u)   |/* Voltage range																	 */
								TSI_GENCS_EXTCHRG(0u) |/*External charge																*/
								TSI_GENCS_PS(4u)      |/* Electrode prescaler													 */
								TSI_GENCS_NSCN(23u)   |/*scans per electrode													*/
								TSI_GENCS_TSIEN_MASK  |/* TSI enable bit														 */
								//TSI_GENCS_TSIIEN_MASK |/*TSI interrupt														*/
								//TSI_GENCS_STPE_MASK   |/* TSI low power mode										 */
								//TSI_GENCS_STM_MASK 	  |/* 0 software trigger/ 1 hardware			  */
								//TSI_GENCS_SCNIP_MASK  |/* scan in progress flag								 */
								//TSI_GENCS_CURSW_MASK  |/* Do not swap current sources					*/
								TSI_GENCS_EOSF_MASK;		 /* End of scan flag, set to 1 to clear*/
								
														
}

void SW_ini(void){
		PORTB->PCR[SW_1]|=PORT_PCR_MUX(1)| PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
		PTB->PDDR &=~(1<<SW_1);
}

// Function to read touch sensor
int Touch_Scan(void)
{
	delayMs(1);
	int scan;
	TSI0->DATA = 	TSI_DATA_TSICH(9u); /* Channel 9 of the TSI	*/
	TSI0->DATA |= TSI_DATA_SWTS_MASK; /* Software trigger 		*/
	
	scan = SCAN_DATA; /*SCAN_DATA TSI0->DATA & 0xFFFF  Accessing the bits held in TSI0_DATA_TSICNT*/
	
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; /* Reset end of scan flag */
	
	return scan - SCAN_OFFSET;
}



void LED_ini(){
	
	SIM->SCGC5|=SIM_SCGC5_PORTB_MASK; /* Enable clock for GPIO B */
	
	PORTB->PCR[BLUE]|=	PORT_PCR_MUX(2);  /* Set pin BLUE of PORT B as TPM */
	TPM0->CONTROLS[BLUE_PWM].CnV = MOD_;    //Initial state 0% brightness
	

	PORTB->PCR[GREEN]	|=	PORT_PCR_MUX(2);  /* Set pin GREEN of PORT B as TPM */
	TPM0->CONTROLS[GREEN_PWM].CnV = MOD_;	//Initial state 0% brightness
		
	
	PORTB->PCR[RED]	|=	PORT_PCR_MUX(2);  /* Set pin RED of PORT B as TPM */
	TPM0->CONTROLS[RED_PWM].CnV = MOD_; 	/*Initial state 0% brightness */
	
	/* EXTERNAL LED FOR INDICATION 
	ex_RED 10
	ex_GREEN 11
	ex_BLUE 13 
	*/
	
			PORTB->PCR[ex_RED]|=PORT_PCR_MUX(1);  /* Set pin ex_RED as GPIO   */
			PTB->PDDR |=(1<<ex_RED);  						/* Set direction as output  */ 
			PTB->PCOR |=(1<<ex_RED);							/* Set initial state to LOW */
	
			PORTB->PCR[ex_BLUE]|=PORT_PCR_MUX(1);
			PTB->PDDR |=(1<<ex_BLUE);
			PTB->PCOR |=(1<<ex_BLUE);
	
			PORTB->PCR[ex_GREEN]|=PORT_PCR_MUX(1);
			PTB->PDDR |=(1<<ex_GREEN);
			PTB->PCOR |=(1<<ex_GREEN);
	
}
void BLUE_ONLY(void){ 
	PTB->PSOR |=(1<<ex_BLUE);
	PTB->PCOR |=(1<<ex_GREEN);
	PTB->PCOR |=(1<<ex_RED);
};

void RED_ONLY(void){
	PTB->PCOR |=(1<<ex_BLUE);
	PTB->PCOR |=(1<<ex_GREEN);
	PTB->PSOR |=(1<<ex_RED);
	
};
void GREEN_ONLY(void){
	PTB->PCOR |=(1<<ex_BLUE);
	PTB->PSOR |=(1<<ex_GREEN);
	PTB->PCOR |=(1<<ex_RED);
};

void ALL_OFF(void){
	PTB->PCOR |=(1<<ex_BLUE);
	PTB->PCOR |=(1<<ex_GREEN);
	PTB->PCOR |=(1<<ex_RED);

}

void Welcome_seq(void){  /* Simple welcome sequence to make device more friendly */
	int i;
	for(i =0; i < 2; i++){
		delayMs(150);
		
		PTB->PSOR |=(1<<ex_GREEN);
		PTB->PSOR |=(1<<ex_BLUE);
		PTB->PSOR |=(1<<ex_RED);
		
		delayMs(100);
		ALL_OFF();
}
	
}

void Init_TPM(){

	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;   /* TPM clock enable */
	

	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);   /* Clk source of TPM */

	
	TPM0->MOD = MOD_;  									 /* Load max value for TPM */
	
	
	
	
	/*Table for setting in reference manual Ctrl+F -> "TPMx_CnSC" */
TPM0->CONTROLS[1].CnSC |=(1UL<<5)|(1UL<<3);
TPM0->CONTROLS[2].CnSC |=(1UL<<5)|(1UL<<3);
TPM0->CONTROLS[3].CnSC |=(1UL<<5)|(1UL<<3);
	
TPM0->SC	 						 |=(1UL<<3);  /* TPM counter increments on every TPM counter clock */



	
	//TPM0->CONF |= TPM_CONF_TRGSEL(0xA);  /* Trigger select for TPM */
	

};

int main(){
	Touch_Init();
	Init_TPM();
	LED_ini();
	SW_ini();
	uint16_t x;
	int ctr=0;
  
	Welcome_seq();
		
	while(1){

		
		if((PTB->PDIR &(1UL<<SW_1))==0){
			delayMs(15); /* Simple debouncing */
			ctr++;
			while((PTB->PDIR & (1<<SW_1))==0); /* Wait for SW_1 release */
			}
			
			switch(ctr){
				
				case 1: //RED
					x=Touch_Scan();
					TPM0->CONTROLS[RED_PWM].CnV = x ;	
					RED_ONLY();
				break;
				
				case 3: //BLUE
					x=Touch_Scan();
					TPM0->CONTROLS[BLUE_PWM].CnV = x ;	
					BLUE_ONLY();				
				break;
				
				case 2: //GREEN
					x=Touch_Scan();
					TPM0->CONTROLS[GREEN_PWM].CnV = x ;		
					GREEN_ONLY();
				break;
				
				case 4: // DO NOTHING AND SHINE
					ALL_OFF();
					break;
				
				default:
					ctr=0;
					break;
			}
			
		}
	}