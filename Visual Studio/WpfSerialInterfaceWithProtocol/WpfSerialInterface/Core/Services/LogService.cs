using Serilog;
using Serilog.Core;
using Serilog.Events;
using System;
using WpfSerialInterfaceWithProtocol.Core.Interfaces;

namespace WpfSerialInterfaceWithProtocol.Core.Services
{
    public class LogService : ILogService
    {
        private readonly ILogger _logger;
        private readonly LoggingLevelSwitch _levelSwitch;

        public LogService()
        {
            _levelSwitch = new LoggingLevelSwitch(LogEventLevel.Information);
            _logger = new LoggerConfiguration()
                .MinimumLevel.ControlledBy(_levelSwitch)
                .Enrich.WithProperty("Application", "SerialInterface")
                .WriteTo.Seq("http://localhost:5341")
                .WriteTo.File("logs/log-.txt", rollingInterval: RollingInterval.Day, retainedFileCountLimit: 7)
                .CreateLogger();
        }

        public void SetLogLevel(LogEventLevel level) => _levelSwitch.MinimumLevel = level;

        // Simple message methods
        public void Trace(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Verbose(message);

        public void Debug(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Debug(message);

        public void Info(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Information(message);

        public void Warning(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Warning(message);

        public void Error(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Error(message);

        public void Fatal(string message, string category = "Default") =>
            _logger.ForContext("Category", category).Fatal(message);

        // Template methods with parameters (Serilog structured logging)
        public void TraceTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Verbose(messageTemplate, propertyValues);

        public void DebugTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Debug(messageTemplate, propertyValues);

        public void InfoTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Information(messageTemplate, propertyValues);

        public void WarningTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Warning(messageTemplate, propertyValues);

        public void ErrorTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Error(messageTemplate, propertyValues);

        public void FatalTemplate(string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Fatal(messageTemplate, propertyValues);

        // Exception methods
        public void Error(Exception exception, string message, string category = "Default") =>
            _logger.ForContext("Category", category).Error(exception, message);

        public void ErrorTemplate(Exception exception, string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Error(exception, messageTemplate, propertyValues);

        public void Fatal(Exception exception, string message, string category = "Default") =>
            _logger.ForContext("Category", category).Fatal(exception, message);

        public void FatalTemplate(Exception exception, string messageTemplate, string category = "Default", params object[] propertyValues) =>
            _logger.ForContext("Category", category).Fatal(exception, messageTemplate, propertyValues);
    }
}