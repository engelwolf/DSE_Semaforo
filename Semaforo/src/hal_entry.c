/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : This is a very simple example application that blinks all LEDs on a board.
 ***********************************************************************************************************************/

#include "hal_data.h"

const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS; /* Define the units to be used with the software Delay function */
uint32_t Delay = 0; /* Calculate the Delay in terms of bsp_delay_units */

#define FLASHING_DELAY_TIME 	(500)
#define WARNING_DELAY_TIME 		(2000)
#define SENSOR_STOP_DELAY_TIME 	(3000)
#define BUTTON_STOP_DELAY_TIME 	(5000)
#define NO_DELAY_TIME 			(0)
#define STOP_INITIAL_DELAY_TIME (100)
#define MAX_FLASHING_CICLES		(5)

void defualt_state();
void flashing_states();
void ambar_warning_state();
void stop_state();

typedef enum estado_semaforo_tag
{
	DEFAULT = 0,
	FLASHING,
	WARNING_TRANSITION,
	STOP_TRANSITION,
	RESET_TRANSITION,
	MAX_TRANSITIONS,
}estado_semaforo_t;

estado_semaforo_t Current_State = DEFAULT;

bool Car_Sensor = false;

/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Integrantes: Ángel Guerrero Sierra / Jorge Luis Oficial López
 *
 **********************************************************************************************************************/


void hal_entry(void)
{
	g_external_irq10.p_api->open(g_external_irq10.p_ctrl,
			g_external_irq10.p_cfg);
	g_external_irq11.p_api->open(g_external_irq11.p_ctrl,
			g_external_irq11.p_cfg);

	while (1) {
		switch(Current_State)
		{
			case DEFAULT:
			{
				defualt_state();
				break;
			}
			case FLASHING:
			{
				flashing_states();
				break;
			}
			case WARNING_TRANSITION:
			{
				ambar_warning_state();
				break;
			}
			case STOP_TRANSITION:
			{
				stop_state();
				break;
			}
			case RESET_TRANSITION:
			{
				ambar_warning_state();
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

/*Sensor del automovil*/
void button_callback_SW4(external_irq_callback_args_t *args) {
	(void) args;
	if (DEFAULT == Current_State)
	{
		Car_Sensor = true;
		Current_State = FLASHING;
	}
}

/* Boton del peaton */
void button_callback_SW5(external_irq_callback_args_t *args) {
	(void) args;
	if (DEFAULT == Current_State)
	{
		Car_Sensor = false;
		Current_State = FLASHING;
	}
}

void defualt_state() {
	Delay = NO_DELAY_TIME;
	g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_LOW);
	R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
	g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_HIGH);
	R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
	g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH);
	R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
}

void flashing_states() {
	for (uint8_t cicle = 0; cicle <MAX_FLASHING_CICLES; cicle++)
	{
		Delay = FLASHING_DELAY_TIME;
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_LOW);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
		Delay = FLASHING_DELAY_TIME;
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
	}
	Current_State = WARNING_TRANSITION;
}

void ambar_warning_state()
{
	if (WARNING_TRANSITION == Current_State) {
		Delay = NO_DELAY_TIME;
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_00, IOPORT_LEVEL_HIGH);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */

		Delay = WARNING_DELAY_TIME;
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_LOW);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */

		Current_State = STOP_TRANSITION;
	} else if (RESET_TRANSITION == Current_State) {
		Delay = WARNING_DELAY_TIME;
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_LOW);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */
		g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_LOW);
		R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */

		Current_State = DEFAULT;
	} else {
		/* Nothing to process */
	}
}

void stop_state()
{
	Delay = STOP_INITIAL_DELAY_TIME;
	g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_02, IOPORT_LEVEL_HIGH);
	R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */

	Delay = BUTTON_STOP_DELAY_TIME;

	if (Car_Sensor)
	{
		Delay = SENSOR_STOP_DELAY_TIME;
	}

	g_ioport.p_api->pinWrite(IOPORT_PORT_06_PIN_01, IOPORT_LEVEL_LOW);
	R_BSP_SoftwareDelay(Delay, bsp_delay_units);/* Delay */

	Current_State = RESET_TRANSITION;
}
