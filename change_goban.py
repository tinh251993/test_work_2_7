import pygame
import math
import go
import tkinter
import time
import tkinter.filedialog
from sys import exit
from datetime import datetime
from change_go import *
from threading import Thread
import threading

import asyncio

BACKGROUND = 'images/Background.png'
BOARD_SIZE = (920, 920)
is_pause = False
can_go_next = False
is_new_game = True
is_stop_thread = False
is_data_mining = False
total_match = 0

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
play_white = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

n_play_white = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

play_black = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

n_play_black = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

suggest_white = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

suggest_black = [
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0],
    [0,0,0,0,0,0,0,0]
]

def init_array():
    global total_match, play_black, play_white, suggest_black, suggest_white, n_play_white, n_play_black
    total_match = 0
    for i in range(8):
        for j in range(8):
            play_white[i][j] = 0
            play_black[i][j] = 0
            suggest_black[i][j] = 0
            suggest_white[i][j] = 0
            n_play_black[i][j] = 0
            n_play_white[i][j] = 0

def play_from_file(board, line_reader, str_leng):
   # print("do nothing")
    global is_new_game, is_stop_thread
    
    global is_pause, can_go_next
    is_pause = True 
    
    for i in range(0, str_leng - 2, 2):
        while is_pause == True:
            if is_stop_thread == True:
                is_stop_thread = False
                return
            if is_new_game == True:
                return
            if can_go_next == True:
                can_go_next = False
                break

            time.sleep(0.2)


        first =  int(ord(line_reader[i]) - ord("A") + 1)
        second = int(ord(line_reader[i+1]) - ord('0'))
                    
        if first in range(1,9) and second in range(1,9):
            if is_new_game == True:
                return
            
            add_stone(board, (second, first))
            time.sleep(1)

def playfromfilemining(board, line_reader, str_leng, winner):
    #print(line_reader)
    for i in range(0, str_leng - 2, 2):
        first =  int(ord(line_reader[i]) - ord("A") + 1)
        second = int(ord(line_reader[i+1]) - ord('0'))
                                
        add_stone_data(board, (second, first),winner)


def pause():
    global is_pause
    is_pause = True

def next_turn():
    global is_pause, can_go_next
    if is_pause == True:
        can_go_next = True


class Stone(Stone):
    def __init__(self, board, color, point, is_draw):
        super().__init__(board, color, point)
        self.is_draw = is_draw
        if is_draw == True:
            self.coords = (5 + self.point[0] * 40 + 20, 5 + self.point[1] * 40 + 20)
            self.draw()



    def draw(self):
        pygame.draw.circle(screen, self.color, self.coords, 18, 0)

        if (self.color == (211,211,211)):
            
            first = chr(ord('A') + self.point[0] - 1)
            text =  first + str(self.point[1])
            font_obj = pygame.font.Font('freesansbold.ttf', 14)        
            newgame_Text = font_obj.render(text, True, (0, 0, 0), (211,211,211))
            newgame_pos = newgame_Text.get_rect()
            newgame_pos.center = (5 + self.point[0] * 40 + 20, 5 + self.point[1] * 40 + 20)
            screen.blit(newgame_Text, newgame_pos)
            # test_Text = font_obj.render(chr(65 + i), True, (0, 0, 0))
            # test_pos = test_Text.get_rect()
            # test_pos.center = (65 + 40 * i, 35)
            # screen.blit(test_Text, test_pos)
        
        pygame.display.update()

    def color(self):
        return super().color()
    
    def remove(self):
        if self.is_draw == True:
            blit_coords = (self.coords[0] - 20, self.coords[1] - 20)
            area_rect = pygame.Rect(blit_coords, (40, 40))
            screen.blit(background, blit_coords, area_rect)
            pygame.display.update()

        super().remove()

