# import gatt

# mac_address = "FC:45:C3:24:91:EB"

# READWRITE_SERVICE = "0000ffe0-0000-1000-8000-00805f9b34fb"
# READWRITE_CHARACTERISTIC = "0000ffe1-0000-1000-8000-00805f9b34fb"

# manager = gatt.DeviceManager(adapter_name='hci0')

# class AnyDevice(gatt.Device):
#     def services_resolved(self):
#         super().services_resolved()
#         print("got here 3")

#         device_information_service = next(
#             s for s in self.services
#             if s.uuid == READWRITE_SERVICE)

#         firmware_version_characteristic = next(
#             c for c in device_information_service.characteristics
#             if c.uuid == READWRITE_CHARACTERISTIC)
    
#         print("got here 4")

#         firmware_version_characteristic.read_value()

#     def characteristic_value_updated(self, characteristic, value):
#         print("Firmware version:", value.decode("utf-8"))


# device = AnyDevice(mac_address=mac_address, manager=manager)
# print("got here")
# device.connect()
# print("got here 2")

# manager.run()
import gatt

class AnyDeviceManager(gatt.DeviceManager):
    def device_discovered(self, device):
        print("Discovered [%s] %s" % (device.mac_address, device.alias()))

manager = AnyDeviceManager(adapter_name='hci0')
manager.start_discovery()
manager.run()