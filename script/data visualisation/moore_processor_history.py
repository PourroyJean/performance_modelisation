import pandas as pd
import matplotlib.pyplot as plt


file = "Moore.xlsx"
# Load spreadsheet: xl
xl = pd.ExcelFile(file)
# Print sheet names
print("Sheet names: ", xl.sheet_names)

#Select a sheet as a DataFrame

df1 = xl.parse(1,
               skiprows=0,        #First line is the column name
               parse_cols=7,      #We can select some columns to import
               names=['Processor',	'NbTransistor',	'Annee',	'Designer',	'Gravure', 'Surface', 'Densite', 'Moore'])  #We rename column as we want [Col1, Col2 etc..]



df1['Gravure'] = df1['Gravure'] * 100
print (df1.head())

pp = df1.plot(x='Annee', y='Densite', kind='scatter')
df1.plot(x='Annee', y='Moore', label='Prediction de G. Moore', ax=pp)
df1.plot(x='Annee', y='Gravure', label='Finesse de gravure', ax=pp)
# df1.plot(x='Annee', y='Moore', label='Prediction de G. Moore', ax=pp)
#
# pp.set_xlabel ("Annees")
# pp.set_ylabel ("Nombre de transistors par processeur")
plt.yscale('log')
x1,x2,y1,y2 = plt.axis()
plt.axis((1980,2020,y1,y2))



#Annotation

#Arrow
texteX1 = 1990
texteY1 = 500
flecheX1 = 1985
flecheY1 = 500
plt.annotate('Intel 80386',
             xy=(flecheX1, flecheY1), xycoords='data',
             xytext=(texteX1, texteY1), textcoords='data',
             arrowprops=dict(arrowstyle="->",
                            linewidth = 5.,
                            color = 'red'),
            )


texteX1 = 2012
texteY1 = 10E5
flecheX1 = 2016
flecheY1 = 15080473.68
plt.annotate('Xeon 22 cores\n Broadwell',
             xy=(flecheX1, flecheY1), xycoords='data',
             xytext=(texteX1, texteY1), textcoords='data',
             arrowprops=dict(arrowstyle="->",
                            linewidth = 5.,
                            color = 'red'),
            )
pp.set_xlabel ("Annee")
pp.set_ylabel ("Densite de transistors (par mm^2)")

plt.show()