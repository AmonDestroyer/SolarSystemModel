# Solar System Model
Interactive graphical model of the solar system utilizing OpenGL and NASA's Open APIs.

## Purpose
This project is for the University of Oregon's CS 441 (Introduction to Computational Graphics) final project.
The project shows knowledge gained and expanded upon during the 2023 Winter term.

### Learnings
- Frustom Artifacting: Have a near and far frustom that are are too far appart from each other will cause issues like missing triangles seen here.

![Distant Frustom Artifacting](/imgs/sphereArtifacting.png "Distant Frustom Artifacting")

- Open Source Tools/References: Finding and using open source tools and references was a major learning in this project as initially I tried to refactor code provided in Project 2A (Rendering a Dog in OpenGL), but I kept running into issues like how to get user input and texture rendering. Using the open source code for json parsing and sphere generation/texturing provided a base that could be referenced and updated.

- Learning how to convert typical math expressions to code was a challenge for this project, but I was able to relearn and properly implement projections of vector math to get the desired location and renderings. 

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

All libraries can be installed after running the below commands.

`sudo apt-get update`

`sudo apt-get install libcurl4-openssl-dev libglfw3-dev libglew-dev libglm-dev`

#### Utilized Open Source Code
Below is a list of all open source code utilized in the generation of this project.
- json: https://github.com/nlohmann/json
- Sphere Generation and Texturing: http://www.songho.ca/opengl/gl_sphere.html

### Additional Installed Libraries
These are libraries installed to reduce warngings and make building easier.
- ntp

## Future Improvements
- Refactoring
  - Refactor the messy main loop into it's own library like Render Manager or something of the like.
- Orbital Mechanics Improvments
  - Add obliquity (axial tilt) for each planet.
  - Allow planets to rotate around their own axis.
- User Interface Improvments
  - Currently the start date is static. Dynamically pull the initial date to be the current day.
  - Make it so the user doesn't need to go to the window to the command line to update the date.
- Visuals
  - Add skybox