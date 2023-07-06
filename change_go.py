WHITE = (255,255,255)
BLACK = (0,0,0)
GRAY = (0,0,0)

class Stone(object):
    def __init__(self, board, color, point):
        self.board = board
        self.point = point
        self.color = color
    def color(self):
        return self.color
    def point(self):
        return self.point
    def equal(self,point):
        if (point[0] == self.point[1]) and point[1] == self.point[0]:
            return True
        return False
    
    def remove(self):
        del self

class Board(object):
    def __init__(self, size):
        self.boardmatrix = [[]]
        self.next = BLACK
        self.black = []
        self.white = []
        self.gray = []
        self.graybutton = []
        self.size = size
        self.startboard(size)
        self.change = []

    def clear(self):
        self.next = BLACK
        self.boardmatrix.clear()
        for i in range(len(self.black)):
            self.black[i].remove()
        for i in range(len(self.white)):
            self.white[i].remove()

        self.black.clear()
        self.white.clear()

    def startboard(self,size):
        arr = []
        arr = [-1 for i in range(size)] 
        self.boardmatrix = [arr for i in range(size)]
        self.next = BLACK      
    
    def search(self, point):
        if self.boardmatrix[point[0] - 1][point[1] - 1] == -1 :                      
            return True
        return False

    def updateblack(self,x,y):
        # currenval = -1
        for i in range(len(self.black)):
            if self.black[i].equal((x,y)) == True:
               self.black.pop(i)
               break
        #        currenval = i
        #        break
        # print("den")
        # self.black.pop(currenval)            
        # print(len(self.black))            

    def updatewhite(self,x,y):
        # currenval = -1
        for i in range(len(self.white)):
            if self.white[i].equal((x,y)) == True:
               self.white.pop(i)
               break

        # self.white.pop(currenval)    
        # print("trang")               
        # print(len(self.white))               
            

    def retake(self, x,y, color):
        self.change.append((x,y))
        newarr = []
        newarr = [-1 for i in range(8)]
        if (color == 0):
            self.updateblack(x,y)
        if (color == 1):
            self.updatewhite(x,y)            
        for i in range(8):
            if ( i == x):
                for j in range(8):
                    val = self.boardmatrix[i][j]
                    newarr[j] = val 
                newarr[y] = color
                self.boardmatrix[i] = newarr


    def take_chess(self,point, color):
        x = point[0] -1
        y = point[1] -1

        #cheo duoi phai
        if (x + 1 < 8 and x + 1 > 0) and (y + 1 < 8 and y + 1 > 0):
            if (self.boardmatrix[x+1][y+1] == 1-color):
                left = x + 1
                right = y + 1
                while(left + 1 < 8 and right + 1 < 8 and self.boardmatrix[left][right] == 1 - color):
                    left = left + 1
                    right = right + 1
                if self.boardmatrix[left][right] == color:
                    begin = x + 1
                    end = y + 1

                    while(begin < left and end < right and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color)
                        begin = begin + 1
                        end = end + 1

        #cheo tren trai
        if (x - 1 >= 0 and x - 1 < 8 ) and (y - 1 >= 0 and y - 1 < 8):
            if (self.boardmatrix[x-1][y-1] == 1-color):
                left = x - 1
                right = y - 1
                while(left - 1 >= 0  and right - 1 >= 0 and self.boardmatrix[left][right] == 1 - color):
                    left = left - 1
                    right = right - 1

                if self.boardmatrix[left][right] == color:
                    begin = x - 1
                    end = y - 1

                    while(begin > left and end > right and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color)
                        begin = begin - 1
                        end = end - 1
        #cheo tren phai
        if (x - 1 >= 0 and x - 1 < 8) and (y + 1 < 8 and y + 1 > 0):
            if  (self.boardmatrix[x-1][y+1] == 1-color): 
                left = x - 1
                right = y + 1
                while(left-1 >= 0  and right+1 < 8 and self.boardmatrix[left][right] == 1 - color):
                    left = left - 1
                    right = right + 1

                if self.boardmatrix[left][right] == color:
                    begin = x - 1
                    end = y + 1

                    while(begin > left and end < right and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color) 
                        begin = begin - 1
                        end  = end + 1
        #cheo duoi trai
        if (x + 1 < 8 and x + 1 > 0) and (y - 1 >= 0 and y - 1 < 8) :
            if (self.boardmatrix[x+1][y-1] == 1-color):
                left = x + 1
                right = y - 1
                while(left+1 < 8  and right-1 >=  0 and self.boardmatrix[left][right] == 1 - color):
                    left = left + 1
                    right = right - 1

                if self.boardmatrix[left][right] == color:
                    begin = x + 1
                    end = y - 1

                    while(begin < left  and end > right and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color)
                        begin = begin + 1
                        end = end - 1
        # sang trai thang
        if (y - 1 >= 0 and x in range(0,7)) :
            if  (self.boardmatrix[x][y-1] == 1-color):
                left = x
                right = y - 1
                while(right-1 >=  0 and self.boardmatrix[left][right] == 1 - color):
                    right = right - 1

                if self.boardmatrix[left][right] == color:
                    begin = x
                    end = y - 1

                    while(end > right and self.boardmatrix[begin][end] == 1 - color):                                                   
                        self.retake(begin, end, color)
                        end = end - 1

        #sang phai thang 
        if (y + 1 < 8 and x in range(0,7)) :
            if (self.boardmatrix[x][y+1] == 1-color):
                left = x
                right = y + 1
                while(right+1 < 8  and self.boardmatrix[left][right] == 1 - color):
                    right = right + 1

                if self.boardmatrix[left][right] == color:
                    begin = x
                    end = y + 1

                    while(end < right  and self.boardmatrix[begin][end] == 1 - color):                                                   
                        self.retake(begin, end, color)
                        end = end + 1

        #len tren            
        if (x - 1 >= 0 and x - 1 < 8):
             if (self.boardmatrix[x-1][y] == 1-color):
                left = x - 1
                right = y 
                while(left - 1  >=0   and right -1 >=  0 and self.boardmatrix[left][right] == 1 - color):
                    left = left - 1

                if self.boardmatrix[left][right] == color:
                    begin = x - 1
                    end = y

                    while(begin > left  and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color)
                        begin = begin - 1

        #xuong duoi
        if (x + 1 < 8 and x + 1 >= 0) :
            if (self.boardmatrix[x+1][y] == 1-color):    
                left = x + 1
                right = y 
                while(left + 1 < 8  and right >=  0 and self.boardmatrix[left][right] == 1 - color):
                    left = left + 1

                if self.boardmatrix[left][right] == color:
                    begin = x + 1
                    end = y

                    while(begin < left  and self.boardmatrix[begin][end] == 1 - color):
                        self.retake(begin, end, color) 
                        begin = begin + 1      


    def takechange(self):
        return self.change
    
    def udpate(self, color, chess):
        if color == BLACK:
            self.black.append(chess)
        if color == WHITE:
            self.white.append(chess)

    def play(self, point, stone):
        self.change.clear()

        if self.next == BLACK:
            for i in range(8):
                if ( i == (point[0] - 1)):
                    arr = []
                    for j in range(8):
                        arr.append(self.boardmatrix[i][j])
                    arr[point[1]-1] = 1
                    self.boardmatrix[i] = arr 
            self.black.append(stone)            
            self.take_chess(point, 1)
            self.showsuggestion()
            self.next = WHITE
        else:
            if self.next == WHITE:
                for i in range(8):
                    if ( i == (point[0] - 1)):
                        arr = []
                        for j in range(8):
                            arr.append(self.boardmatrix[i][j])
                        arr[point[1]-1] = 0
                        self.boardmatrix[i] = arr
            self.white.append(stone)            
            self.take_chess(point, 0)
            self.showsuggestion()
            self.next = BLACK

    def checkvalue(self, x,y, color):
        #check tren 
        if x - 1 >= 0:
            if self.boardmatrix[x-1][y] == -1:
                begin = x
                end = y
                check = False
                while begin < 8 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    begin = begin + 1

                if (check == True) and (x-1,y) not in self.gray:
                    self.gray.append((x-1,y))    

        #check duoi
        if x + 1 < 8:
            if self.boardmatrix[x+1][y] == -1:
                begin = x
                end = y
                check = False
                while begin >= 0 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    begin = begin - 1

                if (check == True) and (x+1,y) not in self.gray:
                    self.gray.append((x+1,y))         
        #check trai
        if y - 1 >= 0:
            if self.boardmatrix[x][y-1] == -1:
                begin = x
                end = y
                check = False
                while end + 1 < 8  and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    end = end + 1

                if (check == True) and (x,y-1) not in self.gray:
                    self.gray.append((x,y-1))         
        #check phai
        if y + 1 < 8 :
            if self.boardmatrix[x][y+1] == -1:
                begin = x
                end = y
                check = False
                while end < 8 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    end = end - 1

                if (check == True) and (x,y+1) not in self.gray:
                    self.gray.append((x,y+1))        
        #check cheo tren phai
        if y + 1 < 8  and x - 1 >= 0 :
            if self.boardmatrix[x-1][y+1] == -1:
                begin = x
                end = y
                check = False
                while begin < 8 and end >=  0 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    begin = begin + 1
                    end = end - 1

                if (check == True) and (x-1,y+1) not in self.gray:
                    self.gray.append((x-1,y+1))         
        #check cheo duoi phai
        if y + 1 < 8  and x + 1 < 8 :
            if self.boardmatrix[x+1][y+1] == -1:
                begin = x
                end = y
                check = False
                while begin >= 0 and end >=  0 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    begin = begin - 1
                    end = end - 1

                if (check == True) and (x+1,y+1) not in self.gray:
                    self.gray.append((x+1,y+1))        
        #check cjep tren trai
        if y - 1 >= 0  and x - 1 >= 0 :
            if self.boardmatrix[x-1][y-1] == -1:
                begin = x
                end = y
                check = False
                while begin < 8 and end < 8 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1-color:
                        check = True
                        break
                    begin = begin + 1
                    end = end + 1

                if (check == True) and (x-1,y-1) not in self.gray:
                    
                    self.gray.append((x-1,y-1))        
        #check cheo duoi trai 
        if y - 1 >= 0  and x + 1 < 8 :
            if self.boardmatrix[x+1][y-1] == -1:
                begin = x
                end = y
                check = False
                while begin >= 0 and end < 8 and check == False:
                    if self.boardmatrix[begin][end] == -1:
                        break
                    if self.boardmatrix[begin][end] == 1 -color:
                        check = True
                        break
                    begin = begin - 1
                    end = end + 1

                if (check == True) and (x+1,y-1) not in self.gray:
                    self.gray.append((x+1,y-1))                    

    def clearsuggestion(self):
        for i in range(len(self.graybutton)):
            self.graybutton[i].remove()
        self.graybutton.clear()            
        self.gray.clear()

    def showsuggestion(self):
        val = 1
        if self.next == WHITE:
            val = 0          
        for i in range(8):
            for j in range(8):
                if self.boardmatrix[i][j] == val:
                    self.checkvalue(i,j,val)                     
                    
    def suggestion(self, stone):
        self.graybutton.append(stone)

    def listsuggestion(self):
        return self.gray
    
    def check(self,x,y):
        if (x-1,y-1) in self.gray:
            return True
        return False
    

    def nextturn(self):
        return self.next

