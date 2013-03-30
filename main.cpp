
/* Simple program:  Create a blank window, wait for keypress, quit.

   Please see the SDL documentation for details on using the SDL API:
   /Developer/Documentation/SDL/docs.html
*/
   
#include "main.h"

int main(int argc, char *argv[])
{
  Game myGame;
  
  myGame.showMenu( );
  myGame.run( );
  myGame.destroy( );
  
  return(0);
}
