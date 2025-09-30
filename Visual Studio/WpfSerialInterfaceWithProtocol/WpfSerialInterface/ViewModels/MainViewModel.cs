using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Input;
using Serilog.Events;
using WpfSerialInterfaceWithProtocol.Core.Interfaces;
using WpfSerialInterfaceWithProtocol.Utilities;
using WpfSerialInterfaceWithProtocol.ViewModels.Base;

namespace WpfSerialInterfaceWithProtocol.ViewModels
{
    public class MainViewModel : ViewModelBase, IDisposable
    {
        private readonly ILogService _logService;
        private readonly ISerialPortService _serialPortService;
        private bool _isDisposed;

        public ObservableCollection<string> AvailablePorts { get; } = new ObservableCollection<string>();
        public ObservableCollection<LogLevelOption> LogLevelOptions { get; } = new ObservableCollection<LogLevelOption>
        {
            new LogLevelOption { Name = "Trace", Level = LogEventLevel.Verbose },
            new LogLevelOption { Name = "Debug", Level = LogEventLevel.Debug },
            new LogLevelOption { Name = "Info", Level = LogEventLevel.Information },
            new LogLevelOption { Name = "Warning", Level = LogEventLevel.Warning },
            new LogLevelOption { Name = "Error", Level = LogEventLevel.Error },
            new LogLevelOption { Name = "Fatal", Level = LogEventLevel.Fatal }
        };

        private LogLevelOption _selectedLogLevel = new LogLevelOption();
        public LogLevelOption SelectedLogLevel
        {
            get => _selectedLogLevel;
            set
            {
                _selectedLogLevel = value;
                _logService.SetLogLevel(value.Level);
                OnPropertyChanged();
            }
        }

        private string _receivedData = "";
        public string ReceivedData
        {
            get => _receivedData;
            set { _receivedData = value; OnPropertyChanged(); }
        }

        private string _selectedPort;
        public string SelectedPort
        {
            get => _selectedPort;
            set { _selectedPort = value; OnPropertyChanged(); }
        }

        private string _status = "Getrennt";
        public string Status
        {
            get => _status;
            set { _status = value; OnPropertyChanged(); }
        }

        private string _sendData = "";
        public string SendData
        {
            get => _sendData;
            set { _sendData = value; OnPropertyChanged(); }
        }

        public ICommand TestLogCommand { get; }
        public ICommand ConnectCommand { get; }
        public ICommand DisconnectCommand { get; }
        public ICommand SendCommand { get; }
        public ICommand ToggleThemeCommand { get; }

        private bool _isDarkMode;
        private void ToggleTheme() => IsDarkMode = !IsDarkMode;

        public bool IsDarkMode
        {
            get => _isDarkMode;
            set
            {
                if (_isDarkMode != value)
                {
                    _isDarkMode = value;
                    OnPropertyChanged();
                    ApplyTheme();
                    Properties.Settings.Default.IsDarkMode = value;
                    Properties.Settings.Default.Save();
                }
            }
        }

        private void ApplyTheme()
        {
            var app = Application.Current;
            var resources = app.Resources.MergedDictionaries;
            resources.Clear();
            resources.Add(new ResourceDictionary { Source = new Uri($"Themes/{(_isDarkMode ? "Dark" : "Light")}Theme.xaml", UriKind.Relative) });
        }

        public MainViewModel(ISerialPortService serialPortService, ILogService logService)
        {
            _serialPortService = serialPortService;
            _logService = logService;

            _selectedLogLevel = LogLevelOptions[2]; // Default: Info
            SelectedLogLevel = _selectedLogLevel;

            TestLogCommand = new RelayCommand(_ => TestLogs());
            ConnectCommand = new AsyncRelayCommand(ConnectAsync,
                () => !string.IsNullOrEmpty(SelectedPort) && !_serialPortService.IsPortOpen());
            DisconnectCommand = new AsyncRelayCommand(DisconnectAsync,
                () => _serialPortService.IsPortOpen());
            SendCommand = new AsyncRelayCommand(SendDataAsync,
                () => !string.IsNullOrEmpty(SendData) && _serialPortService.IsPortOpen());

            LoadPorts();

            // Subscribe to serial port events
            _serialPortService.DataReceived += OnDataReceived;
            _serialPortService.ConnectionChanged += OnConnectionChanged;

            ToggleThemeCommand = new RelayCommand(_ => ToggleTheme());
            IsDarkMode = Properties.Settings.Default.IsDarkMode;
        }

