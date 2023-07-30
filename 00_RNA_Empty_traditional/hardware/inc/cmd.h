/**
 * @file    cmd.h
 * @author  Shiki
 * @brief   A high level command line interface for MSP432 based on usart functions.
 * @version 0.1
 * @date    2023-07-27
 * 
 */

#ifndef __CMD_H__
#define __CMD_H__

#include "usart.h"
#include <stdlib.h>
#include <string.h>

////////////////////// CMD Module Configuration //////////////////////

/* Enable & Baud rate */
#define CMD_BAUD_UCA0     0             // Baud rate for for UCA0. Set to 0 to disable.
#define CMD_BAUD_UCA1     115200        // Baud rate for for UCA1. Set to 0 to disable.
#define CMD_BAUD_UCA2     0             // Baud rate for for UCA2. Set to 0 to disable.
#define CMD_BAUD_UCA3     0             // Baud rate for for UCA3. Set to 0 to disable.

/* Case sensitivity */
#define CMD_CMP           strncasecmp   // Set to strncasecmp for case insensitive, strncmp for case sensitive.

//////////////////////////////////////////////////////////////////////

typedef struct Cmd_s {
    char *cmd; // Name of the command
    void (*func)(char **argv, int argc); // Function pointer to the command function
} Cmd_t;

Cmd_t *CmdList; // Expandable list of commands
uint16_t CmdListSize; // Size of the list

void Cmd_init(void);

#endif
