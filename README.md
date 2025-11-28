# Classes-For-C
A lightweight extension for the C language that provides classes, objects, polymorphism and inheritance.


## Overview and features

Classes For C is a small project - it really is just a single header - that provides some of the most defining characteristics known from object-oriented languages.  

### Features: 
- Defining classes and instantiating objects
- Allocation and Deallocation with new- and delete-like operators
- Inheritance of classes, including multi-level inheritance
- Polymorphism of functions (methods)
- C99 compatibility without additional dependencies
- Low memory and computational overhead


## Usage

TBD  

### Class declaration

### Class definition

### Instantiating classes

### Invoking methods

### Deallocation

## Known Limitations

There are some limitations to Classes For C that the user should be aware of.

**Scattered Memory.** It is important to understand what is referred to as 'object' in the scope of this project, as the word here is used to talk about the high-level OOP-borne concept of one instance of a class.  
Such an object is not an object in the sense of the C standard, which is a region of data storage in the execution environment. In fact, when instaniating an object with `NEW`, there are several objects in the C sense created: The class wrapper struct, the member struct and any additional memory that might be dynamically allocated in the user-defined Init-function.  
As a consequence, usage of the `sizeof` operator on an object will most likely not yield a meaningful result, and cannot easily be substituted either (especially not at compile time).

**No standard way of object copying.** Following up on the last point, `memcpy()` calls with an object as source are rarely useful and there is currently no standardized way of copying object instances. It would be easy to add a function akin to NEW that creates a shallow copy of an object (Class wrapper and member struct), however to create a deep copy likely requires an additional mandatory user-defined Copy-function even if it is not needed.  
As copying objects almost never was a need in my personal projects, the decision was made to not implement a `COPY()` function, so that the developer of a new class is not burdened with writing even more boilerplate code.  

Other features typically associated with OOP notably absent:  

**No garbage collection.** All objects instantiated with `NEW()` must eventually be deallocated with `DELETE()`.  

**No multiple inheritance.** And it will stay that way.  

**No encapsulation in form of private, public and protected members.** No effort in providing encapsulation of class members was made - every member is public by default.