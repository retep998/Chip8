#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <SDL/SDL.h>
#include <Windows.h>
#include <intrin.h>

//C Linkage for the ASM
extern "C" {
//Chip8 Execution Thread
int LogicLoop(void*);
uint8_t Memory[0x1400] = {0};//Entire Chip8 Memory
}

//Blank namespace for local linkage
namespace {
//Local bindings of various sections of the memory
auto& V       = reinterpret_cast<uint8_t(&)[0x10]>     (Memory[0x0]);//Registers
auto& Font    = reinterpret_cast<uint8_t(&)[0x50]>     (Memory[0x10]);//Standard Font
auto& SFont   = reinterpret_cast<uint8_t(&)[0xa0]>     (Memory[0x60]);//Super Font
auto& Stack   = reinterpret_cast<uint16_t(&)[0xd0]>    (Memory[0x100]);//Call Stack
auto& Keys    = reinterpret_cast<int64_t&>             (Memory[0x1d0]);//Keyboard State
auto& Counter = reinterpret_cast<uint64_t&>            (Memory[0x1d8]);//Counter
auto& DT      = reinterpret_cast<int32_t&>             (Memory[0x1e0]);//Delay Timer
auto& CDT     = reinterpret_cast<int32_t&>             (Memory[0x1e4]);//Constant Delay Timer
auto& ST      = reinterpret_cast<int32_t&>             (Memory[0x1e8]);//Sound Timer
auto& CST     = reinterpret_cast<int32_t&>             (Memory[0x1ec]);//Constant Sound Timer
auto& Large   = reinterpret_cast<uint8_t&>             (Memory[0x1f0]);//Super Display Mode
auto& Over    = reinterpret_cast<uint8_t&>             (Memory[0x1f1]);//Execution Finished
auto& Program = reinterpret_cast<char(&)[0xe00]>       (Memory[0x200]);//Program Data
auto& Pixels  = reinterpret_cast<int64_t(&)[0x40][0x2]>(Memory[0x1000]);//Display Pixels
//Default Fonts
uint8_t DeFont[0x50] = {
0xF0, 0x90, 0x90, 0x90, 0xF0,
0x20, 0x60, 0x20, 0x20, 0x70,
0xF0, 0x10, 0xF0, 0x80, 0xF0,
0xF0, 0x10, 0xF0, 0x10, 0xF0,
0x90, 0x90, 0xF0, 0x10, 0x10,
0xF0, 0x80, 0xF0, 0x10, 0xF0,
0xF0, 0x80, 0xF0, 0x90, 0xF0,
0xF0, 0x10, 0x20, 0x40, 0x40,
0xF0, 0x90, 0xF0, 0x90, 0xF0,
0xF0, 0x90, 0xF0, 0x10, 0xF0,
0xF0, 0x90, 0xF0, 0x90, 0x90,
0xE0, 0x90, 0xE0, 0x90, 0xE0,
0xF0, 0x80, 0x80, 0x80, 0xF0,
0xE0, 0x90, 0x90, 0x90, 0xE0,
0xF0, 0x80, 0xF0, 0x80, 0xF0,
0xF0, 0x80, 0xF0, 0x80, 0x80};
//Some local variables
const uint16_t Zoom = 0x4;//Display Scaling
SDL_Surface* Screen = nullptr;//SDL Display Window
//Handles user input
void HandleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT:
            std::exit(0);
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
            case SDLK_1: _bittestandset64(&Keys, 0x0); break;
            case SDLK_2: _bittestandset64(&Keys, 0x1); break;
            case SDLK_3: _bittestandset64(&Keys, 0x2); break;
            case SDLK_4: _bittestandset64(&Keys, 0x3); break;
            case SDLK_q: _bittestandset64(&Keys, 0x4); break;
            case SDLK_w: _bittestandset64(&Keys, 0x5); break;
            case SDLK_e: _bittestandset64(&Keys, 0x6); break;
            case SDLK_r: _bittestandset64(&Keys, 0x7); break;
            case SDLK_a: _bittestandset64(&Keys, 0x8); break;
            case SDLK_s: _bittestandset64(&Keys, 0x9); break;
            case SDLK_d: _bittestandset64(&Keys, 0xa); break;
            case SDLK_f: _bittestandset64(&Keys, 0xb); break;
            case SDLK_z: _bittestandset64(&Keys, 0xc); break;
            case SDLK_x: _bittestandset64(&Keys, 0xd); break;
            case SDLK_c: _bittestandset64(&Keys, 0xe); break;
            case SDLK_v: _bittestandset64(&Keys, 0xf); break;
            case SDLK_ESCAPE: std::exit(0); break;
            }
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
            case SDLK_1: _bittestandreset64(&Keys, 0x0); break;
            case SDLK_2: _bittestandreset64(&Keys, 0x1); break;
            case SDLK_3: _bittestandreset64(&Keys, 0x2); break;
            case SDLK_4: _bittestandreset64(&Keys, 0x3); break;
            case SDLK_q: _bittestandreset64(&Keys, 0x4); break;
            case SDLK_w: _bittestandreset64(&Keys, 0x5); break;
            case SDLK_e: _bittestandreset64(&Keys, 0x6); break;
            case SDLK_r: _bittestandreset64(&Keys, 0x7); break;
            case SDLK_a: _bittestandreset64(&Keys, 0x8); break;
            case SDLK_s: _bittestandreset64(&Keys, 0x9); break;
            case SDLK_d: _bittestandreset64(&Keys, 0xa); break;
            case SDLK_f: _bittestandreset64(&Keys, 0xb); break;
            case SDLK_z: _bittestandreset64(&Keys, 0xc); break;
            case SDLK_x: _bittestandreset64(&Keys, 0xd); break;
            case SDLK_c: _bittestandreset64(&Keys, 0xe); break;
            case SDLK_v: _bittestandreset64(&Keys, 0xf); break;
            }
            break;
        }
    }
}
//Setups the appropriate video mode
void SetupScreen(bool big) {
    if (big) Screen = SDL_SetVideoMode(128*Zoom, 64*Zoom, 8, SDL_SWSURFACE);
    else Screen = SDL_SetVideoMode(64*Zoom, 32*Zoom, 8, SDL_SWSURFACE);
    SDL_Color Colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetColors(Screen, Colors, 0, 2);
}
//Draws the current pixel data to the screen
void UpdateScreen() {
    static uint8_t l = 0;
    const uint8_t n = Large;
    if (l != n) {
        l = n;
        SetupScreen(!!l);
    }
    SDL_Rect r = {0, 0, Zoom, Zoom};
    if (l) for (uint16_t y = 0; y < 64; ++y) {
        r.y = y*Zoom;
        for (uint16_t x = 0; x < 128; ++x) {
            r.x = x*Zoom;
            SDL_FillRect(Screen, &r, _bittest64((int64_t*)&Pixels[y][x>>6], x&0x3f));
        }
    } else for (uint16_t y = 0; y < 32; ++y) {
        r.y = y*Zoom;
        for (uint16_t x = 0; x < 64; ++x) {
            r.x = x*Zoom;
            SDL_FillRect(Screen, &r, _bittest64((int64_t*)&Pixels[y][x>>6], x));
        }
    }
    SDL_Flip(Screen);
}
//Updates the timers and statistics
void UpdateTime() {
    const size_t d = 100;
    static uint64_t p = 0;
    static uint64_t l = 0;
    uint64_t c = Counter;
    uint64_t n = __rdtsc();
    ++CDT;
    if (ST-CST++ > 0) Beep(5000+n&0xff, d/2);
    std::string cpl = std::to_string(static_cast<double>(n-l)/(c-p)).erase(5)+" CPI";
    SDL_WM_SetCaption(cpl.c_str(), "");
    p = c;
    l = n;
    Sleep(d);
}
//Initializes the program data and defaults
void Initialize() {
    std::memcpy(Font, DeFont, 0x50);
    std::ifstream f("loop.ch8", std::ios::binary);
    f.read(Program, 0xe00);
    SDL_Init(SDL_INIT_VIDEO);
    SetupScreen(false);
}
//It's the main loop, what did you expect?
void MainLoop() {
    for (;;) {
        if (Over) return;
        HandleEvents();
        UpdateScreen();
        UpdateTime();
    }
}
//Gets everything started
void Chip8() {
    Initialize();
    SDL_CreateThread(LogicLoop, nullptr);
    MainLoop();
}
}//namespace {

int main(int argc, char *argv[]) {
    Chip8();
    return 0;
}