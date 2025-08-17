using BLE_Interface.ViewModels;
using Plugin.BLE;
using System.Windows;

namespace BLE_Interface.Views
{
    public partial class DeviceScanView : Window
    {
        public DeviceScanView()
        {
            InitializeComponent();
            DataContext = new DeviceScanViewModel(CrossBluetoothLE.Current.Adapter);
        }
    }
}