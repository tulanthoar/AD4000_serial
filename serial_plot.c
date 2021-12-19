
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>

int main()
{
    HANDLE hSerial;
    hSerial = CreateFile(
        "COM4",
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
    dcbSerialParams.BaudRate = 12000000;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = EVENPARITY;
    dcbSerialParams.fParity = TRUE;
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
    unsigned short rbuf[2048] = {0};
    unsigned short w = 0;
    unsigned long numBytesRead;
    unsigned long numBytesWritten;
    while (1)
    {
        while(tbuf[0] > 100){
            ReadFile(hSerial, tbuf, sizeof(tbuf), &numBytesRead, 0); //read 1
        }
        tbuf[0] = 65535;
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        fseek(f, 0, SEEK_SET);
        numBytesWritten = fwrite(rbuf, 1, sizeof(rbuf), f);
        fflush(f);
        Sleep(17);
    }
}