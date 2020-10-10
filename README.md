# W Language

## What is W Lang and why was it created

Back when I was in college I took a compiler course. The course was writing a compiler for C which generated sudo assembly using some tools like GNU Bision. I was inspired by this course and wanted to create my own programing language with a compiler without the use of a framework. I should of done this project in C since the majority of compilers are written in C generally but I wanted to create something to show my skills in C++ as a portfolio project. If this language has a purpose besides writing it for fun it would be to help teach basic concepts of programming. I don't expect this language to get picked up of the sort. 
  
W Lang is a weak typed procedual language when compiled creates bytelike code (psudo assembly) when then the bytecode gets interpreted. W Lang can be thought up like BASIC in terms on being a simple language. This language only conatins intetegers and characters as primitive data types with the support of arrays and strings. I didn't include floating point numbers or boolean primitive values in this iteration because in order to get the development process flowing in order to develop the Abstract Syntax tree design, LL(1) vs LR(1) parser selection, and bytecode processor. Since the foundation of the language is laid out, I plan to revist this project later to include floats, doubles, and booleans as primitaive types. Just like the reason for the primitive types choosing W lang to be a procedual language seem to make sense, and I will also plan to create this language to be Object Oriented as well. It doesn't have any frameworks or anything of that sort to help build applications using W Lang. Maybe in the future there will be such a need of a framework, but as of now thats not the case. 

## How to build and use W_lang compiler

For Mac and Windows
- Go onto Xcode/VisualStudio and run a build. Then take the executable and run the file from the terminal.

If using the Makefile in Main
- just run the make file and take wlang executable file and run file from the terminal.

If you look at the demos folder, copy and paste those files where your executable is located. Once there you can do the following.

./wlang algro.w

or with display

./wlang algro.w -d

## Challenges
1. Creating what the systax and type its going to be. Weak types were a better choice on parsing and then able to distinguish.  

## Known Bugs
1. Raw strings can't be passed as parameters. Work around is to assign a variable and then pass that variable in.
2. In switch statements, you cannot have multiple cases stack on each other. You have to use case and then follow by a break. 

## Future Features
1. Fix know bugs. I just wanted a break from the project before addressing those bugs again.
2. Make the language object oriented.
3. Include floating point numbers.
4. Add try catch.
5. Replace LL(1) parser to LR(1)
6. Create Frameworks for Web, Mobile, Applications, and Tools. This will need more system apis like reading files and so forth.

# How To Code in W Language

Wanna try W lang? Here is how you code it

## Variables

Variable naming convention for W Lang is very similar to languages like C#, Java, and C++. 
```javascript
// correct
let alllower = 234;
let cammelCase = 432;
let _canUseUnderScores = 333;
let or_be_like_python = 222;
let useNumbers123 = 123;

// WRONG
let NOCAPS = 'N';
let 123NumbersFirst = -234;
let $pecialCharacters = 'n';
```

There are only two types of variables. Integers and characters.
```javascript
let integer = 234;
let character = 'W';
```

Don't want your variables to change? Thats fine you can make constants.
```javascript
const theW = 'W'
```

## Arrays and Strings

## Functions

## System Functions

## Files

