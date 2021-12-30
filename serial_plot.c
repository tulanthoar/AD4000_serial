#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    // handle for the serial interface
    HANDLE hSerial;
    hSerial = CreateFile(
        argv[1], // command line argument is COM port
        GENERIC_READ, // read only
        0,
        0,
        OPEN_EXISTING, // open existing file
        FILE_ATTRIBUTE_NORMAL,
        0);
    // check if the file opening was successful
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            printf("serial port does not exist. Inform user.\n");
        }
        printf("some other error occurred. Inform user.\n");
    }

    // struct to define the serial parameters
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    // get the current state
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        printf("error getting state\n");
    }
    // set the baud rate
    dcbSerialParams.BaudRate = 11978688;
    // 8 bits to a byte
    dcbSerialParams.ByteSize = 8;
    // one stop bit
    dcbSerialParams.StopBits = ONESTOPBIT;
    // no parity bit
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fParity = TRUE;
    // set the communication parameters
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    // clear the current content of the serial device
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
    
    // open the output file
    FILE *f = fopen("C:/Users/natha/plotf", "wb");
    if (f == NULL)
    {
        printf("unable to open plotf\n");
        exit(1);
    }
    // flush any data to the file
    fflush(f);
    // buffer to test for the correct trigger point and slope
    unsigned short sbuf[2] = {0, 0};
    // initialize the slope
    short slope = 0;
    // buffer to read the data into
    unsigned short rbuf[2048] = {0};
    // number of bytes read from the serial device
    unsigned long numBytesRead;
    // number of bytes written to the output file
    unsigned long numBytesWritten;
    while (1)
    {
        // check whether the slope is negative, or the value is far below mid voltage,
        // or the value is far above mid voltage
        while((sbuf[0] > 2059) || (sbuf[0] < 2039) || (slope < 0)){
            // if any of the above conditions are true, read the next two values to check the condition again 
            ReadFile(hSerial, sbuf, sizeof(sbuf), &numBytesRead, 0);
            // calculate the new slope
            slope = sbuf[0] - sbuf[1];
        }
        // reset the calculate values for the next iteration
        sbuf[0] = 65535;
        slope = 0;
        // read from the serial device into the buffer
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0);
        // seek to the start of the file to overwrite our prior data
        fseek(f, 0, SEEK_SET);
        // write to the output file
        numBytesWritten = fwrite(rbuf, 1, sizeof(rbuf), f);
        // flush the output buffer
        fflush(f);
    }
}