import math

print('Enter starting lat')
lat1_d = float(input())
lat1 = lat1_d* math.pi/180
print('Enter starting lon')
lon1_d = float(input())
lon1 = lon1_d* math.pi/180
print('Enter final lat')
lat2_d = float(input())
lat2 = lat2_d* math.pi/180
print('Enter final lon')
lon2_d = float(input())
lon2 = lon2_d* math.pi/180
print('Enter every how many meters do you want to recieve a new location')
speed = float(input()) 

r = 6371000
delta_lat = lat2-lat1
delta_lon = lon2-lon1

a = math.sin(delta_lat/2)**2 + math.cos(lat1)*math.cos(lat1)*math.sin(delta_lon/2)**2
c = 2 *math.atan2(math.sqrt(a), math.sqrt(1-a))

d = r*c
print(d)
size = d/speed
print(size)
lat_r = (lat2_d-lat1_d)*(1/size)
lon_r = (lon2_d-lon1_d)*(1/size)

f = open("gpsPoints.txt", "w")
w = str(lat1_d) + " " + str(lon1_d) + "\n"
f.write(w)

for i in range(math.ceil(size)):
    lat1_d = lat1_d + lat_r
    lon1_d = lon1_d + lon_r
    w = str(lat1_d) + " " + str(lon1_d) + "\n"
    f.write(w)

w = str(lat2_d) + " " + str(lon2_d)
f.write(w)
f.close()