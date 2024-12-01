#!/usr/bin/python
# --------------------------------------------------------------------
# build.py
#
# Author: Lain Musgrove (lain.proliant@gmail.com)
# Date: Saturday, November 16 2024
# --------------------------------------------------------------------

from xeno.build import build, recipe, task
from xeno.recipes import checkout, sh
from xeno.recipes.cxx import ENV, compile
from xeno.recipes.shell import ShellRecipe
from xeno.shell import Environment

# --------------------------------------------------------------------
DEPS = [
    "https://github.com/lainproliant/moonlight",
]

SDL_EXTS = ["SDL_image", "SDL_mixer", "SDL_ttf"]
CMAKE_PREFIX_ENV = Environment(
    CMAKE_PREFIX_PATH=["./pfx"], PKG_CONFIG_PATH=["./pfx/lib/pkgconfig"]
)
SDL_EXT_ENV = Environment.context().append(CMAKE_PREFIX_ENV)

ENV.append(
    CMAKE_PREFIX_ENV,
    CFLAGS=["-I./deps/moonlight/include"],
)


# --------------------------------------------------------------------
@recipe
def sdl_compile(src, target, sdl_cflags, sdl_ldflags, headers=[]):
    env = ENV.copy().append(CFLAGS=sdl_cflags, LDFLAGS=sdl_ldflags)
    return compile(src, headers=headers, target=target, env=env)


# --------------------------------------------------------------------
@recipe(factory=True)
def build_sdl_ext(ext):
    target = f"./exts/{ext}"
    return sh("./bin/build-sdl-ext.sh {ext}", ext=ext, target=target, env=SDL_EXT_ENV)


# --------------------------------------------------------------------
@task(keep=True)
def sdl():
    return build_sdl_ext("SDL")


# --------------------------------------------------------------------
@task(dep="sdl", keep=True)
def sdl_exts(sdl):
    return [build_sdl_ext(ext) for ext in SDL_EXTS]


# --------------------------------------------------------------------
@task(keep=True)
def deps():
    return [checkout(repo) for repo in DEPS]


# --------------------------------------------------------------------
@task(dep="sdl_exts")
def sdl_cflags(sdl_exts):
    return sh(
        "pkg-config sdl3 sdl3-image sdl3-ttf sdl3-mixer --cflags",
        env=SDL_EXT_ENV,
        result="args",
    )


# --------------------------------------------------------------------
@task(dep="sdl_exts")
def sdl_ldflags(sdl_exts):
    return sh(
        "pkg-config sdl3 sdl3-image sdl3-ttf sdl3-mixer --libs",
        env=SDL_EXT_ENV,
        result="args",
    )


# --------------------------------------------------------------------
@task(dep="deps", factory=True)
def basic(deps, sdl_exts, sdl_cflags, sdl_ldflags):
    return sdl_compile(
        "./src/basic.cpp",
        target="basic",
        sdl_cflags=sdl_cflags,
        sdl_ldflags=sdl_ldflags,
    )


# --------------------------------------------------------------------
@task()
def cc_json():
    return sh("intercept-build ./build.py basic -R; ./build.py -c basic")


# --------------------------------------------------------------------
build()
