Fast one-off Boggle solver, great for building web UIs.

The goal is to minimize the time to get all the words on a board, including
initialization time.

To get going:

    make clean
    make all
    ./makemm scrabble-words scrabble-words.mmapped

The to find the words on a board:

    $ ./webboggle scrabble-words.mmapped abcdefghijklmnop
    fie 11 02 01
    fin 11 02 13
    fink 11 02 13 22
    fino 11 02 13 23
    glop 21 32 23 33
    ink 02 13 22
    jin 12 02 13
    jink 12 02 13 22
    knife 22 13 02 11 01
    knop 22 13 23 33
    kop 22 23 33
    lop 32 23 33
    mink 03 02 13 22
    nim 13 02 03
    plonk 33 32 23 13 22
    pol 33 23 32

The numbers are coordinates on the board, e.g. `11` = 2nd cell from left, 2nd
cell from top.

The idea is that this is really fast:

    $ time ./webboggle scrabble-words.mmapped abcdefghijklmnop > /dev/null
    0.00s user 0.00s system 60% cpu 0.004 total

i.e. 4ms to load the dictionary and find all the words.

REST Server
-----------

Configure Heroku multi buildpack (we need Python & C):

    heroku config:add BUILDPACK_URL=https://github.com/ddollar/heroku-buildpack-multi.git

