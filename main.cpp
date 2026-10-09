//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>

#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
  
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  } 

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  // window devient la fenêtre sur laquelle on travaille et fait les modifications
  glfwMakeContextCurrent(window);

  // tant qu'on ne ferme pas la fenêtre 
  while (!glfwWindowShouldClose(window)) {
    // fond de couleur
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    // on efface l'image qui était affiché pour en créer une nouvelle 
    glClear(GL_COLOR_BUFFER_BIT);

    // back buffer (dessin) devient le front buffer (visible)
    glfwSwapBuffers(window);
    // récupère les évènement (clics, souris, etc)
    glfwPollEvents();
}
  
  // ferme proprement toutes les ressources de GLFW (pendant de glfwInit())
  glfwTerminate();
  return 0;
}