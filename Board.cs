using System;
using System.Linq;

namespace HoangluuHoangtran_Reversi
{
    public class Board
    {
        // These arrays represents the constant label for squares.
        public static readonly char[] SqrLabelV = {'1', '2', '3', '4', '5', '6', '7', '8'};
        public static readonly char[] SqrLabelH = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

        // These constants represent the contents of a board square.
        public static readonly int Black = -1;
        public static readonly int Empty = 0;
        public static readonly int White = 1;

        // This two-dimensional array represents the squares on the board.
        public int[,] Squares = new int[8, 8];

        public Board()
        {
        }

        /// <summary>
        /// Get Label of play position
        /// </summary>
        /// <param name="row">Vertical Index</param>
        /// <param name="col">Horizontal Index</param>
        /// <returns></returns>
        public static string GetPositionLabel(int row, int col)
        {
            return $"{SqrLabelV[row]}{SqrLabelH[col]}";
        }

        public void SetForNewGame()
        {
            // Clear the board.
            for (var i = 0; i < 8; i++)
            {
                for (var j = 0; j < 8; j++)
                {
                    this.Squares[i, j] = Board.Empty;
                }
            }

            // Set two black and two white discs in the center.
            this.Squares[3, 3] = White;
            this.Squares[3, 4] = Black;
            this.Squares[4, 3] = Black;
            this.Squares[4, 4] = White;
        }

        public void PlacePiece(int color, int row, int col)
        {
            this.Squares[row, col] = color;
        }

        public void PrintBoard()
        {
            foreach (var jLabel in SqrLabelH.Prepend(' '))
            {
                Console.Write($" {jLabel} ");
            }

            Console.Write(Environment.NewLine);

            for (var i = 0; i < 8; i++)
            {
                Console.Write($" {SqrLabelV[i]} ");

                for (var j = 0; j < 8; j++)
                {
                    if (this.Squares[i, j] == Black)
                    {
                        Console.Write(" □ ");
                    }
                    else if (this.Squares[i, j] == White)
                    {
                        Console.Write(" ■ ");
                    }
                    else
                    {
                        Console.Write(" . ");
                    }
                }

                Console.Write(Environment.NewLine);
            }

            Console.Write(Environment.NewLine);
        }
    }
}