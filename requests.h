/* Name: requests.h
 * Project: SMP reader device for Electronika MK-90
 * Creation Date: 2009-07-31
 * Copyright: (c) 2009 by Ilya Danilov
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

/* This header is shared between the firmware and the host software. It
 * defines the USB request numbers (and optionally data types) used to
 * communicate between the host and the device.
 */

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

#define CUSTOM_RQ_READ_DATA		1
/* Request cartridge read  */

#define CUSTOM_RQ_WRITE_DATA	2
/* Request cartridge write  */

#define CUSTOM_RQ_INIT_MEM		3
/* Request cartridge init */

#endif /* __REQUESTS_H_INCLUDED__ */