class Board(Board):
    def __init__(self, size):
        super().__init__(size)
        self.outline = pygame.Rect(45, 45, 320, 320)
        self.draw()

    def draw(self):
        pygame.draw.rect(background, BLACK, self.outline, 3)
        self.outline.inflate_ip(20, 20)
        for i in range(8):
            for j in range(8):
                rect = pygame.Rect(45 + (40 * i), 45 + (40 * j), 40, 40)
                pygame.draw.rect(background, BLACK, rect, 1)
        screen.blit(background, (0, 0))    
        font_obj = pygame.font.Font('freesansbold.ttf', 16)
        t = 1
        button =[]
        button_Text = []
        for i in range(8):                       
            button_Text.append(font_obj.render(chr(49+i), True, (0, 0, 0), ))
            button.append(button_Text[i].get_rect())
            button[i].center = (35, 65 + 40*i)

            screen.blit(button_Text[i], button[i])            
        # screen.blit(font.render("New game", True, (0,0,0)), (45 + 40 * 19 + 3 , 45 + 40 * 1))       

        for i in range(8):
            test_Text = font_obj.render(chr(65 + i), True, (0, 0, 0))
            test_pos = test_Text.get_rect()
            test_pos.center = (65 + 40 * i, 35)
            screen.blit(test_Text, test_pos)

        newgame_Text = font_obj.render("New Game", True, (0, 0, 0), (255,255,255))
        newgame_pos = newgame_Text.get_rect()
        newgame_pos.center = (45 + 40 * 12, 60)
        screen.blit(newgame_Text, newgame_pos)

        

        close_Text = font_obj.render("Close", True, (0, 0, 0), (255,255,255))
        close_pos = close_Text.get_rect()
        close_pos.center = (45 + 40 * 12, 120)
        screen.blit(close_Text, close_pos)

        close_Text = font_obj.render("OpenFile", True, (0, 0, 0), (255,255,255))
        close_pos = close_Text.get_rect()
        close_pos.center = (45 + 40 * 12, 180)
        screen.blit(close_Text, close_pos)

        close_Text = font_obj.render("Pause", True, (0, 0, 0), (255,255,255))
        close_pos = close_Text.get_rect()
        close_pos.center = (45 + 40 * 12, 240)
        screen.blit(close_Text, close_pos)

        close_Text = font_obj.render("Next", True, (0, 0, 0), (255,255,255))
        close_pos = close_Text.get_rect()
        close_pos.center = (45 + 40 * 12, 300)
        screen.blit(close_Text, close_pos)

        close_Text = font_obj.render("Collect Data from file", True, (0, 0, 0), (255,255,255))
        close_pos = close_Text.get_rect()
        close_pos.center = (45 + 40 * 12, 360)
        screen.blit(close_Text, close_pos)        

        pygame.display.update()
    def remove(sefl, arr, color):
        if (color == 1):
            for i in len(arr):
                point = arr[i]

                for j in len(sefl.black):
                    if (sefl.black[j].point == arr[i]):
                        sefl.black[j].remove()
                        sefl.black[j].pop(j)
                        break
        if (color == 0):
            for i in len(arr):
                point = arr[i]

                for j in len(sefl.white):
                    if (sefl.white[j].point == arr[i]):
                        sefl.white[j].remove()
                        sefl.white[j].pop(j)
                        break         


def open_file():
    filepath = tkinter.filedialog.askopenfilename()
    pygame.display.update()
    if (len(filepath) > 0):
        file_reader = open(filepath, "r")
        lines = file_reader.readline()
        file_reader.close() 
        return lines[6:(len(lines))]
    return ""
def clear():
    global is_pause, can_go_next, is_data_mining
    if (is_data_mining == False):
        is_pause = False
        can_go_next = False
        for i in range(9):
            for j in range(9):
                blit_coords = (5 + (i+1) * 40, 5 + (j+1) * 40 )
                area_rect = pygame.Rect(blit_coords, (40, 40))
                screen.blit(background, blit_coords, area_rect)

def clear_data():
    blit_coords = (0, 399)
    area_rect = pygame.Rect(blit_coords, (900, 900))
    screen.blit(background, blit_coords, area_rect)
    pygame.display.update() 

def init_board(board):
    global is_data_mining
    
    current_stone = board.nextturn()
    added_stone = Stone(board, current_stone,(5, 4), True)
    board.play((4,5), added_stone)

    current_stone = board.nextturn()
    added_stone = Stone(board,current_stone, (4, 4), True )
    board.play((4,4), added_stone)

    current_stone = board.nextturn()
    added_stone = Stone(board, current_stone, (4, 5), True )
    board.play((5,4), added_stone)
    dele_suggesetion(board)
    current_stone = board.nextturn()
    added_stone = Stone(board, current_stone,(5, 5), True)
    board.play((5,5), added_stone)
        
    show_suggestion(board)


def show_suggestion(board):
    global is_data_mining, suggest_black, suggest_white
    arr = board.listsuggestion()
    # print(arr)
    for i in range(len(arr)):
        if (is_data_mining == False):
            x = arr[i][0]
            y = arr[i][1]
            added_stone = Stone(board, (211,211,211), (y+1, x+1), True )
            board.suggestion(added_stone)
        else:
            if (board.nextturn() == BLACK):
                suggest_black[arr[i][0]][arr[i][1]] = suggest_black[arr[i][0]][arr[i][1]] + 1
            else:
                suggest_white[arr[i][0]][arr[i][1]] = suggest_white[arr[i][0]][arr[i][1]] + 1
                



