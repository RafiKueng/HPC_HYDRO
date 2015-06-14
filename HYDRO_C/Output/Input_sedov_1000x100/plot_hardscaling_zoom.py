"""
This script produces a t vs N plot for the hard scaling.
"""
#import matplotlib as mpl; mpl.rcParams['font.family'] = 'serif'
from matplotlib import *
rcParams['font.family'] = 'serif'
from matplotlib.patches import *
from numpy import *
from matplotlib.pyplot import *
from sys import *

def ratio_s_v(np, nx, ny):
	return 4.*ny/(nx/np)/(ny+4)


if len(argv) != 2:
		print "Usage: plot_hardscaling.py data.txt"
		exit(1)

datafile = argv[1]

#print "Open file: " + tipsyfile
# Load data file
data = loadtxt(datafile)

N = data[:,0]
t = data[:,1]

title(r'Strong scaling for a 1000 x 100 grid', fontsize = 20)
xlabel('Number of cores', fontsize = 20)
ylabel('Time [s]', fontsize = 20)


print N
print ratio_s_v(N,1000,100)

plot(N[:11],t[:11],'-',color='red',linewidth=2,label='time measurements')
#plot(N, ratio_s_v(N, 1000, 100), '-', color = 'blue', linewidth=2, label=r'$n_{comm}/n_{comp}$')
vlines(8,0,4000,color='green',linestyles='dashed', label='8 cores in a socket')
vlines(16,0,4000,color='blue',linestyles='dashed', label='16 cores in a node (two sockets)')
ylim(0, 4000)




#fill_between(rhocold,ucold,color='orange')

legend()

savefig(datafile+'_zoom.png')
close()

print "Done."
