import serial

class Robot:
    ser = serial.Serial
    
    # receive
    front_left_rot = 0.0
    front_right_rot = 0.0
    back_left_rot = 0.0
    back_right_rot = 0.0
    
    turret_angle = 0.0
    
    pitch = 0.0
    roll = 0.0
    yaw = 0.0
    
    pixel_offset = 0
    
    rx_string = ""
        
    
    # transmit    
    front_left_power = 0
    front_right_power = 0
    back_left_power = 0
    back_right_power = 0
    
    turret_power = 0
    enabled = False
    led = "Rainbow" # "Purple" or "Yellow"
    led_int = 0
    flash = False
    
    # frontL,frontR,backL,backR,turret,enabled(as binary),led(as 0-2),flash(as binary)
    tx_string = "{:d},{:d},{:d},{:d},{:d},{:b},{:d},{:b}"
    
    
    # starts up serial port
    # likely use will be ("/dev/rfcomm0, 115200")
    # baud rate may change
    # returns 1 if it cannot open the port, 0 otherwise
    def __init__(self, dev, baud):
        try: 
            self.ser = serial.Serial(dev, baud)

        except serial.SerialException:
            return 1 # port cannot be opened
        else:
            return 0
    
    
    
    # internal method, dw abt it
    def led_set(self):
        if(self.led == "Rainbow"):
            self.led_int = 0
        elif(self.led == "Purple"):
            self.led_int = 1
        elif(self.led == "Yellow"):
            self.led_int = 2
    
    
    
    
    # sends data set in the object properties
    # returns 1 if failed to send, 0 otherwise
    def send(self):
        self.led_set()
        formatted = self.tx_string.format(self.front_left_power, 
                                          self.front_right_power, 
                                          self.back_left_power, 
                                          self.back_right_power,
                                          self.turret_power,
                                          int(self.enabled),
                                          self.led_int,
                                          int(self.flash))
        
        try:
            self.ser.write(formatted.encode("utf-8"))
        except serial.SerialException:
            return 1
        else:
            return 0
        
        
        
        
    # handles incoming data
    # run once per whatever loop you have
    # get data from object properties
    def receive(self):
        rx_bytes = self.ser.read_until('\n')
        
        rx_string = rx_bytes.decode("utf-8")
        
        rx_list = rx_string.split(',')
        
        self.front_left_rot  = float(rx_list[0])
        self.front_right_rot = float(rx_list[1])
        self.back_left_rot   = float(rx_list[2])
        self.back_right_rot  = float(rx_list[3])
        
        self.turret_angle = float(rx_list[4])
        
        self.pitch = float(rx_list[5])
        self.roll  = float(rx_list[6])
        self.yaw   = float(rx_list[7])
        
        self.pixel_offset = int(rx_list[8])