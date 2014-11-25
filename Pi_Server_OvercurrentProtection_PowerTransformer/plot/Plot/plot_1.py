import numpy as np
import matplotlib.pyplot as plt
from matplotlib.transforms import Bbox
from matplotlib.path import Path
from scipy.misc import imread
import socket
import sys

x_width=1.5
x_width_less=1
x_right_start=1
x_start=-2.5
x_go_down=0.25
linewidth=5 #the gray ones
linewidth_2=10 #the one we are checking
img0 = imread('att.png')
img1 = imread('computer.jpg')
img2 = imread('Power-Transformer.jpg')
img3 = imread('computer.jpg')
img4 = imread('generator.jpg')

color_green="#00ff00"
color_gray="#aaaaaa"
color_red="#ff0000"
color_waiting="#bbbbbb"

textmarker={}
rect1={}
rect2={}
vertices2={}
attimg={}
def paintme():
    global textmarker, rect1 ,rect2 ,vertices2, attimg
    plt.figure(facecolor='white') #background white
    #plt.title('Test')

    textmarker=plt.text(0,1.65,'', fontsize=24) #random object
    rect1 = plt.Rectangle(( x_start, -0.0), x_width, 0.5, facecolor=color_waiting)
    plt.gca().add_patch(rect1)
    rect2 = plt.Rectangle(( x_right_start, -0.0), x_width-x_width_less, 0.5, facecolor=color_waiting)
    plt.gca().add_patch(rect2)
    vertices1= np.array([[x_start+x_width+linewidth_2*0.005 ,+0.25],[x_right_start-linewidth_2*0.005, +0.25]])
    vertices2=plt.plot(vertices1[:,0], vertices1[:,1], color=color_waiting, lw=linewidth_2)
    
    rect = plt.Rectangle(( x_start, -0.5), x_width, 0.5, facecolor=color_gray)#bottom left coordinate, size x, size y, edgecolor="red", lw='2'
    plt.gca().add_patch(rect)
    rect = plt.Rectangle(( x_start, -1), x_width, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)

    rect = plt.Rectangle(( x_start, -3), x_width, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)
    rect = plt.Rectangle(( x_start, -3.5), x_width, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)

    rect = plt.Rectangle(( x_right_start, -0.5), x_width-x_width_less, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)
    rect = plt.Rectangle(( x_right_start, -1), x_width-x_width_less, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)

    rect = plt.Rectangle(( x_right_start, -3.5), x_width-x_width_less, 0.5, facecolor=color_gray)
    plt.gca().add_patch(rect)

    vertices= np.array([[x_start+x_width+linewidth*0.005 ,-2.75],[x_start+x_width+x_go_down-linewidth*0.005, -2.75]])
    plt.plot(vertices[:,0], vertices[:,1], color=color_gray, lw=linewidth)
    vertices= np.array([[x_start+x_width+x_go_down ,-0.75],[x_start+x_width+x_go_down, -2.75]])
    plt.plot(vertices[:,0], vertices[:,1], color=color_gray, lw=linewidth)
    vertices= np.array([[x_start+x_width++x_go_down+linewidth*0.005 ,-0.75],[x_right_start-linewidth*0.005, -0.75]])
    plt.plot(vertices[:,0], vertices[:,1], color=color_gray, lw=linewidth)

    vertices= np.array([[x_start+x_width+linewidth*0.005 ,-3.25],[x_right_start-linewidth*0.005, -3.25]])
    plt.plot(vertices[:,0], vertices[:,1], color=color_gray, lw=linewidth)

    vertices= np.array([[x_start+x_width+linewidth*0.005 ,-0.25],[x_right_start-linewidth*0.005, -0.25]])
    plt.plot(vertices[:,0], vertices[:,1], color=color_gray, lw=linewidth)

    plt.axis('off')
    plt.xlim(-3, 3)
    plt.ylim(-3.51, 2)

    mng = plt.get_current_fig_manager()
    ### works on Ubuntu??? >> did NOT working on windows
    # mng.resize(*mng.window.maxsize())
    mng.window.state('zoomed') #works fine on Windows!

    plt.text(x_start+0.25,1.65,'Master 1', fontsize=24)
    plt.imshow(img1,zorder=0, extent=[x_start, x_start+x_width, 0.5, 1.5]) #x1,x2,y1,y2
    plt.text(x_right_start+0.25,1.65,'Slave 1', fontsize=24)
    plt.imshow(img2,zorder=0, extent=[x_right_start,x_right_start+x_width, 0.5, 1.5]) #x1,x2,y1,y2
    plt.text(x_start+0.25,-1.4,'Master 2', fontsize=24)
    plt.imshow(img3,zorder=0, extent=[x_start, x_start+x_width, -2.5, -1.5]) #x1,x2,y1,y2
    plt.text(x_right_start+0.25,-1.9,'Slave 2', fontsize=24)
    plt.imshow(img4,zorder=0, extent=[x_right_start,x_right_start+x_width, -3, -2]) #x1,x2,y1,y2

    plt.text(-0.6,-3.4,'Industrial Protocol', fontsize=16, color=color_gray)
    plt.text(-0.6,+0.05,'ModbusE', fontsize=16,color=color_gray)
    plt.text(-0.6,-0.45,'Industrial Protocol', fontsize=16,color=color_gray)
    plt.text(-0.6,-0.95,'Industrial Protocol', fontsize=16,color=color_gray) 
    #Slave1
    plt.text(x_right_start+0.1,-0.31,'87     192.168.0.10', fontsize=22)
    plt.text(x_right_start+0.1,+0.19,'50P   192.168.0.10', fontsize=22)
    plt.text(x_right_start+0.1,-0.81,'35P   192.168.0.10', fontsize=22)
    #Master1
    plt.text(x_start+0.1,-0.31,'168.0.0.1 | 87', fontsize=22)
    plt.text(x_start+0.1,+0.19,'168.0.0.1 | 50P', fontsize=22)
    plt.text(x_start+0.1,-0.81,'168.0.0.1 | 35P', fontsize=22)
    #Slave2
    plt.text(x_right_start+0.1,-3.34,'35P   192.168.0.11', fontsize=22)
    #Master2
    plt.text(x_start+0.1,-2.81,'168.0.0.5 | 35P', fontsize=22)
    plt.text(x_start+0.1,-3.31,'168.0.0.5 | 35P', fontsize=22)
    attimg=plt.imshow(img0,zorder=0, extent=[0, 0, 0, 0]) #x1,x2,y1,y2
    plt.show(block=False)
