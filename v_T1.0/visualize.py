import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import csv
mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
axyz = fig.gca(projection='3d')
plt.axis('equal')

x=[]
y=[]
z=[]

a=[]
b=[]
c=[]

e=[]
f=[]
g=[]


with open("20.csv") as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        x.append(float(row[0]))
        y.append(float(row[1]))
        z.append(float(row[3]))


with open("30.csv") as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        a.append(float(row[0]))
        b.append(float(row[1]))
        c.append(float(row[3]))

with open("45.csv") as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')
    for row in readCSV:
        e.append(float(row[0]))
        f.append(float(row[1]))
        g.append(float(row[3]))


axyz.plot(x, y, z, label='20')
axyz.plot(a, b, c, label='30')
axyz.plot(e, f, g, label='45')

axyz.view_init(elev=25, azim=-40)

axyz.legend()
axyz.set_xlabel('Longitude')
axyz.set_ylabel('Latitude')
axyz.set_zlabel('Altitude(in feet)')

plt.figure()
plt.plot(x, y, 'b',label="20")
plt.plot(a, b, 'g', label="30")
plt.plot(e, f, 'r', label="45")

plt.legend = plt.legend(loc='upper right', shadow=True)
plt.axis('equal')
plt.xlabel('Longitude')
plt.ylabel('Latitude')
plt.title('Flight path')
plt.show()
