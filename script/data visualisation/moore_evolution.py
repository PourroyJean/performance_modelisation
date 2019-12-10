import pandas as pd
import matplotlib.pyplot as plt


file = "Moore.xlsx"
# Load spreadsheet: xl
xl = pd.ExcelFile(file)
# Print sheet names
print("Sheet names: ", xl.sheet_names)

#Select a sheet as a DataFrame

df1 = xl.parse(0,
               skiprows=0,        #First line is the column name
               parse_cols=[0,1,2],      #We can select some columns to import
               names=['Annee', 'Transistors', 'Moore'])  #We rename column as we want [Col1, Col2 etc..]
print (df1.head())

pp = df1.plot(x='Annee', y='Transistors', kind='scatter')
df1.plot(x='Annee', y='Moore', label='Prediction de G. Moore', ax=pp)

pp.set_xlabel ("Annees")
pp.set_ylabel ("Nombre de transistors par processeur")
plt.yscale('log')
# x1,x2,y1,y2 = plt.axis()
# plt.axis((-50,x2,y1,y2))



#Annotation

plt.show()