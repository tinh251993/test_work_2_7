using System;
using System.Windows.Forms;
using System.Diagnostics;
using OthelloGame.GamePattern;

namespace OthelloGame
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            BTMMAlgorithm.Run();
            Application.Run(new FrmMain());

            // var parser = new GGFParser();
            // parser.Parse(@"E:\z-Reversi\Dataset\Othello.01e4.ggf\Othello.01e4.ggf");
            // Console.WriteLine($"Parsed {parser.g_gameCount} games.");
        }

    }
}
