// clang++ -o ./snake.out ./snake.cpp --std=c++20
#include <iostream>
// I/O

#include <ctime>
// Time

#include <termios.h>
// Consol 인터페이스 설정
#include <unistd.h>
// Consol 설정에 필요!

#include <random>
// Random


#include <deque>
// Deque

#define GROUND_SIZE 16

using namespace std;

random_device d;
mt19937 gen(d());
uniform_int_distribution<int> dis(0, GROUND_SIZE-1);


struct body {
    int x, y;
};

struct Apple {
    int x, y;
};

class game
{
    private:
        int x;
        int y;
        int rx, ry;
        int movement_angle;
        int length;

        Apple apple;

        deque<body> pos;
        body head;

        bool bodyH[GROUND_SIZE];

        bool gameOver;

    public:
        
        void randomApple()
        {
            apple.x = dis(gen);
            apple.y = dis(gen);
            bodyH[apple.y] = 1;
        }

        game()
        {
            gameOver = false;
            for(int i = 0; i < GROUND_SIZE; i++)
                bodyH[i] = 0;

            x = GROUND_SIZE/2;
            y = GROUND_SIZE/2;
            bodyH[y] = 1;
            

            length = 5;
            for(int i = 0; i < length; i++)
            {
                bodyH[y-i-1] = 1;
                body p = {x, y-i-1};
                pos.push_back(p);
                // cout << y-i-1 << "\n";
            }
            head.x = x; head.y = y;
        
            randomApple();
        }

        bool Update(int moveKey)
        {
            /*
              0
            1 + 2
              3
            */
            switch (moveKey) {
                case 0:
                    _LoopMove(0, -1);
                    break;
                case 1:
                    _LoopMove(-1, 0);
                    break;
                case 2:
                    _LoopMove(1, 0);
                    break;
                case 3:
                    _LoopMove(0, 1);
                    break;
            }
            return gameOver;
        }

        void _LoopMove(int deltaX, int deltaY)
        {
            int Re_x, Re_y;
            Re_x = head.x; Re_y = head.y;

            head.x += deltaX; head.y += deltaY;
            _Event(head.x, head.y);

            if(pos.size()+1 > length){
                bodyH[pos.back().y] = 0;
                pos.pop_back();
            } 

            body p = {Re_x, Re_y};

            pos.push_front(p);
            for(body ps:pos)
            {
                bodyH[ps.y] = 1;
            }
            bodyH[head.y] = 1;
            bodyH[apple.y] = 1;

            // cout << pos.size();
        }

        void _Event(int Hx, int Hy)
        {
            if(apple.x == Hx && apple.y == Hy) {length++; randomApple();}
            if(head.x < 0 || head.y < 0) gameOver = true;
            if(head.x > GROUND_SIZE-1 || head.y > GROUND_SIZE-1) gameOver = true;
            
            for(body ps:pos)
            {
                if(head.x == ps.x && head.y == ps.y) gameOver = true;
            }
        }

        void Display()
        {
            cout << "□";
            for(int i = 0; i < GROUND_SIZE; i++) cout << "□";
            cout << "□\n";

            for (int h = 0; h < GROUND_SIZE; h++) {
                cout << "□";
                if(bodyH[h])
                {
                    for (int w = 0; w < GROUND_SIZE; w++) {
                        bool sw = true;

                        if (apple.x == w && apple.y == h) {
                            cout << "A";
                            continue;;
                        }

                        if (head.x == w && head.y == h) {
                            cout << "#";
                            continue;;
                        }

                        
                        for (int i = 0; i < pos.size(); i++) {
                            if(pos[i].x == w && pos[i].y == h) 
                            {
                                cout << "O";
                                sw = false;
                                break;
                            }
                        }
                        if(sw)
                        {
                            cout << " ";
                        }
                    }
                    cout << "□\n";
                }
                else {for(int i = 0; i < GROUND_SIZE; i++) cout << " "; cout << "□\n";}
            }
            cout << "□";
            for(int i = 0; i < GROUND_SIZE; i++) cout << "□";
            cout << "□\n";
            cout << "Score : " << length - 5 << "\n";
            cout << "\n\nPress ESC to end the game." << "\n";
            cout << "\n\nOperate using the w,a,s,d keys!" << "\n";

        }
};

int main()
{

    struct termios old_tio, new_tio;

    // 기존의 consol 설정 저장
    tcgetattr(STDIN_FILENO, &old_tio);

    new_tio = old_tio;
    new_tio.c_lflag &= ~ICANON;
    // consol 설정 변경
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);


    clock_t start, finish;
    double duration;

    game Game;
    bool gameOver;
    gameOver = false;
    int key = 3;
    bool GameFinish = false;
    while (!gameOver)
    {

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;
        
        if(duration > .16f){

            // 키 입력 감지
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(STDIN_FILENO, &rfds);

            struct timeval tv = {0};
            tv.tv_sec = 0;
            tv.tv_usec = 100000; // 0.1초 간격으로 키 입력 확인

            int ret = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);

            if (ret > 0 && FD_ISSET(STDIN_FILENO, &rfds)) {
                char ch;
                read(STDIN_FILENO, &ch, 1);

                switch (ch) {
                    case 'w':
                        key = 0;
                        break;
                    case 'a':
                        key = 1;
                        break;
                    case 's':
                        key = 3;
                        break;
                    case 'd':
                        key = 2;
                        break;
                    case 27:
                        GameFinish = true;
                        break;
                }        
                if(GameFinish) break;
            }

            Game.Display();
            gameOver = Game.Update(key);
            std::cout << "\x1B[2J\x1B[H";
            start = clock();
            if(gameOver) Game.Display();
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    // consol 설정 원상복귀
    return 0;
}