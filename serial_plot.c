
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    HANDLE hSerial;
    hSerial = CreateFile(
        argv[1],
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            printf("serial port does not exist. Inform user.\n");
        }
        printf("some other error occurred. Inform user.\n");
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        printf("error getting state\n");
    }
    dcbSerialParams.BaudRate = 11978688;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fParity = FALSE;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    FILE *f = fopen("C:/Users/natha/plotf", "wb");
    if (f == NULL)
    {
        printf("unable to open plotf\n");
        exit(1);
    }
    fflush(f);
    unsigned short tbuf[1] = {65535};
    unsigned short sbuf[2] = {0, 0};
    short slope = 0;
    unsigned short rbuf[2048] = {0};
    unsigned short w = 0;
    unsigned long numBytesRead;
    unsigned long numBytesWritten;
    while (1)
    {
        while((sbuf[0] > 2059) || (sbuf[0] < 2039) || (slope > 0)){
            ReadFile(hSerial, sbuf, sizeof(sbuf), &numBytesRead, 0); //read 1
            slope = sbuf[0] - sbuf[1];
        }
        sbuf[0] = 65535;
        slope = 1e4;
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        fseek(f, 0, SEEK_SET);
        numBytesWritten = fwrite(rbuf, 1, sizeof(rbuf), f);
        fflush(f);
    }
}