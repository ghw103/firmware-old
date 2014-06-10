//vertical_estimator.c

#include "vertical_estimator.h"

#include "arm_math.h"
#include <stdio.h>
#include "bound.h"


//#define VZD_DEBUGGING

#ifdef VZD_DEBUGGING
#include "usart.h"
	uint8_t _buff_push[100];
#endif

void vertical_estimator_init(vertical_data* raw_data,vertical_data* filtered_data){

	raw_data->Z =0.0;
	raw_data->Zd =0.0;
	raw_data->Zdd =0.0;

	filtered_data->Z=0.0;
	filtered_data->Zd=0.0;
	filtered_data->Zdd=0.0;
}

	float V_Zd_INS=0.0;
	float V_Zd_INS_error=0.0;
	float V_Zd_INS_longterm_error=0.0;
	float V_Zd_INS_longTermOffset=0.0;
	float V_Zdd_INS_error=0.0;
	//float V_Zd_INS_beforeCorrection;

	float V_Zd_Baro_Prev=0.0;
	float V_Zd_INS_error_Prev=0.0;

	float g_offset =0.0;


void vertical_sense(vertical_data* vertical_filtered_data,vertical_data* vertical_raw_data,attitude_t* attitude,imu_data_t* imu_raw_data){

	float estAlt_prev= vertical_filtered_data->Z;
	float C_nroll=0.0,S_nroll=0.0,C_npitch=0.0,S_npitch=0.0;
	float Axx=0.0,Azx=0.0; //,Ayx=0.0
	float Az_rotated;//Ax_rotated=0.0,Ay_rotated,

	float V_Zd_INS_current_error;
	float V_Zdd_INS_current_error;
	float g_earth = 1.0f;
	float del_g_adder;
	float f=4000.0f;
	float dt=1.0f/f;


	float Zd_INS_Alpha=0.0001f,Zdd_INS_error_Alpha=0.005f;
	/* Update barometer data */

		if(!ADS1246_DRDY_PIN_STATE()){
		vertical_raw_data->Z = barometer_read_altitude() ;
		vertical_filtered_data->Z = lowpass_float(&vertical_filtered_data->Z, &vertical_raw_data->Z, 0.005f);

		vertical_raw_data->Zd = (vertical_filtered_data->Z - estAlt_prev)*1000.0f; // Have to define it somewhere;

		}


		C_nroll = arm_cos_f32(attitude->roll * (-0.01745329252392f));
		S_nroll = arm_sin_f32(attitude->roll * (-0.01745329252392f));

		C_npitch = arm_cos_f32(attitude->pitch * (-0.01745329252392f));
		S_npitch = arm_sin_f32(attitude->pitch * (-0.01745329252392f));


		Axx = imu_raw_data->acc[0];
		//Ayx = imu_raw_data->acc[1]*C_nroll+imu_raw_data->acc[2]*S_nroll;
		Azx = -imu_raw_data->acc[1]*S_nroll+imu_raw_data->acc[2]*C_nroll;

		//Ax_rotated=Axx*C_npitch-Azx*S_npitch;
		//Ay_rotated=Ayx;
		Az_rotated=Axx*S_npitch+Azx*C_npitch;

		vertical_filtered_data->Zdd = Az_rotated;

		/* Rotation matrix */


		/* Starting main algorithm */

		V_Zd_INS+=(Az_rotated-g_earth-g_offset )*9.81f*100.0f*dt;
		V_Zd_INS_error = V_Zd_INS-vertical_raw_data->Zd ;


		/* estimate longterm error haven't been corrected */
		V_Zd_INS_current_error = vertical_filtered_data->Zd - vertical_raw_data->Zd;
		V_Zd_INS_longterm_error = lowpass_float(&V_Zd_INS_longterm_error,&V_Zd_INS_current_error,0.00000001f); // may not be long term enough for new one

		/* apply longterm error correction */
		V_Zd_INS_longTermOffset+=V_Zd_INS_longterm_error*1.0f*1000.5f;
		/* substract applied longterm error from bank */
		V_Zd_INS_longterm_error-=V_Zd_INS_longterm_error*0.210f;
		vertical_filtered_data->Zd =V_Zd_INS-V_Zd_INS_longTermOffset;

		/* estimate the stray acceleration with lowpass filter */
		V_Zdd_INS_current_error = (V_Zd_INS_error-V_Zd_INS_error_Prev)*f*0.01f;
		V_Zdd_INS_error = lowpass_float(&V_Zdd_INS_error,&V_Zdd_INS_current_error,Zdd_INS_error_Alpha);
		
		/* calibrating earth's gravity offset */
		del_g_adder=0.00005f*V_Zdd_INS_error*0.1f;

		g_offset+=del_g_adder;


		/* complementary filter for integrity */
		//V_Zd_INS_beforeCorrection=V_Zd_INS;
		vertical_filtered_data->Zd = vertical_raw_data->Zd * Zd_INS_Alpha+(1.0f-Zd_INS_Alpha)*vertical_filtered_data->Zd;

		/* memory storage for next iteration */
		//V_Zd_INS_correctionOffset=V_Zd_INS-V_Zd_INS_beforeCorrection;
		V_Zd_Baro_Prev=vertical_raw_data->Zd;
		V_Zd_INS_error_Prev=V_Zd_INS_error;

#ifdef VZD_DEBUGGING

		if (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) != RESET) {

			_buff_push[7] = 0;_buff_push[8] = 0;_buff_push[9] = 0;_buff_push[10] = 0;_buff_push[11] = 0;_buff_push[12] = 0;	_buff_push[13] = 0;

			sprintf((char *)_buff_push, "%d,%ld,%ld,%d,100000000000,\r\n",
				(int16_t)(vertical_filtered_data->Zd * 1.0f),
				(int32_t)(vertical_filtered_data->Z* 1.0f),
				(int32_t)(g_offset * 100000.0f),
				(int16_t)(V_Zd_INS * 1.0f));

			usart2_dma_send(_buff_push);


		}	
#endif
}

