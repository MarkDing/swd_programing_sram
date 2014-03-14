import sys

# input_file = "sim3u1xx_USBHID_ram.bin"
input_file ="sim3u1xx_Blinky.bin"
output_file = "bin_array.h"
ifile = open(input_file, mode = 'rb')
binraw = ifile.read()
word_array = []
for i in range(0, len(binraw), 4):
	if (sys.version_info < (3, 0, 0)):
		b0 = ord(binraw[i])
		b1 = ord(binraw[i + 1])
		b2 = ord(binraw[i + 2])
		b3 = ord(binraw[i + 3])
	else:
		b0 = (binraw[i])
		b1 = (binraw[i + 1])
		b2 = (binraw[i + 2])
		b3 = (binraw[i + 3])
	tmp = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)
	word_array.append(hex(tmp))

# print(word_array)
ifile.close()

ofile = open(output_file, mode = 'w')
ofile.write("U32 code binraw[] = {\n")
cnt = 0;
for i in range(0, len(word_array)):
	ofile.write(word_array[i] + ',')
	cnt = cnt + 1
	if (cnt > 6):
		cnt = 0
		ofile.write("\n")
ofile.write("\n};")
ofile.close()
