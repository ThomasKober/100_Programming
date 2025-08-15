using BLE_Interface.Views;
using Plugin.BLE;
using System.Windows;

namespace BLE_Interface
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            CrossBluetoothLE.Current.Adapter.ScanMode = Plugin.BLE.Abstractions.Contracts.ScanMode.LowLatency;
            var window = new DeviceScanView();
            window.Show();
            base.OnStartup(e);
        }
    }
}