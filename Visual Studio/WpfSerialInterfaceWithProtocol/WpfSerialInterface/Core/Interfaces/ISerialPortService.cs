using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System;
using System.Threading.Tasks;

namespace WpfSerialInterfaceWithProtocol.Core.Interfaces
{
    public interface ISerialPortService : IDisposable
    {
        string[] GetAvailablePorts();
        Task<bool> ConnectAsync(string portName, int baudRate = 9600);
        Task DisconnectAsync();
        Task SendDataAsync(string data);
        bool IsPortOpen();

        event Action<string> DataReceived;
        event Action<bool> ConnectionChanged;
    }
}