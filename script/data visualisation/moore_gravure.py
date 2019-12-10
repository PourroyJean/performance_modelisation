import pandas as pd
import matplotlib.pyplot as plt


file = "Moore.xlsx"
# Load spreadsheet: xl
xl = pd.ExcelFile(file)
# Print sheet names
print("Sheet names: ", xl.sheet_names)

#Select a sheet as a DataFrame

df1 = xl.parse(2,
               skiprows=0,        #First line is the column name
               parse_cols=[0, 3],      #We can select some columns to import
               names=['Annee',	'Gravure'])  #We rename column as we want [Col1, Col2 etc..]



print (df1.head())

pp = df1.plot(x='Annee', y='Gravure', kind='scatter')

# pp.set_xlabel ("Annees")
# pp.set_ylabel ("Nombre de transistors par processeur")
x1,x2,y1,y2 = plt.axis()
# plt.axis((1970,2020,-10,y2))



#Annotation
pp.set_xlabel ("Annee")
pp.set_ylabel ("Finesse de gravure d'un transistor (en nm)")

plt.yscale('log')


plt.show()