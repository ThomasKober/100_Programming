using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using WpfSerialInterface.Core.Interfaces;

namespace WpfSerialInterface.Core.Services
{
    public class SerialPortService : ISerialPortService
    {
        private SerialPort? _serialPort;
        private CancellationTokenSource? _cancellationTokenSource;
        private bool _isDisposed;

        public event Action<string>? DataReceived;
        public event Action<bool>? ConnectionChanged;

        public string[] GetAvailablePorts() => SerialPort.GetPortNames();

        private CancellationTokenSource _portStatusCheckTokenSource;

        public bool IsPortOpen()
        {
            return _serialPort?.IsOpen == true;
        }

        public async Task<bool> ConnectAsync(string portName, int baudRate = 9600)
        {
            try
            {
                // Schließe den aktuellen Port, falls offen
                if (_serialPort?.IsOpen == true)
                {
                    await DisconnectAsync();
                }

                _serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One);
                _cancellationTokenSource = new CancellationTokenSource();
                _portStatusCheckTokenSource = new CancellationTokenSource();

                _serialPort.Open();
                ConnectionChanged?.Invoke(true);

                // Starte das Lesen der Daten
                _ = Task.Run(() => ReadDataAsync(_cancellationTokenSource.Token));

                // Starte die Überprüfung des Port-Status
                _ = Task.Run(() => CheckPortStatusAsync(_portStatusCheckTokenSource.Token));

                return true;
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error reconnecting: {ex.Message}");
                return false;
            }
        }

        private async Task CheckPortStatusAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (!cancellationToken.IsCancellationRequested && !_isDisposed)
                {
                    await Task.Delay(1000, cancellationToken); // Überprüfe jede Sekunde

                    if (!IsPortOpen())
                    {
                        Debug.WriteLine("Port is no longer open. Attempting to reconnect...");
                        ConnectionChanged?.Invoke(false);

                        // Versuche, die Verbindung wiederherzustellen
                        await AttemptReconnectAsync(cancellationToken);
                        break;
                    }
                }
            }
            catch (OperationCanceledException)
            {
                Debug.WriteLine("Port status check canceled.");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Port status check error: {ex.Message}");
                ConnectionChanged?.Invoke(false);
            }
        }

        private async Task AttemptReconnectAsync(CancellationToken cancellationToken)
        {
            int reconnectAttempts = 0;
            int maxAttempts = 5; // Maximal 5 Wiederverbindungsversuche
            TimeSpan delayBetweenAttempts = TimeSpan.FromSeconds(3); // Warte 3 Sekunden zwischen den Versuchen

            while (!cancellationToken.IsCancellationRequested && reconnectAttempts < maxAttempts && !_isDisposed)
            {
                reconnectAttempts++;
                Debug.WriteLine($"Reconnect attempt {reconnectAttempts} of {maxAttempts}...");

                try
                {
                    if (_serialPort != null && !string.IsNullOrEmpty(_serialPort.PortName))
                    {
                        bool success = await ConnectAsync(_serialPort.PortName);
                        if (success)
                        {
                            Debug.WriteLine("Reconnected successfully!");
                            ConnectionChanged?.Invoke(true);
                            return; // Erfolgreich verbunden, beende die Methode
                        }
                    }
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"Reconnect attempt {reconnectAttempts} failed: {ex.Message}");
                }

                await Task.Delay(delayBetweenAttempts, cancellationToken);
            }

            Debug.WriteLine("Failed to reconnect after multiple attempts.");
        }

        private async Task ReadDataAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (!cancellationToken.IsCancellationRequested && !_isDisposed)
                {
                    if (_serialPort?.IsOpen == true)
                    {
                        try
                        {
                            string data = _serialPort.ReadLine();
                            DataReceived?.Invoke(data);
                        }
                        catch (TimeoutException)
                        {
                            Debug.WriteLine("Timeout: No data received.");
                        }
                        catch (Exception ex)
                        {
                            Debug.WriteLine($"Connection error: {ex.Message}");
                            ConnectionChanged?.Invoke(false);
                            Debug.WriteLine("Connection lost. Port is no longer available.");
                            break;
                        }
                    }
                    else
                    {
                        Debug.WriteLine("Port is closed.");
                        ConnectionChanged?.Invoke(false);
                        Debug.WriteLine("Connection lost. Port is no longer available.");
                        break;
                    }
                    await Task.Delay(100, cancellationToken);
                }
            }
            catch (OperationCanceledException)
            {
                Debug.WriteLine("Read operation canceled.");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Read error: {ex.Message}");
                ConnectionChanged?.Invoke(false);
                Debug.WriteLine("Connection lost. Port is no longer available.");
            }
        }

        public async Task DisconnectAsync()
        {
            if (_serialPort?.IsOpen == true)
            {
                _cancellationTokenSource?.Cancel();
                _portStatusCheckTokenSource?.Cancel();

                await Task.Run(() => _serialPort?.Close());

                ConnectionChanged?.Invoke(false);
                Debug.WriteLine("Disconnected from port.");
            }
        }

        public async Task SendDataAsync(string data)
        {
            if (_serialPort?.IsOpen == true)
            {
                // Verwende Task.Run, um das Senden im Hintergrund-Thread auszuführen
                await Task.Run(() => _serialPort?.WriteLine(data));
            }
        }

        public void Dispose()
        {
            if (!_isDisposed)
            {
                _cancellationTokenSource?.Cancel();
                _serialPort?.Dispose();
                _isDisposed = true;
            }
        }
    }
}