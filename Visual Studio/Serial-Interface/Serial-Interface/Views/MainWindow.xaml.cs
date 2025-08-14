using Serial_Interface.ViewModels;
using System.Windows;

namespace Serial_Interface.Views
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly MainViewModel _viewModel = new();

        public MainWindow()
        {
            InitializeComponent();
            DataContext = _viewModel;
        }

        private void StartConnection_Click(object sender, RoutedEventArgs e) =>
            _viewModel.StartConnection();

        private void StopConnection_Click(object sender, RoutedEventArgs e) =>
            _viewModel.StopConnection();

        private void SendData_Click(object sender, RoutedEventArgs e) =>
            _viewModel.SendData(SendTextBox.Text);
    }
}
