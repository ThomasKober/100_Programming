using System;
using Serilog.Events;

namespace WpfSerialInterfaceWithProtocol.Core.Interfaces
{
    public interface ILogService
    {
        void SetLogLevel(LogEventLevel level);

        // Simple message methods with optional category
        void Trace(string message, string category = "Default");
        void Debug(string message, string category = "Default");
        void Info(string message, string category = "Default");
        void Warning(string message, string category = "Default");
        void Error(string message, string category = "Default");
        void Fatal(string message, string category = "Default");

        // Template methods with parameters (Serilog structured logging)
        void TraceTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);
        void DebugTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);
        void InfoTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);
        void WarningTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);
        void ErrorTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);
        void FatalTemplate(string messageTemplate, string category = "Default", params object[] propertyValues);

        // Exception methods
        void Error(Exception exception, string message, string category = "Default");
        void ErrorTemplate(Exception exception, string messageTemplate, string category = "Default", params object[] propertyValues);
        void Fatal(Exception exception, string message, string category = "Default");
        void FatalTemplate(Exception exception, string messageTemplate, string category = "Default", params object[] propertyValues);
    }
}