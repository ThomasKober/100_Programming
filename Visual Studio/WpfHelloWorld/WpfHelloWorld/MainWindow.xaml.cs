using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfHelloWorld
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btn_Click(object sender, RoutedEventArgs e)
        {
            if(btnrHello.IsChecked == true)
            {
                MessageBox.Show("Hello World!");
            }
            else if(btnrGoodBy.IsChecked == true)
            {
                MessageBox.Show("Good Bye World!");
            }
            else
            {
                MessageBox.Show("Please select an option.");
            }
        }

        private void btnrHello_Checked(object sender, RoutedEventArgs e)
        {

        }

        private void btnrGoodBy_Checked(object sender, RoutedEventArgs e)
        {

        }
    }
}