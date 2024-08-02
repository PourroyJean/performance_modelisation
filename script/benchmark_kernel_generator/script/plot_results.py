import matplotlib.pyplot as plt
import numpy as np
import sys

# Colorblind colors
colors = ['#377eb8', '#ff7f00', '#4daf4a',
                  '#f781bf', '#a65628', '#984ea3',
                  '#999999', '#e41a1c', '#dede00']

if len(sys.argv) < 3:
    print("Please provide an input file to read data from (CSV kind, with ; separator) and the name of the output file")

if len(sys.argv[1:-1]) % 3 != 0:
    figure, axis = plt.subplots(nrows=3,ncols=len(sys.argv[1:-1]) // 3 + 1, sharex=True)
else:
    figure, axis = plt.subplots(nrows=3,ncols=len(sys.argv[1:-1]) // 3, sharex=True)

# Set plot size
figure.set_figheight(10 * 3)
figure.set_figwidth(25 * (len(sys.argv[1:-1]) // 3))

index_axis=0

for input_file in sys.argv[1:-1]:
    headers = []
    x = []
    all_y = {}

    with open(input_file, 'r') as data:
        # Initialize headers
        headers = data.readline().replace('\n','').split(';')
        
        # And value lists
        for i in range(1, len(headers)):
            if "(Mean)" in headers[i]:
                all_y[headers[i]] = {'min': [], 'max': [], 'mean': []}
            else:
                all_y[headers[i]] = []
        
        # Read all lines
        data = data.readlines()
        for row in data:
            row = row.replace('\n','').split(';')
            
            # Add first value as x axis
            x.append(int(row[0]))
            
            # And others as potential y axis
            for i in range(1, len(row)):
                try:
                    all_y[headers[i]].append(float(row[i])) # Conversion to float to avoid string interpretation on the final plot
                except:
                    if '/' in row[i]:
                        all_y[headers[i]]['max'].append(float(row[i].split("/")[0]))
                        all_y[headers[i]]['min'].append(float(row[i].split("/")[1]))
                        all_y[headers[i]]['mean'].append(float(row[i].split("/")[2]))
                finally:
                    continue

    # Plot the computed axis
    for i in range(1, len(headers)):
        if not isinstance(all_y[headers[i]], dict):
            axis[index_axis % 3, index_axis // 3].plot(x[0:len(all_y[headers[i]])],
                                  all_y[headers[i]],
                                  label=headers[i],
                                  color=colors[i])
        else:
            axis[index_axis % 3, index_axis // 3].plot(x[0:len(all_y[headers[i]]['mean'])],
                                  all_y[headers[i]]['mean'],
                                  label=headers[i],
                                  color=colors[i])
            
            ## Two issues with this code :
            # 1. Unreadable on the final graph, doesn't really give any useful information
            # 2. Substraction for lower error sometimes generate negative results (wrongful calculation in earlier steps)
            #    causing errors @ runtime
            #
            #
            # lower_error = np.subtract(all_y[headers[i]]['mean'], all_y[headers[i]]['min'])
            # upper_error = np.subtract(all_y[headers[i]]['max'], all_y[headers[i]]['mean'])
            # axis[index_axis].errorbar(x=x[0:len(all_y[headers[i]]['mean'])],
            #                           y=all_y[headers[i]]['mean'],
            #                           yerr=[lower_error, upper_error],
            #                         #   fmt='o',
            #                           capsize=3,
            #                           ecolor=colors[i])
            
    # Set axis name
    axis[index_axis % 3, index_axis // 3].set_ylabel(headers[1].split("|")[1])
    axis[index_axis % 3, index_axis // 3].set_xlabel(headers[0])

        
    max_size=0
    for i in all_y:
        max_size = max(len(all_y[i]), max_size)

    # Create final image
    axis[index_axis % 3, index_axis // 3].legend()
    
    index_axis+=1
    
plt.tight_layout()
plt.savefig(sys.argv[-1], pad_inches=0)