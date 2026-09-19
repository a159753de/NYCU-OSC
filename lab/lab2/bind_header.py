import struct
import time
import sys
import serial

# 1. 讀取 Kernel 檔案
with open('new_kernel', 'rb') as f:
    kernel_data = f.read()
    print(f"Host: Kernel size: {len(kernel_data)} bytes")

# 2. 產生 Header
header = struct.pack('<II', 0x544F4F42, len(kernel_data))

# 3. 開啟序列埠
tty = serial.Serial('/dev/pts/8', baudrate=115200, rtscts=False, xonxoff=False, dsrdtr=False)
print("Host: Opened /dev/pts/8 in RAW mode.")

print("Host: Sending header...")
tty.write(header)
tty.flush()
time.sleep(0.5) # 給 Bootloader 充足的時間印出 "Expected Kernel Size"

print("Host: Sending kernel data...")
# 這次我們每次只送 32 bytes，每送一次停 5 毫秒 (極度保守的速度)
chunk_size = 32
for i in range(0, len(kernel_data), chunk_size):
    chunk = kernel_data[i : i + chunk_size]
    tty.write(chunk)
    tty.flush()
    time.sleep(0.005)

print("Host: Finished sending!")
print("Host: [CRITICAL] Holding channel open. DO NOT close this terminal!")

# 關鍵：為了不讓通道關閉導致 QEMU 斷流，我們讓 Python 在這裡死等，且「絕對不去讀取」任何資料
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    tty.close()
    print("Closed.")