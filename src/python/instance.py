from screen import *
from player import *
from pygame import K_q, K_ESCAPE

class Instance(Screen, Input):
  """
  """
  def __init__(self, width, height):
    Screen.__init__(self, width, height)
    Input.__init__(self)
    self.events = {\
      QUIT: self.quit,
      KEYDOWN: self.keydown
    }
    self.keys = {\
      K_q: self.quit,
      K_ESCAPE: self.quit
    # F10: scaling factor
    # F11: Fullscreen
    }


  def resolve(self):
    """
      Every inheriting type will need a resolve loop.
      This function is purely virtual, and does nothing.
    """
    pass

  def quit(self):
    pass