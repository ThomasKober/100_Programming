using System;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Storage.Streams;

namespace Interface_BLE
{
    public partial class MainWindow : Window
    {
        private BluetoothLEDevice _device;
        private GattCharacteristic _characteristic;

        public MainWindow()
        {
            InitializeComponent();
        }

        // Event-Handler für den "BLE-Geräte scannen"-Button
        private async void ScanDevices_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                DeviceList.Items.Clear();

                // DeviceSelector für BLE-Geräte abrufen
                string[] requestedProperties = { "System.Devices.Aep.DeviceAddress", "System.Devices.Aep.IsConnected" };
                var devices = await DeviceInformation.FindAllAsync(
                    BluetoothLEDevice.GetDeviceSelectorFromPairingState(false),
                    requestedProperties);

                foreach (var deviceInfo in devices)
                {
                    Dispatcher.Invoke(() =>
                    {
                        DeviceList.Items.Add(deviceInfo);
                    });
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Fehler beim Scannen: {ex.Message}");
            }
        }

        // Event-Handler für den "Verbinden"-Button
        private async void Connect_Click(object sender, RoutedEventArgs e)
        {
            if (DeviceList.SelectedItem is DeviceInformation selectedDeviceInfo)
            {
                try
                {
                    // Verbinde mit dem ausgewählten Gerät
                    _device = await BluetoothLEDevice.FromIdAsync(selectedDeviceInfo.Id);

                    if (_device != null)
                    {
                        var servicesResult = await _device.GetGattServicesAsync();

                        if (servicesResult.Status == GattCommunicationStatus.Success)
                        {
                            var service = servicesResult.Services.FirstOrDefault(s => s.Uuid.ToString() == ServiceUuid.Text);
                            if (service != null)
                            {
                                var characteristicsResult = await service.GetCharacteristicsAsync();
                                if (characteristicsResult.Status == GattCommunicationStatus.Success)
                                {
                                    var characteristic = characteristicsResult.Characteristics.FirstOrDefault(c => c.Uuid.ToString() == CharacteristicUuid.Text);
                                    if (characteristic != null)
                                    {
                                        _characteristic = characteristic;
                                        _characteristic.ValueChanged += Characteristic_ValueChanged;
                                        var status = await _characteristic.WriteClientCharacteristicConfigurationDescriptorAsync(
                                            GattClientCharacteristicConfigurationDescriptorValue.Notify);

                                        if (status == GattCommunicationStatus.Success)
                                        {
                                            MessageBox.AppendText("Verbunden und Benachrichtigungen aktiviert.\n");
                                        }
                                        else
                                        {
                                            MessageBox.AppendText("Fehler beim Aktivieren der Benachrichtigungen.\n");
                                        }
                                    }
                                    else
                                    {
                                        MessageBox.AppendText("Characteristic nicht gefunden.\n");
                                    }
                                }
                                else
                                {
                                    MessageBox.AppendText("Fehler beim Abrufen der Characteristics.\n");
                                }
                            }
                            else
                            {
                                MessageBox.AppendText("Service nicht gefunden.\n");
                            }
                        }
                        else
                        {
                            MessageBox.AppendText("Fehler beim Abrufen der Services.\n");
                        }
                    }
                    else
                    {
                        MessageBox.AppendText("Fehler beim Verbinden mit dem Gerät.\n");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Fehler beim Verbinden: {ex.Message}");
                }
            }
        }

        // Event-Handler für das ValueChanged-Event des Characteristics
        private void Characteristic_ValueChanged(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            try
            {
                var data = new byte[args.CharacteristicValue.Length];
                DataReader.FromBuffer(args.CharacteristicValue).ReadBytes(data);
                var message = Encoding.UTF8.GetString(data, 0, data.Length);
                Dispatcher.Invoke(() => MessageBox.AppendText($"Empfangen: {message}\n"));
            }
            catch (Exception ex)
            {
                Dispatcher.Invoke(() => MessageBox.AppendText($"Fehler beim Empfangen: {ex.Message}\n"));
            }
        }

        // Event-Handler für den "Senden"-Button
        private async void Send_Click(object sender, RoutedEventArgs e)
        {
            if (_characteristic != null && !string.IsNullOrEmpty(MessageBox.Text))
            {
                try
                {
                    var data = Encoding.UTF8.GetBytes(MessageBox.Text);
                    var result = await _characteristic.WriteValueAsync(data.AsBuffer());

                    if (result == GattCommunicationStatus.Success)
                    {
                        MessageBox.AppendText($"Gesendet: {MessageBox.Text}\n");
                    }
                    else
                    {
                        MessageBox.AppendText("Fehler beim Senden.\n");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.AppendText($"Fehler beim Senden: {ex.Message}\n");
                }
            }
        }
    }
}