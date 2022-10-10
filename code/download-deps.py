import os
import subprocess

os.makedirs("vendor", mode=0o777, exist_ok=True)

subprocess.call(["git", "clone", "https://github.com/libsdl-org/SDL.git", "vendor/sdl2"])
