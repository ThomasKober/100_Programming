
using Microsoft.Extensions.DependencyInjection;
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
            var services = new ServiceCollection();
            ConfigureServices(services);
            _serviceProvider = services.BuildServiceProvider();
        }

        private void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ISerialPortService, SerialPortService>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<MainWindow>();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // Fenster und ViewModel über DI erstellen
            var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
            var mainViewModel = _serviceProvider.GetRequiredService<MainViewModel>();

            // DataContext setzen und Theme anwenden
            mainWindow.DataContext = mainViewModel;
            mainViewModel.UpdateTheme();  // Jetzt zugänglich!

            mainWindow.Show();
        }
    }
}
