#ifndef MODEL_H
#define MODEL_H
#include <allegro5/allegro.h>
#include <stdlib.h>
#include <iostream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_exp.h>

class Model
{
    public:
        Model(unsigned int, unsigned int, double, double);
        ~Model();
        void countTotalEnergy();
        void metropolisStep();
        void drawLattice();
        void setSeed(unsigned long int);

    protected:

    private:
        const unsigned int height;
        const unsigned int width;
        ALLEGRO_BITMAP* lattice;
        //true - spin value 1
        //false - spin value -1
        bool* spin_values;
        gsl_rng* generator;
        double E, T, H;
        void flipSpin(unsigned int);
};

#endif // MODEL_H
