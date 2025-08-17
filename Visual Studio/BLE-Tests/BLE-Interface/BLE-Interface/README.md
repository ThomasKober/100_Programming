BLE-Interface/
│
├── BLE-Interface/                  # Main WPF Application
│   ├── Models/                     # Data models for BLE devices and services
│   │   ├── BleDevice.cs            # Represents a BLE device
│   │   ├── BleService.cs           # Represents a BLE service
│   │   └── BleCharacteristic.cs    # Represents a BLE characteristic
│   │
│   ├── ViewModels/                 # MVVM ViewModels
│   │   ├── MainViewModel.cs        # Main window logic
│   │   ├── DeviceScanViewModel.cs  # Handles device scanning
│   │   └── DeviceDetailViewModel.cs# Handles device details
│   │
│   ├── Views/                      # WPF Views (XAML + code-behind)
│   │   ├── MainWindow.xaml         # Main application window
│   │   ├── DeviceScanView.xaml     # Device scanning UI
│   │   └── DeviceDetailView.xaml   # Device details UI
│   │
│   ├── Services/                   # BLE and utility services
│   │   ├── BleScannerService.cs    # Handles BLE scanning
│   │   ├── BleConnectionService.cs # Handles BLE connections
│   │   └── LoggingService.cs       # Logs BLE events
│   │
│   ├── Utilities/                  # Helper classes
│   │   ├── Extensions.cs           # Extension methods
│   │   └── Constants.cs            # App constants
│   │
│   ├── App.xaml                    # WPF application entry point
│   └── App.xaml.cs                 # App startup logic
│
├── BLE-Interface.Tests/            # Unit tests (optional)
│   ├── ViewModels/
│   └── Services/
│
└── README.md                       # Project documentation

