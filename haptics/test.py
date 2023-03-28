import libm3x as m3x
import serial as Serial

testApp = None

try:
    testApp = m3x.haptic1tactor_handler()
    testApp.write_ble()
    testApp.write_data()
except Exception as err:
    print("ERROR: " + str(err))
    exit()