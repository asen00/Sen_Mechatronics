from ulab import numpy as np
import math

# add 3 sine waves together
t = np.arange(1024)
w1 = 50 * np.sin(500 * t)
w2 = 70 * np.sin(400 * t)
w3 = 90 * np.sin(300 * t)
wave = w1 + w2 + w3

# ulab.np.fft.fft outputs real array and imaginary array
# normalize both by length of input array
re = np.fft.fft(wave)[0]/1024
im = np.fft.fft(wave)[1]/1024

# add in quadrature and sqrt to get abs value
sp = np.zeros(len(re))
for k in range(len(re)):
    sp[k] = math.sqrt((re[k]**2)+(im[k]**2))

str2send = []
for i in range(len(sp)):
    if (math.fmod(i,7) == 0) and (i+6 < len(sp)):
        str2send.append(sp[i]+sp[i+1]+sp[i+2]+sp[i+3]+sp[i+4]+sp[i+5]+sp[i+6])

sendfinal = str2send[:73] # only want one-sided freq range so (1024/7)/2 data points
for j in range(len(sendfinal)):
    print("(" + str(sendfinal[j]) + ",)")