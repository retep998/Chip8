#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#include <cstdint>
#include <fstream>
#include <ctime>
#include <string>
#include <SDL/SDL.h>
#include <chrono>
#include <thread>

extern "C" void LogicLoop(uint8_t *);

class Chip8
{
public:
    Chip8(std::string name) :
        Memory { },
        LastCycle {0},
        LastCount {0},
        WasLarge {false},
        V {reinterpret_cast<uint8_t(&)[0x10]>(Memory[0x0000])},
        Font {reinterpret_cast<uint8_t(&)[0x50]>(Memory[0x0010])},
        SFont {reinterpret_cast<uint8_t(&)[0xa0]>(Memory[0x0060])},
        Keys {reinterpret_cast<int64_t &>(Memory[0x01d0])},
        Counter {reinterpret_cast<uint64_t &>(Memory[0x01d8])},
        DT {reinterpret_cast<int32_t &>(Memory[0x01e0])},
        CDT {reinterpret_cast<int32_t &>(Memory[0x01e4])},
        ST {reinterpret_cast<int32_t &>(Memory[0x01e8])},
        CST {reinterpret_cast<int32_t &>(Memory[0x01ec])},
        Large {reinterpret_cast<bool &>(Memory[0x01f0])},
        Over {reinterpret_cast<uint8_t &>(Memory[0x01f1])},
        ScrollV {reinterpret_cast<uint8_t &>(Memory[0x01f2])},
        ScrollH {reinterpret_cast<uint8_t &>(Memory[0x01f3])},
        Program {reinterpret_cast<char(&)[0xe00]>(Memory[0x0200])},
        Pixels {reinterpret_cast<uint64_t(&)[0x20]>(Memory[0x1000])},
        SPixels {reinterpret_cast<uint64_t(&)[0x40][0x2]>(Memory[0x1000])}
    {
        const uint8_t DefaultFont[0x50] = {
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
            0xF0, 0x80, 0xF0, 0x80, 0x80
        };
        std::memcpy(Font, DefaultFont, 0x50);
        {
            std::ifstream file(name, std::ios::binary);
            file.read(Program, 0xe00);
        }
        SDL_Init(SDL_INIT_VIDEO);
        SetupScreen(false);
        std::thread(std::bind(LogicLoop, Memory)).detach();
        while (!Over) {
            HandleEvents();
            UpdateScreen();
            UpdateTime();
        }
    }

private:
    static const uint16_t Zoom = 0x4;

    uint8_t Memory[0x1400];

    SDL_Surface * Screen;
    uint64_t LastCycle;
    uint64_t LastCount;
    bool WasLarge;

    uint8_t(&V)[0x10];
    uint8_t(&Font)[0x50];
    uint8_t(&SFont)[0xa0];
    int64_t & Keys;
    uint64_t & Counter;
    int32_t & DT;
    int32_t & CDT;
    int32_t & ST;
    int32_t & CST;
    bool & Large;
    uint8_t & Over;
    uint8_t & ScrollV;
    uint8_t & ScrollH;
    char(&Program)[0xe00];
    uint64_t(&Pixels)[0x20];
    uint64_t(&SPixels)[0x40][0x2];

    Chip8 & operator=(const Chip8 &);

    void SetupScreen(bool big)
    {
        if (big) Screen = SDL_SetVideoMode(128 * Zoom, 64 * Zoom, 8, SDL_SWSURFACE);
        else Screen = SDL_SetVideoMode(64 * Zoom, 32 * Zoom, 8, SDL_SWSURFACE);
        SDL_Color Colors[2] = {{0, 15, 0, 255}, {31, 255, 31, 255}};
        SDL_SetColors(Screen, Colors, 0, 2);
    }

    void UpdateScreen()
    {
        const bool large {Large};
        if (large != WasLarge) {
            WasLarge = large;
            SetupScreen(large);
        }
        SDL_Rect rect {0, 0, Zoom, Zoom};
        if (large) for (uint16_t y = 0; y < 64; ++y) {
            rect.y = y * Zoom;
            for (uint16_t x = 0; x < 128; ++x) {
                rect.x = x * Zoom;
                SDL_FillRect(Screen, &rect, (SPixels[y][x >> 6] >> (x & 0x3f)) & 1);
            }
        } else for (uint16_t y = 0; y < 32; ++y) {
            rect.y = y * Zoom;
            for (uint16_t x = 0; x < 64; ++x) {
                rect.x = x * Zoom;
                SDL_FillRect(Screen, &rect, (Pixels[y] >> x) & 1);
            }
        }
        SDL_Flip(Screen);
    }

    void UpdateTime()
    {
        const uint64_t count {Counter};
        const uint64_t cycles {__rdtsc()};
        ++CDT;
        if (ST - CST++ > 0) {
            //Beep(5000 + n & 0xff, d / 2);
        }
        const std::string cpl {std::to_string(static_cast<double>(cycles - LastCycle) / (count - LastCount)).erase(5) + " CPI"};
        SDL_WM_SetCaption(cpl.c_str(), "");
        LastCycle = cycles;
        LastCount = count;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void HandleEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                std::exit(0);
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case SDLK_1: Keys |= 0x0001; break;
                case SDLK_2: Keys |= 0x0002; break;
                case SDLK_3: Keys |= 0x0004; break;
                case SDLK_4: Keys |= 0x0008; break;
                case SDLK_q: Keys |= 0x0010; break;
                case SDLK_w: Keys |= 0x0020; break;
                case SDLK_e: Keys |= 0x0040; break;
                case SDLK_r: Keys |= 0x0080; break;
                case SDLK_a: Keys |= 0x0100; break;
                case SDLK_s: Keys |= 0x0200; break;
                case SDLK_d: Keys |= 0x0400; break;
                case SDLK_f: Keys |= 0x0800; break;
                case SDLK_z: Keys |= 0x1000; break;
                case SDLK_x: Keys |= 0x2000; break;
                case SDLK_c: Keys |= 0x4000; break;
                case SDLK_v: Keys |= 0x8000; break;
                case SDLK_ESCAPE: std::exit(0); break;
                default:;
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                case SDLK_1: Keys = ~(~Keys | 0x0001); break;
                case SDLK_2: Keys = ~(~Keys | 0x0002); break;
                case SDLK_3: Keys = ~(~Keys | 0x0004); break;
                case SDLK_4: Keys = ~(~Keys | 0x0008); break;
                case SDLK_q: Keys = ~(~Keys | 0x0010); break;
                case SDLK_w: Keys = ~(~Keys | 0x0020); break;
                case SDLK_e: Keys = ~(~Keys | 0x0040); break;
                case SDLK_r: Keys = ~(~Keys | 0x0080); break;
                case SDLK_a: Keys = ~(~Keys | 0x0100); break;
                case SDLK_s: Keys = ~(~Keys | 0x0200); break;
                case SDLK_d: Keys = ~(~Keys | 0x0400); break;
                case SDLK_f: Keys = ~(~Keys | 0x0800); break;
                case SDLK_z: Keys = ~(~Keys | 0x1000); break;
                case SDLK_x: Keys = ~(~Keys | 0x2000); break;
                case SDLK_c: Keys = ~(~Keys | 0x4000); break;
                case SDLK_v: Keys = ~(~Keys | 0x8000); break;
                default:;
                }
                break;
            default:;
            }
        }
    }
};//Class Chip8

int main(int argc, char ** argv)
{
    Chip8 {argc > 1 ? argv[0] : "Clear.ch8"};
    return 0;
}