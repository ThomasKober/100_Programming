using System.Configuration;
using System.Data;
using System.Windows;
using WpfSerialInterface.Core.Interfaces;
using WpfSerialInterface.Core.Services;
using WpfSerialInterface.ViewModels;
using WpfSerialInterface.Views;
using Microsoft.Extensions.DependencyInjection;

namespace WpfSerialInterface
{
    public partial class App : Application
    {
        private readonly IServiceProvider _serviceProvider;

        public App()
        {
            var services = new ServiceCollection();
            services.AddSingleton<ISerialPortService, SerialPortService>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<MainWindow>();
            _serviceProvider = services.BuildServiceProvider();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // MainWindow und ViewModel aus dem DI-Container holen
            var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
            var mainViewModel = _serviceProvider.GetRequiredService<MainViewModel>();

            // ViewModel an das Fenster binden
            mainWindow.SetDataContext(mainViewModel);

            // Fenster anzeigen
            mainWindow.Show();
        }
    }
}
