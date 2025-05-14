/*
 * audio.c
 *
 * Contains all of the functions related to audio codec setup.
 */
#include "adventures_with_ip.h"
#include "audio.h"
#include "xtime_l.h" // Include Xilinx time library

#include "cue_sound.h"
#include <stdlib.h>

#include "ball_collisions.h"
#include "ball_sunk.h"
#include "communications.h"

typedef struct{
    u32 *data;
    u32 length;
}Recording;

Recording recordings[NUM_RECORDINGS] = {0};
int current_recording = 0;
u32 left_channel_buffer[MEMORY_SIZE];
u32 right_channel_buffer[MEMORY_SIZE];
int recording = 0;
XTime recording_start_time;
XTime recording_end_time;

float global_volume = 50;

#define COMM_VAL (*(volatile unsigned long *)(0xFFFF0000))


/* ---------------------------------------------------------------------------- *
 * 									IicConfig()									*
 * ---------------------------------------------------------------------------- *
 * Initialises the IIC driver by looking up the configuration in the config
 * table and then initialising it. Also sets the IIC serial clock rate.
 * ---------------------------------------------------------------------------- */
unsigned char IicConfig(unsigned int DeviceIdPS)
{
	XIicPs_Config *Config;
	int Status;

	/* Initialise the IIC driver so that it's ready to use */

	// Look up the configuration in the config table
	Config = XIicPs_LookupConfig(DeviceIdPS);
	if(NULL == Config) {
		return XST_FAILURE;
	}

	// Initialise the IIC driver configuration
	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Set the IIC serial clock rate.
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	return XST_SUCCESS;
}

/* ---------------------------------------------------------------------------- *
 * 								AudioPllConfig()								*
 * ---------------------------------------------------------------------------- *
 * Configures audio codes's internal PLL. With MCLK = 10 MHz it configures the
 * PLL for a VCO frequency = 49.152 MHz, and an audio sample rate of 48 KHz.
 * ---------------------------------------------------------------------------- */
