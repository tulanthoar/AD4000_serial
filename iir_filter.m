clear
HSE = 25e6 / 3; % HSE from stlink
% pll parameters set using STMMXCube
pll2m = 4;
pll2n = 280;
pll2p = 8;
% frequency of SPI signal (SPI signal runs at one half SPI peripheral clk)
F_spi = HSE / pll2m * pll2n / pll2p / 2;
% time per sample
Ts = 16 / F_spi;
% corner frequency
fc = 75e3;
% fc = 1.1e6;
% order of filter
n = 3;
% calculate continuous time coefficients for low pass butterworth filter
[zb,pb,kb] = butter(n,2*pi*fc,'s');
% Sampling frequency
Fs = 1 / Ts;
% sampling frequency in rad/s
Ws = Fs * 2 * pi;
% nyquist frequency in rad/s
nyquist = Ws / 2;
% calculate transfer function coefficients from zeros/poles form
[bb,ab] = zp2tf(zb,pb,kb);

% radial frequency for plotting
wb = linspace(1, nyquist, 4000);
% magnitude of the filter transfer function
hb = freqs(bb,ab,wb);
% magnitude for the aliased signal
hbHigh = zeros(size(hb));
% radial frequency of aliased signal
wbHigh = zeros(size(wb));
% calculated aliased filter magnitude
for i = 1:length(hb)
    hbHigh(i) = hb(i);
    wbHigh(i) = 2 * nyquist - wb(i);
end

figure
% magnitude plot of un-aliased filter
h = plot(wb/(2*pi),mag2db(abs(hb)));
set(h, 'LineWidth', 3)
hold on
% magnitude plot of aliased filter
h = plot(wbHigh/(2*pi),mag2db(abs(hbHigh)));
set(h, 'LineWidth', 3)
axis([10 (nyquist/pi + 80000) -80 5])
grid
xlabel('Frequency (Hz)')
ylabel('Attenuation (dB)')
title(sprintf('Butterworth Filter (n=%d)', n))
legend('Original Signal', 'Aliased Signal')

% create the transfer function in the s domain
Gp = tf(bb, ab);
% plot the transfer function
figure;
bode(Gp);

% convert the transfer function from s domain to z domain
Gcz = c2d(Gp, Ts, 'tustin');
% set the variable to z^-1
Gcz.variable='z^-1';
% numerator coefficients
cn = Gcz.Numerator{1};
% denominator coefficients
cd = Gcz.Denominator{1};
% print the evaluation statements for copy/paste into c source file
format long

if n == 5
        printStr = strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-4] + (%.10e) * aRxBuffer[j-5] \\\n", ...
        "- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
        "- (%.10e) * yi[j-3] - (%.10e) * yi[j-4] \\\n", ...
        "- (%.10e) * yi[j-5];\n\n");
        fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, ...
            cn(5) * 16, cn(6) * 16, ...
        cd(2), cd(3), cd(4), cd(5), cd(6));
            printStr = strcat("yi[j+1] = (%.10e) * aRxBuffer[j+1] + (%.10e) * aRxBuffer[j] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-1] + (%.10e) * aRxBuffer[j-2] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-3] + (%.10e) * aRxBuffer[j-4] \\\n", ...
        "- (%.10e) * yi[j] - (%.10e) * yi[j-1] \\\n", ...
        "- (%.10e) * yi[j-2] - (%.10e) * yi[j-3] \\\n", ...
        "- (%.10e) * yi[j-4];\n\n");
        fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, ...
            cn(5) * 16, cn(6) * 16, ...
        cd(2), cd(3), cd(4), cd(5), cd(6));
    
    fprintf("aTxBufferMini[i] = (uint16_t)yi[j+1];\n\n");
    
            printStr = strcat("yi[j+2] = (%.10e) * aRxBuffer[j+2] + (%.10e) * aRxBuffer[j+1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
        "- (%.10e) * yi[j+1] - (%.10e) * yi[j] \\\n", ...
        "- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
        "- (%.10e) * yi[j-3];\n\n");
        fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, ...
            cn(5) * 16, cn(6) * 16, ...
        cd(2), cd(3), cd(4), cd(5), cd(6));
            printStr = strcat("yi[j+3] = (%.10e) * aRxBuffer[j+3] + (%.10e) * aRxBuffer[j+2] \\\n", ...
        "+ (%.10e) * aRxBuffer[j+1] + (%.10e) * aRxBuffer[j] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-1] + (%.10e) * aRxBuffer[j-2] \\\n", ...
        "- (%.10e) * yi[j+2] - (%.10e) * yi[j+1] \\\n", ...
        "- (%.10e) * yi[j] - (%.10e) * yi[j-1] \\\n", ...
        "- (%.10e) * yi[j-2];\n\n");
        fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, ...
            cn(5) * 16, cn(6) * 16, ...
        cd(2), cd(3), cd(4), cd(5), cd(6));
    
    fprintf("aTxBuffer[i] = (uint16_t)yi[j+3];\n");
end

