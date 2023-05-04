import csv
import matplotlib.pyplot as plt
import numpy as np

def FFTplot(csv_filename, filterType):
    t, V = read_CSV(csv_filename)
    samplerate = 1/(t[1]-t[0])
    f, fftV = FFT(V, samplerate)
    title = csv_filename[:-4]

    unfilteredDict = {'time': t, 'voltage': V, 'freq': f, 'FTvolt': fftV}

    if filterType == 'NO':
        plotNOfilter(t, V, f, fftV, title)
    elif filterType == 'MAF':
        filteredDict = MAF(t, V, samplerate)
        plotWITHfilter(unfilteredDict, filteredDict, 'MAF'+title)
    elif filterType == 'IIR':
        filteredDict = IIR(t, V, samplerate)
        plotWITHfilter(unfilteredDict, filteredDict, 'IIR'+title)
    else:
        filteredDict = FIR(t, V, samplerate)
        plotWITHfilter(unfilteredDict, filteredDict, 'FIR'+title)

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
    
    ax2.loglog(freq, abs(fftD),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    
    plt.savefig('/Users/AntaraSen_1/Documents/GitHub/Sen_Mechatronics/HW9 (DSP)/Plots/unfiltered'+plotname+'.png', dpi=600)
    plt.show()

def plotWITHfilter(u, f, plotname):
    fig, (ax1, ax2) = plt.subplots(2, 1)
    
    ax1.plot(u['time'], u['voltage'], 'b')
    ax1.plot(f['time'], f['voltage'], 'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Voltage')

    ax2.loglog(u['freq'], abs(u['FTvolt']),'b')
    ax2.loglog(f['freq'], abs(f['FTvolt']),'r')
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Voltage (freq)|')

    plt.savefig('/Users/AntaraSen_1/Documents/GitHub/Sen_Mechatronics/HW9 (DSP)/Plots/'+plotname+'.png', dpi=600)
    plt.show()

def MAF(time, voltage, samplerate):
    # MAF help from geeksforgeeks.com
    X = 30
    i = 0
    filteredV = []
    filteredV += ([0] * (X-1)) # sets the first X-1 items to 0 so that t and V array shapes are the same for plotting
    while i < len(voltage) - X + 1:
        window_average = round(np.sum(voltage[i:i+X]) / X, 2)
        filteredV.append(window_average)
        i += 1
    
    f, fftV = FFT(filteredV, samplerate)
    filteredDict = {'time': time, 'voltage': filteredV, 'freq': f, 'FTvolt': fftV}
    return filteredDict

def IIR(time, voltage, samplerate):
    filteredV = []
    A = 0.92
    B = 1-A
    for datapoint in voltage:
        if len(filteredV) == 0:
            filteredV.append(0)
        else:
            filteredV.append((A*filteredV[-1])+(B*datapoint))

    f, fftV = FFT(filteredV, samplerate)
    filteredDict = {'time': time, 'voltage': filteredV, 'freq': f, 'FTvolt': fftV}
    return filteredDict

def FIR(time, voltage, samplerate):
    h = [
    -0.000000000000000001,
    0.000153658410422764,
    0.000649144906776042,
    0.001565506404196108,
    0.003011200887525600,
    0.005109183239820784,
    0.007976171834780882,
    0.011698866728033030,
    0.016310538213988885,
    0.021771577196705527,
    0.027957254599326069,
    0.034655108617965162,
    0.041573169897687758,
    0.048358807015575329,
    0.054626524438085376,
    0.059991776076165751,
    0.064106951376904919,
    0.066695280805697255,
    0.067578558700685201,
    0.066695280805697255,
    0.064106951376904919,
    0.059991776076165765,
    0.054626524438085404,
    0.048358807015575336,
    0.041573169897687778,
    0.034655108617965162,
    0.027957254599326069,
    0.021771577196705538,
    0.016310538213988899,
    0.011698866728033023,
    0.007976171834780900,
    0.005109183239820785,
    0.003011200887525603,
    0.001565506404196107,
    0.000649144906776043,
    0.000153658410422764,
    -0.000000000000000001]
    coeff = np.asarray(h)
    X = len(coeff)
    i = 0
    filteredV = []
    filteredV += ([0] * (X-1)) # sets the first X-1 items to 0 so that t and V array shapes are the same for plotting
    while i < len(voltage) - X + 1:
        window_average = np.sum(np.asarray(voltage[i:i+X])*coeff)
        filteredV.append(window_average)
        i += 1

    f, fftV = FFT(filteredV, samplerate)
    filteredDict = {'time': time, 'voltage': filteredV, 'freq': f, 'FTvolt': fftV}
    return filteredDict

FFTplot('sigD.csv', 'FIR')