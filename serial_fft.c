#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include <io.h>
#include <fcntl.h>
#include <fftw3.h>
#include <math.h>
#include <complex.h>

// argument 1 is name of COM port
// argument 2 is name of file to use in fft_animation.py
// argument 3 is the name of file to record data to
int main(int argc, char *argv[])
{
    if( argc < 4){
        printf("Application requires 3 arguments\n");
        printf("Arg 1 is the name of COM port to use\n");
        printf("Arg 2 is the name of file to plot\n");
        printf("Arg 3 is the name of file to record to\n");
        exit(1);
    }
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
    FILE *fFft = fopen(argv[2], "wb");
    if (fFft == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    // flush any data to the file
    fflush(fFft);

    // open the output file
    FILE *fOut = fopen(argv[3], "wb");
    if (fOut == NULL)
    {
        printf("unable to open outf\n");
        exit(1);
    }
    // flush any data to the file
    fflush(fOut);

    // complex fourier coefficients
    fftwf_complex *out;
    // the inputs are real values
    float *in;
    // fft plan to calculate the transform
    fftwf_plan p;
    // number of points
    unsigned int N = 16384;
    // initialize the input to the fourier transform algorithm
    in = (float*) fftw_malloc(sizeof(float) * N);
    // initialize the output of the fourier transform
    out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N);
    // the magnitude of the fourier coefficients to be plotted
    float abs_out[8193];
    // create the fftw plan, discrete forward transform real to complex 1D
    p = fftwf_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE);
    // buffer for the recieved data
    unsigned short rbuf[16384] = {0};
    // number of bytes read from serial device
    unsigned long numBytesRead;
    // number of bytes written to the output file 
    unsigned long numBytesWrittenOut;
    // number of bytes written to the fft file 
    unsigned long numBytesWrittenFft;
    while (1)
    {
        // read data from serial device
        ReadFile(hSerial, rbuf, sizeof(rbuf), &numBytesRead, 0); //read 1
        // write the read bytes into the output file
        // numBytesWrittenOut = fwrite(rbuf, 1, sizeof(rbuf), fOut);
        // convert the read values from 16 bit integers to doubles
        for(int i = 0; i < N; ++i) in[i] = rbuf[i] * 1.0;
        // execute the fourier transform
        fftwf_execute(p);
        // convert the complex fourier coefficients to magnitudes, subtracting 145 for 0 dB to correspond to full scale
        for(int i = 0; i < N/2 + 1; ++i){
            abs_out[i] = 20 * log10(sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1])) - 167.8;
        }
        // seek to the start of the file to overwrite our prior data
        fseek(fFft, 0, SEEK_SET);
        // write the values to the file
        numBytesWrittenFft = fwrite(abs_out, 1, sizeof(abs_out), fFft);
        // flush the data to the fft file
        // fflush(fOut);
        // flush the data to the fft file
        fflush(fFft);
    }
    // dealocate the memory we reserved
    fftwf_destroy_plan(p);
    fftwf_free(in); fftwf_free(out);
}