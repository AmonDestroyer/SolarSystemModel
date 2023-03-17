# Solar System Model
Interactive graphical model of the solar system utilizing OpenGL and NASA's Open APIs.

## Purpose
This project is for the University of Oregon's CS 441 (Introduction to Computational Graphics) final project.
The project shows knowledge gained and expanded upon during the 2023 Winter term.

### Learning Journey
There were many key learnings taken away from this project. Many ranging from modeling errors to external interfaces.

1. Working on Windows: I found it challenging when working on windows with OpenGL as when I wanted to add new libraries it became extremely difficult to work in wondows and dealing with VS to compile C code. I ended up moving to a Debian VM to work on the project because of the inherrantly difficult error I ran into while working on the project.
2. Learning a New Library: When determining that my model would get it's data from a [NASA API](https://api.nasa.gov/) I had to figure out how my application would interface with the API and this resulted in me having to learn how to utilize [libcurl](https://curl.se/libcurl/) to make these API calls.
3. Parsing API Data: My Model gets it's body position information based on the [NASA's Horizons  API](https://ssd-api.jpl.nasa.gov/doc/horizons.html) which came along with it's own challenge of creating a library that could properly communicate and extract data from the API. The main challenge here was that NASA's Horizons API does not provide a well structured json response, but a dump of data in a [results field](https://ssd-api.jpl.nasa.gov/doc/horizons.html#:~:text=Example%20json%2Dformat%20Output).
4. Trying On My Own: I swear I spent 10 hours, the time initially estimated for item 1 in the project proposal, to figuring out how to apply a texture from various jpegs and render the sphere without artifacts (See Frustom Artifacting in the below section). After taking this time to model staticly colored spheres for each body I ended up needing to move to open source [sphere and texturing](http://www.songho.ca/opengl/gl_sphere.html) which allowed me to apply texturing from my gathered bitmap images.
![OpenGL Spheres](/imgs/openSourceExample.png "OpenGL Spheres")
5. Using Open Source Code:  While using the open source code was helpful, it came with it's own challenges. It took time to learn what each portion of the open source code was doing and how to modify the camera position change the target and ensure the lighting was correct on each body. This was evident in lighting errors I was recieving. See initially I was updating the camera position and direction during each fram render and the lighting was only being set up during initialization. Since the camera was being updated after the lighting this resulted in the lighting to render on the objects as if the light was at the position relative to the cameras coordinate system, not hte homogeneous object coordinates.
![Lighting Error](/imgs/errors_lighting.png "Lighting Error")

#### Key Learnings
- Frustom Artifacting: Have a near and far frustom that are are too far appart from each other will cause issues like missing triangles seen here.

![Distant Frustom Artifacting](/imgs/sphereArtifacting.png "Distant Frustom Artifacting")

### Failures/Successes
#### Failures
- Initial proposal was too ambitious and as such items 1-3 in the proposal were completed.
- I initially tried to work with the provided cmake file, but it was prooved to be more challenging and messy than anticipated. So I ended up moving to a Makefile in the source directory. This meant that the project was less portable due to the lack of a cmake file.

#### Successes
- Being able to read a lot of code and modify it so it did what I wanted it to do.
- Finding resources for textures was initially thought to be challenging, but it was easier than expected. The challenge and implementation was reading in the file.

## System Requirements
OS: Linux

Installed Libraries: See Requried Libraries Section

## Developer Notes
TODO:
- Reference `credentails-skel.ini` needing to be updated and named `credentails.ini`.
- Creation of NASA API account for `API_KEY` needed in `credentails.ini` file.

### Libraries
#### Required Libraries
Below is a list of all required libraries that need to be installed installed.
- libcurl4-openssl-dev
- libglfw3-dev
- libglew-dev
- libglm-dev
- freeglut3-dev

All libraries can be installed after running the below commands.

`sudo apt-get update`

`sudo apt-get install libcurl4-openssl-dev libglfw3-dev libglew-dev libglm-dev freeglut3-dev`

#### Utilized Open Source Code
Below is a list of all open source code utilized in the generation of this project.
- json: https://github.com/nlohmann/json
- Sphere Generation and Texturing: http://www.songho.ca/opengl/gl_sphere.html

### Additional Installed Libraries
These are libraries installed to reduce warngings and make building easier.
- ntp

## Future Improvements
- Rendering Improvements
  - Use LOD spheres depending on how much space the body takes up on the screen.
- Refactoring
  - Refactor the messy main loop into it's own library like Render Manager or something of the like.
- Orbital Mechanics Improvments
  - Add obliquity (axial tilt) for each planet.
  - Allow planets to rotate around their own axis.
- User Interface Improvments
  - Currently the start date is static. Dynamically pull the initial date to be the current day.
  - Make it so the user doesn't need to go to the window to the command line to update the date.
  - Allow the user to adjust sensitivity
- Visuals
  - Add skybox