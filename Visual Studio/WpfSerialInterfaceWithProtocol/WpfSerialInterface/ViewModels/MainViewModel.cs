using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
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
        private readonly DispatcherTimer _portScanTimer;
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

        private string _logData = "";
        public string LogData
        {
            get => _logData;
            set { _logData = value; OnPropertyChanged(); }
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

        private bool _autoReconnect = true;
        public bool AutoReconnect
        {
            get => _autoReconnect;
            set
            {
                _autoReconnect = value;
                _serialPortService.AutoReconnectEnabled = value;
                OnPropertyChanged();

                var statusMsg = $"Auto-Reconnect wurde {(value ? "AKTIVIERT" : "DEAKTIVIERT")}";
                _logService.InfoTemplate("Auto-Reconnect {Status}", "System", value ? "aktiviert" : "deaktiviert");
                AddLogMessage(statusMsg, "INFO");
            }
        }

        public ICommand TestLogCommand { get; }
        public ICommand ConnectCommand { get; }
        public ICommand DisconnectCommand { get; }
        public ICommand SendCommand { get; }
        public ICommand ToggleThemeCommand { get; }
        public ICommand ClearLogDataCommand { get; }

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

                    Properties.Settings.Default.IsDarkMode = value;
                    Properties.Settings.Default.Save();

                    ApplyTheme();
                }
            }
        }

        private void ApplyTheme()
        {
            try
            {
                var app = Application.Current;
                if (app?.Resources == null)
                {
                    _logService.Warning("Application resources not available", "System");
                    return;
                }

                // Create the theme URI - try both relative and pack URI
                var themeName = _isDarkMode ? "DarkTheme.xaml" : "LightTheme.xaml";
                ResourceDictionary newTheme = null;

                try
                {
                    // Try relative URI first
                    newTheme = new ResourceDictionary
                    {
                        Source = new Uri($"Themes/{themeName}", UriKind.Relative)
                    };
                }
                catch
                {
                    // Try pack URI if relative fails
                    newTheme = new ResourceDictionary
                    {
                        Source = new Uri($"/Themes/{themeName}", UriKind.Relative)
                    };
                }

                var resources = app.Resources.MergedDictionaries;

                // Clear all and re-add the new theme
                resources.Clear();
                resources.Add(newTheme);

                _logService.InfoTemplate("Theme switched to {ThemeName}", "System", themeName);
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Failed to apply theme: {Message}", "System", ex.Message);
            }
        }

        public MainViewModel(ISerialPortService serialPortService, ILogService logService)
        {
            _serialPortService = serialPortService;
            _logService = logService;

            _selectedLogLevel = LogLevelOptions[2]; // Default: Info
            SelectedLogLevel = _selectedLogLevel;

            // Enable auto-reconnect by default
            AutoReconnect = true;

            TestLogCommand = new RelayCommand(_ => TestLogs());
            ConnectCommand = new AsyncRelayCommand(ConnectAsync,
                () => !string.IsNullOrEmpty(SelectedPort) && !_serialPortService.IsPortOpen());
            DisconnectCommand = new AsyncRelayCommand(DisconnectAsync,
                () => _serialPortService.IsPortOpen());
            SendCommand = new AsyncRelayCommand(SendDataAsync,
                () => !string.IsNullOrEmpty(SendData) && _serialPortService.IsPortOpen());
            ClearLogDataCommand = new RelayCommand(_ => LogData = "");

            LoadPorts();

            // Subscribe to serial port events
            _serialPortService.DataReceived += OnDataReceived;
            _serialPortService.ConnectionChanged += OnConnectionChanged;
            _serialPortService.ReconnectStatusChanged += OnReconnectStatusChanged;

            ToggleThemeCommand = new RelayCommand(_ => ToggleTheme());

            // Load and apply saved theme preference
            IsDarkMode = Properties.Settings.Default.IsDarkMode;

            // Setup timer for automatic port scanning (every 3 seconds)
            // Timer starts when disconnected
            _portScanTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(3)
            };
            _portScanTimer.Tick += (s, e) => LoadPorts();

            // Only start timer if not connected
            if (!_serialPortService.IsPortOpen())
            {
                _portScanTimer.Start();
            }

            _logService.Info("MainViewModel initialized", "System");
        }

        private void OnDataReceived(string data)
        {
            Application.Current.Dispatcher.BeginInvoke(() =>
            {
                AddLogMessage($"Empfangen: {data}", "DATA");
                _logService.InfoTemplate("Empfangen: {Data}", "Fertigung", data);
            });
        }

        private void OnConnectionChanged(bool isConnected)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                Status = isConnected ? $"Verbunden mit {SelectedPort}" : "Getrennt";

                // Log to UI
                AddLogMessage(isConnected ? $"Verbunden mit {SelectedPort}" : "Verbindung getrennt", "INFO");
                AddLogMessage($"AutoReconnect ist: {(AutoReconnect ? "AKTIVIERT" : "DEAKTIVIERT")}", "DEBUG");

                // Only scan for ports when disconnected
                if (isConnected)
                {
                    _portScanTimer.Stop();
                    _logService.Debug("Port scanning stopped (connected)", "System");
                }
                else
                {
                    _portScanTimer.Start();
                    _logService.Debug("Port scanning started (disconnected)", "System");
                }

                // Force command re-evaluation
                CommandManager.InvalidateRequerySuggested();

                // Also manually notify property changes
                OnPropertyChanged(nameof(Status));
            });
        }

        private void OnReconnectStatusChanged(string status)
        {
            Application.Current.Dispatcher.BeginInvoke(() =>
            {
                AddLogMessage(status, "RECONNECT");
            });
        }

        private void AddLogMessage(string message, string level)
        {
            Application.Current.Dispatcher.BeginInvoke(() =>
            {
                LogData += $"[{DateTime.Now:HH:mm:ss}] [{level}] {message}\n";

                // Limit log data to last 1000 lines
                var lines = LogData.Split('\n');
                if (lines.Length > 1000)
                {
                    LogData = string.Join("\n", lines.Skip(lines.Length - 1000));
                }
            });
        }

        private void LoadPorts()
        {
            try
            {
                var ports = _serialPortService.GetAvailablePorts();

                // Only update if ports have changed
                if (PortsHaveChanged(ports))
                {
                    _logService.DebugTemplate("Port-Liste hat sich geändert. Neue Anzahl: {PortCount}", "System", ports.Length);

                    // Remember current selection
                    string currentSelection = SelectedPort;

                    AvailablePorts.Clear();
                    foreach (var port in ports)
                    {
                        AvailablePorts.Add(port);
                    }

                    // Restore selection if port still exists
                    if (!string.IsNullOrEmpty(currentSelection) && AvailablePorts.Contains(currentSelection))
                    {
                        SelectedPort = currentSelection;
                    }
                    else if (AvailablePorts.Count > 0)
                    {
                        SelectedPort = AvailablePorts[0];
                    }

                    if (ports.Length == 0)
                    {
                        _logService.Debug("Keine COM-Ports gefunden", "System");
                    }
                }
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Fehler beim Laden der verfügbaren Ports", "System");
            }
        }

        private bool PortsHaveChanged(string[] newPorts)
        {
            if (AvailablePorts.Count != newPorts.Length)
                return true;

            foreach (var port in newPorts)
            {
                if (!AvailablePorts.Contains(port))
                    return true;
            }

            return false;
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

            // Add test logs to UI
            AddLogMessage("Test-Log: Trace (Entwicklung)", "TRACE");
            AddLogMessage("Test-Log: Debug (Detailliert)", "DEBUG");
            AddLogMessage("Test-Log: Info (Standard)", "INFO");
            AddLogMessage("Test-Log: Warning (Warnung)", "WARNING");
            AddLogMessage("Test-Log: Error (Fehler)", "ERROR");
            AddLogMessage("Test-Log: Fatal (Kritisch)", "FATAL");
        }

        private async Task ConnectAsync()
        {
            if (string.IsNullOrEmpty(SelectedPort))
            {
                _logService.Warning("Kein Port ausgewählt!", "System");
                AddLogMessage("Kein Port ausgewählt!", "WARNING");
                return;
            }

            AddLogMessage($"Verbinde mit {SelectedPort}...", "INFO");
            bool success = await _serialPortService.ConnectAsync(SelectedPort);
            if (!success)
            {
                _logService.ErrorTemplate("Verbindung zu {PortName} fehlgeschlagen", "System", SelectedPort);
                AddLogMessage($"Verbindung zu {SelectedPort} fehlgeschlagen", "ERROR");
            }
        }

        private async Task DisconnectAsync()
        {
            AddLogMessage("Trenne Verbindung...", "INFO");
            await _serialPortService.DisconnectAsync();
        }

        private async Task SendDataAsync()
        {
            if (string.IsNullOrWhiteSpace(SendData))
            {
                _logService.Warning("Keine Daten zum Senden eingegeben!", "System");
                AddLogMessage("Keine Daten zum Senden eingegeben!", "WARNING");
                return;
            }

            if (!_serialPortService.IsPortOpen())
            {
                _logService.Error("Keine aktive Verbindung!", "System");
                AddLogMessage("Keine aktive Verbindung!", "ERROR");
                return;
            }

            try
            {
                _logService.InfoTemplate("Sende Daten: {Data}", "Fertigung", SendData);
                AddLogMessage($"Sende: {SendData}", "INFO");
                await _serialPortService.SendDataAsync(SendData);
                SendData = ""; // Clear input field
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Fehler beim Senden der Daten", "System");
                AddLogMessage($"Fehler beim Senden: {ex.Message}", "ERROR");
            }
        }

        public void Dispose()
        {
            if (!_isDisposed)
            {
                _isDisposed = true;

                // Stop the timer
                _portScanTimer?.Stop();

                // Unsubscribe from events
                if (_serialPortService != null)
                {
                    _serialPortService.DataReceived -= OnDataReceived;
                    _serialPortService.ConnectionChanged -= OnConnectionChanged;
                    _serialPortService.ReconnectStatusChanged -= OnReconnectStatusChanged;
                }

                // Dispose serial port service (if it implements IDisposable)
                if (_serialPortService is IDisposable disposableService)
                {
                    disposableService.Dispose();
                }

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