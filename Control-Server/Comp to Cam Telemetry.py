import socket
import time
import subprocess
import matplotlib.pyplot as plt
import logging

# Setup logging (newly added)
logging.basicConfig(level=logging.INFO)

# Configuration
arduino_ip = "192.168.1.151"
arduino_port = 8888
data_file = "./send.txt"

def get_device_ip():
    command = "ifconfig | grep 192 |awk '/inet/ {print $2; exit}' "
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if result.returncode == 0:
        return result.stdout.strip()
    else:
        logging.error(f"Command failed with error: {result.stderr}")
        return None

def setup_socket(device_ip):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((device_ip, arduino_port))
    return sock

def send_data(sock, message):
    if message:
        sent = sock.sendto(message.encode(), (arduino_ip, arduino_port))
        return sent
    return None

def receive_data(sock):
    try:
        data, _ = sock.recvfrom(8888)
        return data.decode()
    except socket.timeout:
        logging.warning("Socket timed out")
        return None

def update_plot(ax, depth):
    ax.plot(time.time(), depth, "ro")
    ax.set_xlim(time.time() - 10, time.time())
    ax.set_ylim(-0.3, 2.5)
    plt.draw()
    plt.pause(0.01)

def main():
    device_ip = get_device_ip()
    if not device_ip:
        return

    sock = setup_socket(device_ip)
    last_message = ""

    plt.ion()
    fig, ax = plt.subplots()

    try:
        while True:
            with open(data_file, "r") as f:
                new_message = f.read().strip()

            if new_message != last_message:
                logging.info(f"Sending message: {new_message}")
                send_data(sock, new_message)
                last_message = new_message

            send_data(sock, "xy")
            data = receive_data(sock)

            if data:
                logging.info(f"Received: {data}")
                try:
                    depth = float(data.split("dd")[1].split("tt")[0])
                    update_plot(ax, depth)
                except (IndexError, ValueError) as e:
                    logging.error(f"Failed to parse depth data: {e}")

            time.sleep(0.1)
    except KeyboardInterrupt:
        logging.info("Program interrupted by user")
    finally:
        logging.info("Closing socket")
        sock.close()

if __name__ == "__main__":
    main()
