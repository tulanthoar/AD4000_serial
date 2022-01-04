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
    dcbSerialParams.fParity = FALSE;
    // set the communication parameters
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    // clear the current content of the serial device
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // open the output file
    FILE *f = fopen("C:/Users/natha/outf", "wb");
    if (f == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    // flush any data to the file
    fflush(f);
    // buffer for the recieve data
    unsigned short rbuf[2048] = {0};
    // index used to compare the read values
    unsigned short w = 0;
    // number of bytes read in the operation
    unsigned long numBytesRead;
    // number of bytes written in the operation
    unsigned long numBytesWritten;
    while (1)
    {
        // read bytes from the serial device to fill the receive buffer
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        // write the read bytes into the output file
        numBytesWritten = fwrite(rbuf, 1, sizeof(rbuf), f);

        // check that the number of bytes we read is the number we expect
        if (numBytesRead != sizeof(rbuf))
        {
            printf("number bytes read = %d", numBytesRead);
            exit(1);
        }

        // check that the values we read are a monotonically increasing series
        for (int i = 0; i < sizeof(rbuf) / 2; ++i)
        {
            // check that the value is correct
            if (rbuf[i] != w)
            {
                // if the value is wrong, print the read value and the expected value
                printf("w = %hu, rbuf = %hu", w, rbuf[i]);
                exit(1);
            }
            ++w;
            // reset w when we get to the max value
            if (w == 16384)
                w = 0;
        }
        // print the value of w as an indicator that the program is running
        printf("%hu", w);

        // flush the data to the output file
        fflush(f);
        // check that the number of bytes written is what we expect it to be
        if (numBytesWritten != sizeof(rbuf))
        {
            printf("number bytes written = %d", numBytesWritten);
            exit(1);
        }
    }
}