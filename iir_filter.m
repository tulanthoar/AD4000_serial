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
% print the #DEFINE statements for copy/paste into c source file
format long
if n == 5
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"+ (%.10e) * aRxBuffer[j-4] + (%.10e) * aRxBuffer[j-5] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3] - (%.10e) * yi[j-4] \\\n", ...
"- (%.10e) * yi[j-5];\n\n"), cn(1), cn(2), cn(3), cn(4), cn(5), cn(6), ...
    cd(2), cd(3), cd(4), cd(5), cd(6));
end

if n == 4
fprintf(strcat("#define CALC_YI yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"+ (%.10e) * aRxBuffer[j-4] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3] - (%.10e) * yi[j-4];\n\n"), cn(1), cn(2), cn(3), cn(4), cn(5), ...
    cd(2), cd(3), cd(4), cd(5));
end

if n == 3
fprintf(strcat("#define CALC_YI yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] + (%.10e) * aRxBuffer[j-3] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2] \\\n", ...
"- (%.10e) * yi[j-3];\n\n"), cn(1), cn(2), cn(3), cn(4), ...
    cd(2), cd(3), cd(4));
end

if n == 2
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"+ (%.10e) * aRxBuffer[j-2] \\\n", ...
"- (%.10e) * yi[j-1] - (%.10e) * yi[j-2];\n\n"), cn(1), cn(2), cn(3), ...
    cd(2), cd(3));
end

if n == 1
fprintf(strcat("yi[j] = (%.10e) * aRxBuffer[j] + (%.10e) * aRxBuffer[j-1] \\\n", ...
"- (%.10e) * yi[j-1];\n\n"), cn(1), cn(2), ...
    cd(2));
end