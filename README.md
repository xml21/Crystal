# Crystal Engine

### Description

|	Name	|	Explanation	|
|	-----	|	-----	|
|	Author	|	Grzegorz "xml21" Piasecki	|
|	Version	|	0.0.1 Pre-Alpha |
|	Tested Platforms |	Windows |
|	Target Platforms	|	As many as possible	|
|	Tested Compilers |	MSVC	|
|	Target Compilers	| As many as possible |
|	Tested Render API's |	OpenGL 4.6 |
|	Target Render API's	| OpenGL, DirectX, Vulkan, Metal |

### Feature list:

**Current:**
* Logging system
* Build system
* Events system (Dispatchers, callbacks etc.)
* PCH support
* Layer based event propagation
* UI Editor
* Input handling system
* API agnostic rendering implementation (currently supports OpenGL)
* Shaders system
* Material system
* Render command queue
* Camera system (Orthographic & Perspective view)
* Physically Based Rendering
* Asset import
* Skeletal Mesh animation

**Upcoming:**
* Fast 2D rendering (UI, particles, sprites, etc.)
* Mac, Linux, Android and iOS support
* Rendering API support (DirectX, Vulkan, Metal)
* Fully featured viewer and editor applications
* Fully scripted interaction and behavior
* 2D and 3D physics engine integration
* Procedural terrain and world generation
* Artificial Intelligence
* Audio system

### Branches:
|	Name	|	Latest feature	|
|	-----	|	-----	|
|	`master` (lastest stable version without assets)	|	PBR. Editor UI.	|
|	`development` (nightly version with assets)	|	Skeletal animation system. Material system.	|

### 3rd party libraries :

|	Name	|	Link	|
|	-----	|	-----	|
|	Premake5	|	https://premake.github.io/download.html	|
|	GLFW	|	https://github.com/glfw/glfw	|
|	Glad	|	https://github.com/Dav1dde/glad	|
|	assimp	|	https://github.com/assimp/assimp	|
|	glm	|	https://github.com/g-truc/glm	|
|	imgui	|	https://github.com/ocornut/imgui	|
|	spdlog	|	https://github.com/gabime/spdlog	|
|	stb	|	http://nothings.org/stb/	|


### Naming Convention

|	Variable Type	|	Example	|
|	-----	|	-----	|
| local variable| ```float Var;``` |
| class member variable | ```unsigned int mIntVar;``` |
| static variable |```static int sIntVar;``` |
| class name | ```class MyClass``` |


### Build System

Build system is based on Premake5. It is capable of generating, building and organizing libraries of a foreign dependency.

### Folder naming conventions

|	Folder	|	Explanation	|
|	-----	|	-----	|
|	/Vendor	|	3rd party library	|
|	/Platform	|	Specific Render API related code |
