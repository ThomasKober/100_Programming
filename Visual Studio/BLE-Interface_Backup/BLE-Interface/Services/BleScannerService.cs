using BLE_Interface.Models;
using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows;

namespace BLE_Interface.Services
{
    public class BleScannerService
    {
        private readonly IAdapter _adapter;

        public BleScannerService(IAdapter adapter)
        {
            _adapter = adapter;
        }

        public async Task ScanAsync(ObservableCollection<BleDevice> devices)
        {
            devices.Clear();
            _adapter.DeviceDiscovered += (sender, device) =>
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    devices.Add(new BleDevice
                    {
                        Id = device.Id.ToString(),
                        Name = device.Name ?? "Unknown",
                        Rssi = device.Rssi
                    });
                });
            };
            await _adapter.StartScanningForDevicesAsync();
            await Task.Delay(5000); // Scan for 5 seconds
            await _adapter.StopScanningForDevicesAsync();
        }
    }
}