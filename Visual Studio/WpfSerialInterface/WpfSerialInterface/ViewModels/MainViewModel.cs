using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using WpfSerialInterface.Core.Interfaces;
using WpfSerialInterface.Utilities;
using WpfSerialInterface.ViewModels.Base;
using System.Windows;

namespace WpfSerialInterface.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private readonly ISerialPortService _serialPortService;
        private string _status = "Getrennt";
        private string _receivedData = "";
        private string _sendData = "";

        public ObservableCollection<string> AvailablePorts { get; } = new ObservableCollection<string>();
        public string SelectedPort { get; set; }

        public string Status
        {
            get => _status;
            set { _status = value; OnPropertyChanged(); }
        }

        public string ReceivedData
        {
            get => _receivedData;
            set { _receivedData = value; OnPropertyChanged(); }
        }

        public string SendData
        {
            get => _sendData;
            set { _sendData = value; OnPropertyChanged(); }
        }

        public ICommand ConnectCommand { get; }
        public ICommand DisconnectCommand { get; }
        public ICommand SendCommand { get; }

        public MainViewModel(ISerialPortService serialPortService)
        {
            _serialPortService = serialPortService;

            // Commands initialisieren
            ConnectCommand = new AsyncRelayCommand(ConnectAsync, () => !string.IsNullOrEmpty(SelectedPort));
            DisconnectCommand = new AsyncRelayCommand(DisconnectAsync, () => _serialPortService != null);
            SendCommand = new AsyncRelayCommand(SendDataAsync, () => !string.IsNullOrEmpty(SendData));

            // Events abonnieren
            _serialPortService.DataReceived += data =>
                Application.Current.Dispatcher.Invoke(() => ReceivedData += data + "\n");
            _serialPortService.ConnectionChanged += isConnected =>
                Application.Current.Dispatcher.Invoke(() => Status = isConnected ? $"Verbunden mit {SelectedPort}" : "Getrennt");

            LoadPorts();
        }

        private void LoadPorts()
        {
            AvailablePorts.Clear();
            foreach (var port in _serialPortService.GetAvailablePorts())
                AvailablePorts.Add(port);
        }

        private async Task ConnectAsync()
        {
            if (string.IsNullOrEmpty(SelectedPort)) return;
            bool success = await _serialPortService.ConnectAsync(SelectedPort);
            if (!success)
                Status = "Verbindung fehlgeschlagen";
        }

        private async Task DisconnectAsync() => await _serialPortService.DisconnectAsync();

        private async Task SendDataAsync()
        {
            await _serialPortService.SendDataAsync(SendData);
            SendData = ""; // TextBox leeren
        }
    }
}