if n == 4
    printStr = strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-4] \\\n", ...
        "- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
        "- (%.10e) * yi[j-3] - (%.10e) * yi[j-4];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, cn(5) * 16, ...
        cd(2), cd(3), cd(4), cd(5));
    printStr = strcat("yi[j+1] = (%.10e) * aRxBuffer[j+1] + (%.10e) * aRxBuffer[j] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-1] + (%.10e) * aRxBuffer[j-2] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-3] \\\n", ...
        "- (%.10e) * yi[j] - (%.10e) * yi[j-1] \\\n", ...
        "- (%.10e) * yi[j-2] - (%.10e) * yi[j-3];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, cn(5) * 16, ...
        cd(2), cd(3), cd(4), cd(5));
    fprintf("aTxBufferMini[i] = (uint16_t)yi[j+1];\n\n");
    
    printStr = strcat("yi[j+2] = (%.10e) * aRxBuffer[j+2] + (%.10e) * aRxBuffer[j+1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-2] \\\n", ...
        "- (%.10e) * yi[j+1] - (%.10e) * yi[j] \\\n", ...
        "- (%.10e) * yi[j-1] - (%.10e) * yi[j-2];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, cn(5) * 16, ...
        cd(2), cd(3), cd(4), cd(5));
    printStr = strcat("yi[j+3] = (%.10e) * aRxBuffer[j+3] + (%.10e) * aRxBuffer[j+2] \\\n", ...
        "+ (%.10e) * aRxBuffer[j+1] + (%.10e) * aRxBuffer[j] \\\n", ...
        "+ (%.10e) * aRxBuffer[j-1] \\\n", ...
        "- (%.10e) * yi[j+2] - (%.10e) * yi[j+1] \\\n", ...
        "- (%.10e) * yi[j] - (%.10e) * yi[j-1];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16, cn(5) * 16, ...
        cd(2), cd(3), cd(4), cd(5));
    fprintf("aTxBuffer[i] = (uint16_t)yi[j+3];\n");
end

if n == 3
    printStr = strcat("yi[j] = (%.16e) * aRxBuffer[j] + (%.16e) * aRxBuffer[j-1] \\\n", ...
        "+ (%.16e) * aRxBuffer[j-2] + (%.16e) * aRxBuffer[j-3] \\\n", ...
        "- (%.16e) * yi[j-1] - (%.16e) * yi[j-2] \\\n", ...
        "- (%.16e) * yi[j-3];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16,...
        cd(2), cd(3), cd(4));
    printStr = strcat("yi[j+1] = (%.16e) * aRxBuffer[j+1] + (%.16e) * aRxBuffer[j] \\\n", ...
        "+ (%.16e) * aRxBuffer[j-1] + (%.16e) * aRxBuffer[j-2] \\\n", ...
        "- (%.16e) * yi[j] - (%.16e) * yi[j-1] \\\n", ...
        "- (%.16e) * yi[j-2];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16,...
        cd(2), cd(3), cd(4));
    fprintf("aTxBufferMini[i] = (uint16_t)yi[j+1];\n\n");
    printStr = strcat("yi[j+2] = (%.16e) * aRxBuffer[j+2] + (%.16e) * aRxBuffer[j+1] \\\n", ...
        "+ (%.16e) * aRxBuffer[j] + (%.16e) * aRxBuffer[j-1] \\\n", ...
        "- (%.16e) * yi[j+1] - (%.16e) * yi[j] \\\n", ...
        "- (%.16e) * yi[j-1];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16,...
        cd(2), cd(3), cd(4));
    printStr = strcat("yi[j+3] = (%.16e) * aRxBuffer[j+3] + (%.16e) * aRxBuffer[j+2] \\\n", ...
        "+ (%.16e) * aRxBuffer[j+1] + (%.16e) * aRxBuffer[j] \\\n", ...
        "- (%.16e) * yi[j+2] - (%.16e) * yi[j+1] \\\n", ...
        "- (%.16e) * yi[j];\n\n");
    fprintf(printStr, cn(1) * 16, cn(2) * 16, cn(3) * 16, cn(4) * 16,...
        cd(2), cd(3), cd(4));
    fprintf("aTxBuffer[i] = (uint16_t)yi[j+3];\n");
end

%%
% fix random number generator
rng(0);
% number of bits to simulate
ADCBits = 12;
% end time of simulation
tEnd = 22786 / Fs;
% time for simulation
t = 0:1/Fs:tEnd;
% period of synthetic signal
tSig = tEnd / 100;
% frequency of synthetic signal
fSig = 1/tSig;
% synthetic signal, sin wave
ySig = (2^(ADCBits - 1) - 1) * sin(2*pi*fSig*t);
% SNR of AD7276 at 100 kHz
snrADC = 72.2;
% apply white noise to signal
yADC = ySig + randn(size(ySig))*std(ySig)/db2mag(snrADC);
% discretize the ADC signal
yDig = round(yADC);
% clip the ADC signal
yDig = max(min(yDig, (2^(ADCBits - 1)) - 1), -(2^(ADCBits - 1)));
% calculate the digital noise, the true signal minus the digitzed signal
digNoise = ySig - yDig;
% calculate SNR
ySNR = snr(ySig,digNoise);
% estimate ENOB
yENOB =  (ySNR - 1.7609)/ 6.0206;
% filter the noise
digNoiseFilt = lsim(Gcz, digNoise, t);
% filter the signal
ySigFilt = lsim(Gcz, ySig, t);
% calculate snr of filtered signals
ySNRFilt = snr(ySigFilt,digNoiseFilt);
% estimate ENOB of filtered signals
yENOBFilt =  (ySNRFilt - 1.7609)/ 6.0206;
% reconstruct the filtered, digitized signal
yDigFilt = ySigFilt + digNoiseFilt;
% remove the filter delay by 9 units
yDigFiltNoDelay = yDigFilt;
yDigFiltNoDelay(1:end - 9) = yDigFilt(10:end);
yDigFiltNoDelay(end-9:end) = 0;
% plot the original signal, digitized signal, and filtered signal with the
% delay removed
figure
plot(t, ySig, 'k', 'LineWidth', 2)
hold on
plot(t, yDig, 'o', 'MarkerSize', 12)
plot(t, yDigFiltNoDelay, 'gx', 'MarkerSize', 12)
hold off
legend('Original Signal', 'ADC Signal', 'Filtered Signal')