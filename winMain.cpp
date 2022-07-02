#include <stdc++.h>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/glut.h>
#endif // GLUT Library
//Model
#include "Model.h"
//Chess Game
#include "Chess/Game.h"
//Window size and position
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_POS_X 50
#define WINDOW_POS_Y 50

#define BUTTON_X -100
#define BUTTON_Y -100
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 75

//void keyFunction(unsigned char key, int x, int y);
//void specialFunction(int key, int x, int y);


/**
    Variables for look at function
    eye     - position of eye
    center  - position of the center of view
    up      - up vertex
*/
GLfloat     eyeX = 5.0, eyeY = 0.0, eyeZ = -5.0,
centerX = 0.0, centerY = 0.0, centerZ = 0.0,
upX = 0.0, upY = 0.0, upZ = -1.0;

/**
    Variables for perspective function
*/
GLfloat     fovy = 50.0, zNear = 0.1, zFar = 20.0;

/**
    Variables for light
*/
GLfloat     position[] = { 0.0f, 0.0f, 100.0f, 0.0f };
GLfloat     diffusion[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat     normal_board[] = { 0.0f, 0.0f, 1.0f };
GLfloat     normal_valid_move[] = { 0.0f, 0.0f, -1.0f };
float       ang = 0;
GLfloat mat_diffusion[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };

/**
    Variables for managing view
*/
GLfloat     screen_ratio, zoomOut = 2;

/**
    Model Loading
*/
Model   Pawn = Model("model/pawn.obj");
Model   Rook = Model("model/rook.obj");
Model   Knight = Model("model/knight.obj");
Model   Bishop = Model("model/bishop.obj");
Model   King = Model("model/king.obj");
Model   Queen = Model("model/queen.obj");

bool pressed = false;

Game* chess;
void newGame();

bool    inGame = false, verify = false;
int     selectedRow = 1, selectedCol = 1;
int     moveToRow = 1, moveToCol = 1;
bool    selected = false;
bool    board_rotating = true;
int     rotation = 0;
bool    check = false, checkMate = false;
bool    closeGame = false;
bool	needPromote = false;

GLfloat     chessBoard[12][3] = { {-4.0, -4.0, 0.5},
                                {-4.0,  4.0, 0.5},
                                { 4.0,  4.0, 0.5},
                                { 4.0, -4.0, 0.5},

                                {-4.5, -4.5, 0.5},
                                {-4.5,  4.5, 0.5},
                                { 4.5,  4.5, 0.5},
                                { 4.5, -4.5, 0.5},

                                {-5.0, -5.0, 0.0},
                                {-5.0,  5.0, 0.0},
                                { 5.0,  5.0, 0.0},
                                { 5.0, -5.0, 0.0} };


//展示文字
void showWord(int x, int y, string word)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_HEIGHT / 2, 0, 1);//创建一个正交平行的视景体

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    int len, i;

    len = word.length(); //获取长度
    glRasterPos2i(x, y); // 显示位置
    glColor3f(0, 0, 0);
    for (i = 0; i < len; i++) 
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, word[i]); // 打印
    }
}

//绘制选中后的当前选定区
void drawMoveToSquare()
{
    float r = 1.0 * (moveToRow - 5), c = 1.0 * (moveToCol - 5);
    if (selected)
    {
        glPushMatrix();
        glColor3f(0.5f, 1.0f, 0.0f);
        glTranslatef(r, c, 0.502f);
        glScalef(0.98f, 0.98f, 1.0f);
        glBegin(GL_TRIANGLES);
        glNormal3fv(normal_valid_move);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glEnd();
        glPopMatrix();
    }
    glColor3f(0, 0, 0);
}

//绘制棋盘
void drawChessBoard()
{
    glPushMatrix();
    /**Drawing bottom of the chess board*/
    glNormal3fv(normal_valid_move);
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 8; i < 12; i++) 
        glVertex3fv(chessBoard[i]);
    glEnd();
    /**Drawing top of the chess board*/
    glBegin(GL_QUADS);
    glColor3f(0.55, 0.24, 0.09);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[0]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[4]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[5]);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[1]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[1]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[5]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[6]);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[2]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[2]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[6]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[7]);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[3]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[3]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[7]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[4]);
    glColor3f(0.803, 0.522, 0.247);
    glVertex3fv(chessBoard[0]);
    glEnd();
    /**Drawing side of the chess board*/
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.95, 0.9);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[4]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[8]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[9]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[5]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[5]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[9]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[10]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[6]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[6]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[10]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[11]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[7]);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[7]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[11]);
    glColor3f(1.000, 1.000, 1.000);
    glVertex3fv(chessBoard[8]);
    glColor3f(0.545, 0.271, 0.075);
    glVertex3fv(chessBoard[4]);
    glEnd();
    glPopMatrix();
    glColor3f(0, 0, 0);
}

