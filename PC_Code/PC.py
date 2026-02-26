
import matplotlib.pyplot as plt
import serial
from serial.tools import list_ports
import numpy as np
import time

import matplotlib.pyplot as plt

def plot_dynamic(*data_lists, block=True):


    n = len(data_lists)
    if n == 0:
        return
    
    cols = 2  
    rows = (n + cols - 1) // cols  
    

    fig, axes = plt.subplots(rows, cols, figsize=(cols*4, rows*3), squeeze=False)

    fig.suptitle('Data from Hex String (0xXX\\r\\n0xXX...)', fontsize=16, y=0.98)
    

    titles=['X Data', 'Y Data', 'Z Data', 'U Data']
    for idx, data in enumerate(data_lists):

        row = idx // cols
        col = idx % cols
        ax = axes[row, col]
        
   
        ax.plot(data, color='darkblue', linewidth=1.5, marker='.', markersize=4)

        ax.set_title(titles[idx], fontsize=12)
        ax.set_xlabel('Data Index', fontsize=10)
        ax.set_ylabel('Decimal Value', fontsize=10)

        ax.grid(True, alpha=0.3, linestyle='--')
    

    for idx in range(n, rows*cols):
        row = idx // cols
        col = idx % cols
        fig.delaxes(axes[row, col])
    

    plt.tight_layout(rect=[0, 0, 1, 0.95]) 

    plt.show(block=block)

    if not block:
        plt.pause(0.1)


def get_available_serial_ports():

    available_ports = []
    ports = list_ports.comports()
    for port in ports:
        available_ports.append((port.device, port.description, port.hwid))
    return available_ports

def select_serial_port_by_user_input():

 
    available_ports = get_available_serial_ports()
    
    if not available_ports:
        print("Does't have any available serial ports!")
        return None
    
    print("=" * 50)
    print("Available Serial Ports:")
    for index, (device, desc, hwid) in enumerate(available_ports, start=1):
        print(f"{index}. serial name:{device}")
        print(f"   descript:{desc}")
        print(f"   ID:{hwid}\n")
    print("=" * 50)
    
    user_input = input("Please select a serial port (by number or name):").strip()
    
 
    target_port = None
   
    if user_input.isdigit():
        input_index = int(user_input)
        if 1 <= input_index <= len(available_ports):
            target_port = available_ports[input_index - 1][0]  
   
    else:
        for device, desc, hwid in available_ports:
            if user_input.lower() in device.lower() or user_input.lower() in desc.lower():
                target_port = device
                break
    

    if target_port:
        print(f"foud serial ports:{target_port}")
        return target_port
    else:
        print("invalid input, no matching serial port found.")
        return None
    
def get_list(serial_str):
    hex_str_list = serial_str.replace("\r","").split("\n")
    valid_hex_list =[]

    for item in hex_str_list:
        clean_item = item.strip()
        if clean_item :
            valid_hex_list.append(clean_item)
    if not valid_hex_list:
        print("doesn't have valid hex data")
        return
    
    int_data_list=[]

    for hex_item in valid_hex_list:
        try:
            int_value = int(hex_item,10)
            int_data_list.append((int_value-1856)/3129)
        except ValueError:
            print(f"invalid hex data: {hex_item}")
            continue
        

    return int_data_list
def plot_single(data_list):
    plt.figure(figsize=(12, 6))
    
    plt.plot(data_list, color='darkblue', linewidth=1.5, marker='.', markersize=4, label='Hex to Dec Data')
    

    # plt.bar(range(len(int_data_list)), int_data_list, color='lightblue', alpha=0.7, label='Hex to Dec Data')
    

    plt.title('Data from Hex String (0xXX\\r\\n0xXX...)', fontsize=14)
    plt.xlabel('Data Index', fontsize=12)
    plt.ylabel('Decimal Value', fontsize=12)
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(loc='best')
     

    plt.show()
def hex_str_to_plot(serial_str):
    data_list = []
    data_list = get_list(serial_str)
    plot_single(data_list)
    return 
