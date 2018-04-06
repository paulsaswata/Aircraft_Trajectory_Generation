import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import csv
mpl.rcParams['legend.fontsize'] = 10

a=[]
b=[]
c=[]

e=[]
f=[]
g=[]

with open('path.csv') as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        a.append(float(row[0]))
        b.append(float(row[1]))
        c.append(float(row[3]))

with open('ends.csv') as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        e.append(float(row[0]))
        f.append(float(row[1]))
        g.append(float(row[3]))


plt.plot(a, b, 'r', label="Trajectory")

plt.plot(e, f, '.b')

legend = plt.legend(loc='upper right', shadow=True)
plt.axis('equal')
plt.xlabel('Longitude')
plt.ylabel('Latitude')
plt.title('Flight path')
#plt.show()

fig = plt.figure()
axyz = fig.gca(projection='3d')
plt.axis('equal')

axyz.plot(a, b, c, label='Trajectory')

axyz.scatter(e,f,g, marker='o',  color='r')

axyz.view_init(elev=11, azim=-104)

axyz.legend()
axyz.set_xlabel('Longitude')
axyz.set_ylabel('Latitude')
axyz.set_zlabel('Altitude(in feet)')

plt.show()