def updatechess(board, color):
    arr = board.takechange()
    for i in range(len(arr)):
        x = arr[i][0]
        y = arr[i][1]
        added_stone = Stone(board, color, (y+1, x+1), True)
        board.udpate(color, added_stone)

def updatechess_data(board, color):
    arr = board.takechange()
    for i in range(len(arr)):
        x = arr[i][0]
        y = arr[i][1]
        added_stone = Stone(board, color, (y+1, x+1), False)
        board.udpate(color, added_stone)

def dele_suggesetion(board):
    board.clearsuggestion()

def add_stone(board, point):
    dele_suggesetion(board)
    current_stone = board.nextturn()
    added_stone = Stone(board, current_stone, (point[0], point[1]), True)
    board.play((point[1], point[0]), added_stone)
    show_suggestion(board)
    updatechess(board,current_stone) 

def add_stone_data(board, point, winner):
    global play_black, play_white, suggest_black, suggest_white, n_play_white, n_play_black
    dele_suggesetion(board)
    current_stone = board.nextturn()

    added_stone = Stone(board, current_stone, (point[0], point[1]), False)
    board.play((point[1], point[0]), added_stone)

    if(winner == WHITE and current_stone == WHITE):
        play_white[point[0]-1][point[1]-1] = play_white[point[0]-1][point[1]-1] + 1
    if (winner == WHITE and current_stone == BLACK):
        n_play_black[point[0]-1][point[1]-1] = n_play_black[point[0]-1][point[1]-1] + 1
    if (winner == BLACK and current_stone == BLACK):
        play_black[point[0]-1][point[1]-1] = play_black[point[0]-1][point[1]-1] + 1      
    if (winner == BLACK and current_stone == WHITE):
        n_play_white[point[0]-1][point[1]-1] = n_play_white[point[0]-1][point[1]-1] + 1            

    show_suggestion(board)
    updatechess_data(board,current_stone) 

def datamining(line, board):
    global total_match
    board.clear()
    winner = BLACK

    clear()
    board.startboard(8)
    init_board(board)
    index = line.find("|")

    num1 = line[0:index] 
    line = line[index+1:(len(line))] 
    b = -1
    
    try:
        b = int(num1)     
    except:
        total_match = total_match - 1
        return
    
    index = line.find("|")

    num1 = line[0:index] 
    line_reader = line[index+1:(len(line))] 
    w = int(num1) 

    line_reader = line_reader.upper()
    if w > b:
        winner = WHITE
    else:
        if w == b:
            winner = GRAY

    str_leng = len(line_reader)
    playfromfilemining(board,line_reader,str_leng, winner)
    
