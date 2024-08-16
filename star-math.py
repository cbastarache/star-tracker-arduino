import matplotlib.pyplot as plt
import numpy as np

def mag(x):
    tmp = 0
    for i in range(len(x)):
        tmp += x[i] ** 2
    return np.sqrt(tmp)

def normalize(x):
    magnitude = mag(x)
    for i in range(len(x)):
        x[i] = x[i] / magnitude
    return x

def sphere2comp(be, az):
    x = np.cos(be)*np.cos(az)
    y = np.sin(be)*np.cos(az)
    z = np.sin(az)
    return normalize([x,y,z])

def deg2rad(x):
    return x * np.pi / 180

def rad2deg(x):
    return x * 180 / np.pi 

ax = plt.figure().add_subplot(projection='3d')

# Prepare arrays x, y, z
theta = np.linspace(0, np.pi * 2, 100)

p = sphere2comp( deg2rad(2), deg2rad(45) ) # reference axis (polaris)
s = sphere2comp( deg2rad(5), deg2rad(45) ) # starting point on the circle (star target)

print("p: ", p)
print("s: ", s)

d = -(p[0]*s[0] + p[1]*s[1] + p[2]*s[2]) # plane equation constant

# print("d: ", d)

#center point, where polaris vector intersects the plane
c = [ -d*p[0] / (p[0]**2 + p[1]**2 + p[2]**2),
      -d*p[1] / (p[0]**2 + p[1]**2 + p[2]**2),
      -d*p[2] / (p[0]**2 + p[1]**2 + p[2]**2) ]

#radius from center point to star target
r = np.sqrt( np.power(c[0] - s[0], 2) + np.power(c[1] - s[1], 2) + np.power(c[2] - s[2], 2)) 
print("r: ", r)
# print("c: ", c)

# vector along the plane axis, aka normal to p
a = [ -p[1] / mag(p), p[0] / mag(p), 0] 
a = normalize(a)

# second vector on plane, cross product of a x p
b = [ a[1]*p[2] - p[1]*a[2],
      a[0]*p[2] - p[0]*a[2],
      a[0]*p[1] - p[0]*a[1] ]
b = normalize(b)

print("a: ", a)
print("b: ", b)

# parameterization of a circle in 3d space
x = r * ( np.cos(theta) * a[0] + np.sin(theta) * b[0] ) + c[0]
y = r * ( np.cos(theta) * a[1] + np.sin(theta) * b[1] ) + c[1]
z = r * ( np.cos(theta) * a[2] + np.sin(theta) * b[2] ) + c[2]

az = rad2deg(np.arctan2(z, np.sqrt( np.power(x,2) + np.power(y,2) ))) 
be = rad2deg(np.arctan2(y, x))

# print(be)

startT = np.arccos( ( (s[1] - c[1])/(r * a[1]) - ( b[1]*(s[0]-c[0])/(b[0]*r) ) ) / ( 1 - ( a[0]*b[1]/(b[0]*a[1]) ) ) )
# startT = np.arcsin( ( (x-c[0])/(a[0]*r) - (y-c[1])/(a[1]*r) ) / ( b[0]/a[0] - b[1]/a[1] ) )
# print(rad2deg(startT))

x0 = r * ( np.cos(startT) * a[0] + np.sin(startT) * b[0] ) + c[0]
y0 = r * ( np.cos(startT) * a[1] + np.sin(startT) * b[1] ) + c[1]
z0 = r * ( np.cos(startT) * a[2] + np.sin(startT) * b[2] ) + c[2]
print([x0,y0,z0])

ax.plot(x, y, z, label='parametric curve')
ax.legend()

plt.show()
