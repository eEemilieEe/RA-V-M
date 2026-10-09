#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;

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


  // on ouvre la video 
  VideoCapture video;
  video.open("/Users/emilieb/Desktop/POLYTECH/Marseille/5A/RA/input.mp4");
  if (!video.isOpened()) { 
    fprintf(stderr, "ERROR: could not open video\n"); 
    return 1; 
  }

  // identifiant de la texture
  GLuint textureID;
  // génère un identifiant de texture et le range dans textureID
  glGenTextures(1, &textureID);
  // la texture devient active 
  glBindTexture(GL_TEXTURE_2D, textureID);
  //gère l'échantillonage (plus ou moins de pixel texture que de pixel fenêtre) 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // dis a opengl d'utiliser l'image définie dans glGenTextures et glBindTexture lors du rendu
  glEnable(GL_TEXTURE_2D);
  // pour les videos dont la largeur n'est pas multiple de 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


  // tant qu'on ne ferme pas la fenêtre (1 boucle, 1 image)
  while (!glfwWindowShouldClose(window)) {
    // fond de couleur
    //glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    // on efface l'image qui était affiché pour en créer une nouvelle 
    glClear(GL_COLOR_BUFFER_BIT);
    
    // on stocke l'image de la boucle lue dans une matrice
    Mat frame;
    video >> frame;

    if (frame.empty()) {
      break;
    }
    else {
      // remplis la texture avec ces pixels
      glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
      cv::flip(frame, frame, 0);
      glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(-1, -1);   // coin bas gauche
        glTexCoord2f(1, 0); glVertex2f( 1, -1);   // coin bas droit
        glTexCoord2f(0, 1); glVertex2f(-1, 1);   // coin haut gauche
        glTexCoord2f(1, 1); glVertex2f( 1, 1);   // coin haut droit
      glEnd();
    }


    // back buffer (dessin) devient le front buffer (visible)
    glfwSwapBuffers(window);
    // récupère les évènement (clics, souris, etc)
    glfwPollEvents();
}
  
  // nettoyage  
  glDeleteTextures(1, &textureID);
  video.release();

  // ferme proprement toutes les ressources de GLFW (pendant de glfwInit())
  glfwTerminate();
  return 0;
}