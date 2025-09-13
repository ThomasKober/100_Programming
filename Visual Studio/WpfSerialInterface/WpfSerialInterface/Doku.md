Projektstruktur

WpfSerialInterface/
├── Core/
│   ├── Interfaces/
│   │   └── ISerialPortService.cs          // Interface für die serielle Kommunikation
│   └── Services/
│       └── SerialPortService.cs           // Implementierung des SerialPortService
│
├── Models/
│   └── SerialDataModel.cs                 // (Optional) Datenmodell für empfangene/sendende Daten
│
├── Utilities/
│   ├── RelayCommand.cs                    // Synchroner RelayCommand
│   └── AsyncRelayCommand.cs               // Asynchroner RelayCommand für Task-Unterstützung
│
├── ViewModels/
│   ├── Base/
│   │   └── ViewModelBase.cs               // Basisklasse für ViewModels (INotifyPropertyChanged)
│   └── MainViewModel.cs                   // Haupt-ViewModel mit Logik für die UI
│
├── Views/
│   └── MainWindow.xaml                    // UI-Definition (XAML)
│
├── App.xaml                               // WPF-Anwendungseinstieg
├── App.xaml.cs                            // Konfiguration der Dependency Injection
│
└── Properties/
    └── ...                                // Standard-Eigenschaften (AssemblyInfo, etc.)
