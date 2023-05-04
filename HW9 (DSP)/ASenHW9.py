import csv
import matplotlib.pyplot as plt
import numpy as np

def unfilteredFFTplot(csv_filename):
    t, V = read_CSV(csv_filename)
    samplerate = (t[-1]-t[0])/len(t)
    f, fftV = FFT(V, samplerate)
    title = csv_filename[:-4]
    plotNOfilter(t, V, f, fftV, title)

def read_CSV(csv_filename):
    t = [] # column 0
    data1 = [] # column 1

    filepath = '/Users/AntaraSen_1/Documents/GitHub/Sen_Mechatronics/HW9 (DSP)/'+csv_filename

    with open(filepath) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data1.append(float(row[1])) # second column
    
    return t, data1

def FFT(data, samplerate):
    Fs = samplerate # sample rate
    n = len(data) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(data)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    return frq, Y

def plotNOfilter(time, data, freq, fftD, plotname):
    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(time,data,'b')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(freq,abs(fftD),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.savefig('/Users/AntaraSen_1/Documents/GitHub/Sen_Mechatronics/HW9 (DSP)/Plots/unfiltered'+plotname+'.png', dpi=600)
    plt.show()

def plotWITHfilter(unfilteredDict, filteredDict, plotname):
    pass


unfilteredFFTplot('sigB.csv')