//绘制棋格
void drawBoardSquares()
{
    float r, c;
    for (int row = 1; row <= 8; row++)
    {
        for (int col = 1; col <= 8; col++)
        {
            r = 1.0 * (row - 5);
            c = 1.0 * (col - 5);
            //若是选中前的选择区块
            if (row == selectedRow && col == selectedCol)
            {
                if (selected)//已选中的棋格 
                    glColor3f(0.3f, 0.420f, 0.2f);
                else if (chess->isSquareOccupied(selectedRow, selectedCol))//若该棋格上有棋子
                    if (chess->getPieceColor(selectedRow, selectedCol) == chess->getTurnColor())//若为本方棋子
                        glColor3f(0.0f, 0.5f, 0.0f);
                    else 
                        glColor3f(1.0f, 0.0f, 0.0f);
                else 
                    glColor3f(0.3f, 0.7f, 0.5f);
            }
            else if ((row + col) & 1) //黑色棋格
                glColor3f(1.0, 1.0, 1.0);
            else //白色
                glColor3f(0.0, 0.0, 0.0);

            glPushMatrix();
            glTranslatef(r, c, 0.5f);
            glBegin(GL_TRIANGLES);
            glNormal3fv(normal_board);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glEnd();
            glPopMatrix();
        }
    }
    glColor3f(0, 0, 0);
}

//绘制有效移动
void drawValidMoves()
{
    if (selected)
    {
        //获取选中的棋子的有效移动路径
        std::vector<Move> validMoves = chess->getValidMoves(selectedRow, selectedCol);
        int vec_size = validMoves.size(), row, col;
        for (int id = 0; id < vec_size; id++)
        {
            row = validMoves[id].getDestinationPosition().first;
            col = validMoves[id].getDestinationPosition().second;
            //不同的移动类型，颜色不同
            switch (validMoves[id].getType())
            {
            case MoveType::NORMAL:
                glColor3f(0.8f, 1.0f, 0.6f);
                break;
            case MoveType::CAPTURE:
                glColor3f(1.0f, 0.0f, 0.0f);
                break;
            case MoveType::EN_PASSANT:
                glColor3f(0.8f, 1.0f, 0.6f);
                break;
            case MoveType::CASTLING:
                glColor3f(0.196f, 0.804f, 0.196f);
                break;
            }
            float r = 1.0 * (row - 5), c = 1.0 * (col - 5);
            glPushMatrix();
            glTranslatef(r, c, 0.501);
            glScalef(0.99f, 0.99f, 1.0f);
            glBegin(GL_TRIANGLES);
            glNormal3fv(normal_valid_move);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glEnd();
            glPopMatrix();
        }
    }
    glColor3f(0, 0, 0);
}

//绘制棋子
void drawChessPieces()
{
    float z;
    for (int row = 1; row <= 8; row++)
    {
        for (int col = 1; col <= 8; col++)
        {
            //若该棋格上有棋子
            if (chess->isSquareOccupied(row, col))
            {
                glPushMatrix();
                if (selected && row == selectedRow && col == selectedCol) z = 1.0;
                else z = 0.5;
                glTranslatef((row - 5) * 1.0f + 0.5f, (col - 5) * 1.0f + 0.5f, z+0.5f);
                glScalef(0.01f, 0.01f, 0.01f);
                //判断棋子的颜色
                switch (chess->getPieceColor(row, col))
                {
                case PieceColor::WHITE:
                    glRotatef(90, 0.0f, 0.0f, 1.0f);
                    glColor3f(0.9f, 0.9f, 0.9f);
                    break;
                case PieceColor::BLACK:
                    glRotatef(-90, 0.0f, 0.0f, 1.0f);
                    glColor3f(0.1f, 0.1f, 0.1f);
                    break;
                }
                //判断棋子的类型
                switch (chess->getPiece(row, col)->getType())
                {
                //使用导入的模型数据绘制
                case PieceType::PAWN: Pawn.Draw(); break;
                case PieceType::ROOK: Rook.Draw(); break;
                case PieceType::KNIGHT: Knight.Draw(); break;
                case PieceType::BISHOP: Bishop.Draw(); break;
                case PieceType::QUEEN: Queen.Draw(); break;
                case PieceType::KING: King.Draw(); break;
                }
                glPopMatrix();
            }
        }
    }
    glColor3f(0, 0, 0);
}

