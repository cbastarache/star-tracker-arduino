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

def dot(u, v):
    return (u[0]*v[0] + u[1]*v[1] + u[2]*v[2]) 

def codir(u, v):
    print()
    return dot(u, v) == mag(u)*mag(v)

def deg2rad(x):
    return x * np.pi / 180

def rad2deg(x):
    return x * 180 / np.pi 

ax = plt.figure().add_subplot(projection='3d')

# Prepare arrays x, y, z
theta = np.linspace(0, np.pi * 2, 100)

p = sphere2comp( deg2rad(0), deg2rad(40) ) # reference axis (polaris)
s = sphere2comp( deg2rad(50), deg2rad(25) ) # starting point on the circle (star target)

print("p: ", p)
print("s: ", s)

d = -(p[0]*s[0] + p[1]*s[1] + p[2]*s[2]) # plane equation constant
#plane is ax + by + cz + d = 0
# print("d: ", d)

#center point, where polaris vector intersects the plane
c = [ -d*p[0] / (p[0]**2 + p[1]**2 + p[2]**2),
      -d*p[1] / (p[0]**2 + p[1]**2 + p[2]**2),
      -d*p[2] / (p[0]**2 + p[1]**2 + p[2]**2) ]

#radius from center point to star target
r = np.sqrt( np.power(c[0] - s[0], 2) + np.power(c[1] - s[1], 2) + np.power(c[2] - s[2], 2)) 
print("r: ", r)
print("c: ", c, mag(c))

# vector along the plane axis, aka normal to p
# a = [ -p[1] / mag(p), p[0] / mag(p), 0] 
a = [ s[0] - c[0], 
      s[1] - c[1], 
      s[2] - c[2] ]
# a = [ s[0] - p[0], s[1] - p[1], s[2] - p[2] ]
a = normalize(a)
print(a[1]*p[2], p[1]*a[2])
# second vector on plane, cross product of a x p
b = [  (a[1]*p[2]) - (p[1]*a[2]),  # y, z
      -((a[0]*p[2]) - (p[0]*a[2])),  # x, z
       (a[0]*p[1]) - (p[0]*a[1]) ] # x, y
b = normalize(b)

print("a: ", a)
print("b: ", b)

print("a . p", dot(a,p))
print("a . b", dot(a,b))
print("p . b", dot(p,b))
# print(codir(p, c))

# parameterization of a circle in 3d space
x = r * ( np.cos(theta) * a[0] + np.sin(theta) * b[0] ) + c[0]
y = r * ( np.cos(theta) * a[1] + np.sin(theta) * b[1] ) + c[1]
z = r * ( np.cos(theta) * a[2] + np.sin(theta) * b[2] ) + c[2]

az = rad2deg(np.arctan2(z, np.sqrt( np.power(x,2) + np.power(y,2) ))) 
be = rad2deg(np.arctan2(y, x))

for i in range(len(theta)):
    print(x[i],y[i],z[i])

startT = 0 # because a vector A is defined as pointing towards S
x0 = r * ( np.cos(startT) * a[0] + np.sin(startT) * b[0] ) + c[0]
y0 = r * ( np.cos(startT) * a[1] + np.sin(startT) * b[1] ) + c[1]
z0 = r * ( np.cos(startT) * a[2] + np.sin(startT) * b[2] ) + c[2]
print([x0,y0,z0])

scaler=[2,2,2]
v = np.array([c,s,np.divide(a,scaler),np.divide(b,scaler)])
origin = np.array([[0,0,c[0],c[0]],
                   [0,0,c[1],c[1]],
                   [0,0,c[2],c[2]]])
ax.quiver(*origin, v[:,0], v[:,1], v[:,2])

ax.plot(x, y, z, label='parametric curve')

x = np.linspace(0, 1, 100)
y = np.linspace(-0.2, 0.2, 100)
xx, yy = np.meshgrid(x, y)
eq =  -(p[0]*xx +  p[1]*yy + d)/p[2] 

ax.plot_surface(xx, yy, eq, alpha=0.2)
ax.plot_surface(xx, yy, np.sqrt(1 - xx**2 - yy**2), alpha=0.2)

# ax.legend()
ax.set_box_aspect((1,1,1))

plt.show()
