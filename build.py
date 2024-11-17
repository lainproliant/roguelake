#!/usr/bin/python
# --------------------------------------------------------------------
# build.py
#
# Author: Lain Musgrove (lain.proliant@gmail.com)
# Date: Saturday, November 16 2024
# --------------------------------------------------------------------

from xeno.build import build, task
from xeno.recipes import checkout

# --------------------------------------------------------------------
DEPS = ["https://github.com/lainproliant/moonlight"]


# --------------------------------------------------------------------
@task(keep=True)
def deps():
    return [checkout(repo) for repo in DEPS]


# --------------------------------------------------------------------
build()