def plot_3d_from_dec_lists(all_dec_lists):

    if not isinstance(all_dec_lists, list) or len(all_dec_lists) == 0:
        raise ValueError("all_dec_lists error")
    for sub_list in all_dec_lists:
        if not isinstance(sub_list, list):
            raise ValueError("all_dec_lists should be a list of lists")


    title = '3D Line Chart Visualization for Multiple Lists'
    x_label = 'X (Index of elements in the list)'
    y_label = 'Y (List Number)'
    z_label = 'Z (numerical value)'
    figsize = (10, 7)


    fig = plt.figure(figsize=figsize)
    ax = fig.add_subplot(111, projection='3d')


    for y_idx, dec_list in enumerate(all_dec_lists):
  
        x_coords = np.arange(len(dec_list))  
        y_coords = np.full_like(x_coords, y_idx)  
        z_coords = np.array(dec_list) 


        color = "blue"
        ax.plot(x_coords, y_coords, z_coords,
                color=color, linewidth=2, marker='o', markersize=4,
                label=f'list{y_idx+1}')


    ax.set_title(title, fontsize=12)
    ax.set_xlabel(x_label, fontsize=10)
    ax.set_ylabel(y_label, fontsize=10)
    ax.set_zlabel(z_label, fontsize=10)
    ax.legend(loc='best')  

    plt.tight_layout()
    plt.show()

def scan(POT,STEP,SER,PORT):
    data_lists=[]
    one_step= 1/STEP
    for i in range(1,STEP):
        send_data="BREAK\n"
        SER.write(send_data.encode('ascii'))
        time.sleep(0.1)
        send_data = "POT"+POT+"\n"
        print("POT="+send_data)
        SER.write(send_data.encode('ascii'))
        time.sleep(0.1)
        send_data = "VAL\n"
        SER.write(send_data.encode('ascii'))
        time.sleep(0.1)
        send_data = str(one_step *i)
        print(send_data+"\n")
        SER.write(send_data.encode('ascii'))
        time.sleep(0.1)
        send_data = "BREAK\n"
        SER.write(send_data.encode('ascii'))
        time.sleep(0.1)
        send_data = "READ"+PORT+"\n"
        SER.write(send_data.encode('ascii'))
        time.sleep(1)
        if SER.in_waiting > 0:
            read_data = SER.read_all()        
            data_lists.append(get_list(read_data.decode('utf-8').strip()))
    plot_3d_from_dec_lists(data_lists)
    return 
def serial_communication_demo(target_port):

    if not target_port:
        return
    
    serial_config = {
        "port": target_port,
        "baudrate": 250000,
        "bytesize": serial.EIGHTBITS,
        "parity": serial.PARITY_NONE,
        "stopbits": serial.STOPBITS_ONE,
        "timeout": 10
    }
    while True:
        try:
            with serial.Serial(**serial_config) as ser:
                read_sign = 0x0000
                read_count=0
                data_lists_x=[]
                data_lists_y=[]
                data_lists_z=[]
                data_lists_u=[]
                send_data = input("command to send:") + "\n"
                if "SCAN" in send_data:
                    channel = input("Which port do you want to read??")
                    POT = input("Which POT needs to be operated??") 
                    step= int(input("What is the step length?"))
                    scan(POT,step,ser,channel)
                
                elif "READ" in send_data:
                    if "X" in send_data:
                        ser.write("READX\n".encode('ascii'))
                        time.sleep(1)
                        if ser.in_waiting > 0:
                            read_data = ser.read_all()
                        data_lists_x = get_list(read_data.decode('utf-8').strip())
                        read_count +=1
                    if "Y" in send_data:
                        ser.write("READY\n".encode('ascii'))
                        time.sleep(1)
                        if ser.in_waiting > 0:
                            read_data = ser.read_all()
                        data_lists_y = get_list(read_data.decode('utf-8').strip())
                        read_count +=1
                    if "Z" in send_data:
                        ser.write("READZ\n".encode('ascii'))
                        time.sleep(1)
                        if ser.in_waiting > 0:
                            read_data = ser.read_all()
                        data_lists_z = get_list(read_data.decode('utf-8').strip())
                        read_count +=1
                    if "U" in send_data:
                        ser.write("READU\n".encode('ascii'))
                        time.sleep(1)
                        if ser.in_waiting > 0:
                            read_data = ser.read_all()
                        data_lists_u = get_list(read_data.decode('utf-8').strip())
                        read_sign &= 0x0008
                        read_count +=1
                    if read_count>0:
                        plot_dynamic(data_lists_x, data_lists_y, data_lists_z, data_lists_u)
                    else:
                        print("doesn't have response")
                else:
                    ser.write(send_data.encode('ascii'))
                    
        except serial.SerialException as e:
            print(f"Serial process failed{e}")
        except Exception as e:
            print(f"unknow error{e}")




if __name__ == "__main__":
   
    target_serial_port = select_serial_port_by_user_input()
    

    serial_communication_demo(target_serial_port)
