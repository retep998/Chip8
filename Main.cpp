#include <cstdint>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <random>
#include <SDL/SDL.h>
#include <Windows.h>
namespace std {
    using namespace std::tr2::sys;
}

//Variables
extern "C" {
    void LogicLoop();
    const size_t LowWidth = 0x40;
    const size_t LowHeight = 0x20;
    const size_t HighWidth = 0x80;
    const size_t HighHeight = 0x40;
    const size_t Zoom = 0x4;
    size_t Width = LowWidth;
    size_t Height = LowHeight;
    size_t OldWidth = Width;
    size_t OldHeight = Height;
    uint8_t DT = 0;//Delay timer
    uint8_t ST = 0;//Sound timer
    size_t Counter;
    bool Throttle = true;
    SDL_Surface* Screen = nullptr;
    uint8_t Pixels[HighHeight][HighWidth];
    uint8_t Memory[0x1000] = {0};
    bool Keys[0x10];
    uint8_t Font[0x50] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,//0
        0x20, 0x60, 0x20, 0x20, 0x70,//1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,//2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,//3
        0x90, 0x90, 0xF0, 0x10, 0x10,//4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,//5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,//6
        0xF0, 0x10, 0x20, 0x40, 0x40,//7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,//8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,//9
        0xF0, 0x90, 0xF0, 0x90, 0x90,//A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,//B
        0xF0, 0x80, 0x80, 0x80, 0xF0,//C
        0xE0, 0x90, 0x90, 0x90, 0xE0,//D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,//E
        0xF0, 0x80, 0xF0, 0x80, 0x80//F
    };
}

void ClearScreen() {
    memset(Pixels, 0, sizeof(Pixels));
}
__declspec(noinline) bool DrawSprite(size_t x, size_t y, size_t s, uint8_t* d) {
    bool inv;
    size_t xx = x%Width;
    size_t sub = xx&(sizeof(size_t)*8-1) + (sizeof(size_t)*8-8);
    if (sub) {
        size_t x1 = xx /= sizeof(size_t)*8;
        size_t x2 = x1 + 1;
        if (x2 == Width/sizeof(size_t)) x2 = 0;
        for (; s > 0; --s, ++y, ++d) {
            if (y == Height) y = 0;
        }
    } else {
        for (; s > 0; --s, ++y, ++d) {
            if (y == Height) y = 0;
        }
    }
    return inv;
}
void HandleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
		switch(e.type) {
		case SDL_QUIT:
            std::exit(0);
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
            case SDLK_0: Keys[0x0] = true; break;
            case SDLK_1: Keys[0x1] = true; break;
            case SDLK_2: Keys[0x2] = true; break;
            case SDLK_3: Keys[0x3] = true; break;
            case SDLK_4: Keys[0x4] = true; break;
            case SDLK_5: Keys[0x5] = true; break;
            case SDLK_6: Keys[0x6] = true; break;
            case SDLK_7: Keys[0x7] = true; break;
            case SDLK_8: Keys[0x8] = true; break;
            case SDLK_9: Keys[0x9] = true; break;
            case SDLK_a: Keys[0xA] = true; break;
            case SDLK_b: Keys[0xB] = true; break;
            case SDLK_c: Keys[0xC] = true; break;
            case SDLK_d: Keys[0xD] = true; break;
            case SDLK_e: Keys[0xE] = true; break;
            case SDLK_f: Keys[0xF] = true; break;
            case SDLK_t: Throttle = !Throttle; break;
            }
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
            case SDLK_0: Keys[0x0] = false; break;
            case SDLK_1: Keys[0x1] = false; break;
            case SDLK_2: Keys[0x2] = false; break;
            case SDLK_3: Keys[0x3] = false; break;
            case SDLK_4: Keys[0x4] = false; break;
            case SDLK_5: Keys[0x5] = false; break;
            case SDLK_6: Keys[0x6] = false; break;
            case SDLK_7: Keys[0x7] = false; break;
            case SDLK_8: Keys[0x8] = false; break;
            case SDLK_9: Keys[0x9] = false; break;
            case SDLK_a: Keys[0xA] = false; break;
            case SDLK_b: Keys[0xB] = false; break;
            case SDLK_c: Keys[0xC] = false; break;
            case SDLK_d: Keys[0xD] = false; break;
            case SDLK_e: Keys[0xE] = false; break;
            case SDLK_f: Keys[0xF] = false; break;
            }
            break;
        }
    }
}
void SetupScreen() {
    Screen = SDL_SetVideoMode(Width*Zoom, Height*Zoom, 8, SDL_SWSURFACE);
    SDL_Color Colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetColors(Screen, Colors, 0, 2);
    ClearScreen();
}
void Initialize() {
    std::memcpy(Memory + 0x50, Font, 0x50);
    std::path p = "bench.ch8";
    size_t len = (size_t)std::file_size(p);
    std::ifstream f(p, std::ios::binary);
    f.read(reinterpret_cast<char*>(Memory) + 0x200, len);
    SDL_Init(SDL_INIT_VIDEO);
    srand(clock());
    SetupScreen();
}
void MainLoop() {
    std::chrono::system_clock::time_point Time(std::chrono::system_clock::now());
    size_t Prev = 0;
    const size_t Delay = 100;
    size_t Last = __rdtsc();
    for (;;) {
        Time += std::chrono::milliseconds(Delay);
        std::this_thread::sleep_until(Time);
        if (DT) --DT;
        if (ST) {
            --ST;
            Beep(5000, Delay/2);
        }
        HandleEvents();
        if (OldWidth != Width || OldHeight != Height) {
            OldWidth = Width, OldHeight = Height;
            SetupScreen();
        }
        {
            size_t Cur = Counter;
            size_t Now = __rdtsc();
            SDL_WM_SetCaption((std::to_string((Now - Last)/((Cur - Prev)*65536.)) + " CPL").c_str(), "");
            Prev = Cur;
            Last = Now;
        }
        SDL_Rect r = {0, 0, Zoom, Zoom};
        for (size_t y = 0; y < Height; ++y) {
            r.y = y*Zoom;
            for (size_t x = 0; x < Width; ++x) {
                r.x = x*Zoom;
                SDL_FillRect(Screen, &r, Pixels[y][x]);
            }
        }
        SDL_Flip(Screen);
    }
}

