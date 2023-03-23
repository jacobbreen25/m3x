import libm3x as m3x
import serial as Serial

testApp = None

try:
    testApp = m3x.haptic1tactor_handler()
except Serial.SerialException as err:
    print(err)
    exit()

try:
    testApp.write_ble()
except Exception as err:
    print("ERROR: " + str(err))

testApp.write_data()