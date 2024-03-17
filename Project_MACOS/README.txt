Graphic 50.017 - Assignment 3.

The following document explains the methodology and challenges I ecnoutnered while finishing Assignment 3.
The document also explains how to run and load multiple texture files to the program
Lastly, note that screenshots of my resulting code can be found in the screenshots/ directory.

=========================================
USAGE
=========================================

The usage is quite simple. When creating the executable in the build/ directory. One simply has to execute the binary.
```
./Assignment_3 <texture map to load>
```

Note that the second argument of this execution command is OPTIONAL. That is, if `./Assignment_3` is only ran, it will only load the default texture found in data/

Apart from that, the controls explained in the handout work exactly the same.

=========================================
PART 1: Planar Mapping
=========================================

This section was actually quite simple, since the equation for planar texture mapping was already given. In short I just followed what the handout said to do and calculate.
Apart from that I had some issues loading the texture as I accidently build my file outside the build/ directory. Please ensure that ./Assigment_3 is found within the build/ directory.

=========================================
PART 2: Cylindrical Mapping
=========================================

This section was tougher, as an equation was not given to me. Thus I had to scour the internet for any resources or hints on how to implement a cylindrical parametrization. But I was able to find it in some obscure StackOverflow post that I totally forgot to document.

Apart from 'deriving the equation', the implementation was kind of the same. Just calculate using the vertex's coordinates given by myObject.vertex[i].v .

=========================================
PART 3: Spherical Mapping
=========================================

Just like Cylindrical mapping, an equation was not given. This time though, I set out to derive it myself by remembering the change of corrdinates from 3D to Spherical. Using that information I inferred that to map from a 2D plane (the texture map) to the 3D space of our object we needed some small modification of the regular 3D Carteesian->Spherical mapping.

At the end I ended up with a pretty good solution. And the results are quite similar.
