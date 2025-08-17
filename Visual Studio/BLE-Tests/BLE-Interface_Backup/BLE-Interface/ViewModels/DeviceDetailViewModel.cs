using BLE_Interface.Services;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace BLE_Interface.ViewModels
{
    public class DeviceDetailViewModel : ObservableObject
    {
        private readonly BleConnectionService _connectionService;
        private string _status = "Disconnected";

        public string Status
        {
            get => _status;
            set => SetProperty(ref _status, value);
        }

        public ICommand ConnectCommand { get; }
        public ICommand SendCommand { get; }

        public DeviceDetailViewModel(BleConnectionService connectionService)
        {
            _connectionService = connectionService;
            ConnectCommand = new RelayCommand<string>(async (deviceId) => await ConnectAsync(deviceId));
            SendCommand = new RelayCommand<byte[]>(async (command) => await SendCommandAsync(command));
        }

        private async Task ConnectAsync(string deviceId)
        {
            try
            {
                await _connectionService.ConnectAsync(deviceId, Guid.Parse("YOUR_SERVICE_UUID"), Guid.Parse("YOUR_CHARACTERISTIC_UUID"));
                Status = "Connected";
            }
            catch (Exception ex)
            {
                Status = $"Error: {ex.Message}";
            }
        }

        private async Task SendCommandAsync(byte[] command)
        {
            try
            {
                await _connectionService.SendCommandAsync(command);
                var response = await _connectionService.ReceiveResponseAsync();
                // Handle response
            }
            catch (Exception ex)
            {
                Status = $"Error: {ex.Message}";
            }
        }
    }
}