        private void OnDataReceived(string data)
        {
            Application.Current.Dispatcher.BeginInvoke(() =>
            {
                ReceivedData += $"[{DateTime.Now:HH:mm:ss}] {data}\n";
                _logService.InfoTemplate("Empfangen: {Data}", "Fertigung", data);
            });
        }

        private void OnConnectionChanged(bool isConnected)
        {
            Application.Current.Dispatcher.BeginInvoke(() =>
            {
                Status = isConnected ? $"Verbunden mit {SelectedPort}" : "Getrennt";
                CommandManager.InvalidateRequerySuggested();
            });
        }

        private void LoadPorts()
        {
            try
            {
                _logService.Debug("LoadPorts() wird aufgerufen", "System");

                var ports = _serialPortService.GetAvailablePorts();

                _logService.InfoTemplate("Gefundene Ports: {PortCount}", "System", ports.Length);

                AvailablePorts.Clear();
                foreach (var port in ports)
                {
                    AvailablePorts.Add(port);
                    _logService.DebugTemplate("Port hinzugefügt: {PortName}", "System", port);
                }

                if (ports.Length == 0)
                {
                    _logService.Warning("Keine COM-Ports gefunden", "System");
                }
                else
                {
                    _logService.InfoTemplate("{PortCount} COM-Port(s) geladen", "System", ports.Length);
                }
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Fehler beim Laden der verfügbaren Ports", "System");
            }
        }

        private void TestLogs()
        {
            _logService.Trace("Trace-Log (Entwicklung)", "Entwicklung");
            _logService.Debug("Debug-Log (Detailliert)", "Entwicklung");
            _logService.Info("Info-Log (Standard)", "Fertigung");
            _logService.Warning("Warning-Log (Warnung)", "Fertigung");
            _logService.Error("Error-Log (Fehler)", "System");
            _logService.Fatal("Fatal-Log (Kritisch)", "System");

            // Test template methods
            _logService.InfoTemplate("Test mit Parameter: {TestValue}", "Fertigung", 42);
        }

        private async Task ConnectAsync()
        {
            if (string.IsNullOrEmpty(SelectedPort))
            {
                _logService.Warning("Kein Port ausgewählt!", "System");
                return;
            }

            bool success = await _serialPortService.ConnectAsync(SelectedPort);
            if (!success)
            {
                _logService.ErrorTemplate("Verbindung zu {PortName} fehlgeschlagen", "System", SelectedPort);
            }
        }

        private async Task DisconnectAsync()
        {
            await _serialPortService.DisconnectAsync();
        }

        private async Task SendDataAsync()
        {
            if (string.IsNullOrEmpty(SendData))
            {
                _logService.Warning("Keine Daten zum Senden eingegeben!", "System");
                return;
            }

            if (!_serialPortService.IsPortOpen())
            {
                _logService.Error("Keine aktive Verbindung!", "System");
                return;
            }

            try
            {
                _logService.InfoTemplate("Sende Daten: {Data}", "Fertigung", SendData);
                await _serialPortService.SendDataAsync(SendData);
                SendData = ""; // Clear input field
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Fehler beim Senden der Daten", "System");
            }
        }

        public void Dispose()
        {
            if (!_isDisposed)
            {
                _isDisposed = true;

                // Unsubscribe from events
                _serialPortService.DataReceived -= OnDataReceived;
                _serialPortService.ConnectionChanged -= OnConnectionChanged;

                // Dispose serial port service
                _serialPortService?.Dispose();

                _logService.Info("MainViewModel disposed", "System");
            }
        }
    }

    public class LogLevelOption
    {
        public string Name { get; set; }
        public LogEventLevel Level { get; set; }
    }
}