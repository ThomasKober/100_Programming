using BLE_Interface.Models;
using BLE_Interface.Services;
using Microsoft.Toolkit.Mvvm.ComponentModel;
using Microsoft.Toolkit.Mvvm.Input;
using Plugin.BLE.Abstractions.Contracts;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace BLE_Interface.ViewModels
{
    public class DeviceScanViewModel : ObservableObject
    {
        private readonly BleScannerService _scanner;
        public ObservableCollection<BleDevice> Devices { get; } = new();

        public ICommand ScanCommand { get; }

        public DeviceScanViewModel(IAdapter adapter)
        {
            _scanner = new BleScannerService(adapter);
            ScanCommand = new RelayCommand(async () => await _scanner.ScanAsync(Devices));
        }
    }
}