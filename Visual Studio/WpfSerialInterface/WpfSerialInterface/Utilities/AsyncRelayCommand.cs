using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace WpfSerialInterface.Utilities
{
    public class AsyncRelayCommand : ICommand
    {
        private readonly Func<Task> _execute;
        private readonly Func<bool>? _canExecute;
        private bool _isExecuting;

        public AsyncRelayCommand(Func<Task> execute, Func<bool> canExecute)
        {
            _execute = execute;
            _canExecute = canExecute;
        }

        public bool CanExecute(object parameter) => !_isExecuting && _canExecute();

        public async void Execute(object parameter)
        {
            if (CanExecute(parameter))
            {
                _isExecuting = true;
                try
                {
                    await _execute();
                }
                finally
                {
                    _isExecuting = false;
                    CommandManager.InvalidateRequerySuggested(); // Wichtig für UI-Updates!
                }
            }
        }

        public event EventHandler CanExecuteChanged
        {
            add => CommandManager.RequerySuggested += value;
            remove => CommandManager.RequerySuggested -= value;
        }
    }
}
