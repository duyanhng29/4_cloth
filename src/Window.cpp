////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"

////////////////////////////////////////////////////////////////////////////////

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Project 4";

// Objects to render
//Cube * Window::cube;

Player* Window::player = nullptr;
Cloth* Window::cloth = nullptr;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

//gui
std::vector<std::string> Window::jointsName;
std::vector<glm::vec3> Window::jointsRot;
std::vector<glm::vec3> Window::jointsRotMin;
std::vector<glm::vec3> Window::jointsRotMax;


// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{

	cloth = new Cloth(
		4.0f, // size 
		100.0, //mass 
		25, // N particles
		glm::vec3(-2.0f, 3.0f, 0.0f), //topleft
		glm::vec3(1.0f, 0.0f, 0.0f), // horizontal
		glm::vec3(0.0f, -1.0f, 0.0f), // vertical
		shaderProgram);

	return true;
}

void Window::cleanUp()
{

	if (cloth) delete cloth;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}


// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwSetWindowPos(window, 700, 300);

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	if (height == 0.0f) return;
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}


// update and draw functions
void Window::idleCallback()
{

	// Perform any updates as necessary. 
	Cam->Update();

	//cube->update();
	if (player != nullptr) player->update();

	if (cloth != nullptr) cloth->Update();
	
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	if (cloth != nullptr)
	{
		cloth->Draw(Cam->GetViewProjectMtx());
	}


	glfwPollEvents();

	plotImGUI();

	glfwSwapBuffers(window);
}


// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}


// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;
		case GLFW_KEY_O:
            Cam->SetDistance((Cam->GetDistance() * 2.5));
        	break;
        case GLFW_KEY_I:
            Cam->SetDistance((Cam->GetDistance() / 2.5));
            break;
		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}


void Window::plotImGUI()
{
	

	//start imgui new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;

	//gui window

	ImGui::Begin("Coefficients");

	std::string fps = "FPS: " + std::to_string(cloth->GetFPS());
	ImGui::Text(fps.c_str());


	if (ImGui::TreeNode("Translation and Rotation"))
	{
		float posShift = 0.05f;
		if (ImGui::Button("\t+\t###XP"))
		{
			cloth->TranslateFixedParticles(0, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###XM"))
		{
			cloth->TranslateFixedParticles(0, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis X Translation");

		if (ImGui::Button("\t+\t###YP"))
		{
			cloth->TranslateFixedParticles(1, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###YM"))
		{
			cloth->TranslateFixedParticles(1, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis Y Translation");

		if (ImGui::Button("\t+\t###ZP"))
		{
			cloth->TranslateFixedParticles(2, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###ZM"))
		{
			cloth->TranslateFixedParticles(2, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis Z Translation");

		float rotShift = 0.02f;
		if (ImGui::Button("\t+\t###RXP"))
		{
			cloth->RotateFixedParticles(0, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RXM"))
		{
			cloth->RotateFixedParticles(0, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rotation X");

		if (ImGui::Button("\t+\t###RYP"))
		{
			cloth->RotateFixedParticles(1, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RYM"))
		{
			cloth->RotateFixedParticles(1, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rotation Y");

		if (ImGui::Button("\t+\t###RZP"))
		{
			cloth->RotateFixedParticles(2, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RZM"))
		{
			cloth->RotateFixedParticles(2, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rotation Z");

		ImGui::NewLine();

		int numOfFixedPoints = cloth->GetFixedParticleNum();
		for (int i = 0; i < numOfFixedPoints; i++)
		{
			glm::vec3 pos = cloth->GetFixedParticlePos(i);
			std::string name = "Pos " + std::to_string(i);
			if (ImGui::InputFloat3(name.c_str(), glm::value_ptr(pos)))
			{
				cloth->SetFixedParticlePos(i, pos);
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Cloth Interaction"))
	{
		float clothTotalMass = cloth->GetMass();
		if (ImGui::DragFloat("Mass", &clothTotalMass))
		{
			cloth->SetMass(abs(clothTotalMass));
		}

		float g = cloth->GetGravityAcce();
		if (ImGui::DragFloat("gravity", &g))
		{
			cloth->SetGravityAcce(abs(g));
		}

		float groundPos = cloth->GetGroundPos();
		if (ImGui::DragFloat("Ground", &groundPos))
		{
			cloth->SetGroundPos(groundPos);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpringDamper Interation"))
	{
		float k_spring = cloth->GetSpringConst();
		if (ImGui::DragFloat("Spring Constant", &k_spring))
		{
			cloth->SetSpringConst(abs(k_spring));
		}

		float k_damper = cloth->GetDampingConst();
		if (ImGui::DragFloat("Damping Constant", &k_damper))
		{
			cloth->SetDampingConst(abs(k_damper));
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Aerodynamic Force Interaction"))
	{
		glm::vec3 windVelocity = cloth->GetWindVelocity();
		if (ImGui::InputFloat3("Wind Speed", glm::value_ptr(windVelocity)))
		{
			cloth->SetWindVelocity(windVelocity);

		}


		float rho = cloth->GetFluidDensity();
		if (ImGui::DragFloat("Fluid Density", &rho))
		{
			cloth->SetFluidDensity(abs(rho));
		}

		float C_d = cloth->GetDragConst();
		if (ImGui::DragFloat("Drag Coefficients", &C_d))
		{
			cloth->SetDragConst(abs(C_d));
		}

		ImGui::TreePop();
	}

	ImGui::End();

	//draw imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

