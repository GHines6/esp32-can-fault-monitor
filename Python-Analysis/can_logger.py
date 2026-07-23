import serial
import time
import csv

port = "COM4"
baudrate = 115200
connection = serial.Serial(port, baudrate)

accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z = None, None, None, None, None, None
samples = []

csv_filename = "imu_log.csv"
fieldnames = ["time_s", "accel_x", "accel_y", "accel_z", "gyro_x", "gyro_y", "gyro_z",]
csv_file = open(csv_filename, "w", newline="")
writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
writer.writeheader()

start_time = time.time()

while True:
    raw_line = connection.readline()
    text_line = raw_line.decode("utf-8").strip()
    if text_line.startswith("Accel X:"):
        parts = text_line.split()
        accel_x = float(parts[2])
        print(f"Accel X: {accel_x}")

    elif text_line.startswith("Accel Y:"):
        parts = text_line.split()
        accel_y = float(parts[2])
        print(f"Accel Y: {accel_y}")

    elif text_line.startswith("Accel Z:"):
        parts = text_line.split()
        accel_z = float(parts[2])
        print(f"Accel Z: {accel_z}")

    elif text_line.startswith("Gyro X:"):
        parts = text_line.split()
        gyro_x = float(parts[2])
        print(f"Gyro X: {gyro_x}")

    elif text_line.startswith("Gyro Y:"):
        parts = text_line.split()
        gyro_y = float(parts[2])
        print(f"Gyro Y: {gyro_y}")

    elif text_line.startswith("Gyro Z:"):
        parts = text_line.split()
        gyro_z = float(parts[2])
        print(f"Gyro Z: {gyro_z}")

        if all(value is not None for value in (accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z)):
            sample = {"time_s": time.time() - start_time, "accel_x": accel_x, "accel_y": accel_y, "accel_z": accel_z, "gyro_x": gyro_x, "gyro_y": gyro_y, "gyro_z": gyro_z}
            samples.append(sample)
            writer.writerow(sample)
            csv_file.flush()
            print(sample)
            accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z = None, None, None, None, None, None
