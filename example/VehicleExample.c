#include "VehicleExample.h"
#include <stdio.h>
#include <string.h>

/*========================= Vehicle class definitions ========================*/
// definition of the class Vehicle
CLASSDEF(Vehicle);

// class method prototypes
static void Vehicle_Accelerate(Vehicle* pInst, int speed);
static void Vehicle_Brake(Vehicle* pInst);

// Init method as per CLASSDEF contract
static void Vehicle_Init(Vehicle* pInst, const Vehicle_Cfg* pCfg)
{
    pInst->pMembers->TopSpeed = pCfg->TopSpeed;
    pInst->pMembers->CurrentSpeed = 0;
}

// Deinit method as per CLASSDEF contract
static void Vehicle_Deinit(Vehicle* pInst)
{
    (void) pInst; // nothing to deinitialize
}

// Vtable Init method as per CLASSDEF contract
void Vehicle_VtableInit(Vehicle_Vtable* pVtable)
{
    pVtable->Accelerate = Vehicle_Accelerate;
    pVtable->Brake = Vehicle_Brake;
}

// class method definition for Accelerate
static void Vehicle_Accelerate(Vehicle* pInst, int speed)
{
    if (    (speed > pInst->pMembers->CurrentSpeed)
        &&  (speed <= pInst->pMembers->TopSpeed))
    {
        printf("The vehicle accelerates to %d km/h.\n", speed);
        pInst->pMembers->CurrentSpeed = speed;
    }
}

// class method definition for Brake
static void Vehicle_Brake(Vehicle* pInst)
{
    printf("The vehicle brakes to a halt.\n");
    pInst->pMembers->CurrentSpeed = 0;
}

/*========================= Car class definitions ============================*/
// definition of the class Car that derives from the class Vehicle
CLASSDEF(Car, Vehicle);

// class method prototypes
static void Car_Accelerate(Vehicle* pInst, int speed);
static void Car_Fuel(Car* pInst);

// Init method as per CLASSDEF contract
static void Car_Init(Car* pInst, const Car_Cfg* pCfg)
{
    pInst->pMembers->Seats = pCfg->Seats;
}

// Deinit method as per CLASSDEF contract
static void Car_Deinit(Car* pInst)
{
    (void) pInst; // nothing to deinitialize
}

// Vtable Init method as per CLASSDEF contract
void Car_VtableInit(Car_Vtable* pVtable)
{
    // here, the base class method for Accelerate gets overridden
    pVtable->Base.Accelerate = Car_Accelerate;
    pVtable->Fuel = Car_Fuel;
    // the base class method Brake is not overridden by Car class. Therefore,
    // no action with respect to this method is needed here.
}

// Class method Accelerate. This overrides the method provided from Vehicle
// class, thus the type of the instance variable is of Vehicle*, not Car*.
static void Car_Accelerate(Vehicle* pInst, int speed)
{
    // cast Vehicle instance variable to its child class
    Car* pCar = GET_CHILDCLASS(pInst, Car, Base);
    if (    (speed > pInst->pMembers->CurrentSpeed)
        &&  (speed <= pInst->pMembers->TopSpeed))
    {
        printf("The car with %d seats accelerates to %d km/h.\n", 
            pCar->pMembers->Seats, speed);
        pInst->pMembers->CurrentSpeed = speed;
    }
}

// Class method Fuel
static void Car_Fuel(Car* pInst)
{
    printf("The car was refueled.\n");
}

/*========================= Racecar class definitions ========================*/
// definition of the class Car that derives from the class Vehicle
CLASSDEF(Racecar, Car);

// class method prototypes
static void Racecar_Accelerate(Vehicle* pInst, int speed);

// Init method as per CLASSDEF contract
static void Racecar_Init(Racecar* pInst, const Racecar_Cfg* pCfg)
{
    /* here, a char array is allocated on the heap */
    const int NameLength = 32;
    char* pDriver = calloc(NameLength, sizeof *pDriver);
    (void) memcpy(pDriver, pCfg->pDriver, (NameLength - 1));
    pInst->pMembers->pDriver = pDriver;
}

