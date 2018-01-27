#include "model.h"

using namespace std;
Model::Model(unsigned int _width, unsigned int _height, double _T, double _H) : height(_height), width(_width), T(_T), H(_H)
{
    lattice = al_create_bitmap(_width, _height);
    if(!lattice){
        cerr << "ALLEGRO5 failed to create lettice" << endl;
        exit(1);
    }
    generator = gsl_rng_alloc(gsl_rng_ranlux);
    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_set_target_bitmap(lattice);
    spin_values = new bool[_height * _width];
    bool spin;
    al_lock_bitmap(lattice, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
    for(int a = 0, z = _height * _width; a < z; ++a){
        spin = gsl_rng_uniform_int(generator, 2);
        spin_values[a] = spin;
        al_put_pixel(a / height, a % height, spin ? al_map_rgb(255,0,0) : al_map_rgb(0,0,255));
        spin = !spin;
    }
    al_unlock_bitmap(lattice);
    countTotalEnergy();
    //al_clear_to_color(al_map_rgb(255,0,0));
    al_set_target_backbuffer(display);
}

Model::~Model()
{
    al_destroy_bitmap(lattice);
    gsl_rng_free(generator);
    delete[] spin_values;
}

void Model::drawLattice(){
    al_draw_bitmap(lattice,0,0,0);
}

void Model::setSeed(unsigned long int seed){
    gsl_rng_set(generator, seed);
}

void Model::countTotalEnergy(){
    E = 0.0;
    for(int i = 0, h = height - 1, w = width - 1; i < h; ++i){
        for(int j = 0; j < w; ++j){
            E -= spin_values[i*width + j] == spin_values[i*width + j + 1] ? 1.0 : -1.0;
            E -= spin_values[i*width + j] == spin_values[(i+1)*width + j] ? 1.0 : -1.0;
        }
        E -= spin_values[i*width + w] == spin_values[(i+1)*width + w] ? 1.0 : -1.0;
        E -= spin_values[h*width + i] == spin_values[h*width + i + 1] ? 1.0 : -1.0;
    }
    double ee = 0.0;
    for(int a = 0, z = height*width; a < z; ++a)
        ee -= spin_values[a] ? 1.0 : -1.0;
    E -= H * ee;
}

void Model::flipSpin(unsigned int i){
    spin_values[i] = !spin_values[i];
    ALLEGRO_DISPLAY* display = al_get_current_display();
    al_set_target_bitmap(lattice);
    al_put_pixel(i / height, i % height, spin_values[i] ? al_map_rgb(255,0,0) : al_map_rgb(0,0,255));
    al_set_target_backbuffer(display);
}

void Model::metropolisStep(){
    int i = gsl_rng_uniform_int(generator, height * width);
    double old_e = 0.0, new_e = 0.0;
    if(i >= width){
        old_e -= spin_values[i] == spin_values[i - width] ? 1.0 : -1.0;
        new_e -= !spin_values[i] == spin_values[i - width] ? 1.0 : -1.0;
    }
    if(i <= (height-1)*width){
        old_e -= spin_values[i] == spin_values[i + width] ? 1.0 : -1.0;
        new_e -= !spin_values[i] == spin_values[i + width] ? 1.0 : -1.0;
    }
    if(i%height > 0){
        old_e -= spin_values[i] == spin_values[i - 1] ? 1.0 : -1.0;
        new_e -= !spin_values[i] == spin_values[i - 1] ? 1.0 : -1.0;
    }
    if(i%height < (width-1)){
        old_e -= spin_values[i] == spin_values[i + 1] ? 1.0 : -1.0;
        new_e -= !spin_values[i] == spin_values[i + 1] ? 1.0 : -1.0;
    }
    old_e -= spin_values[i] ? H : -H;
    new_e -= !spin_values[i] ? H : -H;
    if(new_e < old_e){
        flipSpin(i);
    }else{
        double r = gsl_rng_uniform_pos(generator);
        if(r < gsl_sf_exp(- (new_e - old_e) / (T))){
            flipSpin(i);
        }
    }
}