//根据不同的棋子的颜色，其移动时要改变的位置不同
void key_W_pressed(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        if (!selected && selectedRow < 8) selectedRow++;
        if (selected && moveToRow < 8) moveToRow++;
        break;
    case PieceColor::BLACK:
        if (!selected && selectedRow > 1) selectedRow--;
        if (selected && moveToRow > 1) moveToRow--;
        break;
    }
}

void key_D_pressed(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        if (!selected && selectedCol < 8) selectedCol++;
        if (selected && moveToCol < 8) moveToCol++;
        break;
    case PieceColor::BLACK:
        if (!selected && selectedCol > 1) selectedCol--;
        if (selected && moveToCol > 1) moveToCol--;
        break;
    }
}

void key_S_pressed(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        if (!selected && selectedRow > 1) selectedRow--;
        if (selected && moveToRow > 1) moveToRow--;
        break;
    case PieceColor::BLACK:
        if (!selected && selectedRow < 8) selectedRow++;
        if (selected && moveToRow < 8) moveToRow++;
        break;
    }
}

void key_A_pressed(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        if (!selected && selectedCol > 1) selectedCol--;
        if (selected && moveToCol > 1) moveToCol--;
        break;
    case PieceColor::BLACK:
        if (!selected && selectedCol < 8) selectedCol++;
        if (selected && moveToCol < 8) moveToCol++;
        break;
    }
}

//设置默认选中区域
void updateTurn(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        selectedRow = 1;
        selectedCol = 8;
        break;
    case PieceColor::BLACK:
        selectedRow = 8;
        selectedCol = 1;
        break;
    }
}

//旋转棋盘
void doRotationBoard(PieceColor color)
{
    switch (color)
    {
    case PieceColor::WHITE:
        if (rotation < 180)
        {
            rotation += 12;
            Sleep(10);
        }
        else 
            board_rotating = false;
        break;
    case PieceColor::BLACK:
        if (rotation < 360) 
        {
            rotation += 12;
            Sleep(10);
        }
        else
        {
            rotation = 0;
            board_rotating = false;
        }
        break;
    }
}

//这一回合结束
void endOfTurn()
{
    selected = false;
    needPromote = false;
    check = false;
    //进入下一回合
    chess->nextTurn();
    //是否已结束
    if (chess->inCheckMateState())
    {
        checkMate = true;
    }
    //是否将军
    else if (chess->inCheckState())
    {
        check = true;
    }
    //可以旋转棋盘
    board_rotating = true;
    //初始化下一回合的选中棋格
    updateTurn(chess->getTurnColor());
}

