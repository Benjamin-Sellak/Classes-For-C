# Classes-For-C
A lightweight extension for the C language that provides classes, objects, polymorphism and inheritance.

## Overview

**Classes-For-C** is a single-header, macro-based framework that brings a familiar object-oriented programming model to plain C. It is designed to be:

- Lightweight (just one header)
- Explicit and predictable
- Friendly to embedded and low-level environments
- Compatible with standard C toolchains

The framework provides:

- Classes and objects
- Single inheritance with unlimited depth
- Virtual methods (runtime polymorphism)
- Constructor and destructor semantics
- `new` / `delete`-style object lifetime management

All functionality is implemented using standard C structs and macros — no compiler extensions, no runtime dependencies.  
There are several ressources with an in-depth explanation how object-oriented programming in C can be achieved that can be found online - most notably the Book "Object-Oriented Programming with ANSI-C" by Axel-Tobias Schreiner and various publications by Laurent Deniau about [OOP in C](http://ebook.pldworld.com/_eBook/C%EF%BC%8FC++/ldeniau.web.cern.ch/ldeniau/html/oopc/oopc.html). If you are not familiar with the concept of virtual function tables or how C struct subtypes can be defined in a way so that they satisfy the Liskov Substitution Principle, I highly suggest reading up on those ressources before continuing with *Classes For C*.  

---

## Basic Concepts

### What is a class?

A *class* in Classes-For-C consists of four user-defined parts:

1. **The class itself** (`Type`)
2. **A configuration struct** (`Type_Cfg`)
3. **A virtual table** (`Type_Vtable`)
4. **A members struct** (`Type_Members`)

The framework generates the glue code that ties these pieces together.

---

## Declaring a Class

Class declarations belong in header files.

### Base (root) class

```c
CLASSDECL(Vehicle)
```

### Derived class

```c
CLASSDECL(Car, Vehicle)
```

`CLASSDECL` forward-declares all framework-managed structures and functions for the class.

---

## Defining a Class

Each class must be defined **exactly once** in a source (`.c`) file.

### Base class definition

```c
CLASSDEF(Vehicle);
```

### Derived class definition

```c
CLASSDEF(Car, Vehicle);
```

`CLASSDEF` generates:

- The class constructor (`New`)
- The class destructor (`Delete`)
- Vtable initialization logic
- Automatic base-class chaining

---

## Required User Functions

For each class, the user must implement the following static functions:

```c
static void Type_Init(Type* pInst, const Type_Cfg* pCfg);
static void Type_Deinit(Type* pInst);
static void Type_VtableInit(Type_Vtable* pVtable);
```

### Init

Called during object creation, after base classes are initialized.

Typical responsibilities:

- Initialize member variables
- Copy values from the configuration struct
- Set initial state

### Deinit

Called during object destruction, before base classes are deinitialized.

Typical responsibilities:

- Release owned resources
- Perform cleanup

> Do **not** free the object or its members manually — the framework handles this.

### VtableInit

Used to assign or override virtual methods:

```c
pVtable->Accelerate = Vehicle_Accelerate;
```

Base-class methods are already populated before this function is called.

---

## Object Creation and Destruction

### Creating an object

```c
Vehicle_Cfg cfg = {
    .TopSpeed = 180
};

Vehicle* vehicle = NEW(Vehicle, &cfg);
```

### Destroying an object

```c
DELETE(vehicle);
```

Deletion is safe through base-class pointers.

---

## Virtual Methods and Polymorphism

```c
VCALL(vehicle, Accelerate, 50);
```

---

## Inheritance

- Single inheritance only
- Unlimited inheritance depth
- Automatic constructor / destructor chaining

### Downcasting

```c
Car* car = GET_CHILDCLASS(vehiclePtr, Car, Base);
```

---

## Known Limitations and Design Tradeoffs

Classes-For-C is intentionally small and pragmatic. It was developed to solve concrete problems in real projects, not to be a complete object system for C. As a result, users should be aware of the following limitations and tradeoffs.

### Scattered memory layout

An object created with `NEW()` is a *logical* object, not a single C object in the sense of the C standard. Internally, multiple allocations are performed:

- the class wrapper struct
- the class member struct
- any additional memory allocated by user code during `Init`

Because of this:

- `sizeof(object)` is not meaningful
- object instances are not contiguous in memory
- objects cannot be trivially stack-allocated

This design favors flexibility and clean separation of concerns over strict memory locality.

### No standard object copying

There is no built-in way to copy objects.

- `memcpy()` on an object is almost never correct
- shallow copies are insufficient for most real-world classes
- deep copies require class-specific knowledge

To avoid forcing unnecessary boilerplate on every class, the framework intentionally does **not** provide a `COPY()` primitive. If copying is required, it must be implemented explicitly by the class designer.

### Limited runtime safeguards

The framework performs some compile-time validation (e.g. missing required hooks), but not all misuse can be detected early.

Examples of unchecked errors:

- forgetting to initialize a vtable method
- calling a virtual method through a NULL pointer

Such mistakes will typically compile but fail at runtime. This is an explicit tradeoff to keep the framework lightweight and dependency-free.

### No garbage collection

All objects created with `NEW()` must be destroyed using `DELETE()`.

The framework does not provide reference counting or automatic lifetime management.

### No multiple inheritance

Only single inheritance is supported, by design. This keeps object layout predictable and avoids ambiguity in vtable resolution.

### No enforced encapsulation

There is no language-level notion of `private`, `protected`, or `public` members.

All members are technically accessible. Encapsulation can be achieved by convention, for example by:

- storing private state behind a `void*` inside the members struct
- defining private types and functions at file scope

---

## Complete Example

See `VehicleExample.c/.h` for a full working example.

---

## License

MIT License

