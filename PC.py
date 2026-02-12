
import matplotlib.pyplot as plt
import serial
from serial.tools import list_ports
import numpy as np
import time

import matplotlib.pyplot as plt

def plot_dynamic(*data_lists, block=True):


    n = len(data_lists)
    if n == 0:
        print("警告：没有传入任何数据列表！")
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
    """
    枚举系统中所有可用的串口，返回串口信息列表
    返回格式：[(端口名称, 设备描述, 硬件ID), ...]
    """
    available_ports = []
    # 枚举所有可用串口
    ports = list_ports.comports()
    for port in ports:
        # port.device：串口名称（如COM3、/dev/ttyUSB0）
        # port.description：设备描述（如USB-SERIAL CH340 (COM3)）
        # port.hwid：硬件ID（包含设备VID/PID，用于精准匹配）
        available_ports.append((port.device, port.description, port.hwid))
    return available_ports

def select_serial_port_by_user_input():
    """
    枚举可用串口，接收用户输入，匹配目标串口
    """
    # 1. 获取所有可用串口
    available_ports = get_available_serial_ports()
    
    if not available_ports:
        print("未检测到任何可用串口，请检查设备连接！")
        return None
    
    # 2. 打印可用串口列表（供用户参考选择）
    print("=" * 50)
    print("检测到以下可用串口：")
    for index, (device, desc, hwid) in enumerate(available_ports, start=1):
        print(f"{index}. 串口名称：{device}")
        print(f"   设备描述：{desc}")
        print(f"   硬件ID：{hwid}\n")
    print("=" * 50)
    
    # 3. 接收用户输入（支持两种输入方式：编号 / 串口关键字）
    user_input = input("请输入串口编号（如1）或串口关键字（如COM、USB、ttyUSB）：").strip()
    
    # 4. 匹配目标串口
    target_port = None
    # 方式1：匹配输入的编号
    if user_input.isdigit():
        input_index = int(user_input)
        if 1 <= input_index <= len(available_ports):
            target_port = available_ports[input_index - 1][0]  # 提取串口名称
    # 方式2：匹配关键字（模糊查询，匹配串口名称/设备描述）
    else:
        for device, desc, hwid in available_ports:
            if user_input.lower() in device.lower() or user_input.lower() in desc.lower():
                target_port = device
                break
    
    # 5. 验证匹配结果
    if target_port:
        print(f"foud serial ports:{target_port}")
        return target_port
    else:
        print("输入无效，未匹配到对应的串口！")
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
    with open('record.txt', 'w',encoding='utf-8')as f:
        for item in valid_hex_list:
            f.write(f"{item}\n")
    for hex_item in valid_hex_list:
        try:
            int_value = int(hex_item,10)
            int_data_list.append((int_value-1856)/3129)
        except ValueError:
            print(f"invalid hex data: {hex_item}")
            continue
        
    # filted_data= combined_filter(int_data_list)
    # return remove_spike_noise(filted_data, 4, 2)
    return int_data_list
def plot_single(data_list):
    plt.figure(figsize=(12, 6))
    
    # 绘制折线图（适合展示数据变化趋势）
    plt.plot(data_list, color='darkblue', linewidth=1.5, marker='.', markersize=4, label='Hex to Dec Data')
    
    # 绘制柱状图（可选，适合对比单个数据大小，注释掉折线图即可启用）
    # plt.bar(range(len(int_data_list)), int_data_list, color='lightblue', alpha=0.7, label='Hex to Dec Data')
    
    # 添加图表标注，提升可读性
    plt.title('Data from Hex String (0xXX\\r\\n0xXX...)', fontsize=14)
    plt.xlabel('Data Index', fontsize=12)
    plt.ylabel('Decimal Value', fontsize=12)
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.legend(loc='best')
     
    # 显示图表
    plt.show()
def hex_str_to_plot(serial_str):
    data_list = []
    data_list = get_list(serial_str)
    plot_single(data_list)
    return 
def plot_3d_from_dec_lists(all_dec_lists):

    # 1. 严格校验输入参数有效性
    if not isinstance(all_dec_lists, list) or len(all_dec_lists) == 0:
        raise ValueError("all_dec_lists 必须是非空的二维列表！")
    for sub_list in all_dec_lists:
        if not isinstance(sub_list, list):
            raise ValueError("all_dec_lists 的每个元素必须是列表！")

    # 2. 固定初始化所有配置（函数内部固定）
    title = '多个列表的3D折线图可视化'
    x_label = 'X (列表内元素索引)'
    y_label = 'Y (列表编号)'
    z_label = 'Z (数值)'
    figsize = (10, 7)

    # 3. 创建3D画布
    fig = plt.figure(figsize=figsize)
    ax = fig.add_subplot(111, projection='3d')

    # 4. 绘制3D折线图（每个列表对应一条3D折线）
    for y_idx, dec_list in enumerate(all_dec_lists):
        # 构造当前列表的X、Y、Z坐标
        x_coords = np.arange(len(dec_list))  # X轴：元素索引
        y_coords = np.full_like(x_coords, y_idx)  # Y轴：固定为当前列表编号
        z_coords = np.array(dec_list)  # Z轴：列表的十进制数值

        # 为每条线分配不同颜色，添加标记点增强可读性
        color = "blue"
        ax.plot(x_coords, y_coords, z_coords,
                color=color, linewidth=2, marker='o', markersize=4,
                label=f'列表{y_idx+1}')

    # 5. 固定设置图像样式
    ax.set_title(title, fontsize=12)
    ax.set_xlabel(x_label, fontsize=10)
    ax.set_ylabel(y_label, fontsize=10)
    ax.set_zlabel(z_label, fontsize=10)
    ax.legend(loc='best')  # 显示每条线的图例

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
                send_data = input("command to send.：") + "\n"
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
    # 步骤1：根据用户输入选择串口
    target_serial_port = select_serial_port_by_user_input()
    
    # 步骤2：使用匹配到的串口进行通信
    serial_communication_demo(target_serial_port)