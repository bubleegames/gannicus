#Gannicus 

Gannicus is an open-source,
           gameplay-focused,
           deterministic,
           2D fighting game engine
        developed from the ground up and
        built using C++, SDL, and OpenGL.

##Building and testing gannicus on Linux and Mac OS X

The current version of gannicus can be built using Boost.Build.

1. Install the dependencies:

  a) [`SDL`](http://www.libsdl.org/download-1.2.php),
     [`SDL\_image`](http://www.libsdl.org/projects/SDL_image/) and
     [`SDL\_mixer`](http://www.libsdl.org/projects/SDL_mixer/)
      * Linux: Available in most distributions.
      * Mac OS X: Framework packages on website.

  b) [`OpenGL`](http://www.opengl.org/wiki/Getting_Started#Linux) and
     [`GLUT`](http://freeglut.sourceforge.net/)
      * Linux: Available in most distributions (`mesa`, `freeglut`).
      * Mac OS X: System component.

  d) Basic build software
      * Linux: Most distributions have a meta-package. For example:
        * Fedora: `su -c "yum groupinstall 'Development Tools'"`
        * Ubuntu: `su -c "apt-get install build-essential"`
      * Mac OS X: Use `clang++`. See below.

  e) Boost.Build

2. Clone this repository
  (`git clone git@github.com:h-forrest-alexander/gannicus.git`) and `cd` to it.


3. Run `bjam` or `b2` to build the game.

## Running the game

  * Arguments:
    * `eleven`: Play an unlimited time match.
    * `<replay file name>`: Playback a saved replay.