def write_red(textme):
    global textmarker, rect1 ,rect2 ,vertices2, attimg
    textmarker.remove()
    rect1.remove()
    rect2.remove()
    vertices2.pop(0).remove()
    attimg.remove()
    rect1 = plt.Rectangle(( x_start, -0.0), x_width, 0.5, facecolor=color_red)
    plt.gca().add_patch(rect1)
    rect2 = plt.Rectangle(( x_right_start, -0.0), x_width-x_width_less, 0.5, facecolor=color_red)
    plt.gca().add_patch(rect2)
    vertices1= np.array([[x_start+x_width+linewidth_2*0.005 ,+0.25],[x_right_start-linewidth_2*0.005, +0.25]])
    vertices2=plt.plot(vertices1[:,0], vertices1[:,1], color=color_red, lw=linewidth_2)
    #textmarker=plt.text(+0.05,+0.68,'Attack', fontsize=24,color=color_red)
    textmarker=plt.text(-0.25,+0.68,textme, fontsize=24,color=color_red)
    attimg=plt.imshow(img0,zorder=0, extent=[-0.8, -0.3, 0.5, 1]) #x1,x2,y1,y2
    plt.draw()

def write_green():
    global textmarker, rect1 ,rect2 ,vertices2, attimg
    textmarker.remove()
    rect1.remove()
    rect2.remove()
    vertices2.pop(0).remove()
    attimg.remove()
    attimg=plt.imshow(img0,zorder=0, extent=[0, 0, 0, 0]) #x1,x2,y1,y2
    rect1 = plt.Rectangle(( x_start, -0.0), x_width, 0.5, facecolor=color_green)
    plt.gca().add_patch(rect1)
    rect2 = plt.Rectangle(( x_right_start, -0.0), x_width-x_width_less, 0.5, facecolor=color_green)
    plt.gca().add_patch(rect2)
    vertices= np.array([[x_start+x_width+linewidth_2*0.005 ,+0.25],[x_right_start-linewidth_2*0.005, +0.25]])
    vertices2=plt.plot(vertices[:,0], vertices[:,1], color=color_green, lw=linewidth_2)
    textmarker=plt.text(0,1.65,'', fontsize=24,color=color_green)
    plt.draw()
def close_plot():
    plt.close()




'''
HOST = ''   # Symbolic name meaning all available interfaces
PORT = 8888 # Arbitrary non-privileged port

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'Socket created'
#Bind socket to local host and port
try:
    s1.bind((HOST, PORT))
except socket.error as msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
     
print 'Socket bind complete'
 
#Start listening on socket
s1.listen(10)
print 'Socket now listening'
conn1, addr = s1.accept()

while True:
    data=conn1.recv(512)
    print data
    if data=='write_red':
        write_red()
        conn1.send('ok')
    elif (data=='write_green'):
        write_green()
        conn1.send('ok')
    else:
        print 'Unknown command: ', data
        break

raw_input("Press Enter to continue...")
write_red()
raw_input("Press Enter to continue...")
write_green()
raw_input("Press Enter to continue...")
write_red()
raw_input("Press Enter to continue...")
plt.close()
'''
