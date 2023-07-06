using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using OthelloGame.GamePattern;
using OthelloGame;

namespace OthelloGame.GamePattern
{

    public class PatternMining
    {
        /**************************************************************************
         * Hồi đó biến này tên là m_length
         * Lớp này dùng để chứa thông tin thống kê của tập các pattern, vì vậy
         * nếu chỉ để m_length không thì sẽ dẫn tới nhập nhằng, không biết
         * length là length của cái gì.
        ***************************************************************************/
        public String name;
        private int m_patternCommonLength;
        private PatternDescriptor m_patternDescriptors;

        private float[,,] m_gamma;      //m_gamma[pattern-code, pattern-element-idx]
        private ushort[,,] m_win;           //m_win[pattern-code, pattern-element-idx]
        private ushort[, ,] m_candidate;     //m_candidate[pattern-code, pattern-element-idx]
        private float[,,] m_gammaDenominator; //m_gammaDenominator[pattern-code, pattern-element-idx]


        public PatternMining(int patternCommonLength)
        {
            m_patternCommonLength = patternCommonLength;

            int iCard = Power3(patternCommonLength);
            m_gamma = new float[iCard, patternCommonLength, 2];
            m_gammaDenominator = new float[iCard, patternCommonLength, 2];
            m_win = new ushort[iCard, patternCommonLength, 2];
            m_candidate = new ushort[iCard, patternCommonLength, 2];
            

            for (int i = 0; i < iCard; i++)
                for (int j = 0; j < patternCommonLength; j++)
                {
                    m_gamma[i, j, 0] = 1;
                    m_gammaDenominator[i, j, 0] = 0;
                    m_win[i, j, 0] = 0;
                    m_candidate[i, j, 0] = 0;

                    m_gamma[i, j, 1] = 1;
                    m_gammaDenominator[i, j, 1] = 0;
                    m_win[i, j, 1] = 0;
                    m_candidate[i, j, 1] = 0;
                }
        }

        private int Power3(int exp)
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
                case 12: result = 531411;
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="strElements">The elements of the pattern in string from. Example: "{a1, b1, c1, d1, e1, f1}"</param>
        /// <param name="strEmptyPostion">The empty postion of the pattern in string from. Example: "A1"</param>
        public static PatternMining CreatePatternMining(PatternDescriptor patternDescriptor)
        {
            PatternMining patternMining = new PatternMining(patternDescriptor.m_elementPositions.Length);
            patternMining.m_patternDescriptors = patternDescriptor;
            return patternMining;
        }



        public int Length
        { 
            get { return m_patternCommonLength; }
            set { m_patternCommonLength = value; }
        }

        public float[,,] Gamma
        {
            get { return m_gamma; }
        }

        public float getGamma(int id, int index, int turn)
        {
            return m_gamma[id, index, turn];
        }

        public void setGamma(int id, int index, int turn, float value)
        {
            m_gamma[id, index, turn] = value;
        }

        public float getGammaDenominator(int id, int index, int turn)
        {
            return m_gammaDenominator[id, index, turn];
        }

        public void setGammaDenominator(int id, int index, int turn, float value)
        {
            m_gammaDenominator[id, index, turn] = value;
        }

        public void resetGammaDenominator(int id, int index, int turn)
        {
            m_gammaDenominator[id, index, turn] = 0;
        }

        public void addGammaDenominator(int id, int index, int turn, float value)
        {
            m_gammaDenominator[id, index, turn] += value;
        }

        public int getWin(int id, int index, int turn)
        {
            return m_win[id, index, turn];
        }

        public void incWin(int id, int index, int turn)
        {
            m_win[id, index, turn] ++;
        }

        public void resetWin(int id, int index, int turn)
        {
            m_win[id, index, turn] = 0;
        }

        public int getCandidate(int id, int index, int turn)
        {
            return m_candidate[id, index, turn];
        }

        public void incCandidate(int id, int index, int turn)
        {
            m_candidate[id, index, turn]++;
        }

        public PatternDescriptor PatternDescriptors
        {
            get { return m_patternDescriptors; }
        }

        public int[] getTurn(int id)
        {
            int[] turnSet = new int[m_patternCommonLength];

            for (int k = 0; k < m_patternCommonLength; k++)
                turnSet[k] = 0;
            int i = m_patternCommonLength;
            do
            {
                i--;
                turnSet[i] = id % 3;
                id = id / 3;
            } while (id != 0 && i != 0);

            return turnSet;
        }
 
        public override bool Equals(object obj)
        {
            if (!(obj is PatternMining))
                return false;
            return this == (PatternMining)obj;
        }

        public override int GetHashCode()
        {
            return 0;
        }

        public override string ToString()
        {
            return null;
        }

    }
}

