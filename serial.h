/*
 * lc1.h
 *
 *  Created on: 26 dec 2013
 *      Author: pontus
 */

#ifndef LC1_H_
#define LC1_H_

#define 	ISP2_PROTOCOL_SYNC1				(0xA2)
#define 	ISP2_PROTOCOL_SYNC2				(0x80)

#define		ISP2_DEVICE_RECORDING			(0x4000)
#define		ISP2_COMMAND_DATA				(0x1000)
#define		ISP2_CAPABLE_OF_LOG				(0x0800)

#define		ISP2_FUNC2_MASK					(0x1C00)
#define		ISP2_FUNC2_SHIFT_STEPS			(10)

#define		ISP2_AFR_MUL_MASK				(0x007F)
#define		ISP2_AFR_MUL_SHIFT_STEPS		(0)
#define		ISP2_AFR_MUL_BIT7_MASK			(0x0100)
#define		ISP2_AFR_MUL_BIT7_SHIFT_STEPS	(1)

#define		ISP2_LAMBDA_MASK				(0x007F)
#define		ISP2_LAMBDA_SHIFT_STEPS			(0)
#define		ISP2_LAMBDA_TOP_MASK			(0x3F00)
#define		ISP2_LAMBDA_TOP_SHIFT_STEPS		(1)

/**
 * The different states the LC-1 can be in.
 */
enum {
    ISP2_STATE_NO_STATE = 0x00,
    ISP2_STATE_LAMBDA,
    ISP2_STATE_PERCENT,
    ISP2_STATE_CALIBRATION_IN_PROGRESS,
    ISP2_STATE_NEED_CALIBRATION,
    ISP2_STATE_WARMING_UP,
    ISP2_STATE_NEED_HEATER_CALIBRATION,
    ISP2_STATE_ERROR,
    ISP2_STATE_PROTOCOL_ERROR
};

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
int serial_create (char *device);

#endif /* LC1_H_ */