def drawPolygob():
    clear_data()
    global play_black, play_white, suggest_black, suggest_white, total_match
    font_obj = pygame.font.Font('freesansbold.ttf', 16)    
    center_x = 300
    center_y = 530
    radius = 100
   
    
    close_Text = font_obj.render("A1", True, (0, 0, 0), (255,255,255))
    close_pos = close_Text.get_rect()
    close_pos.center = (center_x - 200, center_y)
    screen.blit(close_Text, close_pos)
    pygame.display.update()
        # for angle in range(0, 360, 10):
        #     # Tính toạ độ (x, y) của điểm trên đường tròn
        #     x = center_x + int(radius * math.cos(math.radians(angle)))
        #     y = center_y + int(radius * math.sin(math.radians(angle)))

            # Vẽ điểm trên đường tròn
            # pygame.draw.circle(screen, RED, (x, y), 5)

        # Vẽ hình tròn
    angle = play_black[0][0]*360/(n_play_black[0][0]+play_black[0][0])
    angle = round(angle,2)
    percent  = "Black: Winrate " + str(round((play_black[0][0])*100/(n_play_black[0][0]+play_black[0][0]),2)) + "%"
    sub_angle = math.floor(angle)
    if (angle - sub_angle) > 0.5:
        sub_angle = sub_angle + 1

    p = [(center_x, center_y)]
        
    pygame.draw.circle(screen, YELLOW, (center_x, center_y), radius, 0)
    pygame.display.update()    
    for n in range(-90,sub_angle-90):
        x = center_x + int(radius*math.cos(n*math.pi/180))
        y = center_y+int(radius*math.sin(n*math.pi/180))
        p.append((x, y))

    p.append((center_x, center_y))    
        #print(p)
    if len(p) > 2:
        pygame.draw.polygon(screen, BLACK, p)  
    pygame.display.update()  
        #pygame.gfxdraw.pie(screen, YELLOW, (center_x, center_y, radius, radius), start_angle, end_angle)
    pygame.draw.circle(screen, WHITE, (center_x, center_y), radius + 1, 2)
    pygame.display.update()


    close_Text = font_obj.render(percent, True, (0, 0, 0), (255,255,255))
    close_pos = close_Text.get_rect()
    close_pos.center = (center_x, center_y + 120)
    screen.blit(close_Text, close_pos) 
    pygame.display.update()
    center_x = 600
    center_y = 530
    radius = 100

        # for angle in range(0, 360, 10):
        #     # Tính toạ độ (x, y) của điểm trên đường tròn
        #     x = center_x + int(radius * math.cos(math.radians(angle)))
        #     y = center_y + int(radius * math.sin(math.radians(angle)))

            # Vẽ điểm trên đường tròn
            # pygame.draw.circle(screen, RED, (x, y), 5)

        # Vẽ hình tròn
    angle = (n_play_black[0][0]+play_black[0][0])*360/suggest_black[0][0]
    sub_angle = math.floor(angle)
    angle = round(angle,2)
    percent  = "Black: Pick rate " + str(round((n_play_black[0][0]+play_black[0][0])*100/suggest_black[0][0],2)) + "%"
    if (angle - sub_angle) > 0.5:
        sub_angle = sub_angle + 1

    p = [(center_x, center_y)]
        
    pygame.draw.circle(screen, YELLOW, (center_x, center_y), radius, 0)
    pygame.display.update()    
    for n in range(-90,sub_angle-90):
        x = center_x + int(radius*math.cos(n*math.pi/180))
        y = center_y+int(radius*math.sin(n*math.pi/180))
        p.append((x, y))

    p.append((center_x, center_y))    
        #print(p)
    if len(p) > 2:
        pygame.draw.polygon(screen, BLACK, p)  
    pygame.display.update()   
        #pygame.gfxdraw.pie(screen, YELLOW, (center_x, center_y, radius, radius), start_angle, end_angle)
    close_Text = font_obj.render(percent, True, (0, 0, 0), (255,255,255))
    close_pos = close_Text.get_rect()
    close_pos.center = (center_x, center_y + 120)
    screen.blit(close_Text, close_pos) 
    pygame.display.update()
    pygame.draw.circle(screen, WHITE, (center_x, center_y), radius + 1, 2) 
    pygame.display.update()





    center_x = 300
    center_y = 780
    radius = 100
   
        # for angle in range(0, 360, 10):
        #     # Tính toạ độ (x, y) của điểm trên đường tròn
        #     x = center_x + int(radius * math.cos(math.radians(angle)))
        #     y = center_y + int(radius * math.sin(math.radians(angle)))

            # Vẽ điểm trên đường tròn
            # pygame.draw.circle(screen, RED, (x, y), 5)

        # Vẽ hình tròn
    print(play_black[0][0])
    print(play_white[0][0])        
    angle = play_white[0][0]*360/(play_white[0][0] + n_play_white[0][0])
    angle = round(angle,2)
    percent  = "White: Winrate " + str(round(play_white[0][0]*100/(play_white[0][0] + n_play_white[0][0]),2)) + "%"
    sub_angle = math.floor(angle)
    if (angle - sub_angle) > 0.5:
        sub_angle = sub_angle + 1

    p = [(center_x, center_y)]
        
    pygame.draw.circle(screen, YELLOW, (center_x, center_y), radius, 0)
    pygame.display.update()   
    for n in range(-90,sub_angle-90):
        x = center_x + int(radius*math.cos(n*math.pi/180))
        y = center_y+int(radius*math.sin(n*math.pi/180))
        p.append((x, y))

    p.append((center_x, center_y))    
        #print(p)
    if len(p) > 2:
        pygame.draw.polygon(screen, WHITE, p)  
        
        #pygame.gfxdraw.pie(screen, YELLOW, (center_x, center_y, radius, radius), start_angle, end_angle)
    pygame.draw.circle(screen, BLACK, (center_x, center_y), radius + 1, 2)
    pygame.display.update()


    close_Text = font_obj.render(percent, True, (0, 0, 0), (255,255,255))
    close_pos = close_Text.get_rect()
    close_pos.center = (center_x, center_y + 120)
    screen.blit(close_Text, close_pos) 
    pygame.display.update()


    center_x = 600
    center_y = 780
    radius = 100

        # for angle in range(0, 360, 10):
        #     # Tính toạ độ (x, y) của điểm trên đường tròn
        #     x = center_x + int(radius * math.cos(math.radians(angle)))
        #     y = center_y + int(radius * math.sin(math.radians(angle)))

            # Vẽ điểm trên đường tròn
            # pygame.draw.circle(screen, RED, (x, y), 5)

        # Vẽ hình tròn
    angle = (play_white[0][0] + n_play_white[0][0])*360/suggest_white[0][0]
    sub_angle = math.floor(angle)
    angle = round(angle,2)
    percent  = "White: Pick rate " + str(round((play_white[0][0] + n_play_white[0][0])*100/suggest_white[0][0],2)) + "%"
    if (angle - sub_angle) > 0.5:
        sub_angle = sub_angle + 1

    p = [(center_x, center_y)]
        
    pygame.draw.circle(screen, YELLOW, (center_x, center_y), radius, 0)
    pygame.display.update() 

    for n in range(-90,sub_angle-90):
        x = center_x + int(radius*math.cos(n*math.pi/180))
        y = center_y+int(radius*math.sin(n*math.pi/180))
        p.append((x, y))

    p.append((center_x, center_y))    
        #print(p)
    if len(p) > 2:
        pygame.draw.polygon(screen, WHITE, p)  
        
        #pygame.gfxdraw.pie(screen, YELLOW, (center_x, center_y, radius, radius), start_angle, end_angle)
    close_Text = font_obj.render(percent, True, (0, 0, 0), (255,255,255))
    close_pos = close_Text.get_rect()
    close_pos.center = (center_x, center_y + 120)
    screen.blit(close_Text, close_pos) 
    pygame.display.update()

    pygame.draw.circle(screen, BLACK, (center_x, center_y), radius + 1, 2)
     
    pygame.display.update()

