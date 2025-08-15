using Plugin.BLE.Abstractions.Contracts;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace BLE_Interface.Services
{
    public class BleConnectionService
    {
        private readonly IAdapter _adapter;
        private IDevice? _device;
        private IService? _service;
        private ICharacteristic? _characteristic;

        public BleConnectionService(IAdapter adapter)
        {
            _adapter = adapter;
        }

        public async Task ConnectAsync(string deviceId, Guid serviceUuid, Guid characteristicUuid)
        {
            _device = await _adapter.ConnectToKnownDeviceAsync(Guid.Parse(deviceId));
            if (_device == null) throw new Exception("Device not found");

            var services = await _device.GetServicesAsync();
            _service = services.FirstOrDefault(s => s.Id == serviceUuid);
            if (_service == null) throw new Exception("Service not found");

            _characteristic = _service.Characteristics.FirstOrDefault(c => c.Id == characteristicUuid);
            if (_characteristic == null) throw new Exception("Characteristic not found");
        }

        public async Task SendCommandAsync(byte[] command)
        {
            if (_characteristic == null) throw new Exception("Not connected");
            await _characteristic.WriteAsync(command);
        }

        public async Task<byte[]> ReceiveResponseAsync()
        {
            if (_characteristic == null) throw new Exception("Not connected");
            return await _characteristic.ReadAsync();
        }
    }
}