//        switch (A) {
//        case 0x0:
//            switch (B) {
//            case 0x0:
//                switch (C) {
//                case 0xE:
//                    switch (D) {
//                    case 0x0:
//                        ClearScreen();
//                        break;
//                    case 0xE:
//                        MP = *--SP;
//                        break;
//                    default: Unknown();
//                    }
//                    break;
//                case 0xF:
//                    switch (D) {
//                    case 0xE:
//                        Width = LowWidth;
//                        Height = LowHeight;
//                        break;
//                    case 0xF:
//                        Width = HighWidth;
//                        Height = HighHeight;
//                        break;
//                    default: Unknown();
//                    }
//                    break;
//                default: Unknown();
//                }
//                break;
//            default: Unknown();
//            }
//            break;
//        case 0x1:
//            MP = &Memory[BCD];
//            break;
//        case 0x2:
//            *SP++ = MP;
//            MP = &Memory[BCD];
//            break;
//        case 0x3:
//            if (V[B] == CD) MP += 2;
//            break;
//        case 0x4:
//            if (V[B] != CD) MP += 2;
//            break;
//        case 0x5:
//            if (V[B] == V[C]) MP += 2;
//            break;
//        case 0x6:
//            V[B] = CD;
//            break;
//        case 0x7:
//            V[B] += CD;
//            break;
//        case 0x8:
//            switch (D) {
//            case 0x0:
//                V[B] = V[C];
//                break;
//            case 0x1:
//                V[B] |= V[C];
//                break;
//            case 0x2:
//                V[B] &= V[C];
//                break;
//            case 0x3:
//                V[B] ^= V[C];
//                break;
//            case 0x5:
//                V[0xF] = V[C]>V[B]?0:1;
//                V[B] -= V[C];
//                break;
//            default: Unknown();
//            }
//            break;
//        case 0x9:
//            if (V[B] != V[C]) MP += 2;
//            break;
//        case 0xA:
//            I = BCD;
//            break;
//        case 0xC:
//            V[B] = rand()%((uint16_t)CD+1);
//            break;
//        case 0xD:
//            V[0xF] = DrawSprite(V[B], V[C], D, &Memory[I]);
//            break;
//        case 0xF:
//            switch (CD) {
//            case 0x07:
//                V[B] = DT;
//                break;
//            case 0x15:
//                DT = V[B];
//                break;
//            case 0x18:
//                ST = V[B];
//                break;
//            case 0x29:
//                I = 0x50 + 5*V[B];
//                break;
//            case 0x30://Fuuuuuu
//                break;
//            default: Unknown();
//            }
//            break;
//        default: Unknown();
//        }
//    }
//}
__declspec(noreturn) void ThreadFunc() {
    LogicLoop();
}
void Chip8() {
    Initialize();
    std::thread Thread(ThreadFunc);
    Thread.detach();
    MainLoop();
}
int main(int argc, char* argv[]) {
    Chip8();
    return 0;
}