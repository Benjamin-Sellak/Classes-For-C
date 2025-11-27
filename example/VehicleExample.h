#ifndef VEHICLEEXAMPLE_H_
#define VEHICLEEXAMPLE_H_

#include "classes.h"


/*========================= Vehicle class declarations =======================*/
CLASSDECL(Vehicle)

struct Vehicle_Cfg 
{
    int TopSpeed;
};

struct Vehicle_Vtable
{
    void (*Accelerate)(Vehicle* pInst, int Speed);
    void (*Brake)(Vehicle* pInst);
};

struct Vehicle_Members
{
    int TopSpeed;
    int CurrentSpeed;
};

/*========================= Car class declarations =======================*/
CLASSDECL(Car, Vehicle)

struct Car_Cfg
{
    Vehicle_Cfg Base;
    int Seats;
};

struct Car_Vtable
{
    Vehicle_Vtable Base;
    void (*Fuel)(Car* pInst);
};

struct Car_Members
{
    int Seats;
};

/*========================= Racecar class declarations =======================*/
CLASSDECL(Racecar, Car)

struct Racecar_Cfg
{
    Car_Cfg Base;
    const char* pDriver;
};

struct Racecar_Vtable
{
    Car_Vtable Base;
};

struct Racecar_Members
{
    char* pDriver;
};

#endif 