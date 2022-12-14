import threading
import time
from serial import Serial
import serial.tools.list_ports
import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext

auto_mode_state = 0
vacuum_state = 0
ink_pressure_state = 0
pump_pressure_state = 0
pump_vacuum_state = 0
add_ink_state = 0
add_makeup_state = 0

#Open Serial
ser = serial.Serial()
def open_serial(port):
    ser.baudrate = 9600
    ser.port = port
    try:
        ser.open()
    except:
        print("<INFO> " + main_frame.currentvar.get() + " not plugged in or already in use...")
        main_frame.console_scrolledtext.insert("end", "<INFO> " + main_frame.currentvar.get() + " not plugged in or already in use...\n")
        main_frame.console_scrolledtext.see("end")

#Read Serial Data Thread        
def read_serial_data():
    while True:
        if ser.is_open == True:
            try:
                if (ser.inWaiting() > 0):
                    reading = ser.readline().decode()
                    main_frame.on_data(reading)
            except:
                ser.close()
                main_frame.connect_button.config(text="Connect")
                print("<INFO> Connection lost...")
                main_frame.console_scrolledtext.insert("end", "<INFO> Connection lost...\n")
                main_frame.console_scrolledtext.see("end")
        time.sleep(0.01)

#Thread
serial_thread = threading.Thread(target=read_serial_data, daemon=True)
serial_thread.start()

#Send Line on Return Key press
def return_pressed(event):
    main_frame.send_line()

