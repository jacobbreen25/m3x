import serial
import time
import asyncio
from bleak import BleakScanner, BleakClient, backends, BleakGATTCharacteristic 

class haptic1tactor_handler():
    def __init__(self, name = "m3xBLE", port = "COM3", baudrate = 9600):
        self.port = port
        self.baud = baudrate
        self.name = name
        self.device = None
        try:
            self.serial = serial.Serial(self.port, self.baud)
        except serial.SerialException as err:
            self.serial = None
            asyncio.run(self.__getDevice(self.name))
            if not self.device:
                self.__del__()
                raise NameError("Could not find Serial under port " + str(port) + " and baudrate "  + str(baudrate) + " or BLE Device under the name " + str(name))
        self.rx_char = "e54aa131-bdbf-11ed-a901-0800200c9a66".format(0xFFE1)

    ################# New functions for haptics exp ##################################

    def close_serial(self):
        if self.serial != None:
            self.serial.close()

    def read_data(self,isstream):
        if self.serialBool:
            raw = self.serial.readline()
            return raw
        else:
            raise ValueError("No Serial Available")
        #data = self.serial.readline().decode("Ascii").strip().split(",")
        #return data

    def write_data(self, message = "GO"):
        if self.serial != None:
            self.serial.write(message.encode('ascii'))
            time.sleep(1)
        else:
            raise ValueError("No Serial Available")

    def write_ble(self, message = "GO", name = "m3xBLE"):
        if self.device == None or name != self.name:
            asyncio.run(self.__getDevice(name))
            if not self.device:
                raise NameError("No BLE Object Under the name " + name + " found. Make sure device is on.")
        try:
            asyncio.run(self.__writeDevice(self.device, message))
        except serial.SerialException as err:
            raise err

    async def __getDevice(self, name : str):
        devices = await BleakScanner.discover()
        for d in devices:
            if d.name == name:
                self.device = d
                return
    
    async def __writeDevice(self, ble, message : str):
        def disconnect(client : BleakClient):
            raise ValueError("Disconnected from " + client.address)
        bytedata = bytes(message, encoding='utf-8')
        async with BleakClient(ble, disconnect_callback= lambda : disconnect, timeout=5.0) as client:
            await client.write_gatt_char(self.rx_char, bytedata, True)
            #await asyncio.sleep(1)

    def __del__(self):
        self.serial = None
        self.baud = None
        self.device = None
        self.rx_char = None
        self.name = None
        self.port = None

