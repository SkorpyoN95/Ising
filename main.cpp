#include <allegro5/allegro.h>
#include "model.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;
int main(int argc, char** argv)
{
    if(argc != 5){
        cerr << "Invalid list of arguments" << endl;
        cerr << "Proper form:" << endl << "ising.exe width height kT H" << endl;
        exit(1);
    }
    al_init();
    al_install_keyboard();
    ALLEGRO_KEYBOARD_STATE klawiatura;
    ALLEGRO_DISPLAY * okno = al_create_display( atoi(argv[1]), atoi(argv[2]) );
    al_set_window_title( okno, "Ising's Model" );
    Model model = Model(atoi(argv[1]), atoi(argv[2]), atof(argv[3]), atof(argv[4]));
    while( !al_key_down( & klawiatura, ALLEGRO_KEY_ESCAPE ) )
    {
        al_get_keyboard_state( & klawiatura );
        model.metropolisStep();
        model.drawLattice();
        al_flip_display();
    }
    al_destroy_display( okno );
    return 0;
}