// Deinit method as per CLASSDEF contract
static void Racecar_Deinit(Racecar* pInst)
{
    // here, any memory allocated in _Init must be freed again
    free(pInst->pMembers->pDriver);
}

// Vtable Init method as per CLASSDEF contract
void Racecar_VtableInit(Racecar_Vtable* pVtable)
{
    // here, Base must be used twice
    pVtable->Base.Base.Accelerate = Racecar_Accelerate;
}

// Class method Accelerate. This overrides the method from two levels above
static void Racecar_Accelerate(Vehicle* pInst, int speed)
{
    // cast Vehicle instance variable to its child class
    Racecar* pCar = GET_CHILDCLASS(pInst, Racecar, Base.Base);
    if (    (speed > pInst->pMembers->CurrentSpeed)
        &&  (speed <= pInst->pMembers->TopSpeed))
    {
        printf("The racecar of %s accelerates to %d km/h.\n", 
            pCar->pMembers->pDriver, speed);
        pInst->pMembers->CurrentSpeed = speed;
    }
}

/*========================= main =============================================*/
int main(void)
{
    printf("Demonstration of classes.h\n");
    
    // define constructor arguments for a new vehicle object
    Vehicle_Cfg VehicleCfg = {.TopSpeed = 390};
    // instantiated a new vehicle object
    Vehicle* pV = NEW(Vehicle, &VehicleCfg);
    // call the vehicle method 'Accelerate'
    VCALL(pV, Accelerate, 200);
    // call the vehicle method 'Brake'
    VCALL(pV, Brake);
    // delete the object and free the memory it occupied
    DELETE(pV);

    // now, define the constructor arguments for a Car object which inherits 
    // from Vehicle
    Car_Cfg CarCfg = {.Base = {.TopSpeed = 250}, .Seats = 4};
    // instantiate a new vehicle object
    Car* pC = NEW(Car, &CarCfg);
    // call the Vehicle method 'Accelerate'. As this method belongs to the 
    // Vehicle class, the address to the parent class instance needs to be 
    // provided as argument. The fact, that 'Accelerate' was overridden by the 
    // Car class does not impact the way how the method is called
    VCALL(&(pC->Base), Accelerate, 230);
    // another way to invoke a parent class method is to simply cast the 
    // instance handle to a parent class handle. This is possible because the 
    // first element of a derived class is always the struct of its parent, 
    // i.e. "pC == &(pC->Base)".
    // Although this relation is 'hidden' in the macros CLASSDECL and CLASSDEF,
    // it is useful when handling multiple levels of inheritance, as it avoids
    // the need of nested struct access (pInst->Base.Base.Base...).
    VCALL((Vehicle*) pC, Brake);
    // call the Car method 'Fuel'
    VCALL(pC, Fuel);
    // delete the object and free the memory it occupied
    DELETE(pC);

    // Now define constructor arguments for a racecar. The constructor must
    // initialize all the members of the base classes' cfg structs, too
    const char Driver[] = "Ayrton Senna"; 
    Racecar_Cfg RacecarCfg = 
    {
        .Base = 
        {
            .Base = 
            {
                .TopSpeed = 350
            }, 
            .Seats = 1
        }, 
        .pDriver = Driver
    };
    // instantiate the new Racecar object
    Racecar* pR = NEW(Racecar, &RacecarCfg);
    // call inherited method from two levels above
    VCALL(&(pR->Base.Base), Accelerate, 300);
    // as the double indirection is getting a little ugly, casting may become
    // the preferrable way of invoking base class methods
    VCALL((Vehicle*) pR, Brake);
    // DELETE also handles deallocation of the dynamically allocated string
    // by invoking Racecar_Deinit.
    DELETE(pR);

    printf("End of demonstration of classes.h\n");
}