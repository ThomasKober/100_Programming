using System;

public class LoggingService
{
    public void Log(string message)
    {
        // Log to file, console, or UI
        Console.WriteLine($"[BLE] {DateTime.Now}: {message}");
    }
}