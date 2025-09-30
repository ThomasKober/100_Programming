using Serilog;
using System;
using System.IO.Ports;
using System.Threading;
using System.Threading.Tasks;
using WpfSerialInterfaceWithProtocol.Core.Interfaces;

namespace WpfSerialInterfaceWithProtocol.Core.Services
{
    public class SerialPortService : ISerialPortService, IDisposable
    {
        private readonly ILogger _logger;
        private SerialPort? _serialPort;
        private CancellationTokenSource? _cancellationTokenSource;
        private readonly object _lockObject = new object();
        private bool _isDisposed;

        public event Action<string>? DataReceived;
        public event Action<bool>? ConnectionChanged;

        public SerialPortService(ILogService logService)
        {
            _logger = Log.ForContext<SerialPortService>();
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
                _logger.Debug("Connecting to {PortName} with {BaudRate} baud", portName, baudRate);

                if (IsPortOpen())
                {
                    _logger.Warning("Port {PortName} is already open. Closing first...", portName);
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
                }

                _serialPort.Open();

                _logger.Information("Successfully connected to {PortName}", portName);
                ConnectionChanged?.Invoke(true);

                _ = Task.Run(() => ReadDataAsync(_cancellationTokenSource.Token));
                return true;
            }
            catch (Exception ex)
            {
                _logger.Error(ex, "Failed to connect to {PortName}", portName);
                ConnectionChanged?.Invoke(false);
                await CleanupSerialPort();
                return false;
            }
        }

        private async Task ReadDataAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (!cancellationToken.IsCancellationRequested && !_isDisposed)
                {
                    SerialPort? port;
                    lock (_lockObject)
                    {
                        port = _serialPort;
                    }

                    if (port?.IsOpen == true)
                    {
                        try
                        {
                            string data = await Task.Run(() => port.ReadLine(), cancellationToken);
                            DataReceived?.Invoke(data);
                            _logger.Debug("Received data: {Data}", data);
                        }
                        catch (TimeoutException)
                        {
                            // Expected timeout, continue loop
                        }
                        catch (OperationCanceledException)
                        {
                            break;
                        }
                        catch (Exception ex) when (ex is InvalidOperationException || ex is System.IO.IOException)
                        {
                            _logger.Error(ex, "Error reading from port");
                            ConnectionChanged?.Invoke(false);
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
                _logger.Debug("Read operation canceled.");
            }
        }

        public async Task DisconnectAsync()
        {
            try
            {
                _cancellationTokenSource?.Cancel();
                await Task.Delay(100); // Give time for read operation to stop

                await CleanupSerialPort();

                ConnectionChanged?.Invoke(false);
                _logger.Information("Disconnected from serial port");
            }
            catch (Exception ex)
            {
                _logger.Error(ex, "Error during disconnect");
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
                            _logger.Warning(ex, "Error closing serial port");
                        }
                    }
                    portToDispose = _serialPort;
                    _serialPort = null;
                }

                ctsToDispose = _cancellationTokenSource;
                _cancellationTokenSource = null;
            }

            // Dispose outside lock to avoid potential deadlocks
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

            SerialPort? port;
            lock (_lockObject)
            {
                port = _serialPort;
            }

            if (port?.IsOpen == true)
            {
                try
                {
                    _logger.Debug("Sending data: {Data}", data);
                    await Task.Run(() => port.WriteLine(data));
                }
                catch (Exception ex)
                {
                    _logger.Error(ex, "Error sending data");
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
            if (!_isDisposed)
            {
                _isDisposed = true;
                _cancellationTokenSource?.Cancel();

                // Synchronous cleanup for dispose
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
                            _logger.Warning(ex, "Error closing port during dispose");
                        }
                    }
                    _serialPort?.Dispose();
                    _serialPort = null;
                }

                _cancellationTokenSource?.Dispose();
                _logger.Information("SerialPortService disposed.");
            }
        }
    }
}