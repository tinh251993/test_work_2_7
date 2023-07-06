using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace OthelloGame.GamePattern
{
    public class PatternDescriptor
    {
        public Square[] m_elementPositions;
        public Square m_mustEmptyPosition;

        public PatternDescriptor(Square[] elementPositions, Square mustEmptyPosition)
        {
            m_elementPositions = elementPositions;
            m_mustEmptyPosition = mustEmptyPosition;
        }

        public override string ToString()
        {
            return String.Format("[Len={0}, AppearRatio={1:f3}], Scatter={2:f3}, Uniform1={3:f3}, Uniform2={4:f3}, Sets={5}",
                m_elementPositions.Length);
        }

        /// <summary>
        /// Kiem tra xem pattern hien tai co phai la cha (chua) pattern obj hay khong.
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public bool Contains(PatternDescriptor obj)
        {
            if (this.m_mustEmptyPosition.X != obj.m_mustEmptyPosition.X 
                || this.m_mustEmptyPosition.Y != obj.m_mustEmptyPosition.Y)
                return false;
            foreach (Square pos in obj.m_elementPositions)
                if (Array.IndexOf(this.m_elementPositions, pos) < 0)
                    return false;
            return true;
        }

        public static int Power3(int exp)
        {
            int result = 1;
            switch (exp)
            {
                case 1: result = 3;
                    break;
                case 2: result = 9;
                    break;
                case 3: result = 27;
                    break;
                case 4: result = 81;
                    break;
                case 5: result = 243;
                    break;
                case 6: result = 729;
                    break;
                case 7: result = 2187;
                    break;
                case 8: result = 6561;
                    break;
                case 9: result = 19683;
                    break;
                case 10: result = 59049;
                    break;
                case 11: result = 177147;
                    break;
                case 12: result = 531441;
                    break;
                case 13: result = 1594323;
                    break;
                case 14: result = 4782969;
                    break;
                case 15: result = 14348907;
                    break;
                case 16: result = 43046721;
                    break;
                default:
                    break;

            }
            return result;
        }

        private int CalculatePatternCodeFromPatternElements(int[] patternElements)
        {
            int result = 0;
            int i, len = m_elementPositions.Length;
            for (i = 0; i < len; i++)
                result += (patternElements[len - i - 1] * Power3(i));
            return result;
        }


        public int CalculatePatternCode(GameBoard gameBoard)
        {
            int turn = gameBoard.CurrentPlayer.Turn - 1;
            int len = m_elementPositions.Length;
            int[] pattern = new int[len];
            for (int i = 0; i < len; i++)
                    pattern[i] = gameBoard[m_elementPositions[i].X, m_elementPositions[i].Y];
            return CalculatePatternCodeFromPatternElements(pattern);
        }

        public bool FindPatternInGameBoard(GameBoard gameBoard, int[] patternElements)
        {
            for (int i = 0; i < m_elementPositions.Length; i++)
                if (patternElements[i] != gameBoard[m_elementPositions[i].X, m_elementPositions[i].Y]) 
                    return false;
            return true;
        }

        public int ElementIndexFromElementPosition(Square elementPosition)
        {
            return Array.IndexOf(m_elementPositions, elementPosition);
        }

        public String HummanReadablePatternCode(int patternCode)
        {
            StringBuilder str = new StringBuilder(this.m_elementPositions.Length);
            for (int i = 0; i < m_elementPositions.Length; i++)
            {
                str.Insert(0, patternCode % 3);
                patternCode /= 3;
            }
            return str.ToString();
        }

        public static PatternDescriptor Parse(String strElementPositions, String strMustEmptyPosition)
        {
            List<Square> elementPositions = new List<Square>();
            Square mustEmptyPosition = new Square();
            //parse element postions
            strElementPositions = strElementPositions.Replace("{", "").Replace("}", "");
            foreach (String strElementPosition in strElementPositions.Split(','))
                elementPositions.Add(Square.Parse(strElementPosition));
            mustEmptyPosition = Square.Parse(strMustEmptyPosition);
            return new PatternDescriptor(elementPositions.ToArray(), mustEmptyPosition);
        }

        /// <summary>
        /// Create a new pattern by flipping the pattern over the horizontal central line
        /// </summary>
        /// <returns></returns>
        public PatternDescriptor CopyByFlipHorizontal()
        {
            Square[] elementPositions = new Square[this.m_elementPositions.Length];
            Square mustEmptyPosition = this.m_mustEmptyPosition;
            for (int i = 0; i < elementPositions.Length; i++)
            {
                Square square = this.m_elementPositions[i];
                square.Y = (sbyte)(8 - square.Y + 1);
                elementPositions[i] = square;
            }
            mustEmptyPosition.Y = (sbyte)(8 - mustEmptyPosition.Y + 1);

            return new PatternDescriptor(elementPositions, mustEmptyPosition);
        }

        public PatternDescriptor CopyByFlipVertical()
        {
            Square[] elementPositions = new Square[this.m_elementPositions.Length];
            Square mustEmptyPosition = this.m_mustEmptyPosition;
            for (int i = 0; i < elementPositions.Length; i++)
            {
                Square square = this.m_elementPositions[i];
                square.X = (sbyte)(8 - square.X + 1);
                elementPositions[i] = square;
            }
            mustEmptyPosition.X = (sbyte)(8 - mustEmptyPosition.X + 1);

            return new PatternDescriptor(elementPositions, mustEmptyPosition);

        }

        public PatternDescriptor CopyByFlipMainDiagonal()
        {
            sbyte temp;
            Square[] elementPositions = new Square[this.m_elementPositions.Length];
            Square mustEmptyPosition = this.m_mustEmptyPosition;
            for (int i = 0; i < elementPositions.Length; i++)
            {
                Square square = this.m_elementPositions[i];
                temp = square.X;
                square.X = square.Y;
                square.Y = temp;
                elementPositions[i] = square;
            }

            temp = mustEmptyPosition.X;
            mustEmptyPosition.X = mustEmptyPosition.Y;
            mustEmptyPosition.Y = temp;
            return new PatternDescriptor(elementPositions, mustEmptyPosition);
        }

        public PatternDescriptor CopyByFlipSubDiagonal()
        {
            sbyte temp;
            Square[] elementPositions = new Square[this.m_elementPositions.Length];
            Square mustEmptyPosition = this.m_mustEmptyPosition;
            for (int i = 0; i < elementPositions.Length; i++)
            {
                Square square = this.m_elementPositions[i];
                temp = (sbyte)(8 - square.X + 1);
                square.X = (sbyte)(8 - square.Y + 1);
                square.Y = temp;
                elementPositions[i] = square;
            }

            temp = (sbyte)(8 - mustEmptyPosition.X + 1);
            mustEmptyPosition.X = (sbyte)(8 - mustEmptyPosition.Y + 1);
            mustEmptyPosition.Y = temp;
            return new PatternDescriptor(elementPositions, mustEmptyPosition);
        }
    }
}
