/*
 * lc1.h
 *
 *  Created on: 26 dec 2013
 *      Author: pontus
 */

#ifndef SERIAL_H_
#define SERIAL_H_

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
int serial_create (char *device);

#endif /* SERIAL_H_ */
