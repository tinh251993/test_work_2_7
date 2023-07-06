using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.IO;
using System.Text.RegularExpressions;

namespace OthelloGame
{
    class GGFParser
    {
        const int MAX_GAME_COUNT = 100000;// int.MaxValue;
        public int g_gameCount = 0;
        public int Count = 0;

        public List<DecisionTree.Node[]> ParsedGames = new List<DecisionTree.Node[]>(1000000);
        public List<GameBoard> parsedGames_initGameBoards = new List<GameBoard>();
        public List<List<Square>> parsedGames_moves = new List<List<Square>>(); // Contain list of game records
        public List<List<List<Square>>> parsedGames_legalMoves = new List<List<List<Square>>>();

        private static bool checkGame(GameBoard gameBoard, List<Square> parsedMoves, out List<List<Square>> legalMoves)
        {
            legalMoves = new List<List<Square>>();
            gameBoard = gameBoard.Clone();
            int count = 0;
            foreach (Square move in parsedMoves)
            {
                count++;
                legalMoves.Add(gameBoard.GetLegalMoves());
                if (move.X < 1 || move.X > gameBoard.Width ||
                    move.Y < 1 || move.Y > gameBoard.Height){
                    Console.WriteLine("Invalid Move: {0}, {1}", move.X, move.Y);
                    return false;
                }
                if (!gameBoard.isLegalMove(move.X, move.Y))
                    return false;
                gameBoard.pushPiece(move.X, move.Y);
            }
            if (count > 60) 
                return false;
            return true;
        }

        /// <summary>
        /// Kết quả chạy Initialize được lưu vào file ggfExt
        /// Lần sau có thể chạy Initialize_WithGGFExtFile để chỉ đọc kết lên mà thôi
        /// </summary>
        /// <param name="fileName"></param>
        public void Parse(String fileName)
        {
            //---------------------------------------------------------
            // parse file và tạo node
            //---------------------------------------------------------
            int count = 0;
            StreamReader reader = new StreamReader(fileName);

            GameBoard temp_parsedGameBoard;
            List<Square> temp_parsedMoves;
            int lineIndex = 0;
            while (true)
            {
                //-----------------------------------------------------
                // đọc từng dòng trong file
                //-----------------------------------------------------
                String line = reader.ReadLine();
                if (line == null)
                    break;
                line = line.Trim();
                lineIndex++;

                //parse game line, if there are any error, ignore
                try
                {
                    ParseGame(line, out temp_parsedGameBoard, out temp_parsedMoves);                
                }
                catch (Exception e)
                {
                    Console.WriteLine("Format error at line: {0}. {1}", lineIndex, e.Message);
                    g_gameMiss++;
                    continue;
                    //throw caught;
                }

                //check if the parsed game is valid
                List<List<Square>> legalMoves;
                if (checkGame(temp_parsedGameBoard, temp_parsedMoves, out legalMoves))
                {
                    parsedGames_initGameBoards.Add(temp_parsedGameBoard);
                    parsedGames_moves.Add(temp_parsedMoves);
                    parsedGames_legalMoves.Add(legalMoves);
                    g_gameCount++;

                    //count the number of legal games
                    count += temp_parsedMoves.Count;

                    if (g_gameCount >= MAX_GAME_COUNT)
                        break;
                    continue;
                }
                g_gameMiss++;
            }//end while
        }

        public int g_gameMiss = 0;
        private void ParseGame(String strGame, out GameBoard initGameBoard, out List<Square> parsedMoves)
        {
            //-----------------------------------------------------
            // kiểm tra xem có bắt đầu và kết thúc bằng (; ;)
            //-----------------------------------------------------
            initGameBoard = null;
            parsedMoves = new List<Square>();

            if (strGame.StartsWith("1 ") || strGame.StartsWith("2 "))
                strGame = strGame.Substring(2);

            if (!strGame.StartsWith("(;") || !strGame.EndsWith(";)"))
                throw new ArgumentException("strGame does not contain any game");

            //-----------------------------------------------------
            // tách các cặp prop args ra
            //-----------------------------------------------------
            Regex regex = new Regex(@"(?<prop>[^\[]+)\[(?<args>[^\]]*)\]");
            MatchCollection parsedChunks = regex.Matches(strGame.Substring(2, strGame.Length - 4));
            foreach (Match match in parsedChunks)
            {
                String prop = match.Groups["prop"].Value;
                String args = match.Groups["args"].Value;

                if (prop == "BO")
                {
                    initGameBoard = ParseBOChunk(args);
                    if (initGameBoard == null)
                        throw new ArgumentException("ParseBO Chunk return null");
                    parsedMoves.Clear();
                }
                else if (prop == "B" || prop == "W")
                {
                    Square move = ParseBWChunk(args);

                    // nếu là pass move thì bỏ qua
                    if (move.X < 0 || move.Y < 0)
                        continue;

                    // ngược lại, thêm vào parsedMoves list
                    parsedMoves.Add(move);
                }
            }
        }

        //------------------------------------------------
        // parse Initial board setup
        //------------------------------------------------
        private GameBoard ParseBOChunk(String args)
        {
            // thông số của BO là mot chuoi cac segment, moi segment cach nhau bang khoan trang
            // segment thu nhat la kich thuoc cua ban co
            // cac segment con lai la cac day cac o cua ban co
            // segment cuoi cung cho biet day la nuoc di cua ai
            String[] segments = args.Split(' ');
            if (segments.Length < 2 || segments[0] != "8")
                return null;

            // kiem tra xem du lieu co hop le hay khong
            int boardSize;
            if (!int.TryParse(segments[0], out boardSize))
                return null;
            if (segments.Length < boardSize + 2)
                return null;
            for (int i = 1; i <= boardSize + 1; i++)
                if (segments[i].Length < boardSize)
                    segments[i] = segments[i] + new String('-', boardSize);

            // xay dung ban co
            GameBoard gameBoard = new GameBoard(boardSize, boardSize);
            for (int x = 1; x <= boardSize; x++)
                for (int y = 1; y <= boardSize; y++)
                {
                    char ch = segments[y][x - 1];

                    // xác định xem tại vị trí (x,y) là quân cờ gì
                    switch (ch)
                    {
                        case 'O':
                            gameBoard[x, y] = GameBoardBase.WHITE;
                            break;
                        case '*':
                            gameBoard[x, y] = GameBoardBase.BLACK;
                            break;
                        case '-':
                            gameBoard[x, y] = GameBoardBase.EMPTY;
                            break;
                        default:
                            return null;
                    }
                }

            switch (segments[boardSize + 1][0])
            {
                case 'O':
                    gameBoard.CurrentPlayer = gameBoard.Players[GameBoardBase.WHITE];
                    break;
                case '*':
                    gameBoard.CurrentPlayer = gameBoard.Players[GameBoardBase.BLACK];
                    break;
                default:
                    return null;
            }
            return gameBoard;
        }

        //------------------------------------------------
        // parse moves
        //------------------------------------------------
        private Square ParseBWChunk(String args)
        {
            // thông số của B|W là
            // o move
            // o evalution
            // o time
            String[] segments = args.Split('/');
            String strMove = segments[0];
            if (strMove.StartsWith("pass", StringComparison.CurrentCultureIgnoreCase)|| 
                strMove.StartsWith("pa", StringComparison.CurrentCultureIgnoreCase))
                return new Square(-1, -1);

            Square move = new Square(
                Char.ToLower(strMove[0]) - 'a' + 1,
                int.Parse(strMove.Substring(1)));


            return move;
        }
    }
}