void displayFunction()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//刷新颜色与深度缓存区

    if (inGame)//如果在游戏中
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fovy, screen_ratio, zNear, zoomOut * zFar);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(zoomOut * eyeX, zoomOut * eyeY, zoomOut * eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

        //当可以旋转棋盘时，旋转棋盘，并获取当前的棋子颜色
        if (board_rotating) 
            doRotationBoard(chess->getTurnColor());
        GLfloat ambient_model[] = { 0.5, 0.2, 1.0, 1.0 };

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_model);//设置照明模型参数，
        //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffusion);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);//指定用于光照计算的当前材质属性,镜面反射
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffusion);//指定0号光源，散射

        glScalef(1.0f, 1.0f, -1.0f);

        glLightfv(GL_LIGHT0, GL_POSITION, position);//设置光源位置

        glRotatef(rotation, 0, 0, 1);

        drawChessBoard();//绘制棋盘

        drawBoardSquares();//绘制棋格

        drawChessPieces();//绘制棋子

        drawMoveToSquare();//绘制当前选定区

        drawValidMoves();//绘制待选步骤区

        //我方士兵到达最后一行时提示是否需要晋升
        if (needPromote)
        {
            showWord(-200, WINDOW_HEIGHT / 2 - 24, "Promote to: (Q) Queen | (R) Rook | (B) Bishop | (K) Knight");
        }
        else if (verify) //提示是否要重新开始
            showWord(-200, WINDOW_HEIGHT / 2 - 24, "Are you sure to retry? Yes (O)  or  No (X)");
        else
        {
            if (check)//将军了
            {
                string s = chess->getTurnColor() == PieceColor::BLACK ? "BLACK PIECE" : "WHITE PIECE";
                showWord(-150, WINDOW_HEIGHT / 2 - 24, s + " CHECKED!");
            }
            if (checkMate)//一方获得胜利
            {
                string s = chess->getTurnColor() == PieceColor::BLACK ? "WHITE PLAYER" : "BLACK PLAYER";
                showWord(-100, WINDOW_HEIGHT / 2 - 24, "CHECK MATE!");
                showWord(-140, WINDOW_HEIGHT / 2 - 50, s + " WIN!");
                showWord(-150, -WINDOW_HEIGHT / 2 + 50, "Do you want to play again?");
                showWord(-120, -WINDOW_HEIGHT / 2 + 25, "Yes (O)  or  No (X)");
            }
        }
    }
    else
    {
        showWord(-150, 0, "- - Press Space to Start The Game - -");
    }
    if (closeGame)
        glutExit();

    glutSwapBuffers();//交换两个缓冲区指针，解决屏幕闪烁问题

    glutPostRedisplay();//标记当前窗口需要重新绘制
}

//当窗口改变时，图形随着改变使比例不变
void reshapeFunction(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    screen_ratio = (GLdouble)width / (GLdouble)height;
}

//处理方向键
void specialFunction(int key, int x, int y)
{
    switch (key)
    {
    //拉近或拉远
    case GLUT_KEY_UP:
        zoomOut += 0.1;
        break;
    case GLUT_KEY_DOWN:
        zoomOut -= 0.1;
        break;
    default: break;
    }
}

//处理键盘输入
void keyFunction(unsigned char key, int x, int y)
{
    switch (key)
    {
    //移动当前选择棋格
    case 'w':
    case 'W':
        if (!needPromote && !checkMate && !verify && inGame && !board_rotating) 
            key_W_pressed(chess->getTurnColor());
        break;
    case 'a':
    case 'A':
        if (!needPromote && !checkMate && !verify && inGame && !board_rotating) 
            key_A_pressed(chess->getTurnColor());
        break;
    case 's':
    case 'S':
        if (!needPromote && !checkMate && !verify && inGame && !board_rotating) 
            key_S_pressed(chess->getTurnColor());
        break;
    case 'd':
    case 'D':
        if (!needPromote && !checkMate && !verify && inGame && !board_rotating) 
            key_D_pressed(chess->getTurnColor());
        break;
    //选中棋格
    case ' ':
        if (!needPromote && !checkMate && !verify && inGame && !board_rotating)
        {
            if (selected)//如果已经选中棋子了
            {
                if (chess->move(selectedRow, selectedCol, moveToRow, moveToCol))//如果位置可以移动至此
                {
                    Piece* movedPiece = chess->getPiece(moveToRow, moveToCol);//获取移动的棋子
                    if (movedPiece->getType() == PieceType::PAWN &&
                        ((movedPiece->getColor() == PieceColor::BLACK && moveToRow == chess->getBoard()->MIN_ROW_INDEX)
                            || moveToRow == chess->getBoard()->MAX_ROW_INDEX))//如果是士兵且到了最后一行，则可以选择晋升
                    {
                        needPromote = true;
                    }
                    if (needPromote)//若可以晋升，退出 
                        break;
                    endOfTurn();//结束回合
                }
                selected = false;//若位置不可移动，取消选择
            }
            else if (chess->isSquareOccupied(selectedRow, selectedCol) && chess->getPieceColor(selectedRow, selectedCol) == chess->getTurnColor())//若还未选中，且此时选择的棋格上有自己的棋子
            {
               selected = true;//改为已选中状态
               //将此位置设为目标位置
               moveToRow = selectedRow;
               moveToCol = selectedCol;
            }
        }
        //若游戏未开始，开始游戏
        if (!inGame)
            newGame();
        break;
    //case 'n':
    //case 'N':
    //    if (!inGame)newGame();
    //    else verify = true;
    //    break;
    case 'o': case 'O':    //选择是否是重来
        if (checkMate || verify) 
        { 
            delete chess; 
            newGame(); 
            verify = false; 
        }
        break;
    case 'x': case 'X'://若胜负已分，关闭游戏
        if (checkMate) 
        { 
            closeGame = true; 
            delete chess; 
        }
        break;
    //可晋升时，选择晋升目标
    case 'q': case 'Q':
        if (needPromote)
        {
            //将棋子转换为目标
            chess->promote(moveToRow, moveToCol, PieceType::QUEEN);
            endOfTurn();
            break;
        }
        else break;
    case 'r': case 'R':
        if (needPromote)
        {
            chess->promote(moveToRow, moveToCol, PieceType::ROOK);
            endOfTurn();
            break;
        }
        else break;
    case 'b': case 'B':
        if (needPromote)
        {
            chess->promote(moveToRow, moveToCol, PieceType::BISHOP);
            endOfTurn();
            break;
        }
        else break;
    case 'k': case 'K':
        if (needPromote)
        {
            chess->promote(moveToRow, moveToCol, PieceType::KNIGHT);
            endOfTurn();
            break;
        }
        else break;

    default: break;
    }
}

