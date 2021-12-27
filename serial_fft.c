#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>
#include <fftw3.h>
#include <math.h>
#include <complex.h>

int main()
{
    HANDLE hSerial;
    hSerial = CreateFile(
        "COM5",
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
    dcbSerialParams.fParity = TRUE;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        printf("error setting serial port state\n");
    }
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    FILE *f = fopen("C:/Users/natha/fftf", "wb");
    if (f == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    fflush(f);
    fftw_complex *out;
    double *in;
    fftw_plan p;
    unsigned int N = 4096;
    in = (double*) fftw_malloc(sizeof(double) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    double abs_out[2049];
    p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
    unsigned short rbuf[4096] = {0};
    unsigned short w = 0;
    unsigned long numBytesRead;
    unsigned long numBytesWritten;
    while (1)
    {
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        fftw_execute(p); /* repeat as needed */
        for(int i = 0; i < N; ++i) in[i] = rbuf[i] * 1.0;
        for(int i = 0; i < N/2 + 1; ++i){
            abs_out[i] = 20 * log10(sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1])) - 133.0;
        }
        fseek(f, 0, SEEK_SET);
        numBytesWritten = fwrite(abs_out, 1, sizeof(abs_out), f);
        fflush(f);
    }
    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
}