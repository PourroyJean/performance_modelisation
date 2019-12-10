import pandas as pd
import matplotlib.pyplot as plt
import numpy as np



file = "processor_info.xlsx"
# Load spreadsheet: xl
xl = pd.ExcelFile(file)
# Print sheet names
print("Sheet names: ", xl.sheet_names)

#Select a sheet as a DataFrame

df1 = xl.parse(0,
               skiprows=0,        #First line is the column name
               parse_cols=5,      #We can select some columns to import
               names=["Microprocessor",	"Year",	"Clock Rate", "Pipeline Stages", "Power"])  #We rename column as we want [Col1, Col2 etc..]
print (df1.head())
ar = np.array(df1.iloc[:, 0])

print (ar)

pp = df1.plot(x='Year', y='Pipeline Stages', label ='Profondeur du pipeline',)
df1.plot(x='Year', y='Power', label='Consommation (watt)', ax=pp)
pp.set_xticklabels(df1.Microprocessor)

pp.set_xlabel ("Annees")
pp.set_ylabel ("Profondeur de pipeline et consommation (watt)")
#pp.yticks(ar, tick_lab)
# x1,x2,y1,y2 = plt.axis()
# plt.axis((-50,x2,y1,y2))



#Annotation

plt.show()