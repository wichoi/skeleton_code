
import re

log_file = open("20200624-1.log", 'r')

pre_str = 'DEV\[ 4\] receive temperature in '
p_temp = '\(.{1,3}[.].{1,4}\/.{1,3}\)'      # (11.11/1bb)
p_time = '\d\d:\d\d:\d\d'                   # 12:30:10

while True:
    line = log_file.readline()
    if not line: 
        break
    match = re.search(pre_str, line)
    if match:
        print(line, end="")
        time = re.search(p_time, line).group(0)
        print('time:' + time)

        temp = re.search(p_temp, line).group(0)
        print('temp:' + temp)

log_file.close()


#pattern = "([\w\.-]+)@([\w\.-]+)(\.[\w\.]+)"
#str = "Please contact info@sololearn.com for assistance"
#match = re.search(pattern, str)
#if match:
#    m = match.group()
#    print(m)