#GUI Elements
class MainFrame(tk.Frame):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        
        #Title
        app.title("Continuous Inkjet Printer")
        app.state("zoomed")
        
        # Frame Groups
        self.Top_Group = tk.Frame()
        self.Top_Group.grid(row=0, column=0, pady = 10, padx = 10, sticky="w")
        self.Bottom_Group = tk.Frame()
        self.Bottom_Group.grid(row=1, column=0, pady = 10, padx = 10, sticky="w")
        
        # --Functions--
        # Frames
        self.Functions = tk.Frame(self.Top_Group)
        self.Functions.grid(row=0, column=0, pady = 10, padx = 10, sticky="n")
        #Labels
        self.auto_mode_label = tk.Label(self.Functions, text="Auto Mode")
        self.auto_mode_label.grid(row=0, column=0, sticky="ew", pady = (0, 10))
        self.ink_pressure_label = tk.Label(self.Functions, text="Ink Pressure")
        self.ink_pressure_label.grid(row=1, column=0)
        self.vacuum_label = tk.Label(self.Functions, text="Vacuum")
        self.vacuum_label.grid(row=2, column=0)
        self.pump_pressure_label = tk.Label(self.Functions, text="Pump Pressure")
        self.pump_pressure_label.grid(row=3, column=0)
        self.pump_vacuum_label = tk.Label(self.Functions, text="Pump Vacuum")
        self.pump_vacuum_label.grid(row=4, column=0)
        self.add_ink_label = tk.Label(self.Functions, text="Add Ink")
        self.add_ink_label.grid(row=5, column=0)
        self.add_makeup_label = tk.Label(self.Functions, text="Add MakeUp")
        self.add_makeup_label.grid(row=6, column=0)
        #Buttons
        self.auto_mode_button = tk.Button(self.Functions, text="OFF", command=self.toggle_auto_mode, bg="white")
        self.auto_mode_button.grid(row=0, column=1, sticky="ew", pady = (0, 10))
        self.ink_pressure_button = tk.Button(self.Functions, text="OFF", command=self.toggle_ink_pressure, bg="white")
        self.ink_pressure_button.grid(row=1, column=1, sticky="ew")
        self.vacuum_button = tk.Button(self.Functions, text="OFF", command=self.toggle_vacuum, bg="white")
        self.vacuum_button.grid(row=2, column=1, sticky="ew")
        self.pump_pressure_button = tk.Button(self.Functions, text="OFF", command=self.toggle_pump_pressure, bg="white")
        self.pump_pressure_button.grid(row=3, column=1, sticky="ew")
        self.pump_vacuum_button = tk.Button(self.Functions, text="OFF", command=self.toggle_pump_vacuum, bg="white")
        self.pump_vacuum_button.grid(row=4, column=1, sticky="ew")
        self.add_ink_button = tk.Button(self.Functions, text="OFF", command=self.toggle_add_ink, bg="white")
        self.add_ink_button.grid(row=5, column=1, sticky="ew")
        self.add_makeup_button = tk.Button(self.Functions, text="OFF", command=self.toggle_add_makeup, bg="white")
        self.add_makeup_button.grid(row=6, column=1, sticky="ew")
        
        # --Sensors--
        # Frames
        self.Sensors = tk.Frame(self.Top_Group)
        self.Sensors.grid(row=0, column=1, pady = 10, padx = 10, sticky="n")
        #Labels
        self.fault_label = tk.Label(self.Sensors, text="Fault")
        self.fault_label.grid(row=0, column=0, sticky="ew", pady = (0, 10))
        self.air_pressure_label = tk.Label(self.Sensors, text="Air Pressure Sensor")
        self.air_pressure_label.grid(row=1, column=0)
        self.vacuum_sensor_label = tk.Label(self.Sensors, text="Vacuum Sensor")
        self.vacuum_sensor_label.grid(row=2, column=0)
        self.reservoir_ink_level_label = tk.Label(self.Sensors, text="Reservoir Ink Level")
        self.reservoir_ink_level_label.grid(row=4, column=0)
        self.pump_ink_level_label = tk.Label(self.Sensors, text="Pump Ink Level")
        self.pump_ink_level_label.grid(row=6, column=0)
        self.timer_ink_level_label = tk.Label(self.Sensors, text="Timer Ink Level")
        self.timer_ink_level_label.grid(row=8, column=0)
        #Buttons
        self.fault_button = tk.Button(self.Sensors, text="   ", bg="white")
        self.fault_button.grid(row=0, column=1, sticky="ew", pady = (0, 10))
        self.air_pressure_sensor_button = tk.Button(self.Sensors, text="OFF", bg="white")
        self.air_pressure_sensor_button.grid(row=1, column=1, sticky="ew")
        self.vacuum_sensor_button = tk.Button(self.Sensors, text="OFF", bg="white")
        self.vacuum_sensor_button.grid(row=2, column=1, sticky="ew", pady = (0, 10))
        self.reservoir_ink_level_button_0 = tk.Button(self.Sensors, text="   ", bg="white")
        self.reservoir_ink_level_button_0.grid(row=3, column=1, sticky="ew")
        self.reservoir_ink_level_button_1 = tk.Button(self.Sensors, text="   ", bg="white")
        self.reservoir_ink_level_button_1.grid(row=4, column=1, sticky="ew", pady = (0, 10))
        self.pump_ink_level_button_0 = tk.Button(self.Sensors, text="   ", bg="white")
        self.pump_ink_level_button_0.grid(row=5, column=1, sticky="ew")
        self.pump_ink_level_button_1 = tk.Button(self.Sensors, text="   ", bg="white")
        self.pump_ink_level_button_1.grid(row=6, column=1, sticky="ew", pady = (0, 10))
        self.timer_ink_level_button_0 = tk.Button(self.Sensors, text="   ", bg="white")
        self.timer_ink_level_button_0.grid(row=7, column=1, sticky="ew")
        self.timer_ink_level_button_1 = tk.Button(self.Sensors, text="   ", bg="white")
        self.timer_ink_level_button_1.grid(row=8, column=1, sticky="ew")
        
        # --Ink Time--
        # Frames
        self.Ink_Time = tk.Frame(self.Top_Group)
        self.Ink_Time.grid(row=0, column=3, pady = 10, padx = 10, sticky="n")
        #Labels
        self.ink_time_label = tk.Label(self.Ink_Time, text="Ink Time")
        self.ink_time_label.grid(row=0, column=0, sticky="w")
        #Scrolled Texts
        self.ink_time_scrolledtext = scrolledtext.ScrolledText(self.Ink_Time, wrap = tk.WORD, width = 5, height = 14, font = ("Courier", 12))
        self.ink_time_scrolledtext.grid(row=1, column=0)

        # --Serial Connection--
        #Frames
        self.Serial_Connection = tk.Frame(self.Bottom_Group)
        self.Serial_Connection.grid(row=0, column=0, pady = 10, padx = 10, sticky="n")
        #Labels
        self.select_port_label = tk.Label(self.Serial_Connection, text="Select Port:")
        self.select_port_label.grid(row=0, column=0, pady = 10, padx = 10, sticky="w")
        self.enter_label = tk.Label(self.Serial_Connection, text="Enter:")
        self.enter_label.grid(row=1, column=0, pady = 10, padx = 10, sticky="w")
        self.console_label = tk.Label(self.Serial_Connection, text="Console:")
        self.console_label.grid(row=2, column=0, pady = 10, padx = 10, sticky="nw")
        # Buttons
        self.connect_button = tk.Button(self.Serial_Connection, text="Connect", command=self.connect)
        self.connect_button.grid(row=0, column=2, pady = 10, padx = 10, sticky="we")
        self.refresh_button = tk.Button(self.Serial_Connection, text="Refresh", command=self.find_USB_device)
        self.refresh_button.grid(row=0, column=3, pady = 10, padx = 10, sticky="we")
        self.send_button = tk.Button(self.Serial_Connection, text="Send", command=self.send_line)
        self.send_button.grid(row=1, column=2, pady = 10, padx = 10, sticky="we")
        #Combobox
        self.currentvar = tk.StringVar()
        self.portlist = ttk.Combobox(self.Serial_Connection, width = 27, textvariable = self.currentvar)
        self.portlist.grid(row=0, column=1, pady = 10, padx = 10, sticky="we")
        self.find_USB_device()
        #Entrys
        self.enter_entry = tk.Entry(self.Serial_Connection)
        self.enter_entry.grid(row=1, column=1, pady = 10, padx = 10, sticky="we")
        self.enter_entry.bind('<Return>', return_pressed)
        #Scrolled Texts
        self.console_scrolledtext = scrolledtext.ScrolledText(self.Serial_Connection, wrap = tk.WORD, width = 100, height = 10, font = ("Courier", 12))
        self.console_scrolledtext.grid(row=2, column=1, pady = 10, padx = 10)
        
    def find_USB_device(self):
        self.myports = [tuple(p) for p in list(serial.tools.list_ports.comports())]
        print(self.myports)
        self.items=[p[0] for p in self.myports]
        self.portlist["state"] = "readonly"
        self.portlist["values"] = self.items
        self.portlist.current()
        
    def connect(self):
        if ser.is_open == False:
            print("<INFO> Connecting to " + self.currentvar.get() + "...")
            self.console_scrolledtext.insert("end", "<INFO> Connecting to " + self.currentvar.get() + "...\n")
            open_serial(self.currentvar.get())
            if ser.is_open == True:
                self.connect_button.config(text="Disconnect")
        else:
            ser.close()
            if ser.is_open == False:
                self.connect_button.config(text="Connect")
                print("<INFO> Disconnected")
                self.console_scrolledtext.insert("end", "<INFO> Disconnected...\n")
        self.console_scrolledtext.see("end")
                
    def send_line(self):
        if ser.is_open == True:
            if self.enter_entry.get() != "":
                ser.write(self.enter_entry.get().encode() + b'\n')
                print("<OUT> " + self.enter_entry.get())
                self.console_scrolledtext.insert("end", "<OUT> " + self.enter_entry.get() + "\n")
        else:
            print("<INFO> Not connected to Serial Port")
            self.console_scrolledtext.insert("end", "<INFO> Not connected to Serial Port...\n")
        self.console_scrolledtext.see("end")
        
    def send_command(self, printer_command):
        if ser.is_open == True:
            if printer_command != "":
                ser.write(printer_command.encode() + b'\n')
                print("<OUT> " + printer_command)
                self.console_scrolledtext.insert("end", "<OUT> " + printer_command + "\n")
        else:
            print("<INFO> Not connected to Serial Port")
            self.console_scrolledtext.insert("end", "<INFO> Not connected to Serial Port...\n")
        self.console_scrolledtext.see("end")
                           
    def on_data(self, data):
        global auto_mode_state
        global vacuum_state
        global ink_pressure_state
        global pump_pressure_state
        global pump_vacuum_state
        global add_ink_state
        global add_makeup_state
        
        print("<IN>", data.strip("\n"))
        self.console_scrolledtext.insert("end", "<IN> " + data)
        self.console_scrolledtext.see("end")
        if data.startswith("#") == True:
            state_report_value = int(data.strip("#"))
            
            if state_report_value & (1 << (1 - 1)):
                ink_pressure_state = 1
                self.ink_pressure_button.config(text="ON")
                self.ink_pressure_button.config(bg="green")
            else:
                ink_pressure_state = 0
                self.ink_pressure_button.config(text="OFF")
                self.ink_pressure_button.config(bg="white")
                
            if state_report_value & (1 << (2 - 1)):
                pump_pressure_state = 1
                self.pump_pressure_button.config(text="ON")
                self.pump_pressure_button.config(bg="green")
            else:
                pump_pressure_state = 0
                self.pump_pressure_button.config(text="OFF")
                self.pump_pressure_button.config(bg="white")
                
            if state_report_value & (1 << (3 - 1)):
                pump_vacuum_state = 1
                self.pump_vacuum_button.config(text="ON")
                self.pump_vacuum_button.config(bg="green")
            else:
                pump_vacuum_state = 0
                self.pump_vacuum_button.config(text="OFF")
                self.pump_vacuum_button.config(bg="white")
                
            if state_report_value & (1 << (4 - 1)):
                add_ink_state = 1
                self.add_ink_button.config(text="ON")
                self.add_ink_button.config(bg="green")
            else:
                add_ink_state = 0
                self.add_ink_button.config(text="OFF")
                self.add_ink_button.config(bg="white")
                
            if state_report_value & (1 << (5 - 1)):
                add_makeup_state = 1
                self.add_makeup_button.config(text="ON")
                self.add_makeup_button.config(bg="green")
            else:
                add_makeup_state = 0
                self.add_makeup_button.config(text="OFF")
                self.add_makeup_button.config(bg="white")
                
            if state_report_value & (1 << (6 - 1)):
                self.air_pressure_sensor_button.config(text="ON")
                self.air_pressure_sensor_button.config(bg="green")
            else:
                self.air_pressure_sensor_button.config(text="OFF")
                self.air_pressure_sensor_button.config(bg="white")
                
            if state_report_value & (1 << (7 - 1)):
                self.vacuum_sensor_button.config(text="ON")
                self.vacuum_sensor_button.config(bg="green")
            else:
                self.vacuum_sensor_button.config(text="OFF")
                self.vacuum_sensor_button.config(bg="white")
                
            if state_report_value & (1 << (8 - 1)):
                self.reservoir_ink_level_button_1.config(bg="blue")
            else:
                self.reservoir_ink_level_button_1.config(bg="white")
                
            if state_report_value & (1 << (9 - 1)):
                self.reservoir_ink_level_button_0.config(bg="blue")
            else:
                self.reservoir_ink_level_button_0.config(bg="white")
                
            if state_report_value & (1 << (10 - 1)):
                self.pump_ink_level_button_1.config(bg="blue")
            else:
                self.pump_ink_level_button_1.config(bg="white")
                
            if state_report_value & (1 << (11 - 1)):
                self.pump_ink_level_button_0.config(bg="blue")
            else:
                self.pump_ink_level_button_0.config(bg="white")
                       
            if state_report_value & (1 << (12 - 1)):
                self.timer_ink_level_button_1.config(bg="blue")
            else:
                self.timer_ink_level_button_1.config(bg="white")
                
            if state_report_value & (1 << (13 - 1)):
                self.timer_ink_level_button_0.config(bg="blue")
            else:
                self.timer_ink_level_button_0.config(bg="white")
                
            if state_report_value & (1 << (14 - 1)):
                self.fault_button.config(bg="red")
            else:
                self.fault_button.config(bg="white")
                
            if state_report_value & (1 << (15 - 1)):
                vacuum_state = 1
                self.vacuum_button.config(text="ON")
                self.vacuum_button.config(bg="green")
            else:
                vacuum_state = 0
                self.vacuum_button.config(text="OFF")
                self.vacuum_button.config(bg="white")
                
            if state_report_value & (1 << (16 - 1)):
                auto_mode_state = 1
                self.auto_mode_button.config(text="ON")
                self.auto_mode_button.config(bg="yellow")
            else:
                auto_mode_state = 0
                self.auto_mode_button.config(text="OFF")
                self.auto_mode_button.config(bg="white")
        if data.startswith("$") == True:
            ink_time = data.strip("$")
            self.ink_time_scrolledtext.insert("end", ink_time.strip("\n") + "s\n")
            self.ink_time_scrolledtext.see("end")

    def toggle_ink_pressure(self):
        if ink_pressure_state == 1:
            self.send_command("F002")
        else:
            self.send_command("F001")

    def toggle_pump_pressure(self):
        if pump_pressure_state == 1:
            self.send_command("F004")
        else:
            self.send_command("F003")

    def toggle_pump_vacuum(self):
        if pump_vacuum_state == 1:
            self.send_command("F006")
        else:
            self.send_command("F005")

    def toggle_add_ink(self):
        if add_ink_state == 1:
            self.send_command("F008")
        else:
            self.send_command("F007")
            
    def toggle_add_makeup(self):
        if add_makeup_state == 1:
            self.send_command("F010")
        else:
            self.send_command("F009")
            
    def toggle_vacuum(self):
        if vacuum_state == 1:
            self.send_command("F012")
        else:
            self.send_command("F011")
            
    def toggle_auto_mode(self):
        if auto_mode_state == 1:
            self.send_command("F014")
        else:
            self.send_command("F013")

app = tk.Tk()  
main_frame = MainFrame()
app.mainloop()