void initialize()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//设置颜色缓存的清除值，即所谓的背景颜色。
    glClearDepth(1.0f);//设置深度缓存的清除值
    glEnable(GL_DEPTH_TEST);//glEnable用于启用各种功能，这里开启更新深度缓冲区的功能
    glEnable(GL_SMOOTH);//平滑处理
    glDepthFunc(GL_LEQUAL);// 指定“目标像素与当前像素在z方向上值大小比较”的函数，符合该函数关系的目标像素才进行绘制，
    glShadeModel(GL_SMOOTH);//平滑着色，实现一种颜色到另一种颜色的平滑渐变
    glEnable(GL_NORMALIZE);//使opengl自动normalize 法向量
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_COLOR_MATERIAL);//使用颜色材质

    glEnable(GL_LIGHTING);//// 使用灯光
    glEnable(GL_LIGHT0);


    //unsigned int texture;
    //int width, height, nrChannels;
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    ////
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ////
    //unsigned char* data = stbi_load("222.jpg", &width, &height, &nrChannels, 0); // 自訂載入圖片方式
    //if (data)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //}
    //stbi_image_free(data);

    //glPushMatrix();
    //glScalef(100, 70, 1);
    //glTranslatef(0, 0, -150);
    //glBindTexture(GL_TEXTURE_2D, texture);

    //glBegin(GL_QUADS);
    //glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, -1.0);
    //glTexCoord2d(1.0, 0.0); glVertex2d(+1.0, -1.0);
    //glTexCoord2d(1.0, 1.0); glVertex2d(+1.0, +1.0);
    //glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, +1.0);
    //glEnd();
    //glPopMatrix();

}

void newGame()
{
    chess = new Game();
    selectedRow = 1; selectedCol = 1;
    moveToRow = 1; moveToCol = 1;
    selected = false;
    board_rotating = true;
    rotation = 0;
    inGame = true;
    check = false;
    checkMate = false;
    updateTurn(chess->getTurnColor());
}

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);//初始化GLUT库
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//在创建窗口的时候，指定其显示模式的类型
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);//设置初始窗口的大小
    glutInitWindowPosition(WINDOW_POS_X, WINDOW_POS_Y);//;设置初始窗口的位置
    glutCreateWindow("Chess");//产生一个顶层的窗口.c

    //newGame();

    initialize();
    glutDisplayFunc(displayFunction);//用于注册一个绘图函数，在窗口内容绘制、窗口大小改变、窗口重绘时程序会自动调用display函数重绘窗口
    glutReshapeFunc(reshapeFunction);//窗口改变的时候调用的函数，用于当窗口尺寸改变时，图形比例不发生变化
    glutKeyboardFunc(keyFunction);//输入按键时调用函数处理按键
    glutSpecialFunc(specialFunction);//输入方向键时调用函数
    glutMainLoop();//这里让整个绘图循环进行，相当于死循环
    return 0;
}
