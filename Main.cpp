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
    __declspec(noreturn) void LogicLoop();
    uint64_t Pixels[0x40][0x2] = {0};
    uint8_t Memory[0x1000] = {0};
#pragma pack(1)
    struct {
        uint64_t Counter;
        uint16_t Keys;
        uint8_t Width;
        uint8_t Height;
        uint8_t DT;
        uint8_t ST;
        uint8_t Resized;
    } Vars;
}
const size_t Zoom = 0x4;
bool Throttle = true;
SDL_Surface* Screen = nullptr;
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
void HandleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT:
            std::exit(0);
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
            case SDLK_1: Vars.Keys |= 0x0001; break;
            case SDLK_2: Vars.Keys |= 0x0002; break;
            case SDLK_3: Vars.Keys |= 0x0004; break;
            case SDLK_4: Vars.Keys |= 0x0008; break;
            case SDLK_q: Vars.Keys |= 0x0010; break;
            case SDLK_w: Vars.Keys |= 0x0020; break;
            case SDLK_e: Vars.Keys |= 0x0040; break;
            case SDLK_r: Vars.Keys |= 0x0080; break;
            case SDLK_a: Vars.Keys |= 0x0100; break;
            case SDLK_s: Vars.Keys |= 0x0200; break;
            case SDLK_d: Vars.Keys |= 0x0400; break;
            case SDLK_f: Vars.Keys |= 0x0800; break;
            case SDLK_z: Vars.Keys |= 0x1000; break;
            case SDLK_x: Vars.Keys |= 0x2000; break;
            case SDLK_c: Vars.Keys |= 0x4000; break;
            case SDLK_v: Vars.Keys |= 0x8000; break;
            case SDLK_t: Throttle = !Throttle; break;
            case SDLK_ESCAPE: std::exit(0); break;
            }
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
            case SDLK_1: Vars.Keys &= 0xfffe; break;
            case SDLK_2: Vars.Keys &= 0xfffd; break;
            case SDLK_3: Vars.Keys &= 0xfffb; break;
            case SDLK_4: Vars.Keys &= 0xfff7; break;
            case SDLK_q: Vars.Keys &= 0xffef; break;
            case SDLK_w: Vars.Keys &= 0xffdf; break;
            case SDLK_e: Vars.Keys &= 0xffbf; break;
            case SDLK_r: Vars.Keys &= 0xff7f; break;
            case SDLK_a: Vars.Keys &= 0xfeff; break;
            case SDLK_s: Vars.Keys &= 0xfdff; break;
            case SDLK_d: Vars.Keys &= 0xfbff; break;
            case SDLK_f: Vars.Keys &= 0xf7ff; break;
            case SDLK_z: Vars.Keys &= 0xefff; break;
            case SDLK_x: Vars.Keys &= 0xdfff; break;
            case SDLK_c: Vars.Keys &= 0xbfff; break;
            case SDLK_v: Vars.Keys &= 0x7fff; break;
            }
            break;
        }
    }
}
void SetupScreen() {
    Screen = SDL_SetVideoMode(Vars.Width*Zoom, Vars.Height*Zoom, 8, SDL_SWSURFACE);
    SDL_Color Colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetColors(Screen, Colors, 0, 2);
}
void Initialize() {
    Vars.Keys = 0x0000;
    Vars.Width = 0x40;
    Vars.Height = 0x20;
    Vars.DT = 0;
    Vars.ST = 0;
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
        if (Vars.DT) --Vars.DT;
        if (Vars.ST) {
            --Vars.ST;
            Beep(5000, Delay/2);
        }
        HandleEvents();
        {
            size_t Cur = Vars.Counter;
            size_t Now = __rdtsc();
            double CPL = (Now - Last)/((Cur - Prev)*65536.);
            std::string Str = std::to_string(CPL);
            SDL_WM_SetCaption((Str.erase(Str.length()-5) + " Cycles per Instruction").c_str(), "");
            Prev = Cur;
            Last = Now;
        }
        SDL_Rect r = {0, 0, Zoom, Zoom};
        for (size_t y = 0; y < Vars.Height; ++y) {
            r.y = y*Zoom;
            for (size_t x = 0; x < Vars.Width; ++x) {
                r.x = x*Zoom;
                SDL_FillRect(Screen, &r, _bittest64((int64_t*)&Pixels[y][x>>6], x&0x3f));
            }
        }
        SDL_Flip(Screen);
    }
}
void Chip8() {
    Initialize();
    std::thread Thread(LogicLoop);
    Thread.detach();
    MainLoop();
}
int main(int argc, char* argv[]) {
    Chip8();
    return 0;
}