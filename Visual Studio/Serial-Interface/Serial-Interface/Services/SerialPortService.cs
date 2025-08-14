using System;
using System.IO.Ports;
using System.Threading;

namespace Serial_Interface.Services
{
    public class SerialPortService
    {
        private SerialPort _serialPort;
        private Thread _readThread;
        private bool _isRunning;

        public event Action<string> DataReceived;

        public SerialPortService(string portName, int baudRate)
        {
            _serialPort = new SerialPort(portName, baudRate);
        }

        public void Open()
        {
            if (!_serialPort.IsOpen)
            {
                _serialPort.Open();
                _isRunning = true;
                _readThread = new Thread(ReadData);
                _readThread.Start();
            }
        }

        public void Close()
        {
            _isRunning = false;
            if (_readThread != null && _readThread.IsAlive)
                _readThread.Join();
            if (_serialPort.IsOpen)
                _serialPort.Close();
        }

        public void Send(string data)
        {
            if (_serialPort.IsOpen)
                _serialPort.WriteLine(data);
        }

        private void ReadData()
        {
            while (_isRunning && _serialPort.IsOpen)
            {
                try
                {
                    string receivedData = _serialPort.ReadLine();
                    DataReceived?.Invoke(receivedData);
                }
                catch (Exception ex)
                {
                    DataReceived?.Invoke($"Fehler: {ex.Message}");
                }
            }
        }
    }
}
