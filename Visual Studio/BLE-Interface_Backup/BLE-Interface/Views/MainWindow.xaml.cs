using System.Windows;

public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
        MainFrame.Navigate(new DeviceScanView());
    }
}