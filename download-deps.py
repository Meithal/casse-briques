import os
import subprocess

os.makedirs("vendor", mode=0o777, exist_ok=True)

subprocess.call(["git", "clone", "--depth=1", "-b", "SDL2", "https://github.com/libsdl-org/SDL.git", "vendor/sdl2"])
subprocess.call(["git", "clone", "--depth=1", "https://gitlab.freedesktop.org/freetype/freetype.git", "vendor/freetype"])
subprocess.call(["git", "clone", "--depth=1", "https://github.com/tronkko/dirent", "vendor/dirent"])
subprocess.call(["git", "clone", "--depth=1", "https://gitlab.com/Meithal/cisson.git", "vendor/json"])
