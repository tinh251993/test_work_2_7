using System;
using System.Collections.Generic;
using System.Text;
using OthelloGame.GameDatabase;
using OthelloGame.GamePattern;
using OthelloGame;

namespace OthelloGame.GamePattern
{
    class BTMMAlgorithm
    {
        public static List<PatternMining> g_patternMinings = new List<PatternMining>();
        //public static List<PatternMining> g_patternMinings2 = new List<PatternMining>();
        private static List<GameBoard> parsedGames_initGameBoards = new List<GameBoard>();
        private static List<List<Square>> parsedGames_moves = new List<List<Square>>(); // Contain list of game records
        private static List<List<List<Square>>> parsedGames_moves_legalMoves = null; //legalMoves corresponding to each of parsedGames_moves;
        public static int Count = 0;

        private static List<PatternDescriptor> Sym4(PatternDescriptor input)
        {
            List<PatternDescriptor> result = new List<PatternDescriptor>();
            result.Add(input);
            result.Add(input.CopyByFlipHorizontal());
            result.Add(input.CopyByFlipVertical());
            result.Add(input.CopyByFlipSubDiagonal());

            return result;
        }

        private static List<PatternDescriptor> Sym8(PatternDescriptor input)
        {
            List<PatternDescriptor> result = new List<PatternDescriptor>();
            PatternDescriptor input_ = input.CopyByFlipMainDiagonal();
            result.Add(input);
            result.Add(input.CopyByFlipHorizontal());
            result.Add(input.CopyByFlipVertical());
            result.Add(input.CopyByFlipSubDiagonal());
            result.Add(input_);
            result.Add(input_.CopyByFlipHorizontal());
            result.Add(input_.CopyByFlipVertical());
            result.Add(input_.CopyByFlipSubDiagonal());

            return result;
        }

