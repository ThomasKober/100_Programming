using Microsoft.Extensions.DependencyInjection;
using Serilog;
using WpfSerialInterfaceWithProtocol.Core.Interfaces;
using WpfSerialInterfaceWithProtocol.Core.Services;
using WpfSerialInterfaceWithProtocol.ViewModels;
using WpfSerialInterfaceWithProtocol.Views;
using System.Windows;
using System;

namespace WpfSerialInterfaceWithProtocol
{
    public partial class App : Application
    {
        private IServiceProvider _serviceProvider;

        public App()
        {
            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                .WriteTo.Seq("http://localhost:5341")
                .WriteTo.File("logs/log-.txt", rollingInterval: RollingInterval.Day, retainedFileCountLimit: 7)
                .CreateLogger();

            var services = new ServiceCollection();
            ConfigureServices(services);
            _serviceProvider = services.BuildServiceProvider();
        }

        private void ConfigureServices(IServiceCollection services)
        {
            services.AddSingleton<ILogService, LogService>();
            services.AddSingleton<ISerialPortService, SerialPortService>();
            services.AddSingleton<MainViewModel>();
            services.AddSingleton<MainWindow>();
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            try
            {
                var mainWindow = _serviceProvider.GetRequiredService<MainWindow>();
                var mainViewModel = _serviceProvider.GetRequiredService<MainViewModel>();
                mainWindow.DataContext = mainViewModel;
                mainWindow.Show();

                Log.Information("Application started successfully");
            }
            catch (Exception ex)
            {
                Log.Fatal(ex, "Application failed to start");
                MessageBox.Show($"Application failed to start: {ex.Message}", "Error",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                Shutdown(1);
            }
        }

        protected override void OnExit(ExitEventArgs e)
        {
            try
            {
                // Dispose ViewModel (which will unsubscribe from events and dispose services)
                var mainViewModel = _serviceProvider.GetService<MainViewModel>();
                mainViewModel?.Dispose();

                // Dispose the service provider itself
                if (_serviceProvider is IDisposable disposable)
                {
                    disposable.Dispose();
                }

                Log.Information("Application shutting down");
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Error during application shutdown");
            }
            finally
            {
                Log.CloseAndFlush();
                base.OnExit(e);
            }
        }
    }
}