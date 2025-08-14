using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using Serial_Interface.Services;

namespace Serial_Interface.ViewModels
{
    public class MainViewModel : INotifyPropertyChanged
    {
        private readonly SerialPortService _serialService;
        private string _receivedText;

        public string ReceivedText
        {
            get => _receivedText;
            set { _receivedText = value; OnPropertyChanged(); }
        }

        public MainViewModel()
        {
            _serialService = new SerialPortService("COM10", 9600);
            _serialService.DataReceived += data =>
                Application.Current.Dispatcher.Invoke(() =>
                    ReceivedText += data + "\n");
        }

        public void StartConnection()
        {
            _serialService.Open();
        }

        public void StopConnection()
        {
            _serialService.Close();
        }

        public void SendData(string data)
        {
            _serialService.Send(data);
        }

        public event PropertyChangedEventHandler? PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string name = "") =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