def startContructData(board):
    init_array()
    global total_match, is_data_mining
    global play_black, play_white, suggest_black, suggest_white
    filepath = tkinter.filedialog.askopenfilename()


    file_reader = open(filepath, "r")
    is_data_mining = True
    for line in file_reader:
        total_match  = total_match + 1
        datamining(line,board)
        #print(total_match)
    is_data_mining = False
    file_reader.close()
    drawPolygob()
     




def main():
    while True:
        global is_pause,is_new_game, can_go_next

        #start game 

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                is_stop_thread = True
                exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                
                x = int(math.floor((event.pos[0] - 5) / 40.0))
                y = int(math.floor((event.pos[1] - 5) / 40.0))
                #print(str(x) +  " " + str(y))
                if x in range(1,9) and y in range(1,9):  
                    if is_new_game == True:
                        if (board.search(point=(y,x)) == True):
                            #print("check nuoc di ok")
                            color = board.nextturn()
                            if board.check(y,x) == True:
                                dele_suggesetion(board)
                                stone = Stone(board, color,(x,y), True)                      
                                board.play((y,x), stone)
                                updatechess(board,color)
                                show_suggestion(board)
                        
                            #print("nuoc di fail")        
                        # if color == (255,255,255):
                        #     board.update(arr,1)
                        # else:
                        #     board.update(arr, 0)    
                if x in range(12,14) and y == 1 :
                    is_new_game = True
                    board.clear()
                    clear()
                    board.startboard(8)
                    init_board(board)
                if x in range(12,14) and y == 5 :
                    if is_pause == False:
                        is_pause = True
                    else:
                        is_pause = False

                if x in range(12, 14) and (y == 6 or y == 7) :
                    if (is_pause == True):
                        can_go_next = True

                if x in range(12, 14) and y == 2:
                    is_stop_thread = True
                    exit()
                if x in range(11, 14) and y == 4:
                    line_reader = open_file()
                    is_new_game = False
                    is_pause = True
                    can_go_next = False
                    str_leng = len(line_reader)
                    if (len(line_reader) > 2):
                        board.clear()
                        clear()
                        board.startboard(8)
                        init_board(board)
                    
                        t1 = threading.Thread(target=play_from_file, args=(board,line_reader,str_leng))
                        t1.start()
                if x in range(11,14) and y in range(8,9):
                    #print("go in")
                    startContructData(board)



if __name__ == '__main__':
    pygame.init()
    pygame.display.set_caption('Change2520_Go')
    #print(arr)
    screen = pygame.display.set_mode(BOARD_SIZE, 8, 8)
    background = pygame.image.load(BACKGROUND).convert()
    # init_button(screen)
    board = Board(8)
    init_board(board)
    is_new_game = True
    main()        