void AudioPllConfig() {

	unsigned char u8TxData[8], u8RxData[6];
	int Status;

	Status = IicConfig(XPAR_XIICPS_0_DEVICE_ID);
	if(Status != XST_SUCCESS) {
		xil_printf("\nError initializing IIC");

	}

	// Disable Core Clock
	AudioWriteToReg(R0_CLOCK_CONTROL, 0x0E);

	/* 	MCLK = 10 MHz
		R = 0100 = 4, N = 0x02 0x3C = 572, M = 0x02 0x71 = 625

		PLL required output = 1024x48 KHz
		(PLLout)			= 49.152 MHz

		PLLout/MCLK			= 49.152 MHz/10 MHz
							= 4.9152 MHz
							= R + (N/M)
							= 4 + (572/625) */

	// Write 6 bytes to R1 @ register address 0x4002
	u8TxData[0] = 0x40; // Register write address [15:8]
	u8TxData[1] = 0x02; // Register write address [7:0]
	u8TxData[2] = 0x02; // byte 6 - M[15:8]
	u8TxData[3] = 0x71; // byte 5 - M[7:0]
	u8TxData[4] = 0x02; // byte 4 - N[15:8]
	u8TxData[5] = 0x3C; // byte 3 - N[7:0]
	u8TxData[6] = 0x21; // byte 2 - 7 = reserved, bits 6:3 = R[3:0], 2:1 = X[1:0], 0 = PLL operation mode
	u8TxData[7] = 0x01; // byte 1 - 7:2 = reserved, 1 = PLL Lock, 0 = Core clock enable

	// Write bytes to PLL Control register R1 @ 0x4002
	XIicPs_MasterSendPolled(&Iic, u8TxData, 8, (IIC_SLAVE_ADDR >> 1));
	while(XIicPs_BusIsBusy(&Iic));

	// Register address set: 0x4002
	u8TxData[0] = 0x40;
	u8TxData[1] = 0x02;

	// Poll PLL Lock bit
	do {
		XIicPs_MasterSendPolled(&Iic, u8TxData, 2, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
		XIicPs_MasterRecvPolled(&Iic, u8RxData, 6, (IIC_SLAVE_ADDR >> 1));
		while(XIicPs_BusIsBusy(&Iic));
	}
	while((u8RxData[5] & 0x02) == 0); // while not locked

	AudioWriteToReg(R0_CLOCK_CONTROL, 0x0F);	// 1111
												// bit 3:		CLKSRC = PLL Clock input
												// bits 2:1:	INFREQ = 1024 x fs
												// bit 0:		COREN = Core Clock enabled
}

/* ---------------------------------------------------------------------------- *
 *                              AudioWriteToReg()                               *
 * ---------------------------------------------------------------------------- */
void AudioWriteToReg(unsigned char u8RegAddr, unsigned char u8Data) {
    unsigned char u8TxData[3];

    u8TxData[0] = 0x40;
    u8TxData[1] = u8RegAddr;
    u8TxData[2] = u8Data;

    XIicPs_MasterSendPolled(&Iic, u8TxData, 3, (IIC_SLAVE_ADDR >> 1));
    while (XIicPs_BusIsBusy(&Iic));
}

/* ---------------------------------------------------------------------------- *
 *                          AudioConfigureJacks()                               *
 * ---------------------------------------------------------------------------- */
void AudioConfigureJacks()
{
	AudioWriteToReg(R4_RECORD_MIXER_LEFT_CONTROL_0, 0x01); //enable mixer 1
	AudioWriteToReg(R5_RECORD_MIXER_LEFT_CONTROL_1, 0x07); //unmute Left channel of line in into mxr 1 and set gain to 6 db
	AudioWriteToReg(R6_RECORD_MIXER_RIGHT_CONTROL_0, 0x01); //enable mixer 2
	AudioWriteToReg(R7_RECORD_MIXER_RIGHT_CONTROL_1, 0x07); //unmute Right channel of line in into mxr 2 and set gain to 6 db
	AudioWriteToReg(R19_ADC_CONTROL, 0x13); //enable ADCs

	AudioWriteToReg(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, 0x21); //unmute Left DAC into Mxr 3; enable mxr 3
	AudioWriteToReg(R24_PLAYBACK_MIXER_RIGHT_CONTROL_0, 0x41); //unmute Right DAC into Mxr4; enable mxr 4
	AudioWriteToReg(R26_PLAYBACK_LR_MIXER_LEFT_LINE_OUTPUT_CONTROL, 0x05); //unmute Mxr3 into Mxr5 and set gain to 6db; enable mxr 5
	AudioWriteToReg(R27_PLAYBACK_LR_MIXER_RIGHT_LINE_OUTPUT_CONTROL, 0x11); //unmute Mxr4 into Mxr6 and set gain to 6db; enable mxr 6
	AudioWriteToReg(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, 0xFF);//Mute Left channel of HP port (LHP)
	AudioWriteToReg(R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL, 0xFF); //Mute Right channel of HP port (LHP)
	//AudioWriteToReg(R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL, 0xE6); //set LOUT volume (0db); unmute left channel of Line out port; set Line out port to line out mode
	//AudioWriteToReg(R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL, 0xE6); // set ROUT volume (0db); unmute right channel of Line out port; set Line out port to line out mode
	AudioWriteToReg(R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL, 0xFE); //set LOUT volume (0db); unmute left channel of Line out port; set Line out port to line out mode
	AudioWriteToReg(R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL, 0xFE); // set ROUT volume (0db); unmute right channel of Line out port; set Line out port to line out mode
	AudioWriteToReg(R35_PLAYBACK_POWER_MANAGEMENT, 0x03); //enable left and right channel playback (not sure exactly what this does...)
	AudioWriteToReg(R36_DAC_CONTROL_0, 0x03); //enable both DACs

	AudioWriteToReg(R58_SERIAL_INPUT_ROUTE_CONTROL, 0x01); //Connect I2S serial port output (SDATA_O) to DACs
	AudioWriteToReg(R59_SERIAL_OUTPUT_ROUTE_CONTROL, 0x01); //connect I2S serial port input (SDATA_I) to ADCs

	AudioWriteToReg(R65_CLOCK_ENABLE_0, 0x7F); //Enable clocks
	AudioWriteToReg(R66_CLOCK_ENABLE_1, 0x03); //Enable rest of clocks
}

/* ---------------------------------------------------------------------------- *
 * 								LineinLineoutConfig()							*
 * ---------------------------------------------------------------------------- *
 * Configures Line-In input, ADC's, DAC's, Line-Out and HP-Out.
 * ---------------------------------------------------------------------------- */
void LineinLineoutConfig() {

	AudioWriteToReg(R17_CONVERTER_CONTROL_0, 0x05);//48 KHz
	AudioWriteToReg(R64_SERIAL_PORT_SAMPLING_RATE, 0x05);//48 KHz
	AudioWriteToReg(R19_ADC_CONTROL, 0x13);
	AudioWriteToReg(R36_DAC_CONTROL_0, 0x03);
	AudioWriteToReg(R35_PLAYBACK_POWER_MANAGEMENT, 0x03);
	AudioWriteToReg(R58_SERIAL_INPUT_ROUTE_CONTROL, 0x01);
	AudioWriteToReg(R59_SERIAL_OUTPUT_ROUTE_CONTROL, 0x01);
	AudioWriteToReg(R65_CLOCK_ENABLE_0, 0x7F);
	AudioWriteToReg(R66_CLOCK_ENABLE_1, 0x03);

	AudioWriteToReg(R4_RECORD_MIXER_LEFT_CONTROL_0, 0x01);
	AudioWriteToReg(R5_RECORD_MIXER_LEFT_CONTROL_1, 0x05);//0 dB gain
	AudioWriteToReg(R6_RECORD_MIXER_RIGHT_CONTROL_0, 0x01);
	AudioWriteToReg(R7_RECORD_MIXER_RIGHT_CONTROL_1, 0x05);//0 dB gain

	AudioWriteToReg(R22_PLAYBACK_MIXER_LEFT_CONTROL_0, 0x21);
	AudioWriteToReg(R24_PLAYBACK_MIXER_RIGHT_CONTROL_0, 0x41);
	AudioWriteToReg(R26_PLAYBACK_LR_MIXER_LEFT_LINE_OUTPUT_CONTROL, 0x03);//0 dB
	AudioWriteToReg(R27_PLAYBACK_LR_MIXER_RIGHT_LINE_OUTPUT_CONTROL, 0x09);//0 dB
	AudioWriteToReg(R29_PLAYBACK_HEADPHONE_LEFT_VOLUME_CONTROL, 0xE7);//0 dB
	AudioWriteToReg(R30_PLAYBACK_HEADPHONE_RIGHT_VOLUME_CONTROL, 0xE7);//0 dB
	AudioWriteToReg(R31_PLAYBACK_LINE_OUTPUT_LEFT_VOLUME_CONTROL, 0xE6);//0 dB
	AudioWriteToReg(R32_PLAYBACK_LINE_OUTPUT_RIGHT_VOLUME_CONTROL, 0xE6);//0 dB
}

/* ---------------------------------------------------------------------------- *
 * 								StartRecording()							*
 * ---------------------------------------------------------------------------- *
 * Starts the recording and sets a timer.
 * ---------------------------------------------------------------------------- */
void StartRecording() {
	xil_printf("The recording has started.\r\n");
	recording = 1;
	int sample_counter = 0;
	u32 in_left, in_right;
	XTime start_time, current_time, end_time; // Declare timing variables

	XTime_GetTime(&recording_start_time);  // Get initial timestamp
	start_time = recording_start_time;

	do {
		XTime_GetTime(&current_time);  // Get the current timestamp

		// Wait until enough time has passed for 1 sample (48kHz rate)
		if ((current_time - start_time) >= SAMPLE_PERIOD) {
			start_time = current_time;  // Reset start_time for next cycle

			// Read the Left & Right channel audio from MIC jack
			in_left = Xil_In32(I2S_DATA_RX_L_REG);
			in_right = Xil_In32(I2S_DATA_RX_R_REG);

			// Store the samples in memory
			left_channel_buffer[sample_counter] = in_left;
			right_channel_buffer[sample_counter] = in_right;
			sample_counter++;
		}

	} while (recording && sample_counter < MEMORY_SIZE);  // Loop until stopped or buffer full

	XTime_GetTime(&recording_end_time);
	recording = 0;
}

/* ---------------------------------------------------------------------------- *
 * 								StopRecording()							*
 * ---------------------------------------------------------------------------- *
 * Stops the recording in progress manually triggered by button interrupt handler.
 * ---------------------------------------------------------------------------- */
void StopRecording() {
	recording = 0;
	xil_printf("Recording has been manually stopped by button.\r\n");
}

/* ---------------------------------------------------------------------------- *
 * 								PlayRecording()							*
 * ---------------------------------------------------------------------------- *
 * Plays the audio that was recorded to the Zedboard's memory.
 * ---------------------------------------------------------------------------- */
void PlayRecording() {
//	xil_printf("Playing Back Recorded Audio...\n");

	XTime playback_start_time, current_time;
	XTime_GetTime(&playback_start_time);

	u32 sample_counter = 0;
	XTime sample_time = playback_start_time;
	int mode = 0;

	while (sample_counter < MEMORY_SIZE) {
		XTime_GetTime(&current_time);

		if (mode == 0) {
			if ((current_time - sample_time) >= SAMPLE_PERIOD) {
				sample_time = current_time;  // Reset time for next cycle

				// Send audio data to output
				Xil_Out32(I2S_DATA_TX_L_REG, left_channel_buffer[sample_counter]);
				Xil_Out32(I2S_DATA_TX_R_REG, right_channel_buffer[sample_counter]);

				sample_counter++;
			}

			// Stop playback if we've reached the actual recording duration
			if ((current_time - playback_start_time) >= (recording_end_time - recording_start_time)) {
				xil_printf("Playback finished.\r\n");
				break;
			}
		}
	}
}

/* ---------------------------------------------------------------------------- *
 * 								store_audio()					         		*
 * ---------------------------------------------------------------------------- *
 *                     Initialize Audio Files                                   *
 * ---------------------------------------------------------------------------- */
void store_audio() {

	current_recording = 0;

    recordings[current_recording].data = (u32 *)cue_sound_WAV;
    recordings[current_recording].length = cue_sound_length/8;
    current_recording++;

    recordings[current_recording].data = (u32 *)ball_collision_WAV;
    recordings[current_recording].length = ball_collision_length/8;
    current_recording++;

    recordings[current_recording].data = (u32 *)ball_sunk_WAV;
    recordings[current_recording].length = ball_sunk_length/8;

    current_recording++;
}

/* ---------------------------------------------------------------------------- *
 * 								play_recording()					         	*
 * ---------------------------------------------------------------------------- *
 *              Function to playback a recorded sound effect                    *
 * ---------------------------------------------------------------------------- */
void play_recording(int position) {
    XTime start_time, end_time;
    double increment = (4000000 / SAMPLE_RATE);

    for (int i = 0; i < recordings[position].length; i++) {
        XTime_GetTime(&start_time);

        Xil_Out32(I2S_DATA_TX_L_REG, recordings[position].data[2 * i]/256);
        Xil_Out32(I2S_DATA_TX_R_REG, recordings[position].data[2 * i + 1]/256);

        do{
            XTime_GetTime(&end_time);
        }while(((end_time - start_time) * 1000000.0 / COUNTS_PER_SECOND) < increment);
    }
    usleep(10000);
}





