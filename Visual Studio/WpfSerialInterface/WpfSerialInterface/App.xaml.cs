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
            var services = new ServiceCollection();
            services.AddSingleton<ISerialPortService, SerialPortService>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<MainWindow>();
            _serviceProvider = services.BuildServiceProvider();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
            mainWindow.Show();
        }
    }

}
