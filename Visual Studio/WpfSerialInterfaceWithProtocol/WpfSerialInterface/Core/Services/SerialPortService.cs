using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
using WpfSerialInterfaceWithProtocol.Core.Interfaces;

namespace WpfSerialInterfaceWithProtocol.Core.Services
{
    public class SerialPortService : ISerialPortService, IDisposable
    {
        private readonly ILogService _logService;
        private SerialPort? _serialPort;
        private CancellationTokenSource? _cancellationTokenSource;
        private readonly object _lockObject = new object();
        private bool _isDisposed;
        private string? _lastPortName;
        private int _lastBaudRate;
        private bool _autoReconnectEnabled;
        private Timer? _reconnectTimer;
        private Timer? _connectionCheckTimer;
        private const int ReconnectDelayMs = 3000;
        private const int ConnectionCheckIntervalMs = 1000; // Check every second

        public event Action<string>? DataReceived;
        public event Action<bool>? ConnectionChanged;
        public event Action<string>? ReconnectStatusChanged;

        public bool AutoReconnectEnabled
        {
            get => _autoReconnectEnabled;
            set
            {
                _autoReconnectEnabled = value;
                if (!value)
                {
                    _reconnectTimer?.Dispose();
                    _reconnectTimer = null;
                }
            }
        }

        public SerialPortService(ILogService logService)
        {
            _logService = logService ?? throw new ArgumentNullException(nameof(logService));
        }

        public string[] GetAvailablePorts() => SerialPort.GetPortNames();

        public bool IsPortOpen()
        {
            lock (_lockObject)
            {
                return _serialPort?.IsOpen == true;
            }
        }

        public async Task<bool> ConnectAsync(string portName, int baudRate = 9600)
        {
            if (_isDisposed)
                throw new ObjectDisposedException(nameof(SerialPortService));

            try
            {
                _logService.DebugTemplate("Connecting to {PortName} with {BaudRate} baud", "SerialPort", portName, baudRate);

                if (IsPortOpen())
                {
                    _logService.WarningTemplate("Port {PortName} is already open. Closing first...", "SerialPort", portName);
                    await DisconnectAsync();
                }

                lock (_lockObject)
                {
                    _serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One)
                    {
                        ReadTimeout = 500,
                        WriteTimeout = 500
                    };
                    _cancellationTokenSource = new CancellationTokenSource();

                    _lastPortName = portName;
                    _lastBaudRate = baudRate;
                }

                _serialPort.Open();

                _logService.InfoTemplate("Successfully connected to {PortName}", "SerialPort", portName);
                ConnectionChanged?.Invoke(true);

                _reconnectTimer?.Dispose();
                _reconnectTimer = null;

                // Start connection monitoring
                StartConnectionMonitoring();

                _ = Task.Run(() => ReadDataAsync(_cancellationTokenSource.Token));
                return true;
            }
            catch (Exception ex)
            {
                _logService.ErrorTemplate(ex, "Failed to connect to {PortName}", "SerialPort", portName);
                ConnectionChanged?.Invoke(false);
                await CleanupSerialPort();

                if (_autoReconnectEnabled && !string.IsNullOrEmpty(portName))
                {
                    StartAutoReconnect();
                }

                return false;
            }
        }