        public static void Run()
        {
            //Load game database
            GameMiningDatabase.EnsureInitialized();
            parsedGames_initGameBoards = GameMiningDatabase.parsedGames_initGameBoards;
            parsedGames_moves = GameMiningDatabase.parsedGames_moves;
            parsedGames_moves_legalMoves = GameMiningDatabase.parsedGames_moves_legalMoves;

            //Tao danh sach cac pattern de huan luyen
            List<PatternDescriptor> patternDescs = new List<PatternDescriptor>();
            //List<PatternDescriptor> patternDescs2 = new List<PatternDescriptor>();
            List<PatternDescriptor> temp8 = new List<PatternDescriptor>();
            List<PatternDescriptor> temp4 = new List<PatternDescriptor>();
            PatternDescriptor temp;


            temp = PatternDescriptor.Parse("{A1, A2, A3, B1, B2, B3, C1, C2, C3}", "A1");
            temp4 = Sym4(temp);
            patternDescs.AddRange(temp4);
            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "A1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A1, B2, C3, D4, E5, F6, G7, H8, A2, B1}", "B2");
            temp4 = Sym4(temp);
            patternDescs.AddRange(temp4);
            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "B2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A3, B3, C3, D3, E3, F3, G3, H3}", "C3");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A1, B2, C3, D4, E5, F6, G7, H8}", "C3");
            temp4 = Sym4(temp);
            patternDescs.AddRange(temp4);


            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "B1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, B2, B3, B4, B5, B6, B7, B8}", "B1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "B1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);



            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "C1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, C2, C3, C4, C5, C6, C7, C8}", "C1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, D2, E3, F4, G5, H6}", "C1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "D1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{D1, D2, D3, D4, D5, D6, D7, D8}", "D1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A4, B3, C2, D1, E2, F3, G4, H5}", "D1");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "C2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, C2, C3, C4, C5, C6, C7, C8}", "C2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "C2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "D2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{D1, D2, D3, D4, D5, D6, D7, D8}", "D2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, D2, E3, F4, G5, H6}", "D2");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);

            temp = PatternDescriptor.Parse("{A3, B3, C3, D3, E3, F3, G3, H3}", "D3");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{D1, D2, D3, D4, D5, D6, D7, D8}", "D3");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "D3");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A6, B5, C4, D3, E2, F1}", "D3");
            temp8 = Sym8(temp);
            patternDescs.AddRange(temp8);

            //--------------------------------------------------------------//
            /*
            temp = PatternDescriptor.Parse("{A1, A2, A3, B1, B2, B3, C1, C2, C3}", "A1");
            temp4 = Sym4(temp);
            patternDescs2.AddRange(temp4);
            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "A1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A1, B2, C3, D4, E5, F6, G7, H8, A2, B1}", "B2");
            temp4 = Sym4(temp);
            patternDescs2.AddRange(temp4);
            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "B2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A3, B3, C3, D3, E3, F3, G3, H3}", "C3");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A1, B2, C3, D4, E5, F6, G7, H8}", "C3");
            temp4 = Sym4(temp);
            patternDescs2.AddRange(temp4);


            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "B1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, B2, B3, B4, B5, B6, B7, B8}", "B1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "B1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);



            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "C1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, C2, C3, C4, C5, C6, C7, C8}", "C1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, D2, E3, F4, G5, H6}", "C1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A1, B1, C1, D1, E1, F1, G1, H1}", "D1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, D1, F1, C2, D2, E2, F2, C4, D3}", "D1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A4, B3, C2, D1, E2, F3, G4, H5}", "D1");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "C2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, C2, C3, C4, C5, C6, C7, C8}", "C2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "C2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);


            temp = PatternDescriptor.Parse("{A2, B2, C2, D2, E2, F2, G2, H2}", "D2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{D1, D2, D3, D4, D5, D6, D7, D8}", "D2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{C1, D2, E3, F4, G5, H6}", "D2");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);

            temp = PatternDescriptor.Parse("{A3, B3, C3, D3, E3, F3, G3, H3}", "D3");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{D1, D2, D3, D4, D5, D6, D7, D8}", "D3");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{B1, C2, D3, E4, F5, G6, H7}", "D3");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);
            temp = PatternDescriptor.Parse("{A6, B5, C4, D3, E2, F1}", "D3");
            temp8 = Sym8(temp);
            patternDescs2.AddRange(temp8);*/

            foreach (PatternDescriptor patternDesc in patternDescs)
                g_patternMinings.Add(PatternMining.CreatePatternMining(patternDesc));

            //Huan luyen
            //int gameCount = parsedGames_initGameBoards.Count;
            WinGamma(5000, 100000);
            for (int i = 0; i < 20; i++)
            {
                Console.WriteLine("----------- Epoch {0} ------------", i + 1);
                calculateGammaTest(i, 0, 5000);
                calculateGamma(i, 5000, 100000);
            }

            /*
            identifyPatternCached = null;
            Count = 0;
            foreach (PatternDescriptor patternDesc in patternDescs2)
                g_patternMinings2.Add(PatternMining.CreatePatternMining(patternDesc));

            WinGamma(5000, 100000, g_patternMinings2);
            for (int i = 0; i < 20; i++)
            {
                calculateGammaTest(i, 0, 5000, g_patternMinings2);
                calculateGamma(i, 5000, 100000, g_patternMinings2);
            }*/

        }

        private static int Power3(int exp)
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

        // identify indices of pattern-minings that related to an action
        public static int[,][] identifyPatternCached;
        public static int[] identifyPattern(Square p)
        {
            if (identifyPatternCached == null)//chặn để không tính lại
            {
                identifyPatternCached = new int[10, 10][];
                for (int i = 1; i <= 8; i++)
                    for (int j = 1; j <= 8; j++)
                    {
                        List<int> patternIndexes = new List<int>();
                        for (int iPattern = 0; iPattern < g_patternMinings.Count; iPattern++)
                        {
                            PatternDescriptor patternDesc = g_patternMinings[iPattern].PatternDescriptors;
                            if (patternDesc.m_mustEmptyPosition == new Square(i,j))
                                patternIndexes.Add(iPattern);
                        }
                        identifyPatternCached[i, j] = patternIndexes.ToArray();
                    }
            }
            return identifyPatternCached[p.X, p.Y];
        }

        //Calculate strength of an action 
        //Product all values of features in the action.
        private static float strongOfAction(GameBoard gameBoard, Square p)
        {
            int[] relatedPatterns = identifyPattern(p);
            float strong = 1;
            int turn = gameBoard.CurrentPlayer.Turn - 1;
            for (int i = 0; i < relatedPatterns.Length; i++)
            {
                int pos = g_patternMinings[relatedPatterns[i]].PatternDescriptors.ElementIndexFromElementPosition(p);
                int patternCode = g_patternMinings[relatedPatterns[i]].PatternDescriptors.CalculatePatternCode(gameBoard);
                float gamma = g_patternMinings[relatedPatterns[i]].getGamma(patternCode, pos, turn);
                strong *= gamma;
            }
            return strong;
        }


        private static float Cij(float strong, int patID, int id, int index, int turn)
        {
            float gamma = g_patternMinings[patID].getGamma(id, index, turn);
            return strong / gamma;
        }

        //Calculate overall strength of participants in move j
        private static float calculateE(GameBoard gameBoard, List<Square> legalMoves)
        {
            float e = 0;
            foreach (Square p in legalMoves)
                e += strongOfAction(gameBoard, p);
            return e;
        }

        //--------------------------------------------------------------
        //Calculate Wi (win_count for each feature)
        private static void WinGamma(int begin, int end)
        {
            for (int iGame = begin; iGame < end; iGame++)
            {
                GameBoard gameBoard = parsedGames_initGameBoards[iGame].Clone();
                List<Square> game = parsedGames_moves[iGame]; // a game 

                foreach (Square move in game)
                {
                    List<Square> legalMoves = gameBoard.GetLegalMoves();
                    int turn = gameBoard.CurrentPlayer.Turn - 1;
                    if (legalMoves.Count > 1)
                    {
                        Count++;
                        foreach (Square p in legalMoves)
                        {
                            int[] ArrayOfPatterns = identifyPattern(p);
                            for (int i = 0; i < ArrayOfPatterns.Length; i++)
                            {
                                PatternMining pattern = g_patternMinings[ArrayOfPatterns[i]];
                                int pos = pattern.PatternDescriptors.ElementIndexFromElementPosition(p);
                                int patternCode = pattern.PatternDescriptors.CalculatePatternCode(gameBoard);
                                if (p.Equals(move))
                                    g_patternMinings[ArrayOfPatterns[i]].incWin(patternCode, pos, turn);
                                g_patternMinings[ArrayOfPatterns[i]].incCandidate(patternCode, pos, turn);
                            }
                        }
                    }
                    gameBoard.pushPiece(move.X, move.Y);
                }
            }
        }



        //-------------------------------------------------------------
        // Calculate and update all strengths of all features in which attent in N moves
        private static void calculateGamma(int loop, int begin, int end)
        {
            double likelihood = 0;
            double prob = 0;
            DateTime beginTime = DateTime.Now;
            Console.WriteLine();
            for (int iGame = begin; iGame < end; iGame++)
            {
#if !VERBOSE
                Console.SetCursorPosition(0, Console.CursorTop - 1);
#endif
                Console.WriteLine("Working with {0}", iGame);

                GameBoard gameBoard = parsedGames_initGameBoards[iGame].Clone();
                List<Square> game = parsedGames_moves[iGame]; // a game 
                List<Square> legalMoves = new List<Square>();

                for (int moveIdx = 0; moveIdx < game.Count; moveIdx++)
                {
                    Square move = game[moveIdx];
                    legalMoves = gameBoard.GetLegalMoves();//set of legal moves
                    int turn = gameBoard.CurrentPlayer.Turn - 1;
                    int n = legalMoves.Count;
                    if (n > 1)
                    {
                        float E = 0;//calculateE(gameBoard, legalMoves);
                        float[] strongLegalMoves = new float[n];
                        int iMove = -1;
                        for (int k = 0; k < n; k++)
                        {
                            strongLegalMoves[k] = strongOfAction(gameBoard, legalMoves[k]);
                            if (legalMoves[k].Equals(move))
                                iMove = k;
                            E += strongLegalMoves[k];
                        }
                        float val = strongLegalMoves[iMove] / E;
                        //double val = strongOfAction(gameBoard, move) / E;

                        //foreach (Point p in legalMoves)
                        for (int k = 0; k < n; k++)
                        {
                            int[] ArrayOfPatterns = identifyPattern(legalMoves[k]);
                            for (int i = 0; i < ArrayOfPatterns.Length; i++)
                            {
                                PatternMining pattern = g_patternMinings[ArrayOfPatterns[i]];
                                int pos = pattern.PatternDescriptors.ElementIndexFromElementPosition(legalMoves[k]);//get position of move if it is in empty position of pattern
                                int patternCode = pattern.PatternDescriptors.CalculatePatternCode(gameBoard);
                                //int patternCode = pattern.PatternDescriptors.CalculatePatternCodeCached(
                                //gameBoard, iGame * 1000 + moveIdx);
#if VERBOSE

                                //---------------------------------------------------
                                // In tất cả g tương ứng với move này ra
                                //---------------------------------------------------
                                PatternDescriptor pDesc = PatternArray[ArrayOfPatterns[i]].PatternDescriptors;
                                Console.WriteLine("\t\tPair({0}, {1}) = {2}", pDesc.HummanReadablePatternCode(patternCode), pos, PatternArray[i].Gamma[patternCode, pos, turn]);
                                GameLogger.BeginString(pDesc.HummanReadablePatternCode(patternCode), count, ArrayOfPatterns[i], patternCode, pos, PatternArray[i].Gamma[patternCode, pos, turn], win, candidate);
                                GameLogger.EndValue();
                                //----------------------------------------------------
#endif
                                int candidate = pattern.getCandidate(patternCode, pos, turn);
                                if (candidate > 10)
                                {
                                    float C = Cij(strongLegalMoves[k], ArrayOfPatterns[i], patternCode, pos, turn);
                                    g_patternMinings[ArrayOfPatterns[i]].addGammaDenominator(patternCode, pos, turn, C / E);
                                }
                            }
                        }
                        likelihood += Math.Log(val);
                        prob += val;
                    }
                    gameBoard.pushPiece(move.X, move.Y);
                }

            }
            Console.WriteLine("Likelihood = {0:0.0000000000000000000000000}", likelihood / Count);
            Console.WriteLine("Time spent = {0:0,000} ms", (DateTime.Now - beginTime).TotalMilliseconds);
            Console.WriteLine();
            GameLogger.BeginTest(loop, likelihood / Count, prob / Count);
            GameLogger.EndValue();

            //Calculate gamma after accumulating Cij/Ej for each feature
            for (int k = 0; k < g_patternMinings.Count; k++)
            {
                int len = g_patternMinings[k].Length;
                int iCard = Power3(len);
                for (int iPatID = 0; iPatID < iCard; iPatID++)
                    for (int index = 0; index < len; index++)
                    {
                        float denominator1 = g_patternMinings[k].getGammaDenominator(iPatID, index, 0);
                        if (denominator1 != 0)
                        {
                            int win = g_patternMinings[k].getWin(iPatID, index, 0);
                            double gamma = g_patternMinings[k].getGamma(iPatID, index, 0);
                            double gamma1 = Math.Pow(gamma, 0.75) * Math.Pow(win / denominator1, 0.25);
                            if (gamma1 < 0.01)
                                g_patternMinings[k].setGamma(iPatID, index, 0, (float)0.01);
                            else if (gamma1 > 100)
                                g_patternMinings[k].setGamma(iPatID, index, 0, 100);
                            else
                                g_patternMinings[k].setGamma(iPatID, index, 0, (float)gamma1);
                            g_patternMinings[k].resetGammaDenominator(iPatID, index, 0);
                        }

                        float denominator2 = g_patternMinings[k].getGammaDenominator(iPatID, index, 1);
                        if (denominator2 != 0)
                        {
                            int win = g_patternMinings[k].getWin(iPatID, index, 1);
                            double gamma = g_patternMinings[k].getGamma(iPatID, index, 1);
                            double gamma1 = Math.Pow(gamma, 0.75) * Math.Pow(win / denominator2, 0.25);
                            if (gamma1 < 0.01)
                                g_patternMinings[k].setGamma(iPatID, index, 1, (float)0.01);
                            else if (gamma1 > 100)
                                g_patternMinings[k].setGamma(iPatID, index, 1, 100);
                            else
                                g_patternMinings[k].setGamma(iPatID, index, 1, (float)gamma1);
                            g_patternMinings[k].resetGammaDenominator(iPatID, index, 1);
                        }
                    }
            }

        }


        private static void calculateGammaTest(int loop, int begin, int end)
        {
            double likelihood = 0;
            double prob = 0;
            DateTime beginTime = DateTime.Now;
            Console.WriteLine();
            int count = 0;
            for (int iGame = begin; iGame < end; iGame++)
            {
#if !VERBOSE
                Console.SetCursorPosition(0, Console.CursorTop - 1);
#endif
                Console.WriteLine("Working with {0}", iGame);

                GameBoard gameBoard = parsedGames_initGameBoards[iGame].Clone();
                List<Square> game = parsedGames_moves[iGame]; // a game 
                List<Square> legalMoves = new List<Square>();
                foreach (Square move in game)
                {
                    legalMoves = gameBoard.GetLegalMoves();//set of legal moves
                    int turn = gameBoard.CurrentPlayer.Turn - 1;
                    if (legalMoves.Count > 1)
                    {
                        count++;
                        double E = calculateE(gameBoard, legalMoves);
                        double val = strongOfAction(gameBoard, move) / E;
                        likelihood += Math.Log(val);
                        prob += val;
                    }
                    gameBoard.pushPiece(move.X, move.Y);
                }

            }
            Console.WriteLine("Likelihood = {0:0.0000000000000000000000000}", likelihood / count);
            Console.WriteLine("Time spent = {0:0,000} ms", (DateTime.Now - beginTime).TotalMilliseconds);
            Console.WriteLine();
            GameLogger.BeginTest(loop, likelihood / count, prob / count);
            GameLogger.EndValue();

        }

    }
}
