1. Vollständige Projektstruktur

WpfSerialInterface/
│
├── **Properties/**
│   ├── Settings.settings       # Benutzereinstellungen (z.B. DarkMode)
│   └── Settings.Designer.cs   # Automatisch generierter Code für Settings
│
├── **Core/**
│   ├── **Interfaces/**
│   │   └── ISerialPortService.cs  # Schnittstelle für serielle Kommunikation
│   │
│   └── **Services/**
│       └── SerialPortService.cs    # Implementierung der seriellen Kommunikation
│
├── **Models/**
│   └── SerialDataModel.cs      # (Leeres) Datenmodell für serielle Daten
│
├── **Utilities/**
│   ├── AsyncRelayCommand.cs    # Asynchrone Command-Implementierung
│   └── RelayCommand.cs         # Synchrone Command-Implementierung
│
├── **ViewModels/**
│   ├── **Base/**
│   │   └── ViewModelBase.cs    # Basis-Klasse für ViewModels (INotifyPropertyChanged)
│   │
│   └── MainViewModel.cs        # Haupt-ViewModel für die UI-Logik
│
├── **Views/**
│   └── MainWindow.xaml         # Hauptfenster (UI)
│
├── **Themes/**
│   ├── DarkTheme.xaml          # Dark-Mode-Styles/Ressourcen
│   └── LightTheme.xaml         # Light-Mode-Styles/Ressourcen
│
├── App.xaml                    # Anwendungseinstiegspunkt (DI-Container)
├── App.xaml.cs                 # Logik für App.xaml (Dependency Injection)
└── App.config                  # Konfiguration (Benutzereinstellungen)


2. Zusammenfassung der Funktionalität

Das Projekt ist eine WPF-Anwendung für serielle Kommunikation 
(z.B. mit Arduino, Mikrocontrollern oder anderen Geräten über COM-Ports). Die wichtigsten Features:


Kernfunktionen:


Serielle Verbindung:

Auflistung verfügbarer COM-Ports (automatische Aktualisierung alle 2 Sekunden).
Verbindung zu einem ausgewählten Port mit konfigurierbarer Baudrate (Standard: 9600).
Automatische Wiederverbindungsversuche bei Verbindungstrennung (max. 5 Versuche).
Senden/Empfangen von Daten als Text (Zeilenbasiert mit ReadLine/WriteLine).



UI & Benutzerfreundlichkeit:

Dark/Light-Mode: Theme-Umschaltung mit Speicherung der Einstellung (Settings.Default).
Echtzeit-Anzeige: Empfangene Daten werden in einem ScrollViewer angezeigt.
Statusmeldungen: Visualisierung des Verbindungsstatus (z.B. "Verbunden mit COM3").
Responsive Design: Modernes UI mit abgerundeten Buttons, Farbkontrast und dynamischen Ressourcen.



Architektur:

MVVM-Pattern: Trennung von UI (MainWindow.xaml), Logik (MainViewModel) und Dienst (SerialPortService).
Dependency Injection: Verwaltet Abhängigkeiten (z.B. ISerialPortService) über Microsoft.Extensions.DependencyInjection.
Asynchrone Operationen: Nicht-blockierende Verbindung/Datenübertragung mit Task und CancellationToken.
Event-basierte Kommunikation: Events wie DataReceived oder ConnectionChanged für Echtzeit-Updates.



Fehlerbehandlung:

Robuste Behandlung von Port-Fehlern (z.B. Zeitüberschreitungen, unerwartete Trennungen).
Debug-Ausgaben für Diagnose (via Debug.WriteLine).



Technische Highlights:

Automatische Port-Erkennung: Nutzt SerialPort.GetPortNames() für dynamische Aktualisierung.
Thread-Safety: UI-Updates werden über Application.Current.Dispatcher.Invoke sichergestellt.
Konfigurierbarkeit: Einstellungen (z.B. DarkMode) werden in App.config persistiert.
