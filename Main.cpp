//////////////////////////////////////////////////////////////////////////////
// Chip8 - An emulator for the Chip8 system                                 //
// Copyright © 2013 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as published //
// by the Free Software Foundation, either version 3 of the License, or     //
// (at your option) any later version.                                      //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include <SDL/SDL.h>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <string>
#include <chrono>
#include <thread>

extern "C" void LogicLoop(uint8_t *);
extern "C" void LogicLoopPosix(uint8_t *);

class Chip8 {
public:
    Chip8(std::string name) :
        Memory{},
        Screen{nullptr},
        LastCycle{std::chrono::high_resolution_clock::now()},
        LastCount{0},
        WasLarge{false},
        V{reinterpret_cast<uint8_t (&)[0x10]>(Memory[0x0000])},
        Font{reinterpret_cast<uint8_t (&)[0x50]>(Memory[0x0010])},
        SFont{reinterpret_cast<uint8_t (&)[0xa0]>(Memory[0x0060])},
        Keys{reinterpret_cast<int64_t &>(Memory[0x01d0])},
        Counter{reinterpret_cast<uint64_t &>(Memory[0x01d8])},
        DT{reinterpret_cast<int32_t &>(Memory[0x01e0])},
        CDT{reinterpret_cast<int32_t &>(Memory[0x01e4])},
        ST{reinterpret_cast<int32_t &>(Memory[0x01e8])},
        CST{reinterpret_cast<int32_t &>(Memory[0x01ec])},
        Large{reinterpret_cast<bool &>(Memory[0x01f0])},
        Over{reinterpret_cast<uint8_t &>(Memory[0x01f1])},
        ScrollV{reinterpret_cast<uint8_t &>(Memory[0x01f2])},
        ScrollH{reinterpret_cast<uint8_t &>(Memory[0x01f3])},
        Program{reinterpret_cast<char (&)[0xe00]>(Memory[0x0200])},
        Pixels{reinterpret_cast<uint8_t (&)[0x40][0x20]>(Memory[0x1000])},
        SPixels{reinterpret_cast<uint8_t (&)[0x80][0x40]>(Memory[0x1000])} {
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
                std::ifstream file{name, std::ios::binary};
                file.read(Program, 0xe00);
            }
            SDL_Init(SDL_INIT_VIDEO);
            SetupScreen(false);
            std::thread{[=] {
#ifdef _WIN32
                LogicLoop(Memory);
#else
                LogicLoopPosix(Memory);
#endif
            }}.detach();
            while (!Over) {
                HandleEvents();
                UpdateScreen();
                UpdateTime();
            }
    }

    ~Chip8() {
        SDL_Quit();
    }

private:
    static const uint16_t Zoom = 0x4;

    uint8_t Memory[0x3000];

    SDL_Surface * Screen;
    std::chrono::high_resolution_clock::time_point LastCycle;
    uint64_t LastCount;
    bool WasLarge;

    uint8_t (&V)[0x10];
    uint8_t (&Font)[0x50];
    uint8_t (&SFont)[0xa0];
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
    char (&Program)[0xe00];
    uint8_t (&Pixels)[0x40][0x20];
    uint8_t (&SPixels)[0x80][0x40];

    Chip8 & operator=(const Chip8 &);

    void SetupScreen(bool big) {
        if (big) Screen = SDL_SetVideoMode(128 * Zoom, 64 * Zoom, 8, SDL_SWSURFACE);
        else Screen = SDL_SetVideoMode(64 * Zoom, 32 * Zoom, 8, SDL_SWSURFACE);
    }

    void UpdateScreen() {
        const bool large{Large};
        if (large != WasLarge) {
            WasLarge = large;
            SetupScreen(large);
        }
        SDL_Rect rect {0, 0, Zoom, Zoom};
        if (large) for (uint16_t y {0}; y < 64; ++y) {
            rect.y = y * Zoom;
            for (uint16_t x {0}; x < 128; ++x) {
                rect.x = x * Zoom;
                SDL_FillRect(Screen, &rect, SPixels[y][x]);
            }
        } else for (uint16_t y {0}; y < 32; ++y) {
            rect.y = y * Zoom;
            for (uint16_t x {0}; x < 64; ++x) {
                rect.x = x * Zoom;
                SDL_FillRect(Screen, &rect, Pixels[y][x]);
            }
        }
        SDL_Flip(Screen);
    }

    void UpdateTime() {
        ++CDT;
        if (ST - CST++ > 0) {
            //Beep - Anyone got something cross-platform?
        }
        uint64_t count{Counter};
        const std::chrono::high_resolution_clock::time_point cycle{std::chrono::high_resolution_clock::now()};
        int64_t nano{std::chrono::duration_cast<std::chrono::nanoseconds>(cycle - LastCycle).count()};
        if (LastCount == count) ++count;
        const std::string rate{std::to_string(nano / (count - LastCount)) + " nano per cycle"};
        SDL_WM_SetCaption(rate.c_str(), "");
        LastCycle = cycle;
        LastCount = count;
        std::this_thread::sleep_for(std::chrono::milliseconds{16});
    }

    void HandleEvents() {
        SDL_Event e{};
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

int main(int argc, char ** argv) {
    Chip8{argc > 1 ? argv[1] : "maze.ch8"};
    return 0;
}