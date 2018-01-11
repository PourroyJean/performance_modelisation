#!/bin/python
import pandas as pd
import matplotlib.pyplot as plt
import sys


pd.set_option('display.max_columns', 50)

# file = "log_pcm_SKL.64GB_small.csv"
file = sys.argv[1]


data = pd.read_csv(file,
                   sep=';',
                   comment='#',               #Char reprensenting a comment in the data file
                   na_values=['Nothing'],      #Value that we want to be NaN
                   # skiprows=[0]
                   )
# data = datas [2:]


data.columns = ['Date','Time','Ch0Read','Ch0Write','Ch1Read','Ch1Write','Ch2Read','Ch2Write','Ch3Read','Ch3Write','Ch4Read','Ch4Write','Ch5Read','Ch5Write','READ1','WRITE1',' P. Write (T/s)','MEMORY1','Ch0Read','Ch0Write','Ch1Read','Ch1Write','Ch2Read','Ch2Write','Ch3Read','Ch3Write','Ch4Read','Ch4Write','Ch5Read','Ch5Write','READ2','WRITE2',' P. Write (T/s)','MEMORY2','Read','Write','Memory']

# print data.head ()
# sys.exit(-1)

#4th first lines
# print (data.head())

# print (data ['MEMORY2'])


data['MEMORY'] =  data.MEMORY1 + data.MEMORY2
data['READ']   =  data.READ1   + data.READ2
data['WRITE']  =  data.WRITE1  + data.WRITE2

# print data['MEMORY']

data.plot ( y=['READ', 'WRITE', 'MEMORY'], color = ['#FF8D6D', '#2AD2C9', '#614767'])

plt.show ()

