using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
using WpfSerialInterface.Core.Interfaces;
using WpfSerialInterface.Utilities;
using WpfSerialInterface.ViewModels.Base;


namespace WpfSerialInterface.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private readonly ISerialPortService _serialPortService;
        private DispatcherTimer _portsRefreshTimer;
        private string _selectedPort;
        private string _status = "Getrennt";
        private string _receivedData = "";
        private string _sendData = "";
        private bool _isConnected;

        public ObservableCollection<string> AvailablePorts { get; } = new ObservableCollection<string>();

        public bool IsConnected
        {
            get => _isConnected;
            set
            {
                _isConnected = value;
                OnPropertyChanged();
                // Aktualisiere die CanExecute-Bedingung der Commands
                CommandManager.InvalidateRequerySuggested();
            }
        }

        public string SelectedPort
        {
            get => _selectedPort;
            set
            {
                _selectedPort = value;
                OnPropertyChanged();
                Debug.WriteLine($"SelectedPort geändert: {_selectedPort}");
            }
        }

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
            ConnectCommand = new AsyncRelayCommand(ConnectAsync, () => !string.IsNullOrEmpty(SelectedPort) && !IsConnected);
            DisconnectCommand = new AsyncRelayCommand(DisconnectAsync, () => IsConnected);
            SendCommand = new AsyncRelayCommand(SendDataAsync, () => !string.IsNullOrEmpty(SendData));

            // Timer für regelmäßige Aktualisierung der COM-Ports
            _portsRefreshTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(2) // Alle 2 Sekunden aktualisieren
            };
            _portsRefreshTimer.Tick += (sender, e) => LoadPorts();
            _portsRefreshTimer.Start();

            // Events abonnieren
            _serialPortService.DataReceived += data => Application.Current.Dispatcher.Invoke(() => ReceivedData += data + "\n");
            _serialPortService.ConnectionChanged += isConnected => Application.Current.Dispatcher.Invoke(() => IsConnected = isConnected);

            _serialPortService.ConnectionChanged += isConnected =>
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    IsConnected = isConnected;
                    Status = isConnected ? $"Verbunden mit {SelectedPort}" : "Getrennt (Verbindung verloren)";
                });
            };

            // Timer für regelmäßige Aktualisierung der COM-Ports
            _portsRefreshTimer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(2) };
            _portsRefreshTimer.Tick += (sender, e) => LoadPorts();
            _portsRefreshTimer.Start();

            // Ports beim Start laden
            LoadPorts();
        }

        private void LoadPorts()
        {
            var ports = SerialPort.GetPortNames();
            Debug.WriteLine($"Verfügbare Ports aktualisiert: {string.Join(", ", ports)}");

            // Aktualisiere die ObservableCollection
            var currentSelectedPort = SelectedPort;
            AvailablePorts.Clear();
            foreach (var port in ports)
                AvailablePorts.Add(port);

            // Versuche, den vorher ausgewählten Port wieder auszuwählen
            if (!string.IsNullOrEmpty(currentSelectedPort) && AvailablePorts.Contains(currentSelectedPort))
                SelectedPort = currentSelectedPort;
        }

        private async Task ConnectAsync()
        {
            if (string.IsNullOrEmpty(SelectedPort))
            {
                Status = "Bitte wählen Sie einen Port aus!";
                return;
            }

            bool success = await _serialPortService.ConnectAsync(SelectedPort);
            if (success)
            {
                Status = $"Verbunden mit {SelectedPort}";
                IsConnected = true; // Verbindung hergestellt
            }
            else
            {
                Status = "Verbindung fehlgeschlagen!";
            }
        }

        private async Task DisconnectAsync()
        {
            await _serialPortService.DisconnectAsync();
            Status = "Getrennt";
            IsConnected = false; // Verbindung getrennt
        }

        private async Task SendDataAsync()
        {
            await _serialPortService.SendDataAsync(SendData);
            SendData = "";
        }
    }
}