        private async Task ReadDataAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (!cancellationToken.IsCancellationRequested)
                {
                    SerialPort? port;
                    bool disposed;

                    lock (_lockObject)
                    {
                        port = _serialPort;
                        disposed = _isDisposed;
                    }

                    if (disposed)
                        break;

                    if (port?.IsOpen == true)
                    {
                        try
                        {
                            string data = await Task.Run(() => port.ReadLine(), cancellationToken);
                            DataReceived?.Invoke(data);
                            _logService.DebugTemplate("Received data: {Data}", "SerialPort", data);
                        }
                        catch (TimeoutException)
                        {
                        }
                        catch (OperationCanceledException)
                        {
                            break;
                        }
                        catch (Exception ex) when (ex is InvalidOperationException || ex is System.IO.IOException)
                        {
                            _logService.Error(ex, "Error reading from port", "SerialPort");

                            // First cleanup
                            await CleanupSerialPort();

                            // Then notify connection changed
                            ConnectionChanged?.Invoke(false);

                            _logService.InfoTemplate("AutoReconnectEnabled status: {Status}", "SerialPort", _autoReconnectEnabled);

                            if (_autoReconnectEnabled && !string.IsNullOrEmpty(_lastPortName))
                            {
                                _logService.Info("Calling StartAutoReconnect...", "SerialPort");
                                ReconnectStatusChanged?.Invoke("Verbindung verloren, starte Auto-Reconnect...");
                                StartAutoReconnect();
                            }
                            else
                            {
                                _logService.Info("Auto-reconnect is disabled, not attempting reconnect", "SerialPort");
                                ReconnectStatusChanged?.Invoke("Auto-Reconnect ist deaktiviert");
                            }

                            break;
                        }
                    }
                    else
                    {
                        await Task.Delay(100, cancellationToken);
                    }
                }
            }
            catch (OperationCanceledException)
            {
                _logService.Debug("Read operation canceled.", "SerialPort");
            }
        }

        public async Task DisconnectAsync()
        {
            try
            {
                _reconnectTimer?.Dispose();
                _reconnectTimer = null;

                _cancellationTokenSource?.Cancel();
                await Task.Delay(100);

                await CleanupSerialPort();

                ConnectionChanged?.Invoke(false);
                _logService.Info("Disconnected from serial port", "SerialPort");
            }
            catch (Exception ex)
            {
                _logService.Error(ex, "Error during disconnect", "SerialPort");
            }
        }

        private async Task CleanupSerialPort()
        {
            SerialPort? portToDispose = null;
            CancellationTokenSource? ctsToDispose = null;

            lock (_lockObject)
            {
                if (_serialPort != null)
                {
                    if (_serialPort.IsOpen)
                    {
                        try
                        {
                            _serialPort.Close();
                        }
                        catch (Exception ex)
                        {
                            _logService.Warning(ex, "Error closing serial port", "SerialPort");
                        }
                    }
                    portToDispose = _serialPort;
                    _serialPort = null;
                }

                ctsToDispose = _cancellationTokenSource;
                _cancellationTokenSource = null;
            }

            await Task.Run(() =>
            {
                portToDispose?.Dispose();
                ctsToDispose?.Dispose();
            });
        }

        public async Task SendDataAsync(string data)
        {
            if (_isDisposed)
                throw new ObjectDisposedException(nameof(SerialPortService));

            if (string.IsNullOrWhiteSpace(data))
                throw new ArgumentException("Data cannot be null or empty", nameof(data));

            SerialPort? port;
            lock (_lockObject)
            {
                port = _serialPort;
            }

            if (port?.IsOpen == true)
            {
                try
                {
                    _logService.DebugTemplate("Sending data: {Data}", "SerialPort", data);
                    await Task.Run(() => port.WriteLine(data));
                }
                catch (Exception ex)
                {
                    _logService.Error(ex, "Error sending data", "SerialPort");
                    throw;
                }
            }
            else
            {
                throw new InvalidOperationException("Serial port is not open");
            }
        }

        public void Dispose()
        {
            lock (_lockObject)
            {
                if (_isDisposed)
                    return;

                _isDisposed = true;
            }

            _reconnectTimer?.Dispose();
            _connectionCheckTimer?.Dispose();
            _cancellationTokenSource?.Cancel();

            lock (_lockObject)
            {
                if (_serialPort?.IsOpen == true)
                {
                    try
                    {
                        _serialPort.Close();
                    }
                    catch (Exception ex)
                    {
                        _logService.Warning(ex, "Error closing port during dispose", "SerialPort");
                    }
                }
                _serialPort?.Dispose();
                _serialPort = null;
            }

            _cancellationTokenSource?.Dispose();
            _logService.Info("SerialPortService disposed.", "SerialPort");
        }

        private void StartAutoReconnect()
        {
            if (_isDisposed || string.IsNullOrEmpty(_lastPortName))
                return;

            _reconnectTimer?.Dispose();

            _logService.InfoTemplate("Starting auto-reconnect for {PortName}...", "SerialPort", _lastPortName);
            ReconnectStatusChanged?.Invoke($"Auto-Reconnect aktiviert für {_lastPortName}");

            _reconnectTimer = new Timer(ReconnectCallback, null, ReconnectDelayMs, ReconnectDelayMs);
        }

        private async void ReconnectCallback(object? state)
        {
            if (_isDisposed || !_autoReconnectEnabled)
            {
                _reconnectTimer?.Dispose();
                _reconnectTimer = null;
                return;
            }

            if (!IsPortOpen() && !string.IsNullOrEmpty(_lastPortName))
            {
                _logService.DebugTemplate("Auto-reconnect attempt to {PortName}...", "SerialPort", _lastPortName);
                ReconnectStatusChanged?.Invoke($"Reconnect-Versuch zu {_lastPortName}...");

                try
                {
                    var success = await ConnectAsync(_lastPortName, _lastBaudRate);

                    if (success)
                    {
                        _logService.InfoTemplate("Auto-reconnect successful to {PortName}", "SerialPort", _lastPortName);
                        ReconnectStatusChanged?.Invoke($"Erfolgreich wiederverbunden mit {_lastPortName}");
                        _reconnectTimer?.Dispose();
                        _reconnectTimer = null;
                    }
                    else
                    {
                        _logService.DebugTemplate("Auto-reconnect failed, will retry...", "SerialPort");
                        ReconnectStatusChanged?.Invoke($"Reconnect fehlgeschlagen, erneuter Versuch in {ReconnectDelayMs / 1000}s");
                    }
                }
                catch (Exception ex)
                {
                    _logService.ErrorTemplate(ex, "Error during auto-reconnect attempt", "SerialPort");
                    ReconnectStatusChanged?.Invoke($"Reconnect-Fehler: {ex.Message}");
                }
            }
        }

        private void StartConnectionMonitoring()
        {
            _connectionCheckTimer?.Dispose();

            _connectionCheckTimer = new Timer(state =>
            {
                if (_isDisposed)
                {
                    _connectionCheckTimer?.Dispose();
                    _connectionCheckTimer = null;
                    return;
                }

                SerialPort? port;
                lock (_lockObject)
                {
                    port = _serialPort;
                }

                if (port != null)
                {
                    try
                    {
                        // Check if port is still available in system
                        var availablePorts = SerialPort.GetPortNames();
                        var portName = port.PortName;

                        if (!availablePorts.Contains(portName))
                        {
                            _logService.WarningTemplate("Port {PortName} is no longer available in system", "SerialPort", portName);
                            HandleConnectionLost();
                            return;
                        }

                        // Try to check if port is still open and accessible
                        if (!port.IsOpen)
                        {
                            _logService.Warning("Port is not open", "SerialPort");
                            HandleConnectionLost();
                            return;
                        }

                        // Additional check: Try to access port properties
                        try
                        {
                            var bytesToRead = port.BytesToRead; // This will throw if port is disconnected
                        }
                        catch
                        {
                            _logService.Warning("Port property access failed - connection lost", "SerialPort");
                            HandleConnectionLost();
                        }
                    }
                    catch (Exception ex)
                    {
                        _logService.ErrorTemplate(ex, "Error checking connection", "SerialPort");
                        HandleConnectionLost();
                    }
                }
            }, null, ConnectionCheckIntervalMs, ConnectionCheckIntervalMs);
        }

        private void HandleConnectionLost()
        {
            _connectionCheckTimer?.Dispose();
            _connectionCheckTimer = null;

            Task.Run(async () =>
            {
                _logService.Warning("Connection lost detected!", "SerialPort");
                ReconnectStatusChanged?.Invoke("Verbindung verloren erkannt!");

                await CleanupSerialPort();
                ConnectionChanged?.Invoke(false);

                if (_autoReconnectEnabled && !string.IsNullOrEmpty(_lastPortName))
                {
                    _logService.Info("Starting auto-reconnect after connection loss", "SerialPort");
                    ReconnectStatusChanged?.Invoke("Verbindung verloren, starte Auto-Reconnect...");
                    StartAutoReconnect();
                }
                else
                {
                    _logService.Info("Auto-reconnect is disabled", "SerialPort");
                    ReconnectStatusChanged?.Invoke("Auto-Reconnect ist deaktiviert");
                }
            });
        }
    }
}