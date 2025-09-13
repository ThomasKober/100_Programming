using System;
using System.Collections.Generic;
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

        public async Task<bool> ConnectAsync(string portName, int baudRate = 9600)
        {
            try
            {
                _serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One);
                _cancellationTokenSource = new CancellationTokenSource();
                _serialPort.Open();
                ConnectionChanged?.Invoke(true);
                _ = Task.Run(() => ReadDataAsync(_cancellationTokenSource.Token));
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Fehler: {ex.Message}");
                return false;
            }
        }

        private async Task ReadDataAsync(CancellationToken cancellationToken)
        {
            try
            {
                while (!cancellationToken.IsCancellationRequested && !_isDisposed)
                {
                    if (_serialPort?.IsOpen == true && _serialPort.BytesToRead > 0)
                    {
                        string data = _serialPort.ReadLine();
                        DataReceived?.Invoke(data);
                    }
                    await Task.Delay(100, cancellationToken);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Lesefehler: {ex.Message}");
            }
        }

        public async Task DisconnectAsync()
        {
            if (_serialPort?.IsOpen == true)
            {
                _cancellationTokenSource?.Cancel();
                await Task.Run(() => _serialPort.Close());
                ConnectionChanged?.Invoke(false);
            }
        }

        public async Task SendDataAsync(string data)
        {
            if (_serialPort?.IsOpen == true)
                await Task.Run(() => _serialPort.WriteLine(data));
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
