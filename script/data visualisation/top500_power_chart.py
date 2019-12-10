import pandas as pd
import matplotlib.pyplot as plt


file = "TOP500_201611.xls"
# Load spreadsheet: xl
xl = pd.ExcelFile(file)
# Print sheet names
print("Sheet names: ", xl.sheet_names)

#Select a sheet as a DataFrame

df1 = xl.parse(0,
               skiprows=0,        #First line is the column name
               parse_cols=[0,17],      #We can select some columns to import
               names=['rank', 'Power'])  #We rename column as we want [Col1, Col2 etc..]
print (df1.head())

pp = df1.plot(x='rank', y='Power', kind='scatter')
pp.set_xlabel ("Classement dans le Top500")
pp.set_ylabel ("Consommation en kilo watt")
plt.yscale('log')
x1,x2,y1,y2 = plt.axis()
plt.axis((-50,x2,y1,y2))

print (df1.sort(['Power'], ascending=False).head())


#Annotation
plt.text(200, 1E4, 'Moyenne = ' +str(round(df1['Power'].mean())) + ' kWatt')

plt.show()