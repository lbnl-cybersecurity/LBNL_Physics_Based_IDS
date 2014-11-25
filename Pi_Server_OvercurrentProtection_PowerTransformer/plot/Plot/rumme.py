
import plot_1
'''
import threading
def plot_me():
    

t = threading.Thread(target=plot_me, args = ())
t.daemon = True
t.start()

'''
#Note somewhat this only runs on cmdline but not in the IDLE shell.
plot_1.paintme() #draw the main function

raw_input("Press Enter to continue...")
plot_1.write_green() #all ok
raw_input("Press Enter to continue...")
plot_1.write_red('Attack: Rule 1') #att
raw_input("Press Enter to continue...")
plot_1.write_green() # all ok
raw_input("Press Enter to continue...")
plot_1.write_red('Attack: Rule 2') #att
raw_input("Press Enter to continue...")
plot_1.close_plot() #finally close the plot

