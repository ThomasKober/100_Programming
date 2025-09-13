
using Microsoft.Extensions.DependencyInjection;
using System.Configuration;
using System.Data;
using System.Windows;
using WpfSerialInterface.Core.Interfaces;
using WpfSerialInterface.Core.Services;
using WpfSerialInterface.ViewModels;
using WpfSerialInterface.Views;

namespace WpfSerialInterface
{
    public partial class App : Application
    {
        private readonly IServiceProvider _serviceProvider;

        public App()
        {
            // Dependency Injection konfigurieren
            var services = new ServiceCollection();
            ConfigureServices(services);
            _serviceProvider = services.BuildServiceProvider();
        }

        private void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ISerialPortService, SerialPortService>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<MainWindow>(); // MainWindow als Singleton registrieren
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // MainWindow und ViewModel aus dem DI-Container holen
            var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
            var mainViewModel = _serviceProvider.GetRequiredService<MainViewModel>();

            // ViewModel als DataContext setzen
            mainWindow.DataContext = mainViewModel;

            // Fenster anzeigen
            mainWindow.Show();
        }